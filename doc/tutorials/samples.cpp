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

#include <stdexcept>

#include "placement/function/functions.hpp"
#include "placement/formats/jplace_reader.hpp"
#include "placement/formats/jplace_writer.hpp"
#include "placement/sample.hpp"

#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"

int main()
{
    using namespace genesis::placement;

    // Read a jplace file into a Sample object.
    Sample sample;
    JplaceReader().from_file( "path/to/file.jplace", sample );

    // Loop over the Pqueries of a Sample.
    for( auto const& pquery : sample ) {

        LOG_INFO << "Inspected pquery has:";

        // Output all placement positions of this Pquery.
        for( auto const& placement : pquery.placements() ) {
            LOG_INFO << "  Placement with like_weight_ratio " << placement.like_weight_ratio
                     << " at edge with index " << placement.edge().index();
        }

        // Output all names of this Pquery.
        for( auto const& pname : pquery.names() ) {
            LOG_INFO << "Name " << pname.name;
        }
    }

    // Add a new Pquery with a name and a placement.
    auto new_pquery = sample.add_pquery();
    new_pquery.add_name( "drosophila" );
    new_pquery.add_placement( sample.tree().edge_at( 0 ));

    // Find a Pquery given a name.
    auto found_pquery = find_pquery( sample, "drosophila" );
    if( found_pquery == nullptr ) {
        throw std::runtime_error( "Couldn't find Pquery." );
    }
    LOG_INFO << "Pquery contains " << found_pquery->placement_size() << " placements";

    // Filter out all placements with a like_weight_ratio below 0.5.
    filter_min_weight_threshold( sample, 0.5 );

    // Filter out all but the two most likely placements.
    filter_n_max_weight_placements( sample, 2   );

    // Keep enough placements to get an accumulated like_weight_ratio of 98%, filter out the rest.
    filter_min_accumulated_weight( sample, 0.98 );

    // Write the results back to a file.
    JplaceWriter().to_file( sample, "path/to/new_file.jplace" );
}
