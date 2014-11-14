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
    // NewickParser needs to set the values on the fly
    friend class NewickParser;

public:
    /** @brief Constructor, initializes the item values. */
    NewickParserItem() : branch_length_(0.0), depth_(0), rank_(0), is_leaf_(false) {};

    /**
     * @brief Name of the node.
     *
     * In case it is a leaf, this is usually the name of the taxon represented by the node.
     * Internal nodes are names "Internal Node" in case no name is specified in the Newick format,
     * same applies to the (possibly virtual) root, which is named "Root Node" by default.
     */
    inline std::string name() {return name_;};

    /** @brief Branch length associated with the node, i.e. the branch leading to its parent. */
    inline double branch_length() {return branch_length_;};

    /** @brief Depth of the node in the tree, i.e. its distance from the root. */
    inline int depth() {return depth_;};

    /** @brief Rank of the node, i.e. how many children it has. */
    inline int rank() {return rank_;};

    /** @brief True if the node is a leaf/tip, false otherwise. */
    inline bool is_leaf() {return is_leaf_;};

    /** @brief An arbitrary string that can be attached to a node in Newick format via "{}". */
    inline std::string tag() {return tag_;};

    /** @brief An arbitrary string that can be attached to a node in Newick format via "[]". */
    inline std::string comment() {return comment_;};

protected:
    // storage, can be modified from within NewickParser (friend)
    std::string name_;
    double      branch_length_;
    int         depth_;
    int         rank_;
    bool        is_leaf_;
    std::string tag_;
    std::string comment_;
};

// =============================================================================
//     NewickParser
// =============================================================================

class NewickParser
{
public:
    typedef std::vector<NewickParserItem*> NewickParserItems;

    ~NewickParser();

    bool Process       (const NewickLexer& lexer);
    bool MakeParseTree (const NewickLexer& lexer);
    std::string Dump();

    inline int nodes() {return nodes_;};
    inline int leaves() {return leaves_;};

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
        nodes_ = leaves_ = 0;
        clear();
    }

private:
    int nodes_;
    int leaves_;
    NewickParserItems items_;
};

} // namespace genesis

#endif // include guard
