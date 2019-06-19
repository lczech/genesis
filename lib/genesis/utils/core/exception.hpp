#ifndef GENESIS_UTILS_CORE_EXCEPTION_H_
#define GENESIS_UTILS_CORE_EXCEPTION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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

#include <stdexcept>
#include <string>

namespace genesis {

// =================================================================================================
//     Error Base Class
// =================================================================================================

/**
 * @brief Base class for special genesis exceptions.
 */
class Error : public std::runtime_error
{
public:

    Error( std::string const& message )
        : std::runtime_error( message )
    {}
};

// =================================================================================================
//     File Related Errors
// =================================================================================================

class ExistingFileError : public Error
{
public:

    ExistingFileError( std::string const& message, std::string const& filename )
        : Error( message )
        , filename_( filename )
    {}

    std::string const& filename() const
    {
        return filename_;
    }

private:

    std::string filename_;
};

} // namespace genesis

#endif // include guard
