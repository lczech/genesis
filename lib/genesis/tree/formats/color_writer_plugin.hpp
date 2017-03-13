#ifndef GENESIS_TREE_FORMATS_COLOR_WRITER_PLUGIN_H_
#define GENESIS_TREE_FORMATS_COLOR_WRITER_PLUGIN_H_

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

#include "genesis/utils/tools/color.hpp"
#include "genesis/utils/tools/color/operators.hpp"

#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Color Writer Plugin Base
// =================================================================================================

/**
 * @brief Base class for creating plugin classes that allow coloring of Tree edges.
 *
 * The effect of this class on the Tree output is that (if enabled) a color tag will be added to
 * each edge.
 *
 * There are two ways this plugin can be used:
 *
 *   1. Use `edge_color_vector()` to set a vector of colors for each edge before calling the
 *      actual writing function. This can be done by client code that needs a particular coloring
 *      of the edges, but can also be used by superclasses to set all edge colors at once.
 *   2. Use `set_color()` in the superclass to set the color of individual edges. This is helpful
 *      within the `edge_to_element()` function of the superclass. This function is however not
 *      implemented in this base class, as it needs knowledge of the actual type of Tree output to
 *      work. Thus, this is a function that the plugin classes should provide.
 *
 * If neither of this is done, no color tags will be written. Color tags can also be deactivated
 * by client code using the `enable_color()` option.
 *
 * Furthermore, with the ignored_color option, by default, all edged that are colored black
 * (`Color(0, 0, 0)`) are ignored in the output, i.e., they produce no color tag. See ignored_color
 * to set the value to a different color.
 *
 * For the root node, there is no color tag written, for those reasons:
 *
 *   * In unrooted trees, the color tags for the nodes next to the root will cover all edges
 *     anyway. In other words, there is no "root edge" that could get a color.
 *   * In rooted trees, the root node has two edges. Those two combined edges are usually
 *     interpreted as the one edge where the root is located. So again, there are actual edges
 *     that can be colored - no need for an extra root color.
 *     Caveat: It is possible to give different colors to those edges. What that means is up to
 *     the user.
 *
 * This class is intended to be used as base for plugin classes that do actual tree output.
 */
class ColorWriterPlugin
{
    // -------------------------------------------------------------------------
    //     Base Class
    // -------------------------------------------------------------------------

public:

    virtual ~ColorWriterPlugin() {}

    // -------------------------------------------------------------------------
    //     Properties
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Set the edge colors that shall be written to the output.
     *
     * If this function is called with a vector of size > 0, the edges in the output will be colored
     * according to the values given as a parameter. The vector then needs to contain as many
     * elements as the tree has edges. The elements need to be indexed using the edge.index() value.
     *
     * If this function is called with an empty vector, the color printing is reset to not
     * print the edge colors that might have been set before.
     */
    void edge_colors( std::vector<utils::Color> const& color_vector )
    {
        edge_colors_ = color_vector;
    }

    /**
     * @brief Return the edge colors that are currently set.
     */
    std::vector<utils::Color> const& edge_colors() const
    {
        return edge_colors_;
    }

    /**
     * @brief Set whether colors tags are written to the output.
     */
    void enable_color( bool value )
    {
        enable_color_ = value;
    }

    /**
     * @brief Returns whether colors tags are written to the output.
     */
    bool enable_color() const
    {
        return enable_color_;
    }

    /**
     * @brief Set a color that is used as marker for partially disabling the output of color tags.
     *
     * All edges that have set a color equal to the ignored color will produce no color tag output.
     * This is thus something like [magic pink](https://en.wikipedia.org/wiki/Magic_Pink), where
     * all parts of an image are rendered transparent when originially colored in pink.
     *
     * By default, we do not use an ignored color. If this option is activated, also
     * use_ignored_color() is set to `true`.
     */
    void ignored_color( utils::Color value )
    {
        ignored_color_ = value;
        use_ignored_color_ = true;
    }

    /**
     * @brief Return the currently set ignored color. See the setter for more information.
     */
    utils::Color ignored_color() const
    {
        return ignored_color_;
    }

    /**
     * @brief Set whether to use the ignored_color().
     *
     * If this option is set to `true`, the color that is set via ignored_color() is not written
     * as a color attribute to the output format.
     */
    void use_ignored_color( bool value )
    {
        use_ignored_color_ = value;
    }

    /**
     * @brief Return whether currently an ignored color is used.
     */
    bool use_ignored_color() const
    {
        return use_ignored_color_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<utils::Color> edge_colors_;
    bool                      enable_color_  = true;
    utils::Color              ignored_color_ = utils::Color(0, 0, 0);
    bool                      use_ignored_color_ = false;

};

} // namespace tree
} // namespace genesis

#endif // include guard
