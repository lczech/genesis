/**
 * @brief Documentation strings for the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <map>
#include <string>
#include <utility>

static std::map<std::pair<std::string, std::string>, std::string> doc_strings_ = {
    {{"PlacementMap", "clear"}, "Clears all data of this object. XYZ"},
    {{"PlacementMap", "merge"}, "Adds the pqueries from another ::genesis::PlacementMap objects to this one. ABC"}
};

const char* get_docstring (const std::string& classname, const std::string& functionname)
{
    if (doc_strings_.count({classname, functionname}) > 0) {
        return doc_strings_[{classname, functionname}].c_str();
    } else {
        return "";
    }
}
