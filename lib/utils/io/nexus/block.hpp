#ifndef GENESIS_UTILS_IO_NEXUS_BLOCK_H_
#define GENESIS_UTILS_IO_NEXUS_BLOCK_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <string>
#include <iosfwd>

namespace genesis {
namespace utils {

// =================================================================================================
//     Nexus Block
// =================================================================================================

/**
 * @brief
 */
class NexusBlock
{
    // -----------------------------------------------------
    //     Constructor and Rule of Five
    // -----------------------------------------------------

public:

    NexusBlock()  = default;
    virtual ~NexusBlock() = default;

    NexusBlock( NexusBlock const& ) = default;
    NexusBlock( NexusBlock&& )      = default;

    NexusBlock& operator= ( NexusBlock const& ) = default;
    NexusBlock& operator= ( NexusBlock&& )      = default;

    // -----------------------------------------------------
    //     Virtual Functions
    // -----------------------------------------------------

public:

    virtual std::string block_name() const = 0;
    virtual void to_stream( std::ostream& os ) const = 0;

};

// =================================================================================================
//     Operators
// =================================================================================================

// std::ostream& operator<< ( std::ostream& os, NexusBlock const& block )
inline std::ostream& operator<< ( std::ostream& os, NexusBlock const& block )
{
    block.to_stream( os );
    return os;
}

} // namespace utils
} // namespace genesis

#endif // include guard
