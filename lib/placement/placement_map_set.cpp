/**
 * @brief Implementation of PlacementMapSet class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map_set.hpp"

namespace genesis {

// =============================================================================
//     Modifiers
// =============================================================================

/**
 * @brief
 */
void PlacementMapSet::add (const std::string& name, PlacementMap* map)
{
    maps_.push_back( { name, std::shared_ptr<PlacementMap>(map) } );
}

/**
 * @brief
 */
void PlacementMapSet::clear ()
{
    maps_.clear();
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief
 */
 PlacementMap* PlacementMapSet::get_first(const std::string& name)
{
    auto cm = maps_.begin();
    while (cm != maps_.end()) {
        if (cm->name == name) {
            return cm->map.get();
        }
        ++cm;
    }
    return nullptr;
}

// =============================================================================
//     Dump & Debug
// =============================================================================

/**
 * @brief
 */
std::string PlacementMapSet::dump(bool full)
{
    std::string res = "";
    for (auto& cm : maps_) {
        res += cm.name + "\n";
        if (full) {
            res += cm.map->dump() + "\n";
        }
    }
    return res;
}

} // namespace genesis
