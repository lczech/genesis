#ifndef GENESIS_TREE_PHYLOXML_COLOR_MIXIN_H_
#define GENESIS_TREE_PHYLOXML_COLOR_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/tools/color.hpp"

#include <stdexcept>
#include <vector>

namespace genesis {

// =================================================================================================
//     Phyloxml Color  Mixin
// =================================================================================================

/**
 * @brief
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
     * If this function is called, the edges in the output will be colored according to the values
     * given as a parameter. The vector needs to contain as many elements as the tree has edges.
     * The elements need to be indexed using the edge.index() value.
     */
    void set_edge_color_vector( std::vector<Color> const& color_vector )
    {
        edge_colors_ = color_vector;
    }

    // -------------------------------------------------------------------------
    //     Overridden Virtual Functions
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( TreeType const& tree, XmlDocument& xml ) override
    {
        Base::prepare_writing(tree, xml);

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

        // If an edge color vector was set, use it.
        if (edge_colors_.size() > 0) {
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
};

} // namespace genesis

#endif // include guard
