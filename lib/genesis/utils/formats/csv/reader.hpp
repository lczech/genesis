#ifndef GENESIS_UTILS_FORMATS_CSV_READER_H_
#define GENESIS_UTILS_FORMATS_CSV_READER_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2018 Lucas Czech and HITS gGmbH

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

#include "genesis/utils/io/input_source.hpp"

#include <iosfwd>
#include <memory>
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
 * @brief Read Comma/Character Separated Values (CSV) data and other delimiter-separated formats.
 *
 * This class provides simple facilities for reading data in a format that uses delimiter chars
 * to separate tabulated data into fields, where one line represents one row of the table.
 *
 * The read() function returns the table as a vector, with one entry per line (i.e., row). Each such
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

    using Field = std::string;
    using Line  = std::vector<Field>;
    using Table = std::vector<Line>;

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

    /**
     * @brief Read CSV data from a source and return it as a table, using a vector per line,
     * containing a vector of fields found on that line.
     *
     * Use functions such as utils::from_file() and utils::from_string() to conveniently
     * get an input source that can be used here.
     */
    Table read( std::shared_ptr<BaseInputSource> source ) const;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    /**
     * @brief Parse a whole CSV document and return its contents.
     */
    Table parse_document(
        utils::InputStream& input_stream
    ) const;

    /**
     * @brief Parse one field (i.e., one cell) of the CSV data and return it.
     *
     * This function reads from a given input stream until the column separator or the end of the line
     * or the end of the stream is found. It furthermore trims the necessary chars from the beginning
     * and end of the field, and handles quoted strings according to the settings of the CsvReader.
     *
     * The stream is left at either the separator char, the new line char, or the end of the file,
     * depending on which occurs first.
     *
     * See
     * @link trim_chars( std::string const& chars ) trim_chars()@endlink,
     * @link quotation_chars( std::string const& chars ) quotation_chars()@endlink,
     * @link separator_chars( std::string const& chars ) separator_chars()@endlink,
     * @link use_escapes( bool value ) use_escapes() @endlink and
     * @link use_twin_quotes( bool value ) use_twin_quotes() @endlink
     * to change the behaviour of this function.
     */
    std::string parse_field(
        utils::InputStream& input_stream
    ) const;

    /**
     * @brief Parse one line of the CSV data and return it.
     *
     * This function parses a whole line using parse_field() until the new line char (or the end of the
     * stream) is found. The fields are returned in a vector. The stream is left at either the next char
     * after the new line char or the end of the file, if there is no new line.
     *
     * See @link merge_separators( bool value ) merge_separators() @endlink to change the behaviour of
     * this function.
     */
    std::vector<std::string> parse_line(
        utils::InputStream& input_stream
    ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    /**
     * @brief Set chars that are used to mark comment lines.
     *
     * By default, no chars are used, that is, no line is interpreted as comment. Use this function
     * to change that behaviour, e.g., use `#` as marker for comment lines.
     * All lines starting with any of the set chars are then skipped while reading. The char has to
     * be the first on the line, that is, no leading blanks are allowed.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader&         comment_chars( std::string const& chars )
    {
        comment_chars_ = chars;
        return *this;
    }

    /**
     * @brief Return the currently set chars that are used to mark comment lines.
     *
     * See the @link comment_chars( std::string const& chars ) setter @endlink of this function for
     * more details.
     */
    std::string const& comment_chars() const
    {
        return comment_chars_;
    }

    /**
     * @brief Set chars that are trimmed from the start and end of each field.
     *
     * By default, no chars are trimmed. Use this function to change that behaviour, e.g., to trim
     * spaces and tabs. Be aware that according to some CSV definitions, blanks are considered
     * to be part of the field and should not be trimmed.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader&         trim_chars( std::string const& chars )
    {
        trim_chars_ = chars;
        return *this;
    }

    /**
     * @brief Return the currently set chars that are trimmed from the start and end of each field.
     *
     * See the @link trim_chars( std::string const& chars ) setter @endlink of this function for
     * more details.
     */
    std::string const& trim_chars() const
    {
        return trim_chars_;
    }

    /**
     * @brief Set the chars that are used for quoting strings in fields.
     *
     * By default, the double quotation mark char \" is used as quotation mark. Any other set of chars
     * can be used instead, for example a combination of single and double quotation marks by providing
     * `'"` to this function.
     *
     * Within a quoted part, any char can appear, even new lines. However, in order to use the quotation
     * mark itself, it has to be escaped.
     * See @link use_escapes( bool value ) use_escapes() @endlink and
     * @link use_twin_quotes( bool value ) use_twin_quotes() @endlink for changing the behaviour
     * of escaping with backslashes and with twin quotation marks.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader&         quotation_chars( std::string const& chars )
    {
        quotation_chars_ = chars;
        return *this;
    }

    /**
     * @brief Return the currently set chars for quoting strings in fields.
     *
     * See the @link quotation_chars( std::string const& chars ) setter @endlink of this function for
     * more details.
     */
    std::string const& quotation_chars() const
    {
        return quotation_chars_;
    }

    /**
     * @brief Set the chars used to separate fields of the CSV data.
     *
     * By default, the comma char `,` is used. Any other set of chars can be used instead, for example
     * a combination of tabs and bars by providing `\t|` to this function.
     *
     * Caveat: If more than one char is used as separater, any of them separates fields. That is,
     * the string provided to this function is not taken as a whole to separate fields, but its single
     * chars are used.
     *
     * See @link merge_separators( bool value ) merge_separators() @endlink to set whether consecutive
     * separator chars are merged.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader&         separator_chars( std::string const& chars )
    {
        separator_chars_ = chars;
        return *this;
    }

    /**
     * @brief Return the currently set chars used to separate fields of the CSV data.
     *
     * See the @link separator_chars( std::string const& chars ) setter @endlink of this function
     * for more details.
     */
    std::string const& separator_chars() const
    {
        return separator_chars_;
    }

    /**
     * @brief Set whether to skip empty lines.
     *
     * Default is `false`. If set to `true`, all lines that are empty (that is, no content, or just
     * consisting of spaces and tabs) are skipped while reading.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader& skip_empty_lines( bool value )
    {
        skip_empty_lines_ = value;
        return *this;
    }

    /**
     * @brief Return whether currently empty lines are skipped.
     *
     * See the @link skip_empty_lines( bool value ) setter @endlink of this function for more details.
     */
    bool       skip_empty_lines() const
    {
        return skip_empty_lines_;
    }

    /**
     * @brief Set whether consecutive separater chars are merged or whether each of them creates a
     * new field.
     *
     * Default is `false`. Usually, CSV data has the same number of columns for the whole dataset.
     * Thus, empty fields will result in consecutive separator chars. When this value is set to `false`,
     * those fields are correctly parsed into empty fields.
     *
     * It might however be useful to not create separate empty fields when consecutive separator chars
     * are encountered. This is particularly the case if spaces or tabs are used as separators.
     * In this case it makes sense to have more than one of them consecutively in order to align
     * the columns of the data. For such datasets, this value can be set to `true`.
     *
     * To put it in other words, this value determines whether empty strings resulting from adjacent
     * separator chars are excluded from the output.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader& merge_separators( bool value )
    {
        merge_separators_ = value;
        return *this;
    }

    /**
     * @brief Return the current setting whether consecutive separators are merged or not.
     *
     * See the @link merge_separators( bool value ) setter @endlink of this function for details.
     */
    bool       merge_separators() const
    {
        return merge_separators_;
    }

    /**
     * @brief Set whether to use backslash escape sequences.
     *
     * Default is `false`. If set to `true`, character sequences of `\x` (backslash and some other char)
     * are turned into the respective string form, according to the rules of deescape().
     * Also, see parse_quoted_string() for more information on escaping.
     *
     * This works inside and outside of quoted strings. In order to create new lines within a field,
     * either the sequence `\n` (backslash n) can be used, or a backslash at the end of the line.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader& use_escapes( bool value )
    {
        use_escapes_ = value;
        return *this;
    }

    /**
     * @brief Return whether backslash escape sequences are used.
     *
     * See the @link use_escapes( bool value ) setter @endlink of this function for details.
     */
    bool       use_escapes() const
    {
        return use_escapes_;
    }

    /**
     * @brief Set whether to interpret two consequtive quotation marks as a single ("escaped") one.
     *
     * Default is `true`. Use this setting in order to be able to escape quotation marks by doubling
     * them. This is a common variant in CSV data. It means, whenever two consecutive quotation marks
     * are encountered, they are turned into one (thus, the first one "escapes" the second). This
     * works both inside and outside of regularly quoted parts. That is, the following two fields
     * are interpreted the same:
     *
     *     "My ""old"" friend"
     *     My ""old"" friend
     *
     * This also works in addition to normal backslash escape sequences, see
     * @link use_escapes( bool value ) use_escapes() @endlink for more on this.
     *
     * See @link quotation_chars( std::string const& chars ) quotation_chars() @endlink to set which
     * chars are interpreted as quotation marks.
     *
     * The function returns a reference to the CsvReader object in order to allow a fluent interface.
     */
    CsvReader& use_twin_quotes( bool value )
    {
        use_twin_quotes_ = value;
        return *this;
    }

    /**
     * @brief Return whether to interpret two consequtive quotation marks as a single ("escaped") one.
     *
     * See the @link use_twin_quotes( bool value ) setter @endlink of this function for details.
     */
    bool       use_twin_quotes() const
    {
        return use_twin_quotes_;
    }

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
