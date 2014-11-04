#include <string>
#include <fstream>
#include <streambuf>

#include "log.hh"

namespace genesis {
namespace utils {

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

} // namespace utils
} // namespace genesis
