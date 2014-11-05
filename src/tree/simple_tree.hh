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

        void ReadNewickFile  (std::string fn);
        void WriteNewickFile (std::string fn);

        void ParseNewickString (std::string tree);
        std::string WriteNewickString ();

    protected:
        std::vector<SimpleNode> nodes_;
};

} // namespace genesis

#endif // include guard
