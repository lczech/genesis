#ifndef GENESIS_TREE_IO_NEWICK_COLOR_MIXIN_H_
#define GENESIS_TREE_IO_NEWICK_COLOR_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/newick/element.hpp"
#include "utils/tools/color.hpp"
#include "utils/tools/color/operators.hpp"

#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>

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
 * There are two ways this mixin can be used:
 *
 *   1. Use `edge_color_vector()` to set a vector of colors for each edge before calling the
 *      actual writing function. This can be done by client code that needs a particular coloring
 *      of the edges, but can also be used by superclasses to set all edge colors at once.
 *   2. Use `set_color()` in the superclass to set the color of individual edges. This is helpful
 *      within the `edge_to_element()` function of the superclass.
 *
 * If neither of this is done, no color tags will be written. Color tags can also be deactivated
 * by client code using the `enable_color()` option.
 *
 * For the root node, there is no color tag written, for those reasons:
 *
 *     * In unrooted trees, the color tags for the nodes next to the root will cover all edges
 *       anyway. In other words, there is no "root edge" that could get a color anyway.
 *     * In rooted trees, the root node has two edges. Those two combined edges are usually
 *       interpreted as the one edge where the root is located. So again, there are actual edges
 *       that can be colored - no need for an extra root color.
 *       Caveat: It is possible to give different colors to those edges. What that means is up to
 *       the user.
 * %
 */
template <typename Base>
class NewickColorMixin : public Base
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
     * @brief Set the edge colors that shall be written to the Newick output.
     *
     * If this function is called with a vector of size > 0, the edges in the output will be colored
     * according to the values given as a parameter. The vector then needs to contain as many
     * elements as the tree has edges. The elements need to be indexed using the edge.index() value.
     *
     * If this function is called with an empty vector, the color printing is reset to not
     * print the edge colors that might have been set before.
     */
    void edge_color_vector( std::vector<Color> const& color_vector )
    {
        edge_colors_ = color_vector;
    }

    /**
     * @brief Return the edge colors that are currently set.
     */
    std::vector<Color> edge_color_vector()
    {
        return edge_colors_;
    }

    /**
     * @brief Set whether colors tags are written to the Newick output.
     */
    void enable_color( bool value )
    {
        enable_color_ = value;
    }

    /**
     * @brief Returns whether colors tags are written to the Newick output.
     */
    bool enable_color() const
    {
        return enable_color_;
    }

    /**
     * @brief Set a color that is used as marker for partially disabling the output of color tags.
     *
     * All edges that have set a color equal to the ignored color will produce no color tag output.
     * This is thus something like "magic pink", where all parts of an image are rendered
     * transparent when originially colored in pink.
     *
     * By default, the ignored color is black (`Color(0, 0, 0)`). This way, all edges that have
     * black color will produce no color tag.
     */
    void ignored_color( Color value )
    {
        ignored_color_ = value;
    }

    /**
     * @brief Return the currently set ignored color. See the setter for more information.
     */
    Color ignored_color() const
    {
        return ignored_color_;
    }

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

        if (!enable_color_) {
            return;
        }

        // If an edge color vector was set, it needs to match the tree's edge count.
        if (edge_colors_.size() > 0 && edge_colors_.size() != tree.edge_count()) {
            throw std::length_error(
                "Color vector does not have as many elements as the tree has edges."
            );
        }
    }

    virtual void edge_to_element( EdgeType const& edge, NewickBrokerElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (!enable_color_) {
            return;
        }

        // If an edge color vector was set, use it.
        if (edge_colors_.size() > 0) {
            assert( edge.index() <= edge_colors_.size() );
            set_color(element, edge_colors_[edge.index()]);
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
        if( color != ignored_color_ ) {
            element.comments.push_back( color_tag_prefix_ + color_to_hex(color) + color_tag_suffix_ );
        }
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<Color> edge_colors_;
    std::string        color_tag_prefix_  = "&!color=";
    std::string        color_tag_suffix_  = "";
    bool               enable_color_  = true;
    Color              ignored_color_ = Color(0, 0, 0);

};

} // namespace genesis

#endif // include guard
