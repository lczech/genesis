#ifndef GENESIS_UTILS_FORMATS_NEXUS_TAXA_H_
#define GENESIS_UTILS_FORMATS_NEXUS_TAXA_H_

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

#include "utils/formats/nexus/block.hpp"

#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Nexus Block Taxa
// =================================================================================================

/**
 * @brief
 */
class NexusTaxa : public NexusBlock
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

    NexusTaxa() = default;
    ~NexusTaxa() = default;

    NexusTaxa( NexusTaxa const& ) = default;
    NexusTaxa( NexusTaxa&& )      = default;

    NexusTaxa& operator= ( NexusTaxa const& ) = default;
    NexusTaxa& operator= ( NexusTaxa&& )      = default;

    void swap( NexusTaxa& other )
    {
        using std::swap;
        swap( taxa_, other.taxa_ );
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

public:

    friend bool operator == ( NexusTaxa const& lhs, NexusTaxa const& rhs )
    {
        return lhs.taxa_ == rhs.taxa_;
    }

    friend bool operator != ( NexusTaxa const& lhs, NexusTaxa const& rhs )
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

    void add_taxa( std::unordered_set<std::string> taxa )
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

inline void swap( NexusTaxa& lhs, NexusTaxa& rhs )
{
    lhs.swap(rhs);
}

} // namespace utils
} // namespace genesis

#endif // include guard
