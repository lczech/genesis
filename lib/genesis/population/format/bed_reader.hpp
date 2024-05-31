#ifndef GENESIS_POPULATION_FORMAT_BED_READER_H_
#define GENESIS_POPULATION_FORMAT_BED_READER_H_

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

#include <functional>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     BED Reader
// =================================================================================================

/**
 * @brief Reader for BED (Browser Extensible Data) files.
 *
 * We follow the definition by https://en.wikipedia.org/wiki/BED_(file_format), which itself is
 * based on the UCSC Genome Browser definition of the BED format:
 *
 *  Column number | Title        | Definition
 *  ------------: | :----------- | :---------
 *  1             | chrom        | Chromosome (e.g. chr3, chrY, chr2_random) or scaffold (e.g. scaffold10671) name
 *  2             | chromStart   | Start coordinate on the chromosome or scaffold for the sequence considered (the first base on the chromosome is numbered 0 in the file format - we here however use 1-based coordinates)
 *  3             | chromEnd     | End coordinate on the chromosome or scaffold for the sequence considered. This position is non-inclusive, unlike chromStart.
 *  4             | name         | Name of the line in the BED file
 *  5             | score        | Score between 0 and 1000
 *  6             | strand       | DNA strand orientation (positive ["+"] or negative ["-"] or "." if no strand)
 *  7             | thickStart   | Starting coordinate from which the annotation is displayed in a thicker way on a graphical representation (e.g.: the start codon of a gene)
 *  8             | thickEnd     | End coordinates from which the annotation is no longer displayed in a thicker way on a graphical representation (e.g.: the stop codon of a gene)
 *  9             | itemRgb      | RGB value in the form R,G,B (e.g. 255,0,0) determining the display color of the annotation contained in the BED file
 *  10            | blockCount   | Number of blocks (e.g. exons) on the line of the BED file
 *  11            | blockSizes   | List of values separated by commas corresponding to the size of the blocks (the number of values must correspond to that of the "blockCount")
 *  12            | blockStarts  | List of values separated by commas corresponding to the starting coordinates of the blocks, coordinates calculated relative to those present in the chromStart column (the number of values must correspond to that of the "blockCount")
 *
 * The reader offers to parse every line or the whole file into a Feature format that contains the
 * above columns (as far as present in the file), or to read into a GenomeRegionList structure
 * instead, in which case only the genome coordinates (chromosome and start and end positions) are
 * used. The input needs to have a consistent number of columns, but only the first three are
 * mandatory. They all must be in the above order, and if later (more towards the end of the line)
 * columns are needed, all previous ones need to be filled as well.
 * Any additional columns after these 12 are also read by our parser, but simply ignored.
 *
 * Note that the BED format internally uses 0-based half-open intervals. That is, the start and end
 * coordinates `chromStart = 0` and `chromEnd = 100` define a region starting at the first base,
 * with a length of 100. We here however use 1-based closed intervals, and hence store the same
 * region as `1` and `100`, both in the Feature struct and in the GenomeRegionList.
 *
 * Furthermore, any lines starting with `browser`, `track`, or `#` are read, but currently
 * ignored. We are not quite sure if such lines are allowed in the middle of BED files by the
 * inofficial standard, hence we here also allow that. The obvious downside of this being the BED
 * specification is that chromosome names "browser" and "track" cannot be used.
 */
class BedReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    /**
     * @brief Store all values that can typically appear in the columns of a BED file.
     *
     * The order of fields in the struct is the same as the order of columns.
     * If the file does not have all the columns, the remaining values here are simply left empty
     * or at 0.
     */
    struct Feature
    {
        std::string chrom;
        size_t chrom_start;
        size_t chrom_end;
        std::string name;
        size_t score;
        char strand;
        size_t thick_start;
        size_t thick_end;
        std::string item_rgb;
        size_t block_count;
        std::vector<size_t> block_sizes;
        std::vector<size_t> block_starts;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    BedReader() = default;
    ~BedReader() = default;

    BedReader( BedReader const& ) = default;
    BedReader( BedReader&& )      = default;

    BedReader& operator= ( BedReader const& ) = default;
    BedReader& operator= ( BedReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read a BED input source, and return its content as a list of Feature structs.
     */
    std::vector<Feature> read(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * @brief Read an input source, and return its content as a GenomeLocusSet.
     *
     * This only uses the first three columns, `chrom`, `chromStart`, and `chromEnd`,
     * and ignores everything else.
     *
     * This is the recommended way to read an input for testing whether genome coordinates
     * are covered (filtered / to be considered) for downstream analyses.
     */
    GenomeLocusSet read_as_genome_locus_set(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * @brief Read a BED input source, and return its content as a GenomeRegionList.
     *
     * This only uses the first three columns, `chrom`, `chromStart`, and `chromEnd`,
     * and ignores everything else.
     *
     * If @p merge is set, the individual regions of the file are merged if they overlap.
     * This is more useful of the region list is used to determine coverage, and less useful if
     * regions are meant to indicate some specific parts of the genome, such as genes.
     * See the `overlap` flag of GenomeRegionList::add( GenomeLocus const&, bool ) for details.
     */
    GenomeRegionList read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        bool merge = false
    ) const;

    /**
     * @brief Read a BED input source, and add its content to an existing GenomeRegionList.
     *
     * @copydetails BedReader::read_as_genome_region_list( std::shared_ptr< genesis::utils::BaseInputSource >, bool ) const
     */
    void read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        GenomeRegionList& target,
        bool merge = false
    ) const;

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
     * columns.
     */
    size_t parse_line_(
        utils::InputStream& input_stream,
        Feature&            feature
    ) const;

    /**
     * @brief Go to the next column, skipping spaces and tabs.
     *
     * The function returns whether there is a next column (true), or whether the end of the line
     * or file was reached (false). If true, it also increments the @p found_columns counter,
     * which is used by parse_line_().
     */
    bool next_field_( utils::InputStream& input_stream, size_t& found_columns ) const;

    /**
     * @brief Parse a single string value, delimited from the next column by tabs or spaces.
     */
    std::string parse_string_( utils::InputStream& input_stream ) const;

};

} // namespace population
} // namespace genesis

#endif // include guard
