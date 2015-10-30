/**
 * @brief Implementation of string utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/string/string.hpp"

#include <algorithm>

namespace genesis {

// =================================================================================================
//     Split
// =================================================================================================

std::vector<std::string> string_split (
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
//     Replace
// =================================================================================================

/**
 * @brief Returns a copy of a string, where all occurences of a search string
 * are replaced by a replace string.
 */
std::string string_replace_all (
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
void string_replace_all(
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

// =================================================================================================
//     Escape
// =================================================================================================

/**
 * @brief Returns a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string string_escape (const std::string& text)
{
    std::string tmp;
    tmp = string_replace_all(text, "\r", "\\r");
    tmp = string_replace_all(tmp,  "\n", "\\n");
    tmp = string_replace_all(tmp,  "\t", "\\t");
    tmp = string_replace_all(tmp,  "\"", "\\\"");
    tmp = string_replace_all(tmp,  "\\", "\\\\");
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
std::string string_deescape (const std::string& text)
{
    // TODO this is not fast. could be better by using char[] (save reallocs)
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

std::string string_unify_newlines(const std::string& s)
{
    // TODO string_unify_newlines
    return s;
}

} // namespace genesis
