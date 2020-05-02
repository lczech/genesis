#ifndef GENESIS_UTILS_IO_BASE_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_BASE_OUTPUT_TARGET_H_

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

#include "genesis/utils/core/fs.hpp"

#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Base Output Target
// =================================================================================================

/**
 * @brief Abstract base class for writing data to an output target.
 */
class BaseOutputTarget
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    BaseOutputTarget() = default;

    BaseOutputTarget( BaseOutputTarget const& ) = default;
    BaseOutputTarget( BaseOutputTarget&& )      = default;

    BaseOutputTarget& operator= ( BaseOutputTarget const& ) = default;
    BaseOutputTarget& operator= ( BaseOutputTarget&& )      = default;

    virtual ~BaseOutputTarget()
    {}

    // -------------------------------------------------------------
    //     Members
    // -------------------------------------------------------------

    /**
     * @brief Output stream operator template that simply forwards to the underlying ostream() object.
     *
     * This is the main function used to write output. This way, nothing has to be changed in code
     * that wants to write some output, comared to standard write-to-stream functions.
     * The only change necessary is the initial setup of the output object: Instead of
     * an std::ostream, one has to initialize this class. From then on, usage is identical.
     */
    template<typename T>
    BaseOutputTarget& operator << ( T const& content )
    {
        out_stream_() << content;
        return *this;
    }

    /**
     * @brief Get the underlying output stream that is used for writing.
     */
    std::ostream& ostream()
    {
        // Non-virtual interface.
        return out_stream_();
    }

    /**
     * @brief Get a name of the output target. This is intended for user output.
     */
    std::string target_name() const
    {
        // Non-virtual interface.
        return target_name_();
    }

    /**
     * @brief Get a string representing the output target. This is intended for the writer classes,
     * which for example might want to examine the output file name.
     */
    std::string target_string() const
    {
        // Non-virtual interface.
        return target_string_();
    }

    // -------------------------------------------------------------
    //     Internal Members
    // -------------------------------------------------------------

private:

    virtual std::ostream& out_stream_() = 0;

    virtual std::string target_name_() const = 0;
    virtual std::string target_string_() const = 0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
