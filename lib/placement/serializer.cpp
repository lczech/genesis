/**
 * @brief Implementation of PlacementMapSerializer class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/serializer.hpp"

#include "placement/placement_map.hpp"
#include "placement/newick_adapter.hpp"
#include "tree/newick_processor.hpp"
#include "utils/logging.hpp"
#include "utils/serializer.hpp"

namespace genesis {

unsigned char PlacementMapSerializer::version = 1;

/**
 * @brief Saves the PlacementMap to a binary file that can later be read by using load().
 */
bool PlacementMapSerializer::save (const PlacementMap& map, const std::string& file_name)
{
    // Prepare.
    Serializer ser (file_name);
    if (!ser) {
        LOG_ERR << "Serialization failed.";
        return false;
    }

    // Write header.
    char magic[] = "BPLACE\0\0";
    ser.put_raw(magic, 8);
    ser.put_int<unsigned char>(version);

    // Write tree.
    // TODO if there is a tree serialization in the future, this one could be used here, and in
    // addition to edge numbers, the edge indices can be stored, so that deserialization is easier.
    auto nw = PlacementTreeNewickProcessor();
    ser.put_string(nw.to_string(map.tree()));

    // Write pqueries.
    ser.put_int(map.pqueries().size());
    for (auto& pqry : map.pqueries()) {

        // Write placements.
        ser.put_int(pqry->placements.size());
        for (auto& place : pqry->placements) {
            ser.put_int   (place->edge_num);
            ser.put_float (place->likelihood);
            ser.put_float (place->like_weight_ratio);
            ser.put_float (place->proximal_length);
            ser.put_float (place->pendant_length);
            ser.put_int   (place->parsimony);
        }

        // Write names.
        ser.put_int(pqry->names.size());
        for (auto& name : pqry->names) {
            ser.put_string (name->name);
            ser.put_float  (name->multiplicity);
        }
    }

    return true;
}

/**
 * @brief Loads a PlacementMap from a binary file that was written by using save().
 */
bool PlacementMapSerializer::load (const std::string& file_name, PlacementMap& map)
{
    // Prepare.
    Deserializer des (file_name);
    if (!des) {
        LOG_ERR << "Deserialization failed.";
        return false;
    }

    map.clear();

    // Read and check header.
    std::string magic = des.get_raw_string(8);
    if (strncmp (magic.c_str(), "BPLACE\0\0", 8) != 0) {
        LOG_ERR << "Wrong file format: \"" << magic << "\".";
        return false;
    }
    auto ver = des.get_int<unsigned char>();
    if (ver != version) {
        LOG_ERR << "Wrong serialization version: " << ver;
        return false;
    }

    // Read and check tree.
    auto tree_string = des.get_string();
    if (!PlacementTreeNewickProcessor().from_string(tree_string, map.tree())) {
        LOG_WARN << "Invalid Tree.";
        return false;
    }

    return true;
}

} // namespace genesis
