/**
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/text/string.hpp"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace genesis {
namespace text {

// =================================================================================================
//     Compare
// =================================================================================================

/**
 * @brief Compare two strings case insensitive.
 */
bool equals_ci( std::string const& lhs, std::string const& rhs)
{
    const size_t sz = lhs.size();
    if( rhs.size() != sz ) {
        return false;
    }
    for( size_t i = 0; i < sz; ++i ) {
        if( tolower( lhs[i] ) != tolower( rhs[i] ) ) {
            return false;
        }
    }
    return true;
}

// =================================================================================================
//     Find and Count
// =================================================================================================

/**
 * @brief Returns the number of non-overlapping occurrences of a substring in a string.
 */
size_t count_substring_occurrences( const std::string& str, const std::string& sub )
{
    if (sub.length() == 0) {
        return 0;
    }

    size_t count = 0;
    for(
        size_t offset = str.find(sub);
        offset != std::string::npos;
        offset = str.find( sub, offset + sub.length() )
    ) {
        ++count;
    }

    return count;
}

std::vector<std::string> split (
    const std::string& str,
    const std::string& delimiters,
    const bool trim_empty
) {
    size_t pos;
    size_t last_pos = 0;

    std::vector<std::string> result;

    while (true) {
        pos = str.find_first_of(delimiters, last_pos);

        if (pos == std::string::npos) {
           pos = str.length();

           if (pos != last_pos || !trim_empty) {
              result.push_back(std::string(str.data() + last_pos, pos - last_pos));
           }

           break;
        } else {
           if(pos != last_pos || !trim_empty) {
              result.push_back(std::string(str.data() + last_pos, pos - last_pos));
           }
        }

        last_pos = pos + 1;
    }

    return result;
}

// =================================================================================================
//     Manipulate
// =================================================================================================

/**
 * @brief Returns a copy of a string, where all occurences of a search string
 * are replaced by a replace string.
 */
std::string replace_all (
    const std::string& text, const std::string& search, const std::string& replace
) {
    std::string tmp = text;
    for (size_t pos = 0; ; pos += replace.length()) {
        pos = tmp.find(search, pos);

        if (pos == std::string::npos)
            break;

        tmp.erase(pos, search.length());
        tmp.insert(pos, replace);
    }
    return tmp;
}

// inline version
/*
void replace_all(
    std::string &s, const std::string &search, const std::string &replace
) {
    for (size_t pos = 0; ; pos += replace.length() ) {
        pos = s.find(search, pos);

        if (pos == string::npos)
            break;

        s.erase(pos, search.length());
        s.insert(pos, replace);
    }
}
*/

/**
 * @brief Returns a copy of the input string, with left trimmed white spaces.
 */
std::string trim_right (
    const std::string& s,
    const std::string& delimiters
) {
    auto const pos = s.find_last_not_of(delimiters);
    if( std::string::npos == pos ) {
        return "";
    } else {
        return s.substr( 0, pos + 1 );
    }
}

/**
 * @brief Returns a copy of the input string, with right trimmed white spaces.
 */
std::string trim_left (
    const std::string& s,
    const std::string& delimiters
) {
    auto const pos = s.find_first_not_of(delimiters);
    if( std::string::npos == pos ) {
        return "";
    } else {
        return s.substr(pos);
    }
}

/**
 * @brief Returns a copy of the input string, with trimmed white spaces.
 */
std::string trim (
    const std::string& s,
    const std::string& delimiters
) {
    return trim_left(trim_right(s, delimiters), delimiters);
}

// =================================================================================================
//     Normalize
// =================================================================================================

/**
 * @brief Returns a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string escape (const std::string& text)
{
    std::string tmp;
    tmp = replace_all(text, "\r", "\\r");
    tmp = replace_all(tmp,  "\n", "\\n");
    tmp = replace_all(tmp,  "\t", "\\t");
    tmp = replace_all(tmp,  "\"", "\\\"");
    tmp = replace_all(tmp,  "\\", "\\\\");
    return tmp;
}

/**
 * @brief Returns a string where the escaped characters are transformed into
 * their respective string form.
 *
 * For example, the escape sequence \\n (backshlash n) will be translated into
 * a new line. The same applies for tabs and carrier returns. All other
 * escaped sequences will simply translate into the second char, e.g., a double
 * backslash will become one backslash.
 */
std::string deescape (const std::string& text)
{
    std::string tmp = "";
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\\') {
            if (i+1 >= text.size()){
                break;
            }
            switch (text[i+1]) {
                case 'r' : tmp += '\r'; break;
                case 'n' : tmp += '\n'; break;
                case 't' : tmp += '\t'; break;
                default  : tmp += text[i+1];
            }
            ++i;
        } else {
            tmp += text[i];
        }
    }
    return tmp;
}

std::string unify_newlines(const std::string& s)
{
    throw std::domain_error("unify_newlines is not yet implemented.");
    return s;
}

// =================================================================================================
//     Output
// =================================================================================================

/**
 * @brief Returns a precise(er than to_string) string representation of the input value.
 */
std::string to_string_precise (double value, const int precision)
{
    std::ostringstream out;
    out << std::fixed << std::setprecision(precision) << value;
    return out.str();
}

} // namespace text
} // namespace genesis
