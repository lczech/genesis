#ifndef GENESIS_PLACEMENT_FORMATS_JPLACE_WRITER_H_
#define GENESIS_PLACEMENT_FORMATS_JPLACE_WRITER_H_

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
 * @ingroup placement
 */

#include "genesis/utils/io/output_target.hpp"

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class JsonDocument;
}

namespace placement {
    class Sample;
    class SampleSet;
}

// =================================================================================================
//     Jplace Writer
// =================================================================================================

namespace placement {

/**
 * @brief Write Jplace data.
 *
 * This class provides facilities for writing Jplace data to different output targets.
 *
 * Exemplary usage:
 *
 *     std::string outfile = "path/to/file.jplace";
 *     Sample sample;
 *
 *     JplaceWriter().write( sample, utils::to_file( outfile ));
 *
 * Furthermore, using to_document(), the Sample can also be transformed into our internal
 * Json representaion in form of a utils::JsonDocument that adheres to the Jplace standard.
 * This is probably rarely needed, but might be useful nonetheless.
 *
 * See JplaceReader for more information on the data format. See Sample for more information
 * on the data structure.
 */
class JplaceWriter
{
public:

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    /**
     * @brief Write a Sample to an output target, using the Jplace format.
     *
     * See the output target convenience functions utils::to_file(), utils::to_stream(), and
     * utils::to_string() for examples of how to obtain a suitable output target.
     */
    void write( Sample const& sample, std::shared_ptr<utils::BaseOutputTarget> target );

    /**
     * @brief Store the data of a Sample in a JsonDocument object.
     *
     * This method is not really useful anymore, as we can now directly write to files, strings and
     * streams. It is however kept here for reference and in case someone wants to work with Json files
     * directly.
     */
    utils::JsonDocument to_document( Sample const& sample ) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Get the precision used for printing the `branch_length` floating point numbers
     * of the reference Tree of the Sample.
     */
    size_t branch_length_precision() const
    {
        return branch_length_precision_;
    }

    /**
     * @brief Set the precision used for printing the `branch_length` floating point numbers
     * of the reference Tree of the Sample.
     */
    JplaceWriter& branch_length_precision( size_t value )
    {
        branch_length_precision_ = value;
        return *this;
    }

private:

    size_t branch_length_precision_ = 6;
};

} // namespace placement
} // namespace genesis

#endif // include guard
