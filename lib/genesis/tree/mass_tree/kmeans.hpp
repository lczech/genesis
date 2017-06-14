#ifndef GENESIS_TREE_MASS_TREE_KMEANS_H_
#define GENESIS_TREE_MASS_TREE_KMEANS_H_

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

#include "genesis/utils/math/kmeans.hpp"

namespace genesis {
namespace tree {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Tree;
class TreeNode;
class TreeEdge;

using MassTree = Tree;

// =================================================================================================
//     Mass Tree Kmeans
// =================================================================================================

class MassTreeKmeans
    : public utils::Kmeans< MassTree >
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Constants
    // -------------------------------------------------------------------------

    using Point = MassTree;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    MassTreeKmeans() = default;
    virtual ~MassTreeKmeans() = default;

    MassTreeKmeans( MassTreeKmeans const& ) = default;
    MassTreeKmeans( MassTreeKmeans&& )      = default;

    MassTreeKmeans& operator= ( MassTreeKmeans const& ) = default;
    MassTreeKmeans& operator= ( MassTreeKmeans&& )      = default;

    // -------------------------------------------------------------------------
    //     Settings
    // -------------------------------------------------------------------------

    size_t accumulate_centroid_masses() const;
    void accumulate_centroid_masses( size_t value );

    // -------------------------------------------------------------------------
    //     Default K-Means Functions
    // -------------------------------------------------------------------------

private:

    virtual void pre_loop_hook(
        std::vector<Point>  const& data,
        std::vector<size_t>&       assignments,
        std::vector<Point>&        centroids
    ) override;

    virtual bool data_validation( std::vector<Point> const& data ) const override;

    virtual void update_centroids(
        std::vector<Point>  const& data,
        std::vector<size_t> const& assignments,
        std::vector<Point>&        centroids
    ) override;

    virtual double distance( Point const& lhs, Point const& rhs ) const override;

    // -------------------------------------------------------------------------
    //     Data
    // -------------------------------------------------------------------------

    size_t accumulate_centroid_masses_ = 1;

};

} // namespace tree
} // namespace genesis

#endif // include guard
