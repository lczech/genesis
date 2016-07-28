#ifndef GENESIS_TREE_TREE_NODE_DATA_H_
#define GENESIS_TREE_TREE_NODE_DATA_H_

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

#include <memory>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Node Data Base
// =================================================================================================

/**
 * @brief Base class for storing data on @link TreeNode Nodes@endlink of a Tree.
 *
 * This class merely provides the start point for the class hierarchy of node data classes.
 * In order to correctly achieve polymorphic behaviour, and with its use case in a Tree in mind,
 * this class and its derived classes are only usable via unique pointers to their instances.
 *
 * It thus provides a create() function instead of a default constructor.
 * Stack copies of this class are useless; we always want a pointer to the class.
 * For the same reason, instead of a copy constructor, it provides a clone() function.
 *
 * It is recommended that derived classes follow the same access rules for its constructors.
 * See DefaultNodeData for an example.
 *
 * This class does not contain any data itself. See BaseEdgeData for its "partner" class.
 */
class BaseNodeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    /**
     * @brief Virtual destructor. Needed for polymorphism.
     */
    virtual ~BaseNodeData() = default;

    /**
     * @brief Deleted move constructor. Not viable with polymorphism.
     */
    BaseNodeData( BaseNodeData&& other ) = delete;

    /**
    * @brief Deleted move assignment. Not viable with polymorphism.
    */
    BaseNodeData& operator= ( BaseNodeData&& other ) = delete;

protected:

    /**
     * @brief Protected default constructor. Called from the create() function.
     */
    BaseNodeData() = default;

    /**
     * @brief Protected copy constructor. Called from the clone() function.
     */
    BaseNodeData( BaseNodeData const& other ) = default;

    /**
    * @brief Protected move constructor. Not used right now.
    */
    BaseNodeData& operator= ( BaseNodeData const& other ) = default;

public:

    /**
     * @brief Create a new instance of this class. Use instead of default constructor.
     */
    static std::unique_ptr< BaseNodeData > create()
    {
        return std::unique_ptr< BaseNodeData >( new BaseNodeData() );
    };

    /**
     * @brief Polymorphically copy an instance of this class. Use instead of copy constructor.
     */
    virtual std::unique_ptr< BaseNodeData > clone() const
    {
        return std::unique_ptr< BaseNodeData >( new BaseNodeData( *this ));
    };

};

} // namespace tree
} // namespace genesis

#endif // include guard
