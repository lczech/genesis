#ifndef CUSTOM_BINDING_HELPER_INCLUDED_
#define CUSTOM_BINDING_HELPER_INCLUDED_

#include "genesis/placement/function/manipulation.hpp"

#include "genesis/placement/sample.hpp"
#include "genesis/placement/formats/jplace_reader.hpp"
#include "genesis/placement/function/helper.hpp"

#include "genesis/tree/function/manipulation.hpp"
#include "genesis/tree/common_tree/functions.hpp"
#include "genesis/tree/common_tree/newick_reader.hpp"
#include "genesis/tree/common_tree/tree.hpp"
#include "genesis/tree/formats/newick/reader.hpp"
#include "genesis/tree/iterator/levelorder.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/text/string.hpp"

#include <unordered_map>

inline void extend_helper( pybind11::module &handle)
{
	handle.def(
		"edge_num_to_edge_map",
		[](::genesis::placement::Sample const& smp)
		{
			std::string infile = "./../test/data/placement/unrooted.jplace";
		    ::genesis::placement::Sample sample = ::genesis::placement::JplaceReader().read( ::genesis::utils::from_file( infile ));
		    auto& tree = sample.tree();

			auto en_map = std::unordered_map<int, ::genesis::placement::PlacementTreeEdge*>();
		    for( auto& edge : tree.edges() ) {
		        auto const& edge_data = edge.data<::genesis::placement::PlacementEdgeData>();
		        assert( en_map.count( edge_data.edge_num() ) == 0);
		        en_map.insert( std::make_pair<int,::genesis::placement::PlacementTreeEdge*>(edge_data.edge_num(), &edge) );
		    }
		    return en_map;
		}
	);
}

#endif
