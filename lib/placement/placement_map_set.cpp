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
// void PlacementMapSet::add (const std::string& name, std::shared_ptr<PlacementMap> map)
// {
//     maps_.push_back( { name, map } );
// }

/**
 * @brief
 */
void PlacementMapSet::clear ()
{
    maps_.clear();
}

/**
 * @brief Returns a PlacementMap where all maps of this set have been merged into.
 *
 * For this method to succeed, all PlacementMaps need to have the same topology, including identical
 * edge_nums and node names. The Tree of the returned PlacementMap has the average branch lenghts
 * from the input trees, using TreeSet::average_branch_length_tree().
 */
PlacementMap PlacementMapSet::merge_all()
{
    // The following operations do a lot of traversals on all trees: first some for the
    // average_branch_length_tree, then for the merging again. This could be turned into
    // less traversals by copying code and doing all in one run. However, at the current point, this
    // method will be called once in the beginning of a program run, and thus it is not necessary to
    // optimize for speed. Instead, we opt for clean, separated and easy code here.

    if (maps_.size() == 0) {
        LOG_WARN << "PlacementMapSet is empty.";
        return PlacementMap();
    }

    // Create a new Placement Map and initialize it with the average branch length tree of all
    // maps in this set, but without any placements.
    auto res = PlacementMap(std::make_shared<PlacementTree>(
        tree_set().average_branch_length_tree()
    ));

    // Copy the rest of the data from the first tree to the averaged tree.
    // This is necessary, because the tree copy constructor does not do this for us.
    // TODO fix this!
    for (size_t i = 0; i < res.tree().node_count(); ++i) {
        res.tree().node_at(i)->name = maps_[0].map.tree().node_at(i)->name;
    }
    for (size_t i = 0; i < res.tree().edge_count(); ++i) {
        res.tree().edge_at(i)->edge_num = maps_[0].map.tree().edge_at(i)->edge_num;
    }

    // Add the placements from all maps of this set.
    // In the merge method, we also check for identical topology (again), but mainly for identical
    // taxa names and edge_nums, which is important for correct merging.
    for (auto& map : maps_) {
        if (!res.merge(map.map)) {
            LOG_WARN << "Cannot merge PlacementMaps with different reference trees.";
            return PlacementMap();
        }
    }

    return res;
}

// =============================================================================
//     Accessors
// =============================================================================

/**
 * @brief Get the first PlacementMap in the set that is stored with a given name.
 */
// std::shared_ptr<PlacementMap> PlacementMapSet::get_first(const std::string& name)
// {
//     auto cm = maps_.begin();
//     while (cm != maps_.end()) {
//         if (cm->name == name) {
//             return cm->map;
//         }
//         ++cm;
//     }
//     return nullptr;
// }

/**
 * @brief Returns a TreeSet containing all the trees of the Placement Maps.
 */
TreeSet<PlacementTree> PlacementMapSet::tree_set()
{
    TreeSet<PlacementTree> set;
    for (auto& map : maps_) {
        // TODO this also needs to be fixed with the shared pointers problem
        set.add(map.name, map.map.tree());
        // set.add(map.name, map.map->tree_ptr());
    }
    return set;
}

// =============================================================================
//     Comparators
// =============================================================================

/**
 * @brief Returns true iff all Trees of the PlacementMaps in the set are identical.
 *
 * This is the case if they have the same topology, node names and edge_nums. However, branch
 * lengths are not checked, because usually those differ slightly.
 */
bool PlacementMapSet::all_identical_trees()
{
    auto comparator = [] (
        PlacementTree::ConstIteratorPreorder& it_l,
        PlacementTree::ConstIteratorPreorder& it_r
    ) {
        return it_l.node()->name     == it_r.node()->name     &&
               it_l.edge()->edge_num == it_r.edge()->edge_num;
    };

    return tree_set().all_equal(comparator);
}

// =============================================================================
//     Dump & Debug
// =============================================================================

/**
 * @brief
 */
std::string PlacementMapSet::dump(bool full)
{
    size_t i = 0;
    std::string res = "";
    for (auto& cm : maps_) {
        res += std::to_string(i) + ": " + cm.name + "\n";
        if (full) {
            res += cm.map.dump() + "\n";
        }
        ++i;
    }
    return res;
}

} // namespace genesis
