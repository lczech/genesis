/**
 * @brief Implementation of PlacementMapSet class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/placement_map_set.hpp"

#include "utils/logging.hpp"

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

/**
 * @brief Returns a PlacementMap where all maps of this set have been merged into.
 */
PlacementMap PlacementMapSet::merge_all()
{
    if (maps_.size() == 0) {
        LOG_WARN << "PlacementMapSet is empty.";
        return PlacementMap();
    }

    // TODO use TreeSet::average_branch_length_tree() for the trees!

    auto res = PlacementMap(*maps_.front().map);
    for (size_t i = 1; i < maps_.size(); ++i) {
        res.merge(*maps_[i].map);
    }
    return res;
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief
 */
 std::shared_ptr<PlacementMap> PlacementMapSet::get_first(const std::string& name)
{
    auto cm = maps_.begin();
    while (cm != maps_.end()) {
        if (cm->name == name) {
            return cm->map;
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
