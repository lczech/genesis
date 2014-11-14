#ifndef GNS_TREE_NEWICKPARSER_H_
#define GNS_TREE_NEWICKPARSER_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <string>
#include <vector>

#include "tree/newick_lexer.hh"

namespace genesis {

// =============================================================================
//     NewickParserItem
// =============================================================================

struct NewickParserItem
{
public:
    NewickParserItem() : branch_length(0.0), depth(0), rank(0), is_leaf(false) {};

    /**
     * @brief Name of the node.
     *
     * In case it is a leaf, this is usually the name of the taxon represented by the node.
     * Internal nodes are names "Internal Node" in case no name is specified in the Newick format,
     * same applies to the (possibly virtual) root, which is named "Root Node" by default.
     */
    std::string name;

    /** @brief Branch length associated with the node, i.e. the branch leading to its parent. */
    double      branch_length;

    /** @brief Depth of the node in the tree, i.e. its distance from the root. */
    int         depth;

    /** @brief Rank of the node, i.e. how many children it has. */
    int         rank;

    /** @brief True if the node is a leaf/tip, false otherwise. */
    bool        is_leaf;

    /** @brief An arbitrary string that can be attached to a node in Newick format via "{}". */
    std::string tag;

    /** @brief An arbitrary string that can be attached to a node in Newick format via "[]". */
    std::string comment;
};

// =============================================================================
//     NewickParser
// =============================================================================

class NewickParser
{
public:
    typedef std::vector<NewickParserItem*> NewickParserItems;

    ~NewickParser();

    bool Process (const NewickLexer& lexer);
    std::string Dump();

    inline void clear()
    {
        for (NewickParserItem* item : items_) {
            delete item;
        }
        items_.clear();
    }

protected:
    inline void Init()
    {
        nodes_ = tips_ = 0;
        clear();
    }

private:
    int nodes_;
    int tips_;
    NewickParserItems items_;
};

} // namespace genesis

#endif // include guard
