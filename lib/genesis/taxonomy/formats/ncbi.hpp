#ifndef GENESIS_TAXONOMY_FORMATS_NCBI_H_
#define GENESIS_TAXONOMY_FORMATS_NCBI_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/formats/csv/reader.hpp"

#include <string>
#include <unordered_map>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     NCBI Reading stuff
// =================================================================================================

struct NcbiNode
{
    std::string tax_id;
    std::string parent_tax_id;
    std::string rank;

    mutable Taxon* taxon = nullptr;
};

struct NcbiName
{
    std::string tax_id;
    std::string name;
    std::string name_class;
};

using NcbiNodeLookup = std::unordered_map<std::string, NcbiNode>;
using NcbiNameLookup = std::unordered_map<std::string, NcbiName>;

NcbiNodeLookup convert_ncbi_node_table(
    utils::CsvReader::Table const& node_table,
    size_t tax_id_pos = 0,
    size_t parent_tax_id_pos = 1,
    size_t rank_pos = 2
);

NcbiNameLookup convert_ncbi_name_table(
    utils::CsvReader::Table const& name_table,
    size_t tax_id_pos = 0,
    size_t name_pos = 1,
    size_t name_class_pos = 3,
    std::string name_class_filter = "scientific name"
);

Taxonomy convert_ncbi_tables(
    NcbiNodeLookup const& nodes,
    NcbiNameLookup const& names
);

Taxonomy read_ncbi_taxonomy( std::string const& node_file, std::string const& name_file );

} // namespace taxonomy
} // namespace genesis

#endif // include guard
