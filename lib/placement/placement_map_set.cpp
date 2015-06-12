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

    // Create a new Placement Map and initialize it with the average branch length tree of all
    // maps in this set, but without any placements.
    auto res = PlacementMap(std::make_shared<PlacementTree>(
        tree_set().average_branch_length_tree()
    ));

    // Add the placements from all maps of this set.
    for (auto& map : maps_) {
        res.merge(*map.map);
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

/**
 * @brief Returns a TreeSet containing all the trees of the Placement Maps.
 */
TreeSet<PlacementTree> PlacementMapSet::tree_set()
{
    TreeSet<PlacementTree> set;
    for (auto& map : maps_) {
        set.add(map.name, map.map->tree_ptr());
    }
    return set;
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
