#ifndef GENESIS_TREE_FORMATS_NEWICK_ELEMENT_H_
#define GENESIS_TREE_FORMATS_NEWICK_ELEMENT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup tree
 */

#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class NewickBroker;

// =================================================================================================
//     NewickBrokerElement
// =================================================================================================

/** @brief Store the information for one element of a Newick tree.
 *
 * Most of the class' members are public, as it is intended to serve an an intermediate data
 * exchange format, so different callers might need to modify its content.
 * However, this means paying attention when working with the data, as it can
 * be changed from anywhere.
 *
 * See NewickBroker class for a description of this intermediate format.
 */
struct NewickBrokerElement
{
    friend NewickBroker;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Constructor, initializes the item values.
     */
    NewickBrokerElement()
        : depth( -1 )
        , rank_( -1 )
    {}

    /**
     * @brief Constructor, initializes the item values and sets the depth.
     */
    explicit NewickBrokerElement( long depth )
        : depth( depth )
        , rank_( -1 )
    {}

    /**
     * @brief Constructor, initializes the item values and sets the name and the depth.
     */
    NewickBrokerElement( std::string const& name, long depth )
        : name( name )
        , depth( depth )
        , rank_( -1 )
    {}

    ~NewickBrokerElement() = default;

    NewickBrokerElement(NewickBrokerElement const&) = default;
    NewickBrokerElement(NewickBrokerElement&&)      = default;

    NewickBrokerElement& operator= (NewickBrokerElement const&) = default;
    NewickBrokerElement& operator= (NewickBrokerElement&&)      = default;

    // -------------------------------------------------------------------------
    //     Public Data Members
    // -------------------------------------------------------------------------

    /**
     * @brief Name of the node.
     *
     * In case it is a leaf, this is usually the name of the taxon represented by the node.
     * Internal nodes are named "Internal Node" in case no name is specified in the Newick format,
     * same applies to the (possibly virtual) root, which is named "Root Node" by default.
     */
    std::string name;

    /**
     * @brief Numerical values associated with the node, i.e. branch lengths.
     *
     * In cases wehre the values need to be interpreted as edge values, this is the edge leading to
     * this node's parent.
     */
    std::vector<std::string> values;

    /**
     * @brief Arbitrary strings that can be attached to a node, e.g. in Newick format via "{}".
     */
    std::vector<std::string> tags;

    /**
     * @brief Arbitrary strings that can be attached to a node, e.g. in Newick format via "[]".
     */
    std::vector<std::string> comments;

    /**
     * @brief Depth of the node in the tree, i.e. its distance from the root.
     */
    long depth;

    /**
     * @brief Identifier for the element.
     *
     * This is an arbitrary number that can be assigned to the element, as a helper to keep track
     * of elements throughout their processing. This can be used for instance to create a NewickBroker
     * with elements first, and then find them later in the NewickReader when turning the broker
     * into a tree, by for instance using the identifier in the NewickReader::element_to_node_function
     *
     * By default, it is initialized to max size_t, to make sure that any unset values are obvious.
     */
    size_t identifier = std::numeric_limits<size_t>::max();

    // -------------------------------------------------------------------------
    //     Additional Members
    // -------------------------------------------------------------------------

    /**
     * @brief Returns the rank (number of immediate children) of this node.
     *
     * NewickBroker::assign_ranks() has to be called before using this function. Otherwise,
     * this function will throw an std::logic_error.
     */
    long rank() const
    {
        if (rank_ < 0) {
            throw std::logic_error("NewickBroker::assign_ranks() was not called before.");
        }
        return rank_;
    }

    /**
     * @brief Return whether this is the root node of the tree.
     */
    bool is_root() const
    {
        return depth == 0;
    }

    /**
     * @brief Return whether this is a leaf node.
     */
    bool is_leaf() const
    {
        if (rank_ < 0) {
            throw std::logic_error("NewickBroker::is_leaf() was not called before.");
        }
        return rank_ == 0;
    }

        /**
         * @brief Return whether this is an inner node, i.e., not a leaf node.
         */
    bool is_inner() const
    {
        if (rank_ < 0) {
            throw std::logic_error("NewickBroker::is_leaf() was not called before.");
        }
        return rank_ != 0;
    }

private:

    /**
     * @brief Rank of the node, i.e. how many children it has.
     */
    mutable long rank_;
};

} // namespace tree
} // namespace genesis

#endif // include guard
