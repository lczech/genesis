#ifndef GENESIS_TAXONOMY_FORMATS_NCBI_H_
#define GENESIS_TAXONOMY_FORMATS_NCBI_H_

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

#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/utils/formats/csv/reader.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <string>
#include <unordered_map>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     NCBI Taxonomy Reading
// =================================================================================================

// -------------------------------------------------------------------------
//     Data Structures
// -------------------------------------------------------------------------

struct NcbiNode
{
    unsigned long tax_id;
    unsigned long parent_tax_id;
    std::string rank;

    mutable Taxon* taxon = nullptr;
};

struct NcbiName
{
    unsigned long tax_id;
    std::string name;
    std::string name_class;
};

using NcbiNodeLookup = std::unordered_map<unsigned long, NcbiNode>;
using NcbiNameLookup = std::unordered_map<unsigned long, NcbiName>;

// -------------------------------------------------------------------------
//     Tables
// -------------------------------------------------------------------------

/**
 * @brief Parameters for reading the NCBI node and name tables.
 *
 * We need to extract fields from certain column positions in the table.
 * These should be stable across NCBI releases, but if needed, can be changed here.
 *
 * Furthermore, by default we only add the scientific names into our Taxonomy, and ignore entries
 * with alternative names and other types of classes. If instead the `name_class_filter` is set
 * to an emptry string here, all entries are added to the taxonomy.
 */
struct NcbiTableParameters
{
    size_t node_table_tax_id_pos        = 0;
    size_t node_table_parent_tax_id_pos = 1;
    size_t node_table_rank_pos          = 2;

    size_t name_table_tax_id_pos        = 0;
    size_t name_table_name_pos          = 1;
    size_t name_table_name_class_pos    = 3;

    std::string name_class_filter = "scientific name";
};

NcbiNodeLookup read_ncbi_node_table(
    std::shared_ptr<utils::BaseInputSource> source
);

NcbiNodeLookup read_ncbi_node_table(
    std::shared_ptr<utils::BaseInputSource> source,
    NcbiTableParameters const& params
);

NcbiNameLookup read_ncbi_name_table(
    std::shared_ptr<utils::BaseInputSource> source
);

NcbiNameLookup read_ncbi_name_table(
    std::shared_ptr<utils::BaseInputSource> source,
    NcbiTableParameters const& params
);

// -------------------------------------------------------------------------
//     Taxonomy
// -------------------------------------------------------------------------

Taxonomy convert_ncbi_tables(
    NcbiNodeLookup const& nodes,
    NcbiNameLookup const& names
);

Taxonomy read_ncbi_taxonomy(
    std::string const& node_file,
    std::string const& name_file
);

Taxonomy read_ncbi_taxonomy(
    std::string const& node_file,
    std::string const& name_file,
    NcbiTableParameters const& params
);

Taxonomy read_ncbi_taxonomy(
    std::shared_ptr<utils::BaseInputSource> node_source,
    std::shared_ptr<utils::BaseInputSource> name_source
);

Taxonomy read_ncbi_taxonomy(
    std::shared_ptr<utils::BaseInputSource> node_source,
    std::shared_ptr<utils::BaseInputSource> name_source,
    NcbiTableParameters const& params
);

} // namespace taxonomy
} // namespace genesis

#endif // include guard
