#ifndef GENESIS_TREE_FORMATS_NEWICK_COLOR_WRITER_PLUGIN_H_
#define GENESIS_TREE_FORMATS_NEWICK_COLOR_WRITER_PLUGIN_H_

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

#include "genesis/tree/tree.hpp"
#include "genesis/tree/formats/color_writer_plugin.hpp"
#include "genesis/tree/formats/newick/element.hpp"
#include "genesis/tree/formats/newick/writer.hpp"

#include <assert.h>
#include <stdexcept>
#include <string>

namespace genesis {
namespace tree {

// =================================================================================================
//     Newick Color Writer Plugin
// =================================================================================================

/**
 * @brief Plugin class for Newick output that allows coloring of edges.
 *
 * The effect of this class on the Newick output is that (if enabled) a color tag comment will be
 * added to each Newick element like this: `[&!color=#%c0ffee]`.
 *
 * For more information, see ColorWriterPlugin class.
 */
class NewickColorWriterPlugin : public ColorWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    NewickColorWriterPlugin() = default;
    virtual ~NewickColorWriterPlugin() = default;

    NewickColorWriterPlugin(NewickColorWriterPlugin const&) = default;
    NewickColorWriterPlugin(NewickColorWriterPlugin&&)      = default;

    NewickColorWriterPlugin& operator= (NewickColorWriterPlugin const&) = default;
    NewickColorWriterPlugin& operator= (NewickColorWriterPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

    /**
     * @brief Set the prefix string that is used within the Newick comment before the actual
     * color value.
     *
     * By default, this string is set to `&!color=`, which is for example used by FigTree to mark
     * color in trees. This will result in a Newick comment like `[&!color=#%c0ffee]`.
     */
    void color_tag_prefix( std::string prefix )
    {
        color_tag_prefix_ = prefix;
    }

    /**
     * @brief Get the currently set prefix string. See the setter for more information.
     */
    std::string color_tag_prefix() const
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
    std::string color_tag_suffix() const
    {
        return color_tag_suffix_;
    }

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void prepare_writing( Tree const& tree, NewickBroker& broker ) const
    {
        (void) broker;

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

    void edge_to_element( TreeEdge const& edge, NewickBrokerElement& element ) const
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

    void register_with( NewickWriter& writer ) const
    {
        writer.prepare_writing_plugins.push_back(
            [&]( Tree const& tree, NewickBroker& broker ) {
                prepare_writing( tree, broker );
            }
        );
        writer.edge_to_element_plugins.push_back(
            [&]( TreeEdge const& edge, NewickBrokerElement& element ) {
                edge_to_element( edge, element );
            }
        );
    }

    // -------------------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------------------

private:

    void set_color_( NewickBrokerElement& element, unsigned char r, unsigned char g, unsigned char b ) const
    {
        set_color_( element, utils::Color(r, g, b) );
    }

    void set_color_( NewickBrokerElement& element, utils::Color color ) const
    {
        if(
            ColorWriterPlugin::use_ignored_color() &&
            color == ColorWriterPlugin::ignored_color()
        ) {
            return;
        }

        // TODO do not create new element if there is already one!
        element.comments.push_back( color_tag_prefix_ + color_to_hex(color) + color_tag_suffix_ );
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string color_tag_prefix_ = "&!color=";
    std::string color_tag_suffix_ = "";

};

} // namespace tree
} // namespace genesis

#endif // include guard
