/**
 * @brief Implementation of utility functions.
 *
 * @file
 * @ingroup utils
 */

#include "utils/utils.hpp"

#include <algorithm>
#include <dirent.h>
#include <fstream>
#include <streambuf>
#include <string>

#include "utils/logging.hpp"

namespace genesis {

// =============================================================================
//     Files
// =============================================================================

/**
 * @brief Returns true iff the file exists.
 */
bool file_exists (const std::string& fn)
{
    std::ifstream infile(fn);
    return infile.good();
}

/**
 * @brief Returns the contents of a file as a string.
 *
 * If the file does not exist, a warning is triggered and an emtpty string returned.
 */
std::string file_read (const std::string& fn)
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

/**
 * @brief Writes the content of a string to a file.
 */
bool file_write (const std::string& fn, const std::string& content)
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

/**
 * @brief Get a list of files in a directory.
 */
bool dir_list_files (const std::string& dir, std::vector<std::string>& list)
{
    DIR*           dp;
    struct dirent* dirp;

    if((dp  = opendir(dir.c_str())) == nullptr) {
        LOG_WARN << "Cannot open directory '" << dir << "' (Error " << errno << ").";
        return false;
    }
    while ((dirp = readdir(dp)) != nullptr) {
        std::string fn = std::string(dirp->d_name);
        if (fn == "." || fn == "..") {
            continue;
        }
        list.push_back(fn);
    }
    closedir(dp);
    //~ std::sort(list.begin(), list.end());
    return true;
}

// =============================================================================
//     File names
// =============================================================================

/**
 * @brief Returns information about a file.
 */
std::unordered_map<std::string, std::string> file_info (std::string filename)
{
    std::string basename = file_basename(filename);
    std::unordered_map<std::string, std::string> res;

    res["path"]      = file_path(filename);
    res["basename"]  = basename;
    res["filename"]  = file_filename(basename);
    res["extension"] = file_extension(basename);

    return res;
}

/**
 * @brief Return the size of a file.
 */
size_t file_size (std::string filename)
{
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    return static_cast<size_t>(in.tellg());
}

/**
 * @brief Returns the path leading to a file.
 *
 * Does not resolve the path. Simply splits at the last directory separator.
 */
std::string file_path (std::string filename)
{
    const size_t idx = filename.find_last_of("\\/");
    if (idx != std::string::npos)
    {
        filename.erase(idx);
    }
    return filename;
}

/**
 * @brief Remove directory name from file name if present.
 */
std::string file_basename (std::string filename)
{
    const size_t idx = filename.find_last_of("\\/");
    if (idx != std::string::npos)
    {
        filename.erase(0, idx + 1);
    }
    return filename;
}

/**
 * @brief Remove extension if present.
 *
 * Caveat: Does not remove the path. So, if the filename itself does not contain an extension
 * separator ".", but the path does, this will yield an unwanted result. Call file_basename() first.
 */
std::string file_filename (std::string filename)
{
    const size_t idx = filename.rfind('.');
    if (idx != 0 && idx != std::string::npos)
    {
        filename.erase(idx);
    }
    return filename;
}

/**
 * @brief Returns the extension name of a file.
 *
 * Also see file_filename().
 */
std::string file_extension (std::string filename)
{
    const size_t idx = filename.rfind('.');
    if (idx != 0 && idx != std::string::npos)
    {
        filename.erase(0, idx + 1);
    }
    return filename;
}

// =============================================================================
//     Strings and Chars
// =============================================================================

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

} // namespace genesis
