#ifndef GENESIS_UTILS_FORMATS_NEXUS_BLOCK_H_
#define GENESIS_UTILS_FORMATS_NEXUS_BLOCK_H_

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
