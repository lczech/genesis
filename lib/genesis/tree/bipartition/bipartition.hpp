#ifndef GENESIS_TREE_BIPARTITION_BIPARTITION_H_
#define GENESIS_TREE_BIPARTITION_BIPARTITION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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
#include "genesis/utils/math/bitvector.hpp"

namespace genesis {
namespace tree {

// =============================================================================
//     Bipartition
// =============================================================================

class Bipartition
{
public:

    // -------------------------------------------------------------
    //     Declarations and Constructor
    // -------------------------------------------------------------

    Bipartition() = default;

    Bipartition( TreeLink const& link, utils::Bitvector const& leaf_nodes )
        : link_( &link )
        , leaf_nodes_( leaf_nodes )
    {}

    ~Bipartition() = default;

    Bipartition( Bipartition const& ) = default;
    Bipartition( Bipartition&& )      = default;

    Bipartition& operator= ( Bipartition const& ) = default;
    Bipartition& operator= ( Bipartition&& )      = default;

    // -------------------------------------------------------------
    //     Member Functions
    // -------------------------------------------------------------

    TreeLink const& link() const
    {
        return *link_;
    }

    utils::Bitvector& bitvector()
    {
        return leaf_nodes_;
    }

    utils::Bitvector const& leaf_nodes() const
    {
        return leaf_nodes_;
    }

    void invert()
    {
        leaf_nodes_.negate();
        link_ = &link_->outer();
    }

    bool empty() const
    {
        return link_ == nullptr;
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

protected:

    TreeLink const*  link_ = nullptr;
    utils::Bitvector leaf_nodes_;

};

} // namespace tree
} // namespace genesis

#endif // include guard
