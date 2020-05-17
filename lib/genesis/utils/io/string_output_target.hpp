#ifndef GENESIS_UTILS_IO_STRING_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_STRING_OUTPUT_TARGET_H_

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

#include "genesis/utils/io/base_output_target.hpp"

#include <sstream>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     String Output Target
// =================================================================================================

/**
 * @brief Output target for writing data to a string.
 *
 * The string that is written to is kept by reference in this class. Hence, it has to stay
 * alive for the duration of the data writing process where this class is used.
 * Internally, the data is buffered in a stringstream, and only written to the string
 * on destruction of this class.
 */
class StringOutputTarget final : public BaseOutputTarget
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the output target to a string that is modified.
     */
    explicit StringOutputTarget( std::string& target )
        : target_( target )
    {}

    StringOutputTarget( StringOutputTarget const& ) = delete;
    StringOutputTarget( StringOutputTarget&& )      = delete;

    StringOutputTarget& operator= ( StringOutputTarget const& ) = delete;
    StringOutputTarget& operator= ( StringOutputTarget&& )      = delete;

    ~StringOutputTarget() override
    {
        target_ = stream_.str();
    }

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the output stream function.
     */
    std::ostream& out_stream_() override
    {
        return stream_;
    }

    /**
     * @brief Override of the target name funtion. Returns "output string".
     */
    std::string target_name_() const override
    {
        return "output string";
    }

    /**
     * @brief Override of the target string funtion. Returns "string".
     */
    std::string target_string_() const override
    {
        return "string";
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    std::string& target_;
    std::ostringstream stream_;
};

} // namespace utils
} // namespace genesis

#endif // include guard
