#ifndef GENESIS_POPULATION_FORMAT_GENOME_REGION_READER_H_
#define GENESIS_POPULATION_FORMAT_GENOME_REGION_READER_H_

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

#include "genesis/population/genome_locus_set.hpp"
#include "genesis/population/genome_region.hpp"
#include "genesis/population/genome_region_list.hpp"
#include "genesis/utils/io/input_source.hpp"
#include "genesis/utils/io/input_stream.hpp"

#include <cstdint>
#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     Genome Region Reader
// =================================================================================================

/**
 * @brief Generic reader for inputs that contain a genomic region or locus per line,
 * in different formats.
 *
 * The reader expects an input source, and tries to interpret each line as a position or region
 * in a chromosome, offering a variety of formats:
 *
 *   * "chr" for whole chromosomes,
 *   * "chr:position", "chr:start-end", "chr:start..end" for positions and regions,
 *   * tab- or space-delimited "chr position" or "chr start end" as well for positions and regions.
 *
 * This allows for maximum flexibility when reading in such inputs. Note that this is more flexible
 * than parse_genome_region(), which does not support the tab- and space-delimiation - when parsing
 * an individual string such as coming from a command line argument, we do not want to allow tabs
 * or spaces, as this can get messy, hence we only offer this delimitation for files as of now.
 */
class GenomeRegionReader
{
public:

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GenomeRegionReader() = default;
    ~GenomeRegionReader() = default;

    GenomeRegionReader( GenomeRegionReader const& ) = default;
    GenomeRegionReader( GenomeRegionReader&& )      = default;

    GenomeRegionReader& operator= ( GenomeRegionReader const& ) = default;
    GenomeRegionReader& operator= ( GenomeRegionReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read an input source, and return its content as a GenomeLocusSet.
     *
     * This is the recommended way to read an input for testing whether genome coordinates
     * are covered (filtered / to be considered) for downstream analyses.
     */
    GenomeLocusSet read_as_genome_locus_set(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * @brief Read an input source, and return its content as a GenomeRegionList.
     *
     * If @p merge is set, adjacent coordinates or overlapping regions of the input are merged into
     * contiguous intervals. This is useful of the region list is used to determine coverage,
     * although for that use case, it is recommended to use read_as_genome_locus_set() instead,
     * as this is faster for testing coverage of genomic coordinates.
     * Also, see the `overlap` flag of GenomeRegionList::add( GenomeLocus const&, bool ) for details
     * on the flag.
     */
    GenomeRegionList read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        bool merge = false
    ) const;

    /**
     * @brief Read a `map`/`bim` input source, and add its content to an existing GenomeRegionList.
     *
     * @copydetails GenomeRegionReader::read_as_genome_region_list( std::shared_ptr< genesis::utils::BaseInputSource >, bool ) const
     */
    void read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        GenomeRegionList& target,
        bool merge = false
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool zero_based() const
    {
        return zero_based_;
    }

    GenomeRegionReader& zero_based( bool value )
    {
        zero_based_ = value;
        return *this;
    }

    bool end_exclusive() const
    {
        return end_exclusive_;
    }

    GenomeRegionReader& end_exclusive( bool value )
    {
        end_exclusive_ = value;
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Internal Helpers
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Helper function to process an input source, with a @p callback function that is called
     * for every line, to do the actual storage and downstream steps after parsing.
     */
    void read_(
        std::shared_ptr< utils::BaseInputSource > source,
        std::function<void( GenomeRegion const& region )> callback
    ) const;

    // -------------------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------------------

    bool zero_based_ = false;
    bool end_exclusive_ = false;
};

} // namespace population
} // namespace genesis

#endif // include guard
