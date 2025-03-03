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
 * @brief Read a lookup table mapping from accessions to IDs, as used in Taxon::id(), by storing
 * the mapping to their Taxon in a Taxonomy.
 *
 * We expect some input table with two or more columns, where one column contains an accession name,
 * and another the taxonomic ID in a taxonomy that the accession belongs to. For this, we offer
 * two modes of reading:
 *
 *  - If the table has no header row, the two relevant columns can simply be specified
 *    by their position, e.g., column 0 for the first column.
 *  - If the table contains a header row with column names, those can be specified instead.
 *    Typically, we use for instance "accession" and "taxid".
 *
 * Furthermore, each of those two modes is provided in two variants: One where the AccessionLookup
 * is returned as the result of the reading, and one where a given AccessionLookup can be provided.
 * The latter is useful if there are multiple tables that shall be combined into a single
 * lookup instance.
 *
 * In order for this to work, the constructor of this class takes the target Taxonomy, and
 * builds an internal map from the Taxon::id() to the @link Taxon Taxa@endlink in the Taxonomy.
 * These are the Taxa that the lookup then maps to for each accession name.
 */
class AccessionLookupReader
{
public:

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
        read_table_(
            instream, accession_column_position, taxid_column_position, target
        );
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
            utils::parallel_for_each(
                sources, [&]( std::shared_ptr<utils::BaseInputSource>& source ){
                    utils::InputStream instream( source );
                    read_table_(
                        instream, accession_column_position, taxid_column_position, target
                    );
                },
                thread_pool_
            ).get();
        } else {
            for( auto& source : sources ) {
                utils::InputStream instream( source );
                read_table_(
                    instream, accession_column_position, taxid_column_position, target
                );
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
        std::string const& accession_column_name = "accession",
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
        std::string const& accession_column_name = "accession",
        std::string const& taxid_column_name = "taxid"
    ) const {
        utils::InputStream instream( source );
        auto const col_pos = get_table_header_column_positions_(
            instream, accession_column_name, taxid_column_name
        );
        read_table_(
            instream, col_pos.first, col_pos.second, target
        );
    }

    template<template<typename...> class Hashmap = std::unordered_map>
    AccessionLookup<Hashmap> read_with_column_names(
        std::vector<std::shared_ptr<utils::BaseInputSource>> sources,
        std::string const& accession_column_name = "accession",
        std::string const& taxid_column_name = "taxid"
    ) const {
        // Read all provided files, either in parallel if we have a thread pool, or consecutively.
        AccessionLookup<Hashmap> target;
        if( thread_pool_ ) {
            utils::parallel_for_each(
                sources, [&]( std::shared_ptr<utils::BaseInputSource>& source ){
                    utils::InputStream instream( source );
                    auto const col_pos = get_table_header_column_positions_(
                        instream, accession_column_name, taxid_column_name
                    );
                    read_table_(
                        instream, col_pos.first, col_pos.second, target
                    );
                },
                thread_pool_
            ).get();
        } else {
            for( auto& source : sources ) {
                utils::InputStream instream( source );
                auto const col_pos = get_table_header_column_positions_(
                    instream, accession_column_name, taxid_column_name
                );
                read_table_(
                    instream, col_pos.first, col_pos.second, target
                );
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

    // ---------------------------------------------------------------------
    //     Counts and Reporting
    // ---------------------------------------------------------------------

    /**
     * @brief Return how many accessions were processed in the input table in total.
     */
    size_t processed_accessions_count() const
    {
        return acc_count_;
    }

    /**
     * @brief Return how many accessions were valid, i.e., were added to the AccessionLookup.
     *
     * This usually corresponds to the final value of AccessionLookup::size(),
     * unless other sources were used as well to add accessions to the lookup.
     */
    size_t valid_accessions_count() const
    {
        return val_count_;
    }

    /**
     * @brief Return how many accessions were invalid, i.e., for which there was no valid tax id
     * in the underlying Taxonomy.
     *
     * This requires skip_accessions_with_invalid_tax_id() to be set to `true`, as otherwise,
     * an exception is thrown when an accession with an invalid tax id is encountered.
     */
    size_t invalid_accessions_count() const
    {
        return inv_count_;
    }

    /**
     * @brief Produce a short summary report of the reading, listing the counts.
     */
    std::string report() const
    {
        auto const acc_count = static_cast<double>( acc_count_ );
        auto const val_percent = 100.0 * static_cast<double>( val_count_ ) / acc_count;
        // auto const inv_percent = 100.0 * static_cast<double>( inv_count_ ) / acc_count;
        return (
            "Processed " + std::to_string( acc_count_ ) + " input accession names, of which " +
            std::to_string( val_count_ ) + "(" + std::to_string( val_percent ) +
            "%) had a valid taxonomic ID in the taxonomy."
        );
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
    void read_table_(
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
            ++acc_count_;

            // Convert the tax id to numeric. Using two methods here
            // for string and string view, again for speed.
            #if GENESIS_CPP_STD >= GENESIS_CPP_STD_17
                auto const taxid = utils::convert_from_chars<uint64_t>( cols[tid_pos] );
            #else
                auto const taxid = std::stoull( cols[tid_pos] );
            #endif // GENESIS_CPP_STD >= GENESIS_CPP_STD_17
            auto tax_it = tax_id_to_taxon_.find( taxid );
            if( tax_it == tax_id_to_taxon_.end() ) {
                ++inv_count_;
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
            target.add( std::string( cols[acc_pos] ), tax_it->second );
            ++val_count_;
        }
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

    // Counts of processing the table, for the total,
    // and for the accessions with a valid and invalid tax id.
    mutable std::atomic<size_t> acc_count_ = 0;
    mutable std::atomic<size_t> val_count_ = 0;
    mutable std::atomic<size_t> inv_count_ = 0;

    std::shared_ptr<utils::ThreadPool> thread_pool_;

};

} // namespace taxonomy
} // namespace genesis

#endif // include guard
