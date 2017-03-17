#ifndef GENESIS_TREE_TREE_EDGE_DATA_H_
#define GENESIS_TREE_TREE_EDGE_DATA_H_

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

#include <memory>

namespace genesis {
namespace tree {

// =================================================================================================
//     Tree Edge Data Base
// =================================================================================================

/**
 * @brief Base class for storing data on @link TreeEdge Edges@endlink of a Tree.
 *
 * This class provides the start point for the class hierarchy of edge data classes. In order to
 * correctly achieve polymorphic behaviour, and with its use case in a Tree in mind, this class and
 * its derived classes are supposed to be used via unique pointers to their instances. Although
 * this yields automatic memory management, it unfortunately disallows covariant return types.
 *
 * The class thus provides a static create() function instead of a default constructor. This needs
 * to be reimplemented in derived classes in order to allow creation of objects of the exact derived
 * type. For similar reasons, instead of a copy constructor, it provides a clone() function.
 *
 * Furthermore, the class provides a virtual recreate() function that default-constructs an object
 * of the same derived type as it was called on. This is allows to create new data instances for a
 * Tree without the need to know the exact derived type. For example, this can be used when adding
 * new Edges to a Tree, in order to fill them with default-constructed data of the same type of
 * some other Edges of the Tree.
 *
 * It is recommended that derived classes follow the same access rules for its constructors.
 * See DefaultEdgeData for an example.
 *
 * This class does not contain any data itself. See BaseNodeData for its "partner" class.
 */
class BaseEdgeData
{
    // -------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------

public:

    /**
     * @brief Virtual destructor. Needed for polymorphism.
     */
    virtual ~BaseEdgeData() = default;

    /**
     * @brief Deleted move constructor.
     *
     * Not viable with polymorphism. Also, we don't need it. The data is held by pointer (in order
     * to allow polymorphism in the first place), so we can simply move the pointer instead.
     */
    BaseEdgeData( BaseEdgeData&& ) = delete;

    /**
     * @brief Deleted move assignment.
     *
     * Not viable with polymorphism. Also, we don't need it. The data is held by pointer (in order
     * to allow polymorphism in the first place), so we can simply move the pointer instead.
     */
    BaseEdgeData& operator= ( BaseEdgeData&& ) = delete;

protected:

    /**
     * @brief Protected default constructor. Called from the create() function.
     */
    BaseEdgeData() = default;

    /**
     * @brief Protected copy constructor. Called from the clone() function.
     */
    BaseEdgeData( BaseEdgeData const& ) = default;

    /**
    * @brief Protected copy assignment. Not used right now.
    */
    BaseEdgeData& operator= ( BaseEdgeData const& ) = default;

public:

    /**
     * @brief Create a new instance of this class. Use instead of default constructor.
     */
    static std::unique_ptr< BaseEdgeData > create()
    {
        return std::unique_ptr< BaseEdgeData >( new BaseEdgeData() );
    };

    /**
     * @brief Polymorphically create a default-constructed instance of this class with the same
     * derived type as it was called on.
     */
    virtual std::unique_ptr< BaseEdgeData > recreate() const
    {
        return std::unique_ptr< BaseEdgeData >( new BaseEdgeData() );
    };

    /**
     * @brief Polymorphically copy an instance of this class. Use instead of copy constructor.
     */
    virtual std::unique_ptr< BaseEdgeData > clone() const
    {
        return std::unique_ptr< BaseEdgeData >( new BaseEdgeData( *this ));
    };

};

} // namespace tree
} // namespace genesis

#endif // include guard
