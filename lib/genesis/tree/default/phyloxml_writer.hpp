#ifndef GENESIS_TREE_DEFAULT_PHYLOXML_WRITER_H_
#define GENESIS_TREE_DEFAULT_PHYLOXML_WRITER_H_

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

#include "genesis/tree/default/tree.hpp"
#include "genesis/tree/formats/phyloxml/writer.hpp"
#include "genesis/utils/formats/xml/document.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Default Tree Phyloxml Writer Plugin
// =================================================================================================

/**
 * @brief
 */
class DefaultTreePhyloxmlWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    DefaultTreePhyloxmlWriterPlugin() = default;
    virtual ~DefaultTreePhyloxmlWriterPlugin() = default;

    DefaultTreePhyloxmlWriterPlugin(DefaultTreePhyloxmlWriterPlugin const&) = default;
    DefaultTreePhyloxmlWriterPlugin(DefaultTreePhyloxmlWriterPlugin&&)      = default;

    DefaultTreePhyloxmlWriterPlugin& operator= (DefaultTreePhyloxmlWriterPlugin const&) = default;
    DefaultTreePhyloxmlWriterPlugin& operator= (DefaultTreePhyloxmlWriterPlugin&&)      = default;

    // -------------------------------------------------------------------------
    //     Plugin Functions
    // -------------------------------------------------------------------------

    void node_to_element( TreeNode const& node, utils::XmlElement& element ) const
    {
        set_name_(element, node.data<DefaultNodeData>().name);
    }

    void edge_to_element( TreeEdge const& edge, utils::XmlElement& element ) const
    {
        set_branch_length_(element, edge.data<DefaultEdgeData>().branch_length);
    }

    void register_with( PhyloxmlWriter& writer ) const
    {
        writer.node_to_element_plugins.push_back(
            [&]( TreeNode const& node, utils::XmlElement& element ) {
                node_to_element( node, element );
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

    void set_name_( utils::XmlElement& element, const std::string& name ) const
    {
        // TODO do not create new element if there is already one!
        auto name_e = utils::make_unique< utils::XmlElement >( "name" );
        name_e->append_markup(name);
        element.content.push_back(std::move(name_e));
    }

    void set_branch_length_( utils::XmlElement& element, double length ) const
    {
        // TODO do not create new element if there is already one!
        auto bl_e = utils::make_unique< utils::XmlElement >( "branch_length" );
        bl_e->append_markup(std::to_string(length));
        element.content.push_back(std::move(bl_e));
    }

};

// =================================================================================================
//     Default Tree Phyloxml Writer
// =================================================================================================

class DefaultTreePhyloxmlWriter
    : public PhyloxmlWriter
    , public DefaultTreePhyloxmlWriterPlugin
{
public:

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    DefaultTreePhyloxmlWriter()
    {
        register_with( *this );
    }
};

} // namespace tree
} // namespace genesis

#endif // include guard
