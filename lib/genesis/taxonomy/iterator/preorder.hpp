#ifndef GENESIS_TAXONOMY_ITERATOR_PREORDER_H_
#define GENESIS_TAXONOMY_ITERATOR_PREORDER_H_

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
 * @ingroup taxonomy
 */

#include "genesis/taxonomy/taxonomy.hpp"
#include "genesis/taxonomy/taxon.hpp"
#include "genesis/utils/core/range.hpp"

#include <assert.h>
#include <functional>
#include <iterator>
#include <stack>

namespace genesis {
namespace taxonomy {

// =================================================================================================
//     Preorder For Each
// =================================================================================================

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in preorder.
 *
 * The given Taxonomy is traversed in preorder (i.e., a variant of depth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 *
 * This is the non-const version of the function.
 */
inline void preorder_for_each(
    Taxonomy& tax,
    std::function< void( Taxon& )> fn,
    bool include_inner_taxa = true
) {
    for( auto& t : tax ) {
        if( include_inner_taxa || t.size() == 0 ) {
            fn( t );
        }

        preorder_for_each( t, fn, include_inner_taxa );
    }
}

/**
 * @brief Apply a function to all taxa of the Taxonomy, traversing it in preorder.
 *
 * The given Taxonomy is traversed in preorder (i.e., a variant of depth-first search). If
 * `include_inner_taxa` is set to true (default), the provided functional is called for all
 * @link Taxon Taxa @endlink. Otherwise, the functional is only called for the taxa of lowest rank,
 * that is, for each Taxon that does not have sub-taxa.
 *
 * This is the const version of the function.
 */
inline void preorder_for_each(
    Taxonomy const& tax,
    std::function< void( Taxon const& )> fn,
    bool include_inner_taxa = true
) {
    for( auto const& t : tax ) {
        if( include_inner_taxa || t.size() == 0 ) {
            fn( t );
        }

        preorder_for_each( t, fn, include_inner_taxa );
    }
}

// =================================================================================================
//     Preorder Iterator
// =================================================================================================

template <typename TaxonomyType, typename TaxonType>
class IteratorPreorder
{

public:

    // -----------------------------------------------------
    //     Typedefs
    // -----------------------------------------------------

    using iterator_category = std::forward_iterator_tag;
    using self_type         = IteratorPreorder<TaxonomyType, TaxonType>;

    // -----------------------------------------------------
    //     Constructors and Rule of Five
    // -----------------------------------------------------

    IteratorPreorder ()
    {}

    explicit IteratorPreorder( TaxonomyType& taxonomy )
    {
        // Add the child taxa to the stack, backwards, so that the top one is the one we
        // want to visit first.
        for( size_t i = 0; i < taxonomy.size(); ++i ) {
            stack_.push( &taxonomy.at( taxonomy.size() - 1 - i ));
        }
    }

    ~IteratorPreorder() = default;

    IteratorPreorder( IteratorPreorder const& ) = default;
    IteratorPreorder( IteratorPreorder&& )      = default;

    IteratorPreorder& operator= ( IteratorPreorder const& ) = default;
    IteratorPreorder& operator= ( IteratorPreorder&& )      = default;

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

    self_type operator * ()
    {
        return *this;
    }

    self_type operator ++ ()
    {
        // Get the current stack top and remove it. We will then prepare its children for visit
        // by adding them to the stack.
        auto cur_tax = stack_.top();
        stack_.pop();

        // Add the child taxa to the stack, backwards, so that the top one is the one we
        // want to visit first.
        auto const cur_size = cur_tax->size();
        for( size_t i = 0; i < cur_size; ++i ) {
            stack_.push( &cur_tax->at( cur_size - 1 - i ));
        }

        return *this;
    }

    self_type operator ++ (int)
    {
        self_type tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator == (const self_type &other) const
    {
        return other.stack_ == stack_;
    }

    bool operator != (const self_type &other) const
    {
        return !(other == *this);
    }

    // -----------------------------------------------------
    //     Members
    // -----------------------------------------------------

    TaxonType& taxon() const
    {
        return *stack_.top();
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    std::stack< TaxonType* > stack_;
};

// =================================================================================================
//     Preorder Wrapper Functions
// =================================================================================================

template<typename TaxonomyType>
utils::Range< IteratorPreorder< Taxonomy const, Taxon const >>
inline preorder( TaxonomyType const& taxonomy )
{
    return {
        IteratorPreorder< Taxonomy const, Taxon const >( taxonomy ),
        IteratorPreorder< Taxonomy const, Taxon const >()
    };
}

template<typename TaxonomyType>
utils::Range< IteratorPreorder< Taxonomy, Taxon >>
inline preorder( TaxonomyType& taxonomy )
{
    return {
        IteratorPreorder< Taxonomy, Taxon >( taxonomy ),
        IteratorPreorder< Taxonomy, Taxon >()
    };
}

} // namespace taxonomy
} // namespace genesis

#endif // include guard
