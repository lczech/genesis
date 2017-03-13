#ifndef GENESIS_TREE_FORMATS_PHYLOXML_COLOR_WRITER_PLUGIN_H_
#define GENESIS_TREE_FORMATS_PHYLOXML_COLOR_WRITER_PLUGIN_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include "genesis/tree/formats/color_writer_plugin.hpp"
#include "genesis/tree/formats/phyloxml/writer.hpp"
#include "genesis/tree/tree.hpp"
#include "genesis/utils/formats/xml/document.hpp"

#include <assert.h>
#include <stdexcept>

namespace genesis {
namespace tree {

// =================================================================================================
//     Phyloxml Color Writer Plugin
// =================================================================================================

/**
 * @brief Plugin class for PhyloXML output that allows coloring of edges.
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
 * For more information, see ColorWriterPlugin class.
 */
class PhyloxmlColorWriterPlugin : public ColorWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    PhyloxmlColorWriterPlugin() = default;
    virtual ~PhyloxmlColorWriterPlugin() = default;

    PhyloxmlColorWriterPlugin(PhyloxmlColorWriterPlugin const&) = default;
    PhyloxmlColorWriterPlugin(PhyloxmlColorWriterPlugin&&)      = default;

    PhyloxmlColorWriterPlugin& operator= (PhyloxmlColorWriterPlugin const&) = default;
    PhyloxmlColorWriterPlugin& operator= (PhyloxmlColorWriterPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //      Plugin Functions
    // -------------------------------------------------------------------------

    void prepare_writing( Tree const& tree, utils::XmlDocument& xml ) const
    {
        (void) xml;

        if (!ColorWriterPlugin::enable_color()) {
            return;
        }

        // If an edge color vector was set, it needs to match the tree's edge count.
        if (
            ColorWriterPlugin::edge_colors().size() > 0 &&
            ColorWriterPlugin::edge_colors().size() != tree.edge_count()
        ) {
            throw std::length_error(
                "Color vector does not have as many elements as the tree has edges."
            );
        }
    }

    void edge_to_element( TreeEdge const& edge, utils::XmlElement& element ) const
    {
        if (!ColorWriterPlugin::enable_color()) {
            return;
        }

        // If an edge color vector was set, use it.
        if (ColorWriterPlugin::edge_colors().size() > 0) {
            assert( edge.index() <= ColorWriterPlugin::edge_colors().size() );
            set_color_(element, ColorWriterPlugin::edge_colors()[edge.index()]);
        }
    }

    void register_with( PhyloxmlWriter& writer ) const
    {
        writer.prepare_writing_plugins.push_back(
            [&]( Tree const& tree, utils::XmlDocument& xml ) {
                prepare_writing( tree, xml );
            }
        );
        writer.edge_to_element_plugins.push_back(
            [&]( TreeEdge const& edge, utils::XmlElement& element ) {
                edge_to_element( edge, element );
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

private:

    void set_color_( utils::XmlElement& element, unsigned char r, unsigned char g, unsigned char b ) const
    {
        if(
            ColorWriterPlugin::use_ignored_color() &&
            utils::Color(r, g, b) == ColorWriterPlugin::ignored_color()
        ) {
            return;
        }

        // TODO do not create new element if there is already one!
        auto re = utils::make_unique< utils::XmlElement >("red");
        re->append_markup(std::to_string(r));

        auto ge = utils::make_unique< utils::XmlElement >("green");
        ge->append_markup(std::to_string(g));

        auto be = utils::make_unique< utils::XmlElement >("blue");
        be->append_markup(std::to_string(b));

        auto color = utils::make_unique< utils::XmlElement >("color");
        color->content.push_back(std::move(re));
        color->content.push_back(std::move(ge));
        color->content.push_back(std::move(be));

        element.content.push_back(std::move(color));
    }

    void set_color_( utils::XmlElement& element, utils::Color color ) const
    {
        set_color_( element, color.r(), color.g(), color.b() );
    }

};

} // namespace tree
} // namespace genesis

#endif // include guard
