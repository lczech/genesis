#ifndef GENESIS_POPULATION_FORMATS_SYNC_READER_H_
#define GENESIS_POPULATION_FORMATS_SYNC_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

#include "genesis/population/variant.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <string>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     PoPoolation2 Synchronized File Reader
// =================================================================================================

/**
 * @brief Reader for PoPoolation2's "synchronized" files.
 *
 * These files are a simple tally of the counts at each position and sample in a (m)pileup file.
 * Sync files are structured as follows. Each line represents a position on a chromosome:
 *
 *     2R  2302    T   0:7:0:0:0:0 0:7:0:0:0:0
 *     2R  2303    T   0:8:0:0:0:0 0:8:0:0:0:0
 *     2R  2304    C   0:0:9:0:0:0 0:0:9:0:0:0
 *     2R  2305    C   1:0:9:0:0:0 0:0:9:1:0:0
 *
 * where:
 *
 *   - col1: reference contig/chromosome
 *   - col2: position within the reference contig/chromosome
 *   - col3: reference character (base)
 *   - col4: allele frequencies of population number 1
 *   - col5: allele frequencies of population number 2
 *   - coln: allele frequencies of population number n
 *
 * The allele frequencies are in the format `A:T:C:G:N:D`, i.e: count of bases `A`,
 * count of bases `T`, etc, and deletion count in the end (character '*' in the mpileup).
 *
 * See https://sourceforge.net/p/popoolation2/wiki/Tutorial/ for the original format description.
 * Unfortunately, the file format does not support sample names.
 *
 * We here support an ad-hoc extension of the `sync` format that offers a header line to store
 * sample names, which are usually not available in the `sync` format. We currently expect a
 * fixed format:
 *
 *     #chr	pos	ref	S1 S2...
 *
 * starting with a number sign (hashtag) `#` symbol, optionally followed by a tab character,
 * and then listing the fixed columns `chr`, `pos`, and `ref`, followed by the sample name
 * columns, all tab-delimited.
 *
 * We furthermore allow a custom extension of the format, where `.:.:.:.:.:.` represents missing data.
 * See allow_missing() and https://github.com/lczech/grenedalf/issues/4 for details.
 *
 * Note on our internal data representation: The reader returns a Variant per line, where most of
 * the data is set based on the sync input content. However, the sync format does not have
 * alternative bases. By default, we leave it hence as 'N'. See however the guess_alt_base() setting
 * to instead estimate the alternative base from the data.
 */
class SyncReader
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SyncReader() = default;
    ~SyncReader() = default;

    SyncReader( SyncReader const& ) = default;
    SyncReader( SyncReader&& )      = default;

    SyncReader& operator= ( SyncReader const& ) = default;
    SyncReader& operator= ( SyncReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Read Header
    // ---------------------------------------------------------------------

    /**
     * @brief Read the header line, if there is one. Do nothing if there is not.
     *
     * Has to be called at the start of reading a @p source file, as otherwise the reading will
     * have already moved on from the header line.
     *
     * This is support for an ad-hoc extension of the `sync` format that offers a header line
     * to store sample names, which are usually not available in the `sync` format. We currently
     * expect a fixed format:
     *
     *     #chr	pos	ref	S1 S2...
     *
     * starting with a number sign (hashtag) `#` symbol, optionally followed by a tab character,
     * and then listing the fixed columns `chr`, `pos`, and `ref`, followed by the sample name
     * columns, all tab-delimited.
     *
     * The return value of the function are the values of the sample name columns, i.e., the
     * sample names.
     */
    std::vector<std::string> read_header(
        utils::InputStream& input_stream
    ) const;

    /**
     * @brief Read the header line, if there is one, only reading specific columns.
     * Do nothing if there is not.
     *
     * This overload of the function additionally takes a vector indicating which sample names
     * to read and return (where @p sample_filter is `true`), and ignores the rest (where
     * @p sample_filter is `false`). The size of @p sample_filter has to match the number of
     * sample name columns; an exception is thrown otherwise.
     *
     * This function hence is meant to match the read() and parse_line() overloads that also
     * take this type of filter.
     */
    std::vector<std::string> read_header(
        utils::InputStream& input_stream,
        std::vector<bool> const& sample_filter
    ) const;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read the whole input into a vector of Variant%s.
     */
    std::vector<Variant> read(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * brief Read the whole input into a vector of Variant%s, using a subset of the sample columns.
     *
     * The overload expects a vector indicating which columns to read and which to skip.
     * The Variant%s produced for each line of input only contain as many entries as there are
     * `true` values in the provided @p sample_filter. If the size of the @p sample_filter does
     * not match the number of sample columns, an exception is thrown.
     */
    std::vector<Variant> read(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<bool> const&                  sample_filter
    ) const;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    /**
     * @brief Read a single line into the provided @p Variant.
     *
     * Returns wheather the reading was successful, or not, i.e., wheather the input is at its end.
     */
    bool parse_line(
        utils::InputStream& input_stream,
        Variant&            sample_set
    ) const;

    /**
     * @brief Read a single line into the provided @p Variant, using a subset of the sample columns.
     *
     * This is an equivalent overload as described in the read() functions. See there for details.
     */
    bool parse_line(
        utils::InputStream&      input_stream,
        Variant&                 sample_set,
        std::vector<bool> const& sample_filter
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool guess_alt_base() const
    {
        return guess_alt_base_;
    }

    /**
     * @brief Set to guess the alternative base of the Variant, instead of leaving it at 'N'.
     *
     * Excluding the reference base, we use the base of the remaining three that has the highest
     * total count across all samples, unless all of them are zero, in which case we do not set
     * the altnative base. We also skip cases where the ref is not in `ACGT`, as then the
     * alternative base is also meaningless. In these cases, the alternative will be `N`.
     *
     * Note though that this can lead to conflicts between different files, if the second most
     * abundant nucleotide differs between them, e.g., in non-bia   llelic positions. Usually we can
     * deal with this, see for example VariantParallelInputStream::Iterator::joined_variant().
     * Still, it is important to keep this in mind.
     */
    SyncReader& guess_alt_base( bool value )
    {
        guess_alt_base_ = value;
        return *this;
    }

    bool allow_missing() const
    {
        return allow_missing_;
    }

    /**
     * @brief Set whether to allow missing data in the format suggested by Kapun et al.
     *
     * In order to distinguish missing/masked data from true zero-coverage positions, Kapun suggested
     * to use the notation `.:.:.:.:.:.` for masked sites. When this is activate (default), we
     * allow to read these, but still (as of now) produce zero-coverage sites, as we currently
     * do not have an internal representation for denoting that.
     * See https://github.com/lczech/grenedalf/issues/4 for details.
     */
    SyncReader& allow_missing( bool value )
    {
        allow_missing_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------------------

private:

    bool parse_line_(
        utils::InputStream&      input_stream,
        Variant&                 sample_set,
        std::vector<bool> const& sample_filter,
        bool                     use_sample_filter
    ) const;

    // Only use intrinsics version for the compilers that support them!
    #if defined(__GNUC__) || defined(__GNUG__) || defined(__clang__)

        void parse_sample_gcc_intrinsic_(
            utils::InputStream& input_stream,
            BaseCounts&         sample
        ) const;

    #endif

    void parse_sample_simple_(
        utils::InputStream& input_stream,
        BaseCounts&         sample
    ) const;

    void parse_sample_(
        utils::InputStream& input_stream,
        BaseCounts&         sample
    ) const;

    void skip_sample_(
        utils::InputStream& input_stream
    ) const;

    // -------------------------------------------------------------------------
    //     Member Varables
    // -------------------------------------------------------------------------

private:

    bool guess_alt_base_ = false;
    bool allow_missing_ = true;

};

} // namespace population
} // namespace genesis

#endif // include guard
