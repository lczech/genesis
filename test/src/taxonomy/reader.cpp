/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * @ingroup test
 */

#include "common.hpp"

#include "lib/taxonomy/formats/taxonomy_reader.hpp"
#include "lib/taxonomy/taxon.hpp"
#include "lib/taxonomy/taxonomy.hpp"
#include "lib/taxonomy/functions.hpp"

#include <stdexcept>

using namespace genesis::taxonomy;

TEST( Taxonomy, Reader )
{
    // Skip test if no data availabe.
    NEEDS_TEST_DATA;

    Taxonomy tax;

    auto reader = TaxonomyReader();
    reader.from_file( "/home/lucas/Downloads/silva/tax_slv_ssu_123.1.txt", tax );

    // LOG_DBG << "tax size " << tax.size();
    // LOG_DBG << "tax tots " << total_taxa_count( tax );

    // LOG_DBG << tax;
}
