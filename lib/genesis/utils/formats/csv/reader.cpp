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

#include "genesis/utils/formats/csv/reader.hpp"

#include <algorithm>
#include <assert.h>
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "genesis/utils/core/fs.hpp"
#include "genesis/utils/core/std.hpp"
#include "genesis/utils/io/input_stream.hpp"
#include "genesis/utils/io/parser.hpp"
#include "genesis/utils/io/scanner.hpp"
#include "genesis/utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Reading
// =================================================================================================

/**
 * @brief Read CSV data until the end of the stream is reached, and return it.
 */
CsvReader::table CsvReader::from_stream( std::istream& is ) const
{
    utils::InputStream it( utils::make_unique< utils::StreamInputSource >( is ));
    return parse_document( it );
}

/**
 * @brief Read a CSV file and return its contents.
 */
CsvReader::table CsvReader::from_file( std::string const& fn ) const
{
    utils::InputStream it( utils::make_unique< utils::FileInputSource >( fn ));
    return parse_document( it );
}

/**
 * @brief Read a string in CSV format and return its contents.
 */
CsvReader::table CsvReader::from_string( std::string const& fs ) const
{
    utils::InputStream it( utils::make_unique< utils::StringInputSource >( fs ));
    return parse_document( it );
}

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Parse a whole CSV document and return its contents.
 */
CsvReader::table CsvReader::parse_document(
    utils::InputStream& input_stream
) const {
    auto& it = input_stream;
    table result;

    while( it ) {
        // Parse the next line and push it if it has content.
        // (If the file ends on empty lines, this might not be the case).
        auto line = parse_line( it );
        if( line.size() > 0 ) {
            result.push_back( line );
        }
    }

    return result;
}

/**
 * @brief Parse one field (i.e., one column) of the CSV data and return it.
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
std::string CsvReader::parse_field( utils::InputStream& input_stream ) const
{
    // Init. We use a buffer in order to keep memory reallocations to a minimum.
    // See see member variable in the class declaration for more information.
    auto& it = input_stream;
    buffer_ = "";

    // Trim the start of the field by skipping chars if needed.
    // (The end needs to be actually trimmed, as we don't know when it comes.)
    skip_while( it, [&] ( char c ) {
        return trim_chars_.find( c ) != std::string::npos;
    });

    // Read as long as there is input. We will break when finding a new line later.
    while( it ) {

        // Treat escape sequences if needed.
        if( use_escapes_ && *it == '\\' ) {

            // Skip the backslash.
            ++it;

            // We found an escaping backslash. This cannot be the end of the stream.
            if( !it ) {
                throw std::runtime_error(
                    "Unexpected end of string at " + it.at() + ". Expecting escape sequence."
                );
            }

            // De-escape.
            buffer_ += deescape( *it );

            // Next char. We skip the rest of this loop, as we already treated the current char.
            ++it;
            continue;
        }

        // Finish reading at the end of the line or when one of the separator chars is found.
        if( *it == '\n' || separator_chars_.find( *it ) != std::string::npos ) {
            break;
        }

        // Parse quoted strings if needed.
        // We add them to the result, even when they occur in the middle of a field.
        if( quotation_chars_.find( *it ) != std::string::npos ) {

            // If the parsing results in an empty string, this means that there were two
            // consecutive quotation marks. So, if also use_twin_quotes is activated, we need
            // to add one quotation mark to the result.
            // In all other cases (i.e., there was content in the quoted string, or we do not
            // use twin quotes), add this content to the result.
            char qm = *it;
            auto qs = parse_quoted_string( it, use_escapes_, use_twin_quotes_, false );
            if( qs == "" && use_twin_quotes_ ) {
                buffer_ += qm;
            } else {
                buffer_ += qs;
            }
            continue;
        }

        // In any other case, simply read the char.
        buffer_ += *it;
        ++it;
    }

    // Now do the last trimming step and return the result.
    return trim_right( buffer_, trim_chars_ );
}

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
std::vector<std::string> CsvReader::parse_line( utils::InputStream& input_stream ) const
{
    // Init.
    auto& it = input_stream;
    std::vector<std::string> result;
    size_t field_count = 0;

    // Read until one of the inner breaking conditions applies.
    // We need this to make sure that the stream can also end with a separator char
    // (it then depends on the settings whether an empty field is added to the line).
    while( true ) {

        // Skip comment lines if needed.
        while( comment_chars_.find( *it ) != std::string::npos ) {
            skip_until( it, '\n' );
            assert( *it == '\n' );
            ++it;
        }

        auto field = parse_field( it );
        ++field_count;

        // Store the field if it has content. If not, store it anyway if we do not want to
        // merge adjacent separators (i.e., leave out empty fields).
        if( field.size() > 0 || ! merge_separators_ ) {
            result.push_back( field );
        }

        // No more input or end of the line. Leave.
        if( ! it || *it == '\n' ) {
            // We can go to the next char even if its the end of the stream. Nothing bad happens.
            ++it;

            // Skip empty lines and continue parsing, if needed.
            // We need the additional field counter to make sure that we do not skip lines that
            // "seem" empty because all their fields are empty and were merged (in case
            // merge_separator is true).
            if( skip_empty_lines_
                && field_count == 1
                && std::all_of( field.begin(), field.end(), isblank )
            ) {

                // Special case: The file ends on an empty line.
                // We then return an empty vector as a sign that there was nothing left -
                // the reader functions will not add a line then.
                if( ! it ) {
                    return std::vector<std::string>();
                }

                // Reset and parse next line.
                result.clear();
                field_count = 0;
                continue;
            }

            // If this was not an empty line that we skipped, we are done with this line.
            break;
        }

        // If we are here, parse_field left the stream at the separator char. Assert that this
        // is the case. We do not want to check this in release, as it is expensive.
        // Move to the next char, so that we can scan the next field.
        assert( separator_chars_.find( *it ) != std::string::npos );
        ++it;
    }

    // Special case: Merge separators is set to true and all fields were empty. This results
    // in no content, but we at least want to return one empty field for that line.
    if( result.size() == 0 ) {
        assert( merge_separators_ == true );
        result.push_back( "" );
    }

    return result;
}

// =================================================================================================
//     Properties
// =================================================================================================

// ---------------------------------------------------------------------
//     comment_chars
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
CsvReader& CsvReader::comment_chars( std::string const& chars )
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
std::string const& CsvReader::comment_chars() const
{
    return comment_chars_;
}

// ---------------------------------------------------------------------
//     trim_chars
// ---------------------------------------------------------------------

/**
 * @brief Set chars that are trimmed from the start and end of each field.
 *
 * By default, no chars are trimmed. Use this function to change that behaviour, e.g., to trim
 * spaces and tabs. Be aware that according to some CSV definitions, blanks are considered
 * to be part of the field and should not be trimmed.
 *
 * The function returns a reference to the CsvReader object in order to allow a fluent interface.
 */
CsvReader& CsvReader::trim_chars( std::string const& chars )
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
std::string const& CsvReader::trim_chars() const
{
    return trim_chars_;
}

// ---------------------------------------------------------------------
//     quotation_chars
// ---------------------------------------------------------------------

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
CsvReader& CsvReader::quotation_chars( std::string const& chars )
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
std::string const& CsvReader::quotation_chars() const
{
    return quotation_chars_;
}

// ---------------------------------------------------------------------
//     separator_chars
// ---------------------------------------------------------------------

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
CsvReader& CsvReader::separator_chars( std::string const& chars )
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
std::string const& CsvReader::separator_chars() const
{
    return separator_chars_;
}

// ---------------------------------------------------------------------
//     skip_empty_lines
// ---------------------------------------------------------------------

/**
 * @brief Set whether to skip empty lines.
 *
 * Default is `false`. If set to `true`, all lines that are empty (that is, no content, or just
 * consisting of spaces and tabs) are skipped while reading.
 *
 * The function returns a reference to the CsvReader object in order to allow a fluent interface.
 */
CsvReader& CsvReader::skip_empty_lines( bool value )
{
    skip_empty_lines_ = value;
    return *this;
}

/**
 * @brief Return whether currently empty lines are skipped.
 *
 * See the @link skip_empty_lines( bool value ) setter @endlink of this function for more details.
 */
bool CsvReader::skip_empty_lines() const
{
    return skip_empty_lines_;
}

// ---------------------------------------------------------------------
//     merge_separators
// ---------------------------------------------------------------------

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
CsvReader& CsvReader::merge_separators( bool value )
{
    merge_separators_ = value;
    return *this;
}

/**
 * @brief Return the current setting whether consecutive separators are merged or not.
 *
 * See the @link merge_separators( bool value ) setter @endlink of this function for details.
 */
bool CsvReader::merge_separators() const
{
    return merge_separators_;
}

// ---------------------------------------------------------------------
//     use_escapes
// ---------------------------------------------------------------------

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
CsvReader& CsvReader::use_escapes( bool value )
{
    use_escapes_ = value;
    return *this;
}

/**
 * @brief Return whether backslash escape sequences are used.
 *
 * See the @link use_escapes( bool value ) setter @endlink of this function for details.
 */
bool CsvReader::use_escapes() const
{
    return use_escapes_;
}

// ---------------------------------------------------------------------
//     use_twin_quotes
// ---------------------------------------------------------------------

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
CsvReader& CsvReader::use_twin_quotes( bool value )
{
    use_twin_quotes_ = value;
    return *this;
}

/**
 * @brief Return whether to interpret two consequtive quotation marks as a single ("escaped") one.
 *
 * See the @link use_twin_quotes( bool value ) setter @endlink of this function for details.
 */
bool CsvReader::use_twin_quotes() const
{
    return use_twin_quotes_;
}

} // namespace utils
} // namespace genesis
