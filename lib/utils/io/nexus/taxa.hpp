#ifndef GENESIS_UTILS_IO_NEXUS_TAXA_H_
#define GENESIS_UTILS_IO_NEXUS_TAXA_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "utils/io/nexus/block.hpp"

#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace nexus {

// =================================================================================================
//     Nexus Block Taxa
// =================================================================================================

/**
 * @brief
 */
class Taxa : public Block
{

    // -----------------------------------------------------
    //     Member Types
    // -----------------------------------------------------

public:

    using container      = std::unordered_set<std::string>;
    using const_iterator = container::const_iterator;

    // -----------------------------------------------------
    //     Constructor and Rule of Five
    // -----------------------------------------------------

public:

    Taxa() = default;
    ~Taxa() = default;

    Taxa( Taxa const& ) = default;
    Taxa( Taxa&& )      = default;

    Taxa& operator= ( Taxa const& ) = default;
    Taxa& operator= ( Taxa&& )      = default;

    void swap( Taxa& other )
    {
        using std::swap;
        swap( taxa_, other.taxa_ );
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

public:

    friend bool operator == ( Taxa const& lhs, Taxa const& rhs )
    {
        return lhs.taxa_ == rhs.taxa_;
    }

    friend bool operator != ( Taxa const& lhs, Taxa const& rhs )
    {
        return !(lhs == rhs);
    }

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

public:

    bool empty() const
    {
        return taxa_.empty();
    }

    size_t size() const
    {
        return taxa_.size();
    }

    bool has_taxon( std::string name ) const
    {
        return taxa_.count(name);
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

public:

    const_iterator begin() const
    {
        return taxa_.cbegin();
    }

    const_iterator end() const
    {
        return taxa_.cend();
    }

    // -----------------------------------------------------
    //     Modifiers
    // -----------------------------------------------------

public:

    void add_taxon( std::string name )
    {
        taxa_.insert( name );
    }

    void add_taxa( std::vector<std::string> taxa )
    {
        taxa_.insert( taxa.begin(), taxa.end() );
    }

    void erase_taxon( std::string name )
    {
        taxa_.erase( name );
    }

    void clear()
    {
        taxa_.clear();
    }

    // -----------------------------------------------------
    //     Virtual Functions
    // -----------------------------------------------------

public:

    std::string block_name() const override
    {
        return "TAXA";
    }

    void to_stream( std::ostream& os ) const override
    {
        os << "    dimensions ntax=" << taxa_.size() << ";\n";
        os << "    taxlabels\n";
        for( auto& taxon : taxa_ ) {
            os << "        " << taxon << "\n";
        }
        os << "    ;\n";
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    container taxa_;

};

// =================================================================================================
//     Operators
// =================================================================================================

inline void swap( Taxa& lhs, Taxa& rhs )
{
    lhs.swap(rhs);
}

} // namespace nexus
} // namespace genesis

#endif // include guard
