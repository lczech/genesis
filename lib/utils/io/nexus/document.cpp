/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/nexus/document.hpp"

namespace genesis {
namespace nexus {

// =================================================================================================
//     Accessors
// =================================================================================================

bool Document::has_block( std::string block_name ) const
{
    return get_block( block_name ) != nullptr;
}

Block const* Document::get_block( std::string block_name ) const
{
    // Could use std::find_if instead, but checking iterators is also a bit tedious...
    for( auto& bptr : data_ ) {
        if( bptr->block_name() == block_name ) {
            return bptr.get();
        }
    }
    return nullptr;
}

Block*       Document::get_block( std::string block_name )
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

Block* Document::set_block( std::unique_ptr<Block> block )
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

// void Document::set_block_trees( Trees const& trees )
// {
//     for( auto& bptr : data_ ) {
//         if( bptr->block_name() == trees.block_name() ) {
//             bptr = make_unique<Trees>(trees);
//             return;
//         }
//     }
//     data_.push_back( make_unique<Trees>(trees) );
// }
//
// void Document::set_block_taxa( Taxa const& taxa )
// {
//     data_.push_back( make_unique<Taxa>(taxa) );
// }

// const_iterator Document::begin() const
// {
//
// }
//
// const_iterator Document::end() const
// {
//
// }

} // namespace nexus
} // namespace genesis
