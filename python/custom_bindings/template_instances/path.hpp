#include "genesis/tree/iterator/path.hpp"
#include "genesis/utils/containers/range.hpp"
#include "genesis/tree/tree/node.hpp"
#include "genesis/tree/tree/link.hpp"

void path_t1()
{
	::genesis::tree::TreeNode t1;
	::genesis::tree::TreeNode t2;
	::genesis::tree::path< ::genesis::tree::TreeNode >( t1, t2);

	::genesis::tree::TreeLink l1;
        ::genesis::tree::TreeLink l2;
        ::genesis::tree::path< ::genesis::tree::TreeLink >( l1, l2);
}
