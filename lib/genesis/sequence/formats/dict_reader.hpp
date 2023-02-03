#ifndef GENESIS_SEQUENCE_FORMATS_DICT_READER_H_
#define GENESIS_SEQUENCE_FORMATS_DICT_READER_H_

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
 * @ingroup sequence
 */

#include "genesis/sequence/sequence_dict.hpp"
#include "genesis/utils/io/input_source.hpp"

#include <iosfwd>
#include <memory>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Dict Reader
// =================================================================================================

/**
 * @brief Read a `.dict` sequence dictionary file,
 * describing, e.g., reference genome sequence properties.
 *
 * The file format is for example produced by `GATK/Picard CreateSequenceDictionary`
 * or by `samtools dict`.
 *
 * Exemplary usage:
 *
 *     std::string infile = "path/to/file.dict";
 *     auto dict = DictReader().read( utils::from_file( infile ));
 *
 * More information on the format can be found at:
 *
 *    * http://www.htslib.org/doc/samtools-dict.html
 *    * https://gatk.broadinstitute.org/hc/en-us/articles/360036729911-CreateSequenceDictionary-Picard-
 *
 * See there for details.
 */
class DictReader
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    DictReader() = default;
    ~DictReader() = default;

    DictReader( DictReader const& ) = default;
    DictReader( DictReader&& )      = default;

    DictReader& operator= ( DictReader const& ) = default;
    DictReader& operator= ( DictReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    SequenceDict read( std::shared_ptr<utils::BaseInputSource> source ) const;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
