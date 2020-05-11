#ifndef GENESIS_UTILS_FORMATS_NEXUS_WRITER_H_
#define GENESIS_UTILS_FORMATS_NEXUS_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech

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

#include "genesis/utils/io/output_target.hpp"

#include <iosfwd>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class NexusDocument;

// =================================================================================================
//     Nexus Writer
// =================================================================================================

/**
 * @brief
 */
class NexusWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    NexusWriter()  = default;
    ~NexusWriter() = default;

    NexusWriter( NexusWriter const& ) = default;
    NexusWriter( NexusWriter&& )      = default;

    NexusWriter& operator= ( NexusWriter const& ) = default;
    NexusWriter& operator= ( NexusWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Write Functions
    // ---------------------------------------------------------------------

    /**
     * @brief Write a NexusDocument to an output target, using the Nexus format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( NexusDocument const& document, std::shared_ptr<utils::BaseOutputTarget> target ) const;

    /**
     * @brief Directly return a NexusDocument as a string in Nexus format.
     *
     * This might be useful in some cases.
     */
    std::string to_string( NexusDocument const& document ) const;
};

} // namespace utils
} // namespace genesis

#endif // include guard
