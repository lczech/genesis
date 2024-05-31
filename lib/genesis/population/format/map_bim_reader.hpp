#ifndef GENESIS_POPULATION_FORMAT_MAP_BIM_READER_H_
#define GENESIS_POPULATION_FORMAT_MAP_BIM_READER_H_

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
//     MAP/BIM Reader
// =================================================================================================

/**
 * @brief Reader for map/bim files as used by PLINK.
 *
 * This reader offers to process `map`/`bim` files as for example used by PLINK. The formats are pretty
 * similar, so we can read all with one reader.
 * See https://www.cog-genomics.org/plink/2.0/formats#bim
 * and https://www.cog-genomics.org/plink/2.0/formats#map
 * for details on these formats.
 *
 * Basically, a `map` file contains four columns (1-4 below), of which column 3 (position in morgans
 * or centimorgans) is optional. Then, `bim` files are an extension of this, which add two additional
 * columns for the alleles.
 *
 *  1. Chromosome code (either an integer, or 'X'/'Y'/'XY'/'MT'; '0' indicates unknown) or name
 *  2. Variant identifier
 *  3. Position in morgans or centimorgans (optional; safe to use dummy value of '0')
 *  4. Base-pair coordinate (1-based; limited to 2^31-2)
 *  5. Allele 1 (corresponding to clear bits in PLINK .bed; usually minor)
 *  6. Allele 2 (corresponding to set bits in PLINK .bed; usually major)
 *
 * For example:
 *
 *     2       2_71210 0       71210   C       A
 *     2       2_71228 0       71228   G       C
 *     2       2_71282 0       71282   T       C
 *
 * All lines must have the same number of columns (so either no lines contain the centimorgans
 * column, or all of them do).
 *
 * According to the PLINK standard, negative base-pair coordinates are skipped. We do the same here
 * by default. This behaviour can also be changed with the skip_negative_coordinates() setting,
 * so that downstream processes can decide what to make of this themselves, if needed.
 * The format description however does not mention 0 as a coordinate value. As this is a tricky
 * special case, we throw an exception in this case, to make sure that users are aware of this.
 * Also, we internally use 0 as an undefined or "any" coordinate, so just blindly accpeting this
 * here might lead to unexpected behaviour downstream; hence the exception.
 */
class MapBimReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Store all values that can typically appear in the columns of a map/bim file.
     *
     * The order of fields in the struct is the same as the order of columns.
     * If the file does not have all the columns, the remaining values here are simply left empty
     * or at 0.
     *
     * Note that here, according to the file format, we call the value `position` that denotes the
     * position in (centi)morgans, whereas the value that we typically call "position" in the rest
     * of our code base (genome coordinate) is here called `coordinate`, to comply with the format.
     */
    struct Feature
    {
        std::string chromosome;
        std::string variant_id;
        double position;
        int64_t coordinate;
        char allele_1;
        char allele_2;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    MapBimReader() = default;
    ~MapBimReader() = default;

    MapBimReader( MapBimReader const& ) = default;
    MapBimReader( MapBimReader&& )      = default;

    MapBimReader& operator= ( MapBimReader const& ) = default;
    MapBimReader& operator= ( MapBimReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read a `map`/`bim` input source, and return its content as a list of Feature structs.
     */
    std::vector<Feature> read(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * @brief Read an input source, and return its content as a GenomeLocusSet.
     *
     * This only uses the columns `chromosome` and `coordinate` and ignores everything else.
     *
     * This is the recommended way to read an input for testing whether genome coordinates
     * are covered (filtered / to be considered) for downstream analyses.
     */
    GenomeLocusSet read_as_genome_locus_set(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * @brief Read a `map`/`bim` input source, and return its content as a GenomeRegionList.
     *
     * This only uses the columns `chromosome` and `coordinate` and ignores everything else.
     *
     * If @p merge is set, adjacent coordinates of the file are merged into contiguous intervals.
     * This is useful of the region list is used to determine coverage, and the default here.
     * See the `overlap` flag of GenomeRegionList::add( GenomeLocus const&, bool ) for details.
     */
    GenomeRegionList read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        bool merge = true
    ) const;

    /**
     * @brief Read a `map`/`bim` input source, and add its content to an existing GenomeRegionList.
     *
     * @copydetails MapBimReader::read_as_genome_region_list( std::shared_ptr< genesis::utils::BaseInputSource >, bool ) const
     */
    void read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        GenomeRegionList& target,
        bool merge = true
    ) const;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    bool skip_negative_coordinates() const
    {
        return skip_negative_coordinates_;
    }

    MapBimReader& skip_negative_coordinates( bool value )
    {
        skip_negative_coordinates_ = value;
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
        std::function<void(Feature&&)> callback
    ) const;

    /**
     * @brief Parse a single line into the given @p feature, and return the number of columns
     * found in the line.
     *
     * The return value is used by read_() to make sure that all lines have the same number of
     * columns. We also use a @p buffer to avoid memory reallications in between lines.
     */
    size_t parse_line_(
        utils::InputStream&       input_stream,
        Feature&                  feature,
        std::vector<std::string>& buffer
    ) const;

    // -------------------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------------------

    bool skip_negative_coordinates_ = true;

};

} // namespace population
} // namespace genesis

#endif // include guard
