#ifndef GNS_TREE_SIMPLETREE_H_
#define GNS_TREE_SIMPLETREE_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

 #include <string>
 #include <vector>

 #include "tree.hh"
 #include "simple_node.hh"

namespace genesis {

class SimpleTree : public Tree
{
    public:
        SimpleTree() {};
        ~SimpleTree() {};

        void AddChild (Node child, double branch_length);

    protected:
        std::vector<SimpleNode> nodes_;
};

} // namespace genesis

#endif // include guard
