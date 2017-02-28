#ifndef GENESIS_SEQUENCE_FORMATS_PHYLIP_WRITER_H_
#define GENESIS_SEQUENCE_FORMATS_PHYLIP_WRITER_H_

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
 * @ingroup sequence
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class SequenceSet;

// =================================================================================================
//     Phylip Writer
// =================================================================================================

class PhylipWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    PhylipWriter()  = default;
    ~PhylipWriter() = default;

    PhylipWriter( PhylipWriter const& ) = default;
    PhylipWriter( PhylipWriter&& )      = default;

    PhylipWriter& operator= ( PhylipWriter const& ) = default;
    PhylipWriter& operator= ( PhylipWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    // void to_stream_interleaved ( SequenceSet const& sset, std::ostream& os ) const;
    // void to_stream_sequential  ( SequenceSet const& sset, std::ostream& os ) const;

    void        to_stream ( SequenceSet const& sset, std::ostream&      os ) const;
    void        to_file   ( SequenceSet const& sset, std::string const& fn ) const;
    std::string to_string ( SequenceSet const& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    PhylipWriter& label_length( size_t value );
    size_t        label_length() const;

    PhylipWriter& line_length( size_t value );
    size_t        line_length() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t label_length_ = 0;
    size_t line_length_  = 80;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
