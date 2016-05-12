#ifndef GENESIS_UTILS_FORMATS_NEXUS_TREES_H_
#define GENESIS_UTILS_FORMATS_NEXUS_TREES_H_

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

#include "utils/core/std.hpp"

#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Nexus Block Trees
// =================================================================================================

/**
 * @brief
 */
class NexusTrees : public NexusBlock
{

    // -----------------------------------------------------
    //     Member Types
    // -----------------------------------------------------

public:

    struct Entry
    {
        std::string name;
        std::vector<std::string> properties;
        std::string tree;
    };

    using container      = std::vector<Entry>;
    using const_iterator = container::const_iterator;

    // -----------------------------------------------------
    //     Constructor and Rule of Five
    // -----------------------------------------------------

public:

    NexusTrees() = default;
    ~NexusTrees() = default;

    NexusTrees( NexusTrees const& ) = default;
    NexusTrees( NexusTrees&& )      = default;

    NexusTrees& operator= ( NexusTrees const& ) = default;
    NexusTrees& operator= ( NexusTrees&& )      = default;

    void swap( NexusTrees& other )
    {
        using std::swap;
        swap( entries_, other.entries_ );
    }

    // -----------------------------------------------------
    //     Operators
    // -----------------------------------------------------

public:

    // friend bool operator == ( NexusTrees const& lhs, NexusTrees const& rhs )
    // {
    //     return lhs.entries_ == rhs.entries_;
    // }
    //
    // friend bool operator != ( NexusTrees const& lhs, NexusTrees const& rhs )
    // {
    //     return !(lhs == rhs);
    // }

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

public:

    bool empty() const
    {
        return entries_.empty();
    }

    size_t size() const
    {
        return entries_.size();
    }

    bool has_tree( std::string name ) const
    {
        return entries_.end() != std::find_if(
            entries_.begin(),
            entries_.end(),
            [&name] (Entry const& entry) {
                return entry.name == name;
            }
        );

        // for( auto& entry : entries_ ) {
        //     if( entry.name == name ) {
        //         return true;
        //     }
        // }
        // return false;
    }

    // -----------------------------------------------------
    //     Iterators
    // -----------------------------------------------------

public:

    const_iterator begin() const
    {
        return entries_.cbegin();
    }

    const_iterator end() const
    {
        return entries_.cend();
    }

    // -----------------------------------------------------
    //     Modifiers
    // -----------------------------------------------------

public:

    void add_tree( std::string name, std::string tree )
    {
        if( has_tree(name) ) {
            throw std::invalid_argument(
                "Tree with name '" + name + "' already exists in this tree block."
            );
        }

        // auto e = Entry();
        // e.name = name;
        // e.tree = tree;
        // entries_.push_back(e);

        entries_.push_back({ name, {}, tree });
        // entries_.emplace_back(name, {}, tree);
    }

    void erase_tree( std::string name )
    {
        utils::erase_if(
            entries_,
            [&name] (Entry const& entry) {
                return entry.name == name;
            }
        );
    }

    void clear()
    {
        entries_.clear();
    }

    // -----------------------------------------------------
    //     Virtual Functions
    // -----------------------------------------------------

public:

    std::string block_name() const override
    {
        return "TREES";
    }

    void to_stream( std::ostream& os ) const override
    {
        for( auto& entry : entries_ ) {
            os << "    TREE " << entry.name << " = ";
            for( auto& prop : entry.properties ) {
                os << "[" << prop << "] ";
            }
            os << entry.tree << "\n";
        }
    }

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    container entries_;

};

// =================================================================================================
//     Operators
// =================================================================================================

inline void swap( NexusTrees& lhs, NexusTrees& rhs )
{
    lhs.swap(rhs);
}

} // namespace utils
} // namespace genesis

#endif // include guard
