#ifndef GENESIS_UTILS_FORMATS_CSV_READER_H_
#define GENESIS_UTILS_FORMATS_CSV_READER_H_

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
 * @ingroup utils
 */

#include <iosfwd>
#include <string>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class InputStream;

// =================================================================================================
//     Csv Reader
// =================================================================================================

/**
 * @brief Read Comma Separated Values (CSV) data and other delimiter-separated formats.
 *
 * This class provides simple facilities for reading data in a format that uses delimiter chars
 * to separate tabulated data into fields, where one line represents one row of the table.
 *
 * It supports to read
 *
 *   * from_stream()
 *   * from_file()
 *   * from_string()
 *
 * Those functions return the table as a vector, with one entry per line (i.e., row). Each such
 * entry is itself a vector of strings, representing the fields (values of the columns) of that row.
 *
 * There are several properties that can be changed in order to customize the behaviour. By default,
 * the reader uses the comma char to separate fields and uses double quotation marks. See the
 * property functions for more information.
 *
 * If the data is too big to be read at once into memory, or if you want to parse the data line by
 * line, you can also use the parser functions parse_line() and parse_field() directly.
 */
class CsvReader
{
public:

    // ---------------------------------------------------------------------
    //     Typedefs and Enums
    // ---------------------------------------------------------------------

    typedef std::string        field;
    typedef std::vector<field> row;
    typedef std::vector<row>   table;

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

    table from_stream( std::istream&      is ) const;
    table from_file  ( std::string const& fn ) const;
    table from_string( std::string const& fs ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    table parse_document(
        utils::InputStream& input_stream
    ) const;

    std::string parse_field(
        utils::InputStream& input_stream
    ) const;

    std::vector<std::string> parse_line(
        utils::InputStream& input_stream
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    CsvReader&         comment_chars( std::string const& chars );
    std::string const& comment_chars() const;

    CsvReader&         trim_chars( std::string const& chars );
    std::string const& trim_chars() const;

    CsvReader&         quotation_chars( std::string const& chars );
    std::string const& quotation_chars() const;

    CsvReader&         separator_chars( std::string const& chars );
    std::string const& separator_chars() const;

    CsvReader& skip_empty_lines( bool value );
    bool       skip_empty_lines() const;

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

    // We store the following char sets as strings and use find() to check whether a given char
    // is part of the sets. This is linear in length of the string. As there are usually just a
    // few chars in there, this is fast. We also tested with a char lookup table, which offers
    // constant time, but still was slower. See also http://stackoverflow.com/a/29068727/4184258
    std::string comment_chars_     = "";
    std::string trim_chars_        = "";
    std::string quotation_chars_   = "\"";
    std::string separator_chars_   = ",";

    bool        skip_empty_lines_  = false;
    bool        merge_separators_  = false;
    bool        use_escapes_       = false;
    bool        use_twin_quotes_   = true;

    // We use a buffer in order to make copying and resizing strings rare and hence fast.
    // This buffer will grow for bigger csv input fields (but never shrink). We then copy from it,
    // so that the new strings are as small as possible. After some fields, the buffer size
    // approaches a value where it rarely needs to grow any more. Speedup (using a 4MB file): ~20%.
    mutable std::string buffer_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
