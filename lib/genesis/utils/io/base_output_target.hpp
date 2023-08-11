#ifndef GENESIS_UTILS_IO_BASE_OUTPUT_TARGET_H_
#define GENESIS_UTILS_IO_BASE_OUTPUT_TARGET_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2023 Lucas Czech

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
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/fs.hpp"

#include <ostream>
#include <string>

namespace genesis {
namespace utils {

// =================================================================================================
//     Base Output Target
// =================================================================================================

/**
 * @brief Abstract base class for writing data to an output target.
 *
 * The class is an interface that allows writing to different targets, and adds a layer of abstraction
 * around using simple `std::ostream` functionality. In particular, we want to add some checks,
 * naming of the streams, etc. Internally however, the derived classes of this base class use
 * `std::ostream`, and make it accessible.
 *
 * @see FileOutputTarget, GzipOutputTarget, StreamOutputTarget, StringOutputTarget for our derived
 * output target classes.
 * @see to_file(), to_gzip_block_file(), to_stream(), to_string() for helper functions to create
 * these classes, to add some syntactic sugar and make it easy to use.
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
     *
     * Alternatively, the ostream() function can be used to obtain the underlying stream that is
     * internally used to write the data. Also, see flush() to synchronize the writing with the
     * actual target of the stream.
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
     * @brief Flush output stream buffer.
     *
     * Internally, the different output target derived classes use some variant of `std::ostream`
     * to do the writing. Hence, the target might need flushing in cases where we want to synchronize
     * it while writing, before closing the stream.
     */
    std::ostream& flush()
    {
        return out_stream_().flush();
    }

    /**
     * @brief Get a name of the output target. This is intended for user output.
     *
     * This will for example return something like "output file (/path/to/file.txt)", so that
     * users know what type of output stream it is, and where it streams to.
     */
    std::string target_name() const
    {
        // Non-virtual interface.
        return target_name_();
    }

    /**
     * @brief Get a string representing the output target.
     *
     * This is intended for the writer classes, which for example might want to examine the output
     * file name. Hence, this function is meant to return just the file path (for a file target).
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

    // Non-virtual interface.
    virtual std::ostream& out_stream_() = 0;
    virtual std::string target_name_() const = 0;
    virtual std::string target_string_() const = 0;

};

} // namespace utils
} // namespace genesis

#endif // include guard
