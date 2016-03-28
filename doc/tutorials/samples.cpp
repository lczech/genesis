#include <stdexcept>

#include "placement/function/functions.hpp"
#include "placement/io/jplace_reader.hpp"
#include "placement/io/jplace_writer.hpp"
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

    // Filter out all placements with a like_weight_ration below 0.5,
    // filter out all but the two most likely placements.
    filter_min_weight_threshold(    sample, 0.5 );
    filter_n_max_weight_placements( sample, 2   );

    // Write the results back to a file.
    JplaceWriter().to_file( sample, "path/to/new_file.jplace" );
}
