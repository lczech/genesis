#ifndef GENESIS_UTILS_IO_NEXUS_DOCUMENT_H_
#define GENESIS_UTILS_IO_NEXUS_DOCUMENT_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/nexus/block.hpp"
#include "utils/io/nexus/taxa.hpp"
#include "utils/io/nexus/trees.hpp"

#include "utils/core/std.hpp"

#include <memory>
#include <string>
#include <vector>

namespace genesis {
namespace nexus {

// =================================================================================================
//     Nexus Document
// =================================================================================================

/**
 * @brief
 */
class Document
{
    // ---------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------

public:

    using container      = std::vector<std::unique_ptr<Block>>;
    using const_iterator = container::const_iterator;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    bool has_block( std::string block_name ) const;

    Block const* get_block( std::string block_name ) const;
    Block*       get_block( std::string block_name );

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    // void push_back_block(Block const& block)
    // {
    //     data_.push_back(make_unique<Block>(block));
    // }

    // void set_block_trees( Trees const& trees );
    // void set_block_taxa( Taxa const& taxa );

    Block* set_block( std::unique_ptr<Block> block );

    // ---------------------------------------------------------------------
    //     Iterators
    // ---------------------------------------------------------------------

public:

    const_iterator begin() const
    {
        return data_.cbegin();
    }

    const_iterator end() const
    {
        return data_.cend();
    }

    // ---------------------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------------------

private:

    container data_;

};

} // namespace nexus
} // namespace genesis

#endif // include guard
