#ifndef GENESIS_PLACEMENT_FORMATS_PHYLOXML_WRITER_H_
#define GENESIS_PLACEMENT_FORMATS_PHYLOXML_WRITER_H_

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
 * @ingroup placement
 */

#include "placement/placement_tree.hpp"
#include "tree/default/phyloxml_writer.hpp"
#include "tree/formats/phyloxml/writer.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Placement Tree Phyloxml Writer
// =================================================================================================

typedef tree::DefaultTreePhyloxmlWriterMixin< tree::PhyloxmlWriter >
    PlacementTreePhyloxmlWriter;

} // namespace placement
} // namespace genesis

#endif // include guard
