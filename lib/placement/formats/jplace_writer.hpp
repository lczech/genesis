#ifndef GENESIS_PLACEMENT_FORMATS_JPLACE_WRITER_H_
#define GENESIS_PLACEMENT_FORMATS_JPLACE_WRITER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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
 * This class provides facilities for writing Jplace data. It supports to write
 *
 *   * to_stream()
 *   * to_file()
 *   * to_string()
 *   * to_document()
 *
 * Exemplary usage:
 *
 *     std::string outfile = "path/to/file.jplace";
 *     Sample sample;
 *
 *     JplaceWriter()
 *         .to_file( sample, outfile );
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

    void        to_stream   ( Sample const& smp, std::ostream& os ) const;
    void        to_file     ( Sample const& smp, std::string const& filename) const;

    void        to_string   ( Sample const& smp, std::string&       output) const;
    std::string to_string   ( Sample const& smp) const;

    void        to_document ( Sample const& smp, utils::JsonDocument& doc) const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

public:

    // TODO for now, this is all public. use getters and setters instead

    /**
     * @brief The precision used for printing the `branch_length` floating point numbers.
     */
    int branch_length_precision = 6;
};

} // namespace placement
} // namespace genesis

#endif // include guard
