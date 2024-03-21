#ifndef GENESIS_POPULATION_FORMAT_GFF_READER_H_
#define GENESIS_POPULATION_FORMAT_GFF_READER_H_

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

#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace population {

// =================================================================================================
//     GFF/GTF Reader
// =================================================================================================

/**
 * @brief Reader for GFF2 and GFF3 (General %Feature Format) and GTF (General Transfer Format) files.
 *
 * See https://uswest.ensembl.org/info/website/upload/gff.html for the format description.
 * Lines starting with `track ` or `browser ` (including a trailing white space) are ignored,
 * as are comment lines starting with `#` (or for that matter, `##` for directives),
 * and empty lines.
 *
 * We currently do not support the underlying ontology features, and simply store the ninth field
 * of the file as a string in Feature::attributes_group. This is also how we support all three
 * formats, GFF2, GFF3, and GTF in one parser: We simply ignore the parts that are different
 * between them. If need, this last field has to be parsed by the user.
 *
 * See also http://gmod.org/wiki/GFF2, http://gmod.org/wiki/GFF3, and
 * http://genome.ucsc.edu/FAQ/FAQformat.html#format3 for additional information,
 * and https://github.com/The-Sequence-Ontology/Specifications/blob/master/gff3.md for a thorough
 * specification of GFF3.
 */
class GffReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    // using Attribute = std::pair<std::string, std::string>;

    /**
     * @brief
     */
    struct Feature
    {
        std::string seqname;
        std::string source;
        std::string feature;
        size_t start;
        size_t end;
        double score;
        char strand;
        signed char frame;
        std::string attributes_group;
        // std::vector<Attribute> attributes;
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    GffReader() = default;
    ~GffReader() = default;

    GffReader( GffReader const& ) = default;
    GffReader( GffReader&& )      = default;

    GffReader& operator= ( GffReader const& ) = default;
    GffReader& operator= ( GffReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    /**
     * @brief Read a GFF2/GFF3/GTF input source, and return its content as a list of Feature structs.
     */
    std::vector<Feature> read( std::shared_ptr< utils::BaseInputSource > source ) const;

    /**
     * @brief Read an input source, and return its content as a GenomeLocusSet.
     *
     * This only uses the columns `seqname`, `start`, and `end`,
     * and ignores everything else.
     *
     * This is the recommended way to read an input for testing whether genome coordinates
     * are covered (filtered / to be considered) for downstream analyses.
     */
    GenomeLocusSet read_as_genome_locus_set(
        std::shared_ptr< utils::BaseInputSource > source
    ) const;

    /**
     * @brief Read a GFF2/GFF3/GTF input source, and return its content as a GenomeRegionList.
     *
     * This only uses the columns `seqname`, `start`, and `end`,
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
     * @brief Read a GFF2/GFF3/GTF input source, and add its content to an existing GenomeRegionList.
     *
     * @copydetails GffReader::read_as_genome_region_list( std::shared_ptr< genesis::utils::BaseInputSource >, bool ) const
     */
    void read_as_genome_region_list(
        std::shared_ptr< utils::BaseInputSource > source,
        GenomeRegionList& target,
        bool merge = false
    ) const;

    // -------------------------------------------------------------------------
    //     Parsing
    // -------------------------------------------------------------------------

    bool parse_line(
        utils::InputStream& input_stream,
        Feature&            feature
    ) const;

};

} // namespace population
} // namespace genesis

#endif // include guard
