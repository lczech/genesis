#ifndef GENESIS_POPULATION_FORMATS_SYNC_READER_H_
#define GENESIS_POPULATION_FORMATS_SYNC_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2021 Lucas Czech

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
 * See https://sourceforge.net/p/popoolation2/wiki/Tutorial/ for the format description.
 *
 * Note on our internal data representation: The reader returns a Variant per line, where most of
 * the data is set based on the sync input content. However, the sync format does not have altnative
 * bases, so we instead try to estimate the alternative based on counts:
 * Excluding the reference base, we use the base of the remaining three that has the highest total
 * count across all samples, unless all of them are zero, in which case we do not set the altnative
 * base. We also skip cases where the ref is not in `ACGT`, as then the alternative base is also
 * meaningless. In these cases, the alternative will be `N`.
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
    //     Reading
    // ---------------------------------------------------------------------

    std::vector<Variant> read(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    std::vector<Variant> read(
        std::shared_ptr< utils::BaseInputSource > source,
        std::vector<bool> const&                  sample_filter
    ) const;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    bool parse_line(
        utils::InputStream& input_stream,
        Variant&            sample_set
    ) const;

    bool parse_line(
        utils::InputStream&      input_stream,
        Variant&                 sample_set,
        std::vector<bool> const& sample_filter
    ) const;

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

};

} // namespace population
} // namespace genesis

#endif // include guard
