#ifndef GENESIS_PLACEMENT_H_
#define GENESIS_PLACEMENT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
 * @brief This header includes all headers in 'placement'.
 * This makes it easy to integrate the code as a library, as all
 * symbols of these headers are available after including this header.
 *
 * You do not need to edit this file manually. Simply run the script
 * make_genesis_header.sh in ./tools/deploy to update this file.
 */

#include <genesis/placement/format/edge_color.hpp>
#include <genesis/placement/format/jplace_reader.hpp>
#include <genesis/placement/format/jplace_writer.hpp>
#include <genesis/placement/format/newick_reader.hpp>
#include <genesis/placement/format/newick_writer.hpp>
#include <genesis/placement/format/serializer.hpp>
#include <genesis/placement/function/cog.hpp>
#include <genesis/placement/function/distance.hpp>
#include <genesis/placement/function/emd.hpp>
#include <genesis/placement/function/epca.hpp>
#include <genesis/placement/function/function.hpp>
#include <genesis/placement/function/helper.hpp>
#include <genesis/placement/function/manipulation.hpp>
#include <genesis/placement/function/mass.hpp>
#include <genesis/placement/function/measure.hpp>
#include <genesis/placement/function/nhd.hpp>
#include <genesis/placement/function/operator.hpp>
#include <genesis/placement/function/sample_set.hpp>
#include <genesis/placement/function/tree.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/pquery/plain.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/placement/simulator/distribution.hpp>
#include <genesis/placement/simulator/function.hpp>
#include <genesis/placement/simulator/simulator.hpp>

#endif // include guard
