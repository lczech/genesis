#ifndef GENESIS_POPULATION_FORMATS_GFF_READER_H_
#define GENESIS_POPULATION_FORMATS_GFF_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup population
 */

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
 * @brief Reader for GFF (General Feature Format) and GTF (General Transfer Format) files.
 *
 * See https://uswest.ensembl.org/info/website/upload/gff.html for the format description.
 *
 * See also http://gmod.org/wiki/GFF2 and http://gmod.org/wiki/GFF3 for additional information.
 */
class GffReader
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    using Attribute = std::pair<std::string, std::string>;

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
        std::vector<Attribute> attributes;
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

    std::vector<Feature> read( std::shared_ptr< utils::BaseInputSource > source ) const;

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
