#ifndef GENESIS_TREE_IO_NEWICK_COLOR_MIXIN_H_
#define GENESIS_TREE_IO_NEWICK_COLOR_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/color_mixin.hpp"
#include "tree/io/newick/element.hpp"

#include <assert.h>
#include <stdexcept>
#include <string>

namespace genesis {

// =================================================================================================
//     Newick Color Mixin
// =================================================================================================

/**
 * @brief Mixin class for Newick output that allows coloring of edges.
 *
 * The effect of this class on the Newick output is that (if enabled) a color tag comment will be
 * added to each Newick element like this: `[&!color=#c0ffee]`.
 *
 * For more information, see ColorMixin class.
 */
template <typename Base>
class NewickColorMixin : public Base, public ColorMixin
{
    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Set the prefix string that is used within the Newick comment before the actual
     * color value.
     *
     * By default, this string is set to `&!color=`, which is for example used by FigTree to mark
     * color in trees. This will result in a Newick comment like `[&!color=#c0ffee]`.
     */
    void color_tag_prefix( std::string prefix )
    {
        color_tag_prefix_ = prefix;
    }

    /**
     * @brief Get the currently set prefix string. See the setter for more information.
     */
    std::string color_tag_prefix()
    {
        return color_tag_prefix_;
    }

    /**
     * @brief Set the suffix string that is used within the Newick comment after the actual
     * color value.
     *
     * By default, this string is empty. See the setter color_tag_prefix() for more information.
     */
    void color_tag_suffix( std::string suffix )
    {
        color_tag_suffix_ = suffix;
    }

    /**
     * @brief Get the currently set suffix string. See the setter for more information.
     */
    std::string color_tag_suffix()
    {
        return color_tag_suffix_;
    }

    // -------------------------------------------------------------------------
    //     Overridden Member Functions
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( TreeType const& tree, NewickBroker& broker ) override
    {
        Base::prepare_writing(tree, broker);

        if (!ColorMixin::enable_color()) {
            return;
        }

        // If an edge color vector was set, it needs to match the tree's edge count.
        if (
            ColorMixin::edge_colors().size() > 0 &&
            ColorMixin::edge_colors().size() != tree.edge_count()
        ) {
            throw std::length_error(
                "Color vector does not have as many elements as the tree has edges."
            );
        }
    }

    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (!ColorMixin::enable_color()) {
            return;
        }

        // If an edge color vector was set, use it.
        if (ColorMixin::edge_colors().size() > 0) {
            assert( edge.index() <= ColorMixin::edge_colors().size() );
            set_color(element, ColorMixin::edge_colors()[edge.index()]);
        }
    }

    // -------------------------------------------------------------------------
    //     Mixin Functions
    // -------------------------------------------------------------------------

protected:

    void set_color( NewickBrokerElement& element, unsigned char r, unsigned char g, unsigned char b )
    {
        set_color( element, Color(r, g, b) );
    }

    void set_color( NewickBrokerElement& element, Color color )
    {
        if( color != ColorMixin::ignored_color() ) {
            // TODO do not create new element if there is already one!
            element.comments.push_back( color_tag_prefix_ + color_to_hex(color) + color_tag_suffix_ );
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string color_tag_prefix_ = "&!color=";
    std::string color_tag_suffix_ = "";

};

} // namespace genesis

#endif // include guard
