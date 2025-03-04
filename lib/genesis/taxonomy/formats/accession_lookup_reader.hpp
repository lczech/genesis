#ifndef GENESIS_TAXONOMY_FORMATS_ACCESSION_LOOKUP_READER_H_
#define GENESIS_TAXONOMY_FORMATS_ACCESSION_LOOKUP_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/accession_lookup.hpp"
#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/text/convert.hpp"
#include "genesis/utils/text/string.hpp"
#include "genesis/utils/threading/thread_functions.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <atomic>
#include <cassert>
#include <cctype>
#include <limits>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Accession Lookup Reader
// =================================================================================================

/**
 * @brief Read a lookup table mapping from accessions to taxids, as used in Taxon::id(), by storing
 * the mapping to their Taxon in a Taxonomy.
 *
 * We expect some input table with two or more columns, where one column contains an accession name,
 * and another the taxonomic ID in a taxonomy that the accession belongs to. For this, we offer
 * two modes of reading:
 *
 *  - If the table has no header row, the two relevant columns can simply be specified
 *    by their position, e.g., column 0 for the first column.
 *  - If the table contains a header row with column names, those can be specified instead.
 *    Typically, we use for instance "accession.version" and "taxid".
 *    This is the format used by the NCBI accession2taxid tables.
 *
 * Furthermore, each of those two modes is provided in two variants: One where the AccessionLookup
 * is returned as the result of the reading, and one where a given AccessionLookup can be provided.
 * The latter is useful if there are multiple tables that shall be combined into a single
 * lookup instance. We however recommend to use the multi-file reading option instead for this
 * use case, as this can parallelize the reading to get 2x speedup.
 *
 * In order for this to work, the constructor of this class takes the target Taxonomy, and
 * builds an internal map from the Taxon::id() to the @link Taxon Taxa@endlink in the Taxonomy.
 * These are the Taxa that the lookup then maps to for each accession name.
 *
 * Note: See https://ftp.ncbi.nlm.nih.gov/pub/taxonomy/accession2taxid/README for details on
 * the format used by the NCBI accession2taxid tables. In short, these files are strucutred as
 *
 *     accession<TAB>accession.version<TAB>taxid<TAB>gi
 *
 * with the `accession.version` being the one typically used to label reference sequences,
 * and the `gi` being phased out, meaning that we do not expect it in more recent releases.
 */
class AccessionLookupReader
{
public:

    // ---------------------------------------------------------------------
    //     Helper Structs
    // ---------------------------------------------------------------------

    struct Report
    {
        /**
         * @brief Source that was read from, typically an `accession2taxid` table file.
         */
        std::string source;

        /**
         * @brief How many accessions were processed in the input table in total.
         */
        size_t processed_count = 0;

        /**
         * @brief How many accessions were valid, i.e., were added to the AccessionLookup.
         */
        size_t valid_count = 0;

        /**
         * @brief How many accessions were invalid, i.e., for which there was no valid tax id
         * in the underlying Taxonomy.
         *
         * This requires skip_accessions_with_invalid_tax_id() to be set to `true`, as otherwise,
         * an exception is thrown when an accession with an invalid tax id is encountered.
         */
        size_t invalid_count = 0;

        /**
         * @brief How many accessions were mismatching duplicates.
         *
         * This requires ignore_mismatching_duplicates() to be set, as otherwise, an exception
         * is thrown in the case of mismatching duplicate entries.
         */
        size_t mismatching_duplicate_count = 0;
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    AccessionLookupReader();

    AccessionLookupReader( Taxonomy& tax )
    {
        fill_taxon_map_( tax );
    }

    AccessionLookupReader( Taxonomy& tax, std::shared_ptr<utils::ThreadPool> thread_pool )
    {
        fill_taxon_map_( tax );
        thread_pool_ = thread_pool;
    }

    ~AccessionLookupReader() = default;

    AccessionLookupReader( AccessionLookupReader const& ) = default;
    AccessionLookupReader( AccessionLookupReader&& )      = default;

    AccessionLookupReader& operator= ( AccessionLookupReader const& ) = default;
    AccessionLookupReader& operator= ( AccessionLookupReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Read with Column Positions
    // ---------------------------------------------------------------------

    template<template<typename...> class Hashmap = std::unordered_map>
    AccessionLookup<Hashmap> read_with_column_positions(
        std::shared_ptr<utils::BaseInputSource> source,
        size_t accession_column_position = 0,
        size_t taxid_column_position = 1
    ) const {
        AccessionLookup<Hashmap> target;
        read_with_column_positions(
            source, target, accession_column_position, taxid_column_position
        );
        return target;
    }

    template<template<typename...> class Hashmap = std::unordered_map>
    void read_with_column_positions(
        std::shared_ptr<utils::BaseInputSource> source,
        AccessionLookup<Hashmap>& target,
        size_t accession_column_position = 0,
        size_t taxid_column_position = 1
    ) const {
        utils::InputStream instream( source );
        auto report = read_table_(
            instream, accession_column_position, taxid_column_position, target
        );
        report.source = source->source_string();
        reports_.push_back( report );
    }

    template<template<typename...> class Hashmap = std::unordered_map>
    AccessionLookup<Hashmap> read_with_column_positions(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
        size_t accession_column_position = 0,
        size_t taxid_column_position = 1
    ) const {
        // Read all provided files, either in parallel if we have a thread pool, or consecutively.
        AccessionLookup<Hashmap> target;
        if( thread_pool_ ) {
            // For the multithreaded, we need a bit of extra care when adding the reports
            // to our result, to avoid concurrency issues. We resize here to what we need,
            // and then use an offset, so that each thread gets its report to write to.
            auto& reports = reports_;
            size_t report_offset = reports.size();
            reports.resize( reports.size() + sources.size() );

            // Now add files in parallel. In our tests, this is about 2x as fast as single
            // threaded reading. Not much more can be done though it seems, as the rehashing
            // of the hash map tables becomes a bottleneck eventually.
            utils::parallel_for(
                0, sources.size(),
                [&]( size_t source_index ){
                    auto& source = sources[source_index];
                    utils::InputStream instream( source );
                    auto report = read_table_(
                        instream, accession_column_position, taxid_column_position, target
                    );
                    report.source = source->source_string();
                    reports[report_offset + source_index] = report;
                },
                thread_pool_
            ).get();
        } else {
            for( auto& source : sources ) {
                utils::InputStream instream( source );
                auto report = read_table_(
                    instream, accession_column_position, taxid_column_position, target
                );
                report.source = source->source_string();
                reports_.push_back( report );
            }
        }
        return target;
    }

    // ---------------------------------------------------------------------
    //     Read with Column Names
    // ---------------------------------------------------------------------

    template<template<typename...> class Hashmap = std::unordered_map>
    AccessionLookup<Hashmap> read_with_column_names(
        std::shared_ptr<utils::BaseInputSource> source,
        std::string const& accession_column_name = "accession.version",
        std::string const& taxid_column_name = "taxid"
    ) const {
        AccessionLookup<Hashmap> target;
        read_with_column_names(
            source, target, accession_column_name, taxid_column_name
        );
        return target;
    }

    template<template<typename...> class Hashmap = std::unordered_map>
    void read_with_column_names(
        std::shared_ptr<utils::BaseInputSource> source,
        AccessionLookup<Hashmap>& target,
        std::string const& accession_column_name = "accession.version",
        std::string const& taxid_column_name = "taxid"
    ) const {
        utils::InputStream instream( source );
        auto const col_pos = get_table_header_column_positions_(
            instream, accession_column_name, taxid_column_name
        );
        auto report = read_table_(
            instream, col_pos.first, col_pos.second, target
        );
        report.source = source->source_string();
        reports_.push_back( report );
    }

    template<template<typename...> class Hashmap = std::unordered_map>
    AccessionLookup<Hashmap> read_with_column_names(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
        std::string const& accession_column_name = "accession.version",
        std::string const& taxid_column_name = "taxid"
    ) const {
        // Read all provided files, either in parallel if we have a thread pool, or consecutively.
        AccessionLookup<Hashmap> target;
        if( thread_pool_ ) {
            // Same as above. See there for details.
            auto& reports = reports_;
            size_t report_offset = reports.size();
            reports.resize( reports.size() + sources.size() );
            utils::parallel_for(
                0, sources.size(),
                [&]( size_t source_index ){
                    auto& source = sources[source_index];
                    utils::InputStream instream( source );
                    auto const col_pos = get_table_header_column_positions_(
                        instream, accession_column_name, taxid_column_name
                    );
                    auto report = read_table_(
                        instream, col_pos.first, col_pos.second, target
                    );
                    report.source = source->source_string();
                    reports[report_offset + source_index] = report;
                },
                thread_pool_
            ).get();
        } else {
            for( auto& source : sources ) {
                utils::InputStream instream( source );
                auto const col_pos = get_table_header_column_positions_(
                    instream, accession_column_name, taxid_column_name
                );
                auto report = read_table_(
                    instream, col_pos.first, col_pos.second, target
                );
                report.source = source->source_string();
                reports_.push_back( report );
            }
        }
        return target;
    }

    // ---------------------------------------------------------------------
    //     Settings
    // ---------------------------------------------------------------------

    /**
     * @brief Set the separator char used in the input table.
     */
    void separator_char( char c )
    {
        separator_char_ = c;
    }

    char separator_char() const
    {
        return separator_char_;
    }

    /**
     * @brief Set how to react to accessions for which the input table contains a tax id that
     * is not present in the underlying Taxonomy.
     *
     * By default, we throw an exception. With this setting here, instead such accessions can be
     * skipped. Their count is then available via invalid_accessions_count().
     */
    void skip_accessions_with_invalid_tax_id( bool value )
    {
        skip_accessions_with_invalid_tax_id_ = value;
    }

    bool skip_accessions_with_invalid_tax_id() const
    {
        return skip_accessions_with_invalid_tax_id_;
    }

    /**
     * @brief Decide what to do in case of conflicting entries.
     *
     * When adding a new entry to the hash map, we might find that the accession already exists
     * in the target hash map. If the entry value is identical, nothing happens.
     * If an entry is added for an existing key, but a different value, we throw an exception
     * by default. If ignore_mismatching_duplicates is set however, we just ignore the new
     * entry, and keep the existing one. This is useful to work with the messy reality of data.
     */
    void ignore_mismatching_duplicates( bool value )
    {
        ignore_mismatching_duplicates_ = value;
    }

    bool ignore_mismatching_duplicates() const
    {
        return ignore_mismatching_duplicates_;
    }

    // ---------------------------------------------------------------------
    //     Reporting
    // ---------------------------------------------------------------------

    /**
     * @brief Return all Report%s created during reading. There is one pre input source.
     */
    std::vector<Report> const& reports()
    {
        return reports_;
    }

    /**
     * @brief Produce a short summary report of reading a table, listing the counts.
     */
    static std::string print( Report const& report )
    {
        // Calculate proportions
        auto const acc_count = static_cast<double>( report.processed_count );
        auto const val_prop = static_cast<double>( report.valid_count ) / acc_count;
        auto const inv_prop = static_cast<double>( report.invalid_count ) / acc_count;
        auto const mis_prop = static_cast<double>( report.mismatching_duplicate_count ) / acc_count;

        // Print tablulated counts and percentages
        std::string p = "In " + report.source;
        p += "\n    processed: " + std::to_string( report.processed_count );
        p += "\n    valid:     " + std::to_string( report.valid_count );
        p += " (" + std::to_string( 100.0 * val_prop ) + "%)";
        p += "\n    invalid:    " + std::to_string( report.invalid_count );
        p += " (" + std::to_string( 100.0 * inv_prop ) + "%)";
        if( report.mismatching_duplicate_count ) {
            p += "\n    mismatch:   " + std::to_string( report.mismatching_duplicate_count );
            p += " (" + std::to_string( 100.0 * mis_prop ) + "%)";
        }
        return p;

        // return (
        //     "In " + report.source + ": Processed " + std::to_string( report.processed_count ) +
        //     " input accession names, of which " + std::to_string( report.valid_count ) +
        //     "(" + std::to_string( val_prop ) + "%) had a valid taxonomic ID in the taxonomy."
        // );
    }

    std::string print_reports() const
    {
        std::string result;
        for( auto const& report : reports_ ) {
            result += print( report ) + "\n";
        }
        return result;
    }

    // ---------------------------------------------------------------------
    //     Internal Members
    // ---------------------------------------------------------------------

private:

    void fill_taxon_map_(
        Taxonomy& tax
    ) {
        // Iterate the whole underlying taxonomy, and add an entry for each taxon
        // to our internal lookup table from taxon id to the taxon pointer.
        // This is then used when adding an accession to find the taxa.
        preorder_for_each(
            tax,
            [&]( Taxon& taxon )
            {
                auto const result = tax_id_to_taxon_.insert( std::make_pair( taxon.id(), &taxon ));
                if( !result.second ) {
                    throw std::runtime_error(
                        "Taxon id " + std::to_string( taxon.id() ) +
                        " occurs multiple times in Taxonomy"
                    );
                }
            }
        );
    }

    std::pair<size_t, size_t> get_table_header_column_positions_(
        utils::InputStream& instream,
        std::string const& accession_column_name,
        std::string const& taxid_column_name
    ) const {
        // Read the header row, and split it into column names
        auto const header = instream.get_line();
        auto const cols = utils::split( header, separator_char_, false );

        // Find the two columns we are intersted in.
        size_t const max = std::numeric_limits<size_t>::max();
        size_t acc_pos = max;
        size_t tid_pos = max;
        for( size_t i = 0; i < cols.size(); ++i ) {
            if( cols[i] == accession_column_name ) {
                if( acc_pos != max ) {
                    throw std::runtime_error(
                        "Column name '" + accession_column_name +
                        "' occurs multiple times in accession lookp table header row"
                    );
                }
                acc_pos = i;
            }
            if( cols[i] == taxid_column_name ) {
                if( tid_pos != max ) {
                    throw std::runtime_error(
                        "Column name '" + taxid_column_name +
                        "' occurs multiple times in accession lookp table header row"
                    );
                }
                tid_pos = i;
            }
        }
        if( acc_pos == max || tid_pos == max ) {
            throw std::runtime_error(
                "Accession lookup table does not contain column names for the two needed columns '" +
                accession_column_name + "' and '" + taxid_column_name + "'"
            );
        }
        return std::make_pair( acc_pos, tid_pos );
    }

    template<template<typename...> class Hashmap>
    Report read_table_(
        utils::InputStream& instream,
        size_t acc_pos,
        size_t tid_pos,
        AccessionLookup<Hashmap>& target
    ) const {
        // If we have a default constructed instance of this class, we cannot do anything.
        // Need access to the taxonomy in order to map the lookup to somewhere...
        if( tax_id_to_taxon_.empty() ) {
            throw std::runtime_error(
                "Cannot read accession lookup table for an empty taxonomy, "
                "or taxonomy has not been provided for table reading."
            );
        }
        if( acc_pos == tid_pos ) {
            throw std::invalid_argument(
                "Identical column positions given for accession and taxid columns "
                "for reading accession lookup table."
            );
        }

        // Parse the table, line by line.
        Report report;
        while( instream ) {
            // Get the next line (moves the inut stream), and split the line in the table.
            // Depending on the standard, we can use a view, which is cheaper.
            #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
                auto const line = instream.get_line_view();
                auto const cols = utils::split_view( line, separator_char_, false );
            #else
                auto const line = instream.get_line();
                auto const cols = utils::split( line, separator_char_, false );
            #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17

            // Basic sanity check of the columns.
            if( acc_pos >= cols.size() || tid_pos >= cols.size() ) {
                throw std::runtime_error(
                    "Invalid accession lookup table with inconsistent number of columns"
                );
            }
            ++report.processed_count;

            // Convert the tax id to numeric. Using two methods here
            // for string and string view, again for speed.
            #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
                auto const taxid = utils::convert_from_chars<uint64_t>( cols[tid_pos] );
            #else
                auto const taxid = std::stoull( cols[tid_pos] );
            #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17
            auto tax_it = tax_id_to_taxon_.find( taxid );
            if( tax_it == tax_id_to_taxon_.end() ) {
                ++report.invalid_count;
                if( skip_accessions_with_invalid_tax_id_ ) {
                    continue;
                } else {
                    throw std::runtime_error(
                        "Invalid accession lookup table, containing an entry for tax id '" +
                        std::string( cols[tid_pos] ) + "' which is not part of the taxonomy"
                    );
                }
            }

            // Add the entry. We assume that the target hashmap takes care of thread sychronization.
            bool const added = target.add(
                std::string( cols[acc_pos] ),
                tax_it->second,
                ignore_mismatching_duplicates_
            );
            if( !added ) {
                ++report.mismatching_duplicate_count;
            }
            ++report.valid_count;
        }
        return report;
    }

    // ---------------------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------------------

private:

    // Internal lookup from tax id to the Taxon that contains it.
    // We use this to map accessions to Taxa.
    std::unordered_map<uint64_t, Taxon*> tax_id_to_taxon_;

    // Settings for reading the table
    char separator_char_ = '\t';
    bool skip_accessions_with_invalid_tax_id_ = false;
    bool ignore_mismatching_duplicates_ = false;

    // Store all reports for a read operation
    mutable std::vector<Report> reports_;

    std::shared_ptr<utils::ThreadPool> thread_pool_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
