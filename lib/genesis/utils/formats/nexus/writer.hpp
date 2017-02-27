#ifndef GENESIS_UTILS_FORMATS_NEXUS_WRITER_H_
#define GENESIS_UTILS_FORMATS_NEXUS_WRITER_H_

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

    void        to_stream   ( NexusDocument const& doc, std::ostream& out ) const;
    void        to_file     ( NexusDocument const& doc, std::string const& filename) const;
    void        to_string   ( NexusDocument const& doc, std::string& output) const;
    std::string to_string   ( NexusDocument const& doc) const;
};

} // namespace utils
} // namespace genesis

#endif // include guard
