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

#include "utils/formats/csv/reader.hpp"

#include <assert.h>
#include <stdexcept>

#include "utils/core/fs.hpp"
#include "utils/io/counting_istream.hpp"
#include "utils/io/lexer/parser.hpp"
#include "utils/io/lexer/scanner.hpp"
#include "utils/text/string.hpp"

namespace genesis {
namespace utils {

// =================================================================================================
//     Parsing
// =================================================================================================

/**
 * @brief Parse one field (i.e., one column) of the CSV data and return it.
 *
 * This function reads from a given input stream until the column separator or the end of the line
 * or the end of the stream is found. It furthermore trims the necessary chars from the beginning
 * and end of the field, and handles quoted strings according to the settings of the CsvReader.
 *
 * The stream is left at either the separater char, the new line char, or the end of the file,
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
std::string CsvReader::parse_field( utils::CountingIstream& input_stream ) const
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

    while( it ) {
        // Finish reading at the end of the line or when one of the separator chars is found.
        if( *it == '\n' || separator_chars_.find( *it ) != std::string::npos ) {
            break;
        }

        // Parse quoted strings if needed.
        // We add them to the result, even when they occur in the middle of a field.
        if( quotation_chars_.find( *it ) != std::string::npos ) {
            buffer_ += parse_quoted_string( it, use_escapes_, use_twin_quotes_, false );
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
 * stream) is found. The fields are returned in a vector. The stream is left at either the new line
 * char or the end of the file, if there is no new line.
 *
 * See @link merge_separators( bool value ) merge_separators() @endlink to change the behaviour of
 * this function.
 */
std::vector<std::string> CsvReader::parse_line( utils::CountingIstream& input_stream ) const
{
    // Init.
    auto& it = input_stream;
    std::vector<std::string> result;

    // Read until one of the inner breaking conditions applies.
    // We need this to make sure that the stream can also end with a separator char
    // (it then depends on the settings whether an empty field is added to the line).
    while( true ) {
        auto field = parse_field( it );

        // Store the field if it has content. If not, store it anyway if we do not want to
        // merge adjacent separators (i.e., leave out empty fields).
        if( field.size() > 0 || ! merge_separators_ ) {
            result.push_back( field );
        }

        // No more input. Leave the parser.
        if( ! it ) {
            break;
        }

        // If we are at the end of the line, go to the next one, and leave the parser.
        if( *it == '\n' ) {
            ++it;
            break;
        }

        // If we are here, parse_field left the stream at the separator char. Assert that this
        // is the case. We do not want to check this in release, as it is expensive.
        // Move to the next char, so that we can scan the next field.
        assert( separator_chars_.find( *it ) != std::string::npos );
        ++it;
    }

    return result;
}

// =================================================================================================
//     Properties
// =================================================================================================

// ---------------------------------------------------------------------
//     trim_chars
// ---------------------------------------------------------------------

/**
 * @brief Set chars that are trimmed from the start and end of each field.
 *
 * By default, space and tab chars are trimmed. Use this function
 * to change that behaviour. Be aware that according to some CSV definitions, blanks are considered
 * to bepart of a field and should not be ignored. If you are working with a file that adheres to
 * such a standard, call this function with an empty string to disable trimming.
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
 * '" to this function.
 *
 * See also @link use_escapes( bool value ) use_escapes() @endlink and
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
 *
 */
CsvReader& CsvReader::use_escapes( bool value )
{
    use_escapes_ = value;
    return *this;
}

/**
 * @brief
 */
bool CsvReader::use_escapes() const
{
    return use_escapes_;
}

// ---------------------------------------------------------------------
//     use_twin_quotes
// ---------------------------------------------------------------------

/**
 * @brief
 *
 * Same effect as trim empty in split
 */
CsvReader& CsvReader::use_twin_quotes( bool value )
{
    use_twin_quotes_ = value;
    return *this;
}

/**
 * @brief
 */
bool CsvReader::use_twin_quotes() const
{
    return use_twin_quotes_;
}

} // namespace utils
} // namespace genesis
