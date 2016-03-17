/**
 * @brief Implementation of SampleSerializer class.
 *
 * @file
 * @ingroup placement
 */

#include "placement/io/serializer.hpp"

#include "placement/io/newick_processor.hpp"
#include "placement/sample.hpp"
#include "tree/io/newick/processor.hpp"
#include "utils/core/logging.hpp"
#include "utils/io/serializer.hpp"

#include <stdexcept>

namespace genesis {
namespace placement {

// =================================================================================================
//     Version
// =================================================================================================

/**
 * @brief Version of this serialization helper. Is written to the stream and read again to make
 * sure that different versions don't crash inexpectedly.
 */
unsigned char SampleSerializer::version = 1;

// =================================================================================================
//     Save
// =================================================================================================

/**
 * @brief Saves the Sample to a binary file that can later be read by using load().
 */
void SampleSerializer::save (const Sample& map, const std::string& file_name)
{
    // Prepare.
    utils::Serializer ser (file_name);
    if (!ser) {
        throw std::invalid_argument("Serialization failed.");
    }

    // Write header.
    char magic[] = "BPLACE\0\0";
    ser.put_raw(magic, 8);
    ser.put_int<unsigned char>(version);

    // Write tree.
    // TODO if there is a tree serialization in the future, this one could be used here, and in
    // addition to edge numbers, the edge indices can be stored, so that deserialization is easier.
    auto nw = PlacementTreeNewickProcessor();
    nw.enable_names(true);
    nw.enable_branch_lengths(true);
    ser.put_string(nw.to_string(map.tree()));

    // Write pqueries.
    ser.put_int(map.pqueries().size());
    for (auto& pqry : map.pqueries()) {

        // Write placements.
        ser.put_int(pqry->placement_size());
        for( auto pit = pqry->begin_placements(); pit != pqry->end_placements(); ++pit ) {
            auto const& place = *pit;

            // We set the edge index instead of edge num. This is faster, simpler to resorte, and
            // consinstend with Pquery.add_placement() parameters.
            ser.put_int   (place.edge().index());

            ser.put_float (place.likelihood);
            ser.put_float (place.like_weight_ratio);
            ser.put_float (place.proximal_length);
            ser.put_float (place.pendant_length);
            ser.put_int   (place.parsimony);
        }

        // Write names.
        ser.put_int(pqry->name_size());
        for( auto name_it = pqry->begin_names(); name_it != pqry->end_names(); ++name_it ) {
            auto& name = *name_it;
            ser.put_string (name.name);
            ser.put_float  (name.multiplicity);
        }
    }
}

// =================================================================================================
//     Load
// =================================================================================================

/**
 * @brief Loads a Sample from a binary file that was written by using save().
 */
void SampleSerializer::load (const std::string& file_name, Sample& map)
{
    // Prepare, check stream status.
    utils::Deserializer des (file_name);
    if (!des) {
        throw std::invalid_argument("Deserialization failed.");
    }

    map.clear();

    // Read and check header.
    std::string magic = des.get_raw_string(8);
    if (strncmp (magic.c_str(), "BPLACE\0\0", 8) != 0) {
        throw std::invalid_argument("Wrong file format: \"" + magic + "\".");
    }
    auto ver = des.get_int<unsigned char>();
    if (ver != version) {
        throw std::invalid_argument("Wrong serialization version: " + std::to_string(ver));
    }

    // Read and check tree.
    auto tree_string = des.get_string();
    if (!PlacementTreeNewickProcessor().from_string(tree_string, map.tree())) {
        throw std::invalid_argument("Invalid Tree.");
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
            auto&  place    = pqry->add_placement(*edge);

            place.likelihood        = des.get_float<double>();
            place.like_weight_ratio = des.get_float<double>();
            place.proximal_length   = des.get_float<double>();
            place.pendant_length    = des.get_float<double>();
            place.parsimony         = des.get_int<int>();
        }

        // Read names.
        size_t num_names = des.get_int<size_t>();
        for (size_t n = 0; n < num_names; ++n) {
            auto name = pqry->add_name( des.get_string() );
            name.multiplicity = des.get_float<double>();
        }
    }

    if (!des.succeeded()) {
        throw std::invalid_argument("Deserialization failed.");
    }
}

} // namespace placement
} // namespace genesis
