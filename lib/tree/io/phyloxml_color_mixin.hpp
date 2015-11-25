#ifndef GENESIS_TREE_PHYLOXML_COLOR_MIXIN_H_
#define GENESIS_TREE_PHYLOXML_COLOR_MIXIN_H_

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "utils/tools/color.hpp"

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
protected:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    typedef typename Base::TreeType TreeType;
    typedef typename Base::NodeType NodeType;
    typedef typename Base::EdgeType EdgeType;
    typedef typename Base::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Mixin Functions
    // -------------------------------------------------------------------------

    void set_color (XmlElement& element, unsigned char r, unsigned char g, unsigned char b)
    {
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

    void set_color (XmlElement& element, Color color)
    {
        set_color (element, color.r(), color.g(), color.b());
    }

};

} // namespace genesis

#endif // include guard
