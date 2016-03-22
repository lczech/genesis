#ifndef GENESIS_TREE_TREE_SET_H_
#define GENESIS_TREE_TREE_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <ostream>
#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward declarations
// =================================================================================================

template <class NodeDataType, class EdgeDataType>
class  Tree;

// =================================================================================================
//     Tree Set
// =================================================================================================

template <class TreeType_>
class TreeSet
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs
    // -------------------------------------------------------------------------

    using TreeType       = TreeType_;

    /**
     * @brief Store a Tree together with a name for it.
     */
    struct NamedTree
    {
        std::string name;
        TreeType    tree;
    };

    using iterator       = typename std::vector<NamedTree>::iterator;
    using const_iterator = typename std::vector<NamedTree>::const_iterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    TreeSet()  = default;
    ~TreeSet() = default;

    TreeSet( TreeSet const& ) = default;
    TreeSet( TreeSet&& )      = default;

    TreeSet& operator= ( TreeSet const& ) = default;
    TreeSet& operator= ( TreeSet&& )      = default;

    void swap( TreeSet& other );

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void add( std::string const& name, TreeType const& tree );

    void remove_at( size_t index );
    void clear();

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    iterator       begin();
    const_iterator begin() const;

    iterator       end();
    const_iterator end() const;

          NamedTree& at ( size_t index );
    const NamedTree& at ( size_t index ) const;

          NamedTree& operator [] (const std::size_t index);
    const NamedTree& operator [] (const std::size_t index) const;

    bool   empty() const;
    size_t size()  const;

    // -------------------------------------------------------------------------
    //     Output
    // -------------------------------------------------------------------------

    /**
     * @brief Write a list of all names of the Tree%s in a TreeSet to a stream.
     */
    friend std::ostream& operator << ( std::ostream& out, TreeSet const& tset )
    {
        // If provided with the optional parameter `full`, also dump all Trees.
        // TODO this was meant for full output. turn it into a printer instead!
        bool full = false;

        for (auto& ct : tset) {
            out << ct.name << "\n";
            if (full) {
                out << ct.tree.dump() << "\n";
            }
        }
        return out;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // We use a vector of elements here, because we want to preserve the order in which
    // elements are inserted into the TreeMap. This is not the case with simple maps.
    std::vector<NamedTree> trees_;

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/tree_set.tpp"

#endif // include guard
