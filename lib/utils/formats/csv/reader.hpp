#ifndef GENESIS_UTILS_FORMATS_CSV_READER_H_
#define GENESIS_UTILS_FORMATS_CSV_READER_H_

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
 * @ingroup utils
 */

#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class CountingIstream;

// =================================================================================================
//     Json Reader
// =================================================================================================

/**
 * @brief Read Comma Separated Values (CSV) data.
 */
class CsvReader
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    CsvReader()  = default;
    ~CsvReader() = default;

    CsvReader( CsvReader const& ) = default;
    CsvReader( CsvReader&& )      = default;

    CsvReader& operator= ( CsvReader const& ) = default;
    CsvReader& operator= ( CsvReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    // void from_stream ( std::istream&      is, SequenceSet& sset ) const;
    // void from_file   ( std::string const& fn, SequenceSet& sset ) const;
    // void from_string ( std::string const& fs, SequenceSet& sset ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    std::string parse_field(
        utils::CountingIstream& input_stream
    ) const;

    std::vector<std::string> parse_line(
        utils::CountingIstream& input_stream
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    CsvReader&         trim_chars( std::string const& chars );
    std::string const& trim_chars() const;

    CsvReader&         quotation_chars( std::string const& chars );
    std::string const& quotation_chars() const;

    CsvReader&         separator_chars( std::string const& chars );
    std::string const& separator_chars() const;

    CsvReader& merge_separators( bool value );
    bool       merge_separators() const;

    CsvReader& use_escapes( bool value );
    bool       use_escapes() const;

    CsvReader& use_twin_quotes( bool value );
    bool       use_twin_quotes() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    std::string trim_chars_        = " \t";
    std::string quotation_chars_   = "\"";
    std::string separator_chars_   = ",";

    bool        merge_separators_  = false;
    bool        use_escapes_       = false;
    bool        use_twin_quotes_   = true;

    // We use a buffer in order to make copying and resizing strings as small and fast as possible.
    // This buffer will grow for bigger csv input fields (but never shrink). We then copy from it,
    // so that the new strings are as small as possible. After some fields, the buffer size
    // approaches a value where it rarely needs to grow any more.
    mutable std::string buffer_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
