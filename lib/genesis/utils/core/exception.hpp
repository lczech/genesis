#ifndef GENESIS_UTILS_CORE_EXCEPTION_H_
#define GENESIS_UTILS_CORE_EXCEPTION_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2020 Lucas Czech and HITS gGmbH

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
 * @ingroup except
 */

#include <stdexcept>
#include <string>

namespace genesis {
namespace except {

// =================================================================================================
//     Exception Base Class
// =================================================================================================

/**
 * @brief Base class for genesis exceptions.
 *
 * This class serves as the base from which all exceptions that are thrown from genesis shall
 * be derived. We are far from having done this yet (unfortunately, we had this idea too late...),
 * so for now, there are only a few exceptions that use this class as their base.
 * But in the long term, we want to change that, so that developers using genesis can easily
 * catch all genesis-related exceptions.
 */
class Exception
    : public std::exception
{
public:

    Exception( std::string const& message )
        : message_( message )
    {}

    char const* what() const noexcept override {
        return message_.c_str();
    }

protected:

    std::string message_;

};

// =================================================================================================
//     File Related Errors
// =================================================================================================

/**
 * @brief Exception class for general input/output errors.
 */
class IOError
    : public Exception
{
public:

    IOError( std::string const& message, std::string const& filename )
        : Exception( message )
        , filename_( filename )
    {}

    std::string const& filename() const
    {
        return filename_;
    }

protected:

    std::string filename_;
};

/**
 * @brief Exception class that is thrown if trying to write to an existing file.
 *
 * See for example file_output_stream(). This exception is not thrown if
 * Options::get().allow_file_overwriting() is set to true.
 */
class ExistingFileError
    : public IOError
{
public:

    ExistingFileError( std::string const& message, std::string const& filename )
        : IOError( message, filename )
    {}
};

} // namespace except
} // namespace genesis

#endif // include guard
