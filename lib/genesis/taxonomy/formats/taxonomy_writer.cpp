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

#include "genesis/taxonomy/formats/taxonomy_writer.hpp"

#include "genesis/taxonomy/iterator/preorder.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxopath.hpp"

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/output_stream.hpp"
#include "genesis/utils/text/string.hpp"

#include <cassert>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Writing
// =================================================================================================

void TaxonomyWriter::to_stream( Taxonomy const& tax, std::ostream& os ) const
{
    for( auto const& tit : preorder( tax )) {
        os << tax_gen_.to_string( tit.taxon() );

        if( write_ids_ ) {
            os << "\t" << tit.taxon().id();
        }
        if( write_ranks_ ) {
            os << "\t" << tit.taxon().rank();
        }

        os << "\n";
    }
}

void TaxonomyWriter::to_file( Taxonomy const& tax, std::string const& filename ) const
{
    std::ofstream ofs;
    utils::file_output_stream( filename, ofs );
    to_stream( tax, ofs );
}

std::string TaxonomyWriter::to_string( Taxonomy const& tax ) const
{
    std::ostringstream oss;
    to_stream( tax, oss );
    return oss.str();
}

} // namespace taxonomy
} // namespace genesis
