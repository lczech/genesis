#ifndef GENESIS_PLACEMENT_PLACEMENT_MAP_SET_H_
#define GENESIS_PLACEMENT_PLACEMENT_MAP_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <memory>
#include <vector>

#include "placement/placement_map.hpp"

namespace genesis {

// =============================================================================
//     PlacementMapSet
// =============================================================================

class PlacementMapSet
{
public:

    // -----------------------------------------------------
    //     Constructor and Typedefs
    // -----------------------------------------------------

    struct NamedMap
    {
        std::string                   name;
        std::shared_ptr<PlacementMap> map;
    };

    typedef typename std::vector<NamedMap>::iterator       iterator;
    typedef typename std::vector<NamedMap>::const_iterator const_iterator;

    // -----------------------------------------------------
    //     Modifiers
    // -----------------------------------------------------

    void add (const std::string& name, PlacementMap* map);
    void clear();

    // -----------------------------------------------------
    //     Accessors
    // -----------------------------------------------------

    PlacementMap* get_first (const std::string& name);

    inline iterator begin()
    {
        return maps_.begin();
    }

    inline iterator end()
    {
        return maps_.end();
    }

    inline const_iterator cbegin() const
    {
        return maps_.cbegin();
    }

    inline const_iterator cend() const
    {
        return maps_.cend();
    }

    inline const NamedMap& operator [] (const std::size_t index) const
    {
        return maps_[index];
    }

    /**
     * @brief Returns whether the placement map set is empty.
     */
    inline bool empty() const
    {
        return maps_.empty();
    }

    /**
     * @brief Returns the size of the placement map set.
     */
    inline size_t size() const
    {
        return maps_.size();
    }

    // -----------------------------------------------------
    //     Dump & Debug
    // -----------------------------------------------------

    std::string dump (bool full = false);

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

protected:

    std::vector<NamedMap> maps_;

};

} // namespace genesis

#endif // include guard
