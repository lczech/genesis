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

#include "newick_lexer.hh"
#include "node.hh"

namespace genesis {

class Tree
{
public:
    Tree() {};
    ~Tree() {};

    void FromNewickFile (std::string fn);
    void ToNewickFile   (std::string fn);

    void FromNewickString (std::string tree);
    std::string ToNewickString ();
};

} // namespace genesis

#endif // include guard
