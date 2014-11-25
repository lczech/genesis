#ifndef GNS_TREE_TREE_H_
#define GNS_TREE_TREE_H_

/**
 * @brief Provides the basic interface for phylogenetic trees.
 *
 * For more information, see Tree class.
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

#include "tree/tree_branch.hh"
#include "tree/tree_link.hh"
#include "tree/tree_node.hh"

namespace genesis {

class Tree
{
public:
    Tree() {};
    virtual ~Tree() {};
    void clear();

    void FromNewickFile (const std::string& fn);
    void ToNewickFile   (const std::string& fn);

    void FromNewickString (const std::string& tree);
    std::string ToNewickString ();

protected:
    std::vector<TreeBranch*> branches_;
    std::vector<TreeLink*>   links_;
    std::vector<TreeNode*>   nodes_;

    void Init(const int leaf_count);
};

} // namespace genesis

#endif // include guard
