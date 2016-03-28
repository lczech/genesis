#ifndef GENESIS_TREE_IO_PHYLOXML_COLOR_WRITER_MIXIN_H_
#define GENESIS_TREE_IO_PHYLOXML_COLOR_WRITER_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "tree/io/color_writer_mixin.hpp"
#include "utils/io/xml/document.hpp"

#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Phyloxml Color Writer Mixin
// =================================================================================================

/**
 * @brief Mixin class for PhyloXML output that allows coloring of edges.
 *
 * The effect of this class on the PhyloXML output is that (if enabled) a color tag will be added
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
 * For more information, see ColorWriterMixin class.
 */
template <typename Base>
class PhyloxmlColorWriterMixin : public Base, public ColorWriterMixin
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
    //     Overridden Virtual Functions
    // -------------------------------------------------------------------------

protected:

    virtual void prepare_writing( TreeType const& tree, utils::XmlDocument& xml ) override
    {
        Base::prepare_writing(tree, xml);

        if (!ColorWriterMixin::enable_color()) {
            return;
        }

        // If an edge color vector was set, it needs to match the tree's edge count.
        if (
            ColorWriterMixin::edge_colors().size() > 0 &&
            ColorWriterMixin::edge_colors().size() != tree.edge_count()
        ) {
            throw std::length_error(
                "Color vector does not have as many elements as the tree has edges."
            );
        }
    }

    virtual void edge_to_element( EdgeType const& edge, utils::XmlElement& element ) override
    {
        Base::edge_to_element(edge, element);

        if (!ColorWriterMixin::enable_color()) {
            return;
        }

        // If an edge color vector was set, use it.
        if (ColorWriterMixin::edge_colors().size() > 0) {
            assert( edge.index() <= ColorWriterMixin::edge_colors().size() );
            set_color(element, ColorWriterMixin::edge_colors()[edge.index()]);
        }
    }

    // -------------------------------------------------------------------------
    //     Mixin Functions
    // -------------------------------------------------------------------------

protected:

    void set_color( utils::XmlElement& element, unsigned char r, unsigned char g, unsigned char b )
    {
        if( utils::Color(r, g, b) == ColorWriterMixin::ignored_color() ) {
            return;
        }

        // TODO do not create new element if there is already one!
        auto re = make_unique< utils::XmlElement >("red");
        re->append_markup(std::to_string(r));

        auto ge = make_unique< utils::XmlElement >("green");
        ge->append_markup(std::to_string(g));

        auto be = make_unique< utils::XmlElement >("blue");
        be->append_markup(std::to_string(b));

        auto color = make_unique< utils::XmlElement >("color");
        color->content.push_back(std::move(re));
        color->content.push_back(std::move(ge));
        color->content.push_back(std::move(be));

        element.content.push_back(std::move(color));
    }

    void set_color( utils::XmlElement& element, utils::Color color )
    {
        set_color( element, color.r(), color.g(), color.b() );
    }

};

} // namespace tree
} // namespace genesis

#endif // include guard
