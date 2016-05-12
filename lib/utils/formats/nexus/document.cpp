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
 * @ingroup utils
 */

#include "utils/formats/nexus/document.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Accessors
// =================================================================================================

bool NexusDocument::has_block( std::string block_name ) const
{
    return get_block( block_name ) != nullptr;
}

NexusBlock const* NexusDocument::get_block( std::string block_name ) const
{
    // Could use std::find_if instead, but checking iterators is also a bit tedious...
    for( auto& bptr : data_ ) {
        if( bptr->block_name() == block_name ) {
            return bptr.get();
        }
    }
    return nullptr;
}

NexusBlock*       NexusDocument::get_block( std::string block_name )
{
    // Identical to the const version of this function... a bit ugly!
    for( auto& bptr : data_ ) {
        if( bptr->block_name() == block_name ) {
            return bptr.get();
        }
    }
    return nullptr;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

NexusBlock* NexusDocument::set_block( std::unique_ptr<NexusBlock> block )
{
    // First check if we already have a block with this name.
    // If so, replace it and return its pointer.
    for( auto& bptr : data_ ) {
        if( bptr->block_name() == block->block_name() ) {
            bptr = std::move(block);
            return bptr.get();
        }
    }

    // If there is no block with this name yet, create one.
    data_.push_back( std::move(block) );
    return data_.back().get();
}

// void NexusDocument::set_block_trees( Trees const& trees )
// {
//     for( auto& bptr : data_ ) {
//         if( bptr->block_name() == trees.block_name() ) {
//             bptr = utils::make_unique<Trees>(trees);
//             return;
//         }
//     }
//     data_.push_back( utils::make_unique<Trees>(trees) );
// }
//
// void NexusDocument::set_block_taxa( Taxa const& taxa )
// {
//     data_.push_back( utils::make_unique<Taxa>(taxa) );
// }

// const_iterator NexusDocument::begin() const
// {
//
// }
//
// const_iterator NexusDocument::end() const
// {
//
// }

} // namespace utils
} // namespace genesis
