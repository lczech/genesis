/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2019 Lucas Czech and HITS gGmbH

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
#include <cassert>
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

CsvReader::Table CsvReader::read( std::shared_ptr<BaseInputSource> source ) const
{
    utils::InputStream it( source );
    return parse_document( it );
}

// =================================================================================================
//     Parse Document
// =================================================================================================

CsvReader::Table CsvReader::parse_document(
    utils::InputStream& input_stream
) const {
    auto& it = input_stream;
    Table result;

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

// =================================================================================================
//     Parse Field
// =================================================================================================

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

// =================================================================================================
//     Parse Line
// =================================================================================================

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

} // namespace utils
} // namespace genesis
