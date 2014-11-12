/**
 * @brief Implementation of utility functions.
 *
 * @file
 * @ingroup utils
 */

 #include <string>
#include <fstream>
#include <streambuf>

#include "logging.hh"

namespace genesis {
namespace utils {

/**
 * @brief Returns the contents of a file as a string.
 *
 * If the file does not exist, a warning is triggered and an emtpty string
 * returned.
 */
std::string ReadFile (const std::string fn)
{
    std::ifstream t(fn);
    std::string str;

    if (!t.good()) {
        LOG_WARN << "File '" << fn << "' does not exist.";
        return "";
    }

    t.seekg(0, std::ios::end);
    str.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(t)),
                std::istreambuf_iterator<char>());
    return str;
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
std::string StringDeescape (const std::string text)
{
    // TODO this is not fast. could be better by using char[] (save reallocs)
    std::string tmp = "";
    for (size_t i = 0; i < text.size(); i++) {
        if (text[i] == '\\') {
            if (i+1 >= text.size()){
                break;
            }
            switch (text[i+1]) {
                case 'n' : tmp += '\n'; break;
                case 't' : tmp += '\t'; break;
                case 'r' : tmp += '\r'; break;
                default  : tmp += text[i+1];
            }
            ++i;
        } else {
            tmp += text[i];
        }
    }
    return tmp;
}

/**
 * @brief Returns a copy of a string, where all occurences of a search string
 * are replaced by a replace string.
 */
std::string StringReplaceAll (
    const std::string &s, const std::string &search, const std::string &replace
) {
    std::string tmp = s;
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
void StringReplaceAll(
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

} // namespace utils
} // namespace genesis
