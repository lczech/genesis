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
namespace utils {

// =================================================================================================
//     Nexus Document
// =================================================================================================

/**
 * @brief
 */
class NexusDocument
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    NexusDocument()  = default;
    ~NexusDocument() = default;

    // NexusDocument( NexusDocument const& ) = default;
    NexusDocument( NexusDocument&& )      = default;

    NexusDocument& operator= ( NexusDocument const& ) = default;
    NexusDocument& operator= ( NexusDocument&& )      = default;

    // ---------------------------------------------------------------------
    //     Member Types
    // ---------------------------------------------------------------------

    using container      = std::vector<std::unique_ptr<NexusBlock>>;
    using const_iterator = container::const_iterator;

    // ---------------------------------------------------------------------
    //     Accessors
    // ---------------------------------------------------------------------

    bool has_block( std::string block_name ) const;

    NexusBlock const* get_block( std::string block_name ) const;
    NexusBlock*       get_block( std::string block_name );

    // ---------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------

    // void push_back_block(NexusBlock const& block)
    // {
    //     data_.push_back(make_unique<NexusBlock>(block));
    // }

    // void set_block_trees( Trees const& trees );
    // void set_block_taxa( Taxa const& taxa );

    NexusBlock* set_block( std::unique_ptr<NexusBlock> block );

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

} // namespace utils
} // namespace genesis

#endif // include guard
