#ifndef GENESIS_UTILS_IO_STRING_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_STRING_OUTPUT_TARGET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include <genesis/utils/io/base_output_target.hpp>

#include <sstream>
#include <string>

namespace genesis {
namespace utils {
namespace io {

// =================================================================================================
//     String Output Target
// =================================================================================================

/**
 * @brief Output target for writing data to a string.
 *
 * The target string to write to can be specified when constructing this class. In that case,
 * it has to stay alive for the duration of the data writing process where this class is used.
 * Internally, the data is buffered in a stringstream, and only written to the string
 * on destruction of this class.
 *
 *     std::string target;
 *     write_output_target( content, to_string( target ));
 *     // use target
 *
 * Alternatively, when no target string is specified upon construction, the written results can
 * be obtained later via the get() method. We use the io::to_string() method here, which returns
 * a `std::shared_ptr` to the base class BaseOutputTarget, so usage is a bit tricky:
 *
 *     auto target = to_string();
 *     write_output_target( content, target );
 *     // use static_cast<StringOutputTarget*>( target.get() )->get()
 *
 * See BaseOutputTarget for more information on the functionality.
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
    explicit StringOutputTarget()
    {}

    /**
     * @brief Construct the output target to a string that is modified.
     */
    explicit StringOutputTarget( std::string& target )
        : target_( &target )
    {}

    StringOutputTarget( StringOutputTarget const& ) = delete;
    StringOutputTarget( StringOutputTarget&& )      = default;

    StringOutputTarget& operator= ( StringOutputTarget const& ) = delete;
    StringOutputTarget& operator= ( StringOutputTarget&& )      = default;

    ~StringOutputTarget() override
    {
        // Only write to the target if it was given.
        if( target_ ) {
            *target_ = stream_.str();
        }
    }

    // -------------------------------------------------------------
    //     Public Interface
    // -------------------------------------------------------------

    /**
     * @brief Get the string result.
     */
    std::string get() const
    {
        return stream_.str();
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

    std::string* target_ = nullptr;
    std::ostringstream stream_;
};

} // namespace io
} // namespace utils
} // namespace genesis

#endif // include guard
