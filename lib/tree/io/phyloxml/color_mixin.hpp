#ifndef GENESIS_TREE_PHYLOXML_COLOR_MIXIN_H_
#define GENESIS_TREE_PHYLOXML_COLOR_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/tools/color.hpp"

#include <assert.h>
#include <stdexcept>
#include <vector>

namespace genesis {

// =================================================================================================
//     Phyloxml Color Mixin
// =================================================================================================

/**
 * @brief Mixin class for PhyloXML output that allows coloring of edges.
 *
 * The effect of this class on the PhyloXML output is that (if activated) a color tag will be added
 * to each clade like this:
 *
 *     <clade>
 *         [other tags, like name or branch_length]
 *         <color>
 *             <red>0</red>
 *             <green>128</green>
 *             <blue>255</blue>
 *         </color>
 *     </clade>
 *
 * There are two ways this mixin can be used:
 *
 *   1. Use `edge_color_vector()` to set a vector of colors for each edge before calling the
 *      actual printing function. This can be done by client code that needs a particular coloring
 *      of the edges, but can also be used by superclasses to set all edge colors at once.
 *   2. Use `set_color()` in the superclass to set the color of individual edges. This is helpful
 *      within the `edge_to_element()` function of the superclass.
 *
 * If neither of this is done, no color tags will be written. Color tags can also be deactivated
 * by client code using the `write_color()` option.
 */
template <typename Base>
class PhyloxmlColorMixin : public Base
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
     * @brief Set the edge colors that shall be written to the PhyloXML output.
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
     * @brief Returns whether colors tags are written to the PhyloXML output.
     */
    bool write_color() const
    {
        return write_color_;
    }

    /**
     * @brief Set whether colors tags are written to the PhyloXML output.
     */
    void write_color(bool value)
    {
        write_color_ = value;
    }

    // -------------------------------------------------------------------------
    //     Overridden Virtual Functions
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( TreeType const& tree, XmlDocument& xml ) override
    {
        Base::prepare_writing(tree, xml);

        if (!write_color_) {
            return;
        }

        // If an edge color vector was set, it needs to match the tree's edge count.
        if (edge_colors_.size() > 0 && edge_colors_.size() != tree.edge_count()) {
            throw std::length_error(
                "Color vector does not have as many elements as the tree has edges."
            );
        }
    }

    virtual void edge_to_element( EdgeType const& edge, XmlElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (!write_color_) {
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

    void set_color( XmlElement& element, unsigned char r, unsigned char g, unsigned char b )
    {
        // TODO do not create new element if there is already one!
        auto re = make_unique<XmlElement>("red");
        re->append_markup(std::to_string(r));

        auto ge = make_unique<XmlElement>("green");
        ge->append_markup(std::to_string(g));

        auto be = make_unique<XmlElement>("blue");
        be->append_markup(std::to_string(b));

        auto color = make_unique<XmlElement>("color");
        color->content.push_back(std::move(re));
        color->content.push_back(std::move(ge));
        color->content.push_back(std::move(be));

        element.content.push_back(std::move(color));
    }

    void set_color( XmlElement& element, Color color )
    {
        set_color( element, color.r(), color.g(), color.b() );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<Color> edge_colors_;
    bool write_color_ = true;
};

} // namespace genesis

#endif // include guard
