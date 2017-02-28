#ifndef GENESIS_UTILS_FORMATS_NEXUS_DOCUMENT_H_
#define GENESIS_UTILS_FORMATS_NEXUS_DOCUMENT_H_

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
 * @ingroup utils
 */

#include "genesis/utils/formats/nexus/block.hpp"
#include "genesis/utils/formats/nexus/taxa.hpp"
#include "genesis/utils/formats/nexus/trees.hpp"

#include "genesis/utils/core/std.hpp"

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
    //     data_.push_back( utils::make_unique<NexusBlock>(block) );
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
