#include "genesis/tree/iterator/path_set.hpp"
#include "genesis/utils/containers/range.hpp"
#include "genesis/tree/tree/node.hpp"

void path_set_t1()
{
        ::genesis::tree::TreeNode t1;
        ::genesis::tree::TreeNode t2;
	::genesis::tree::TreeNode t3;
        ::genesis::tree::path_set< ::genesis::tree::TreeNode >( t1, t2, t3);
}
