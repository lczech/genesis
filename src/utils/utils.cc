/**
 * @brief Implementation of utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/utils.hh"

#include <fstream>
#include <streambuf>
#include <string>

#include "utils/logging.hh"

namespace genesis {

// ---------------------------------------------------------
//     Files
// ---------------------------------------------------------

/** @brief Returns true iff the file exists. */
bool FileExists (const std::string& fn)
{
    std::ifstream infile(fn);
    return infile.good();
}

/**
 * @brief Returns the contents of a file as a string.
 *
 * If the file does not exist, a warning is triggered and an emtpty string returned.
 */
std::string FileRead (const std::string& fn)
{
    std::ifstream infile(fn);
    std::string   str;

    if (!infile.good()) {
        LOG_WARN << "Cannot read from file '" << fn << "'.";
        return "";
    }

    infile.seekg(0, std::ios::end);
    str.reserve(infile.tellg());
    infile.seekg(0, std::ios::beg);

    str.assign((std::istreambuf_iterator<char>(infile)),
                std::istreambuf_iterator<char>());
    return str;
}

/** @brief Writes the content of a string to a file. */
bool FileWrite (const std::string& fn, const std::string& content)
{
    // TODO check if path exists, create if not (make a function for that)
    // TODO check if file exists, trigger warning?, check if writable

    std::ofstream outfile(fn);
    if (!outfile.good()) {
        LOG_WARN << "Cannot write to file '" << fn << "'.";
        return false;
    }
    outfile << content;
    return true;
}

// ---------------------------------------------------------
//     Strings and Chars
// ---------------------------------------------------------

/**
 * @brief Returns a string where special chars are replaces by their escape sequence.
 *
 * All new lines are transformed into either \\r or \\n, tabs into \\t.
 * Double quotation marks are preceeded by a backslash, also the backslash itself will be escaped,
 * so that `"` becomes `\"` and `\` becomes `\\`.
 */
std::string StringEscape (const std::string& text)
{
    std::string tmp;
    tmp = StringReplaceAll(text, "\r", "\\r");
    tmp = StringReplaceAll(tmp,  "\n", "\\n");
    tmp = StringReplaceAll(tmp,  "\t", "\\t");
    tmp = StringReplaceAll(tmp,  "\"", "\\\"");
    tmp = StringReplaceAll(tmp,  "\\", "\\\\");
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
std::string StringDeescape (const std::string& text)
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

/**
 * @brief Returns a copy of a string, where all occurences of a search string
 * are replaced by a replace string.
 */
std::string StringReplaceAll (
    const std::string& s, const std::string& search, const std::string& replace
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

} // namespace genesis
