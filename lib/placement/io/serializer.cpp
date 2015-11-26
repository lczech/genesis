/**
 * @brief Implementation of PlacementMapSerializer class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/io/serializer.hpp"

#include "placement/io/newick_processor.hpp"
#include "placement/placement_map.hpp"
#include "tree/io/newick/processor.hpp"
#include "utils/core/logging.hpp"
#include "utils/io/serializer.hpp"

namespace genesis {

// =================================================================================================
//     Version
// =================================================================================================

/**
 * @brief Version of this serialization helper. Is written to the stream and read again to make
 * sure that different versions don't crash inexpectedly.
 */
unsigned char PlacementMapSerializer::version = 1;

// =================================================================================================
//     Save
// =================================================================================================

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
    // TODO !!! remove
    // nw.print_names          = true;
    // nw.print_branch_lengths = true;
    // nw.print_comments       = false;
    // nw.print_tags           = true;
    ser.put_string(nw.to_string(map.tree()));

    // Write pqueries.
    ser.put_int(map.pqueries().size());
    for (auto& pqry : map.pqueries()) {

        // Write placements.
        ser.put_int(pqry->placements.size());
        for (auto& place : pqry->placements) {
            // We set the edge index instead of edge num. This is faster, simpler to resorte, and
            // consinstend with Pquery.add_placement() parameters.
            ser.put_int   (place->edge->index());

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

// =================================================================================================
//     Load
// =================================================================================================

/**
 * @brief Loads a PlacementMap from a binary file that was written by using save().
 */
bool PlacementMapSerializer::load (const std::string& file_name, PlacementMap& map)
{
    // Prepare, check stream status.
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

    // Read pqueries.
    size_t num_pqueries = des.get_int<size_t>();
    for (size_t i = 0; i < num_pqueries; ++i) {
        Pquery* pqry = map.add_pquery();

        // Read placements.
        size_t num_place = des.get_int<size_t>();
        for (size_t p = 0; p < num_place; ++p) {
            // Get edge index, add the placement there.
            size_t edge_idx = des.get_int<size_t>();
            auto   edge     = map.tree().edge_at(edge_idx);
            auto   place    = pqry->emplace_placement(edge);

            place->likelihood        = des.get_float<double>();
            place->like_weight_ratio = des.get_float<double>();
            place->proximal_length   = des.get_float<double>();
            place->pendant_length    = des.get_float<double>();
            place->parsimony         = des.get_int<int>();
        }

        // Read names.
        size_t num_names = des.get_int<size_t>();
        for (size_t n = 0; n < num_names; ++n) {
            auto name = pqry->emplace_name(des.get_string());
            name->multiplicity = des.get_float<double>();
        }
    }

    return des.succeeded();
}

} // namespace genesis
