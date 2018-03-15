#ifndef GENESIS_PLACEMENT_H_
#define GENESIS_PLACEMENT_H_

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
 * @brief This header includes all headers in 'placement'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include "genesis/placement/formats/edge_color.hpp"
#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/formats/jplace_writer.hpp"
#include "genesis/placement/formats/newick_reader.hpp"
#include "genesis/placement/formats/newick_writer.hpp"
#include "genesis/placement/formats/serializer.hpp"
#include "genesis/placement/function/cog.hpp"
#include "genesis/placement/function/distances.hpp"
#include "genesis/placement/function/emd.hpp"
#include "genesis/placement/function/epca.hpp"
#include "genesis/placement/function/functions.hpp"
#include "genesis/placement/function/helper.hpp"
#include "genesis/placement/function/measures.hpp"
#include "genesis/placement/function/nhd.hpp"
#include "genesis/placement/function/operators.hpp"
#include "genesis/placement/function/sample_set.hpp"
#include "genesis/placement/function/tree.hpp"
#include "genesis/placement/placement_tree.hpp"
#include "genesis/placement/pquery.hpp"
#include "genesis/placement/pquery/name.hpp"
#include "genesis/placement/pquery/placement.hpp"
#include "genesis/placement/pquery/plain.hpp"
#include "genesis/placement/sample.hpp"
#include "genesis/placement/sample_set.hpp"
#include "genesis/placement/simulator/distributions.hpp"
#include "genesis/placement/simulator/functions.hpp"
#include "genesis/placement/simulator/simulator.hpp"

#endif // include guard
