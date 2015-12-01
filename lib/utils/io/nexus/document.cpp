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
    for( auto& bptr : data_ ) {
        if( bptr->block_name() == block_name ) {
            return true;
        }
    }
    return false;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void Document::set_block_trees( Trees const& trees )
{
    for( auto& bptr : data_ ) {
        if( bptr->block_name() == trees.block_name() ) {
            bptr = make_unique<Trees>(trees);
            return;
        }
    }
    data_.push_back( make_unique<Trees>(trees) );
}

void Document::set_block_taxa( Taxa const& taxa )
{
    data_.push_back( make_unique<Taxa>(taxa) );
}

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
