#ifndef GENESIS_TREE_FORMATS_PHYLOXML_WRITER_H_
#define GENESIS_TREE_FORMATS_PHYLOXML_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

#include <string>

namespace genesis {

// =================================================================================================
//     Forward declarations
// =================================================================================================

namespace utils {
    class XmlDocument;
    class XmlElement;
}

// =================================================================================================
//     Phyloxml Writer
// =================================================================================================

namespace tree {

template <typename TreeType_>
class PhyloxmlWriter
{

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

public:

    typedef TreeType_ TreeType;
    typedef typename TreeType::NodeType NodeType;
    typedef typename TreeType::EdgeType EdgeType;
    typedef typename TreeType::LinkType LinkType;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    virtual ~PhyloxmlWriter() {}

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

protected:

    // virtual void prepare_reading( XmlDocument const& xml, TreeType& tree );
    // virtual void element_to_node( XmlElement const& element, NodeType& edge );
    // virtual void element_to_edge( XmlElement const& element, EdgeType& node );
    // virtual void finish_reading( XmlDocument const& xml, TreeType& tree );

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

public:

    void        to_file     (const TreeType& tree, const std::string filename);
    void        to_string   (const TreeType& tree, std::string& ts);
    std::string to_string   (const TreeType& tree);
    void        to_document (const TreeType& tree, utils::XmlDocument& xml);

protected:

    virtual void prepare_writing( TreeType const& tree, utils::XmlDocument& xml );
    virtual void node_to_element( NodeType const& node, utils::XmlElement&  element );
    virtual void edge_to_element( EdgeType const& edge, utils::XmlElement&  element );
    virtual void finish_writing(  TreeType const& tree, utils::XmlDocument& xml );

};

} // namespace tree
} // namespace genesis

// =================================================================================================
//     Inclusion of the implementation
// =================================================================================================

// This is a class template, so do the inclusion here.
#include "tree/formats/phyloxml/writer.tpp"

#endif // include guard
