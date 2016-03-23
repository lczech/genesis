/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include "placement/io/jplace_writer.hpp"

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "placement/function/operators.hpp"
#include "placement/io/newick_writer.hpp"
#include "placement/sample_set.hpp"
#include "placement/sample.hpp"
#include "tree/io/newick/writer.hpp"
#include "utils/core/fs.hpp"
#include "utils/core/logging.hpp"
#include "utils/core/options.hpp"
#include "utils/core/std.hpp"
#include "utils/io/json/document.hpp"
#include "utils/io/json/writer.hpp"

namespace genesis {
namespace placement {

// =================================================================================================
//     Parsing
// =================================================================================================

// =================================================================================================
//     Printing
// =================================================================================================

/**
 * @brief Write the data of a Sample to a file in `Jplace` format.
 *
 * If the file already exists, the function throws `std::runtime_error`.
 * The function uses utils::file_write. See there for other exceptions that can be thrown.
 */
void JplaceWriter::to_file (const Sample& smp, const std::string filename) const
{
    if( utils::file_exists(filename) ) {
        throw std::runtime_error( "Jplace file '" + filename + "' already exist." );
    }
    std::string ts;
    to_string(smp, ts);
    utils::file_write( ts, filename );
}

/**
 * @brief Store the data of a Sample in a string in `Jplace` format.
 */
void JplaceWriter::to_string (const Sample& smp, std::string&  output) const
{
    output = to_string(smp);
}

/**
 * @brief Return the data of a Sample as a string in `Jplace` format.
 */
std::string JplaceWriter::to_string (const Sample& smp) const
{
    utils::JsonDocument json;
    to_document(smp, json);
    return utils::JsonWriter().to_string(json);
}

/**
 * @brief Store the data of a Sample in a JsonDocument object.
 */
void JplaceWriter::to_document (const Sample& smp, utils::JsonDocument& doc) const
{
    // Simplify the code. Specifying utils::Json... is cumbersome.
    using namespace utils;

    doc.clear();

    // set tree
    auto nwp = PlacementTreeNewickWriter();
    nwp.enable_names(true);
    nwp.enable_branch_lengths(true);
    doc.set("tree", new JsonValueString(nwp.to_string(smp.tree())));

    // set placements
    JsonValueArray* placements_arr = new JsonValueArray();
    for (auto& pqry : smp.pqueries()) {
        JsonValueObject* jpqry      = new JsonValueObject();
        placements_arr->push_back(jpqry);

        // set placements
        JsonValueArray* pqry_p_arr  = new JsonValueArray();
        for( auto const& pqry_place : pqry.placements() ) {
            JsonValueArray* pqry_fields = new JsonValueArray();
            pqry_fields->push_back(new JsonValueNumber(pqry_place.edge_num()));
            pqry_fields->push_back(new JsonValueNumber(pqry_place.likelihood));
            pqry_fields->push_back(new JsonValueNumber(pqry_place.like_weight_ratio));

            // convert from proximal to distal length.
            pqry_fields->push_back(new JsonValueNumber(
                pqry_place.edge().data.branch_length - pqry_place.proximal_length
            ));
            pqry_fields->push_back(new JsonValueNumber(pqry_place.pendant_length));
            pqry_p_arr->push_back(pqry_fields);
        }
        jpqry->set("p", pqry_p_arr);

        // find out whether names have multiplicity
        bool has_nm = false;
        for( auto const& pqry_name : pqry.names() ) {
            has_nm |= ( pqry_name.multiplicity != 0.0 );
        }

        // set named multiplicity / name
        if (has_nm) {
            JsonValueArray* pqry_nm_arr = new JsonValueArray();
            for( auto const& pqry_name : pqry.names() ) {
                JsonValueArray* pqry_nm_val = new JsonValueArray();
                pqry_nm_val->push_back(new JsonValueString(pqry_name.name));
                pqry_nm_val->push_back(new JsonValueNumber(pqry_name.multiplicity));
                pqry_nm_arr->push_back(pqry_nm_val);
            }
            jpqry->set("nm", pqry_nm_arr);
        } else {
            JsonValueArray* pqry_n_arr  = new JsonValueArray();
            for( auto const& pqry_name : pqry.names() ) {
                pqry_n_arr->push_back(new JsonValueString(pqry_name.name));
            }
            jpqry->set("n", pqry_n_arr);
        }
    }
    doc.set("placements", placements_arr);

    // set fields
    JsonValueArray* jfields = new JsonValueArray();
    jfields->push_back(new JsonValueString("edge_num"));
    jfields->push_back(new JsonValueString("likelihood"));
    jfields->push_back(new JsonValueString("like_weight_ratio"));
    jfields->push_back(new JsonValueString("distal_length"));
    jfields->push_back(new JsonValueString("pendant_length"));
    doc.set("fields", jfields);

    // set version
    doc.set("version", new JsonValueNumber(3));

    // set metadata
    JsonValueObject* jmetadata = new JsonValueObject();
    jmetadata->set("invocation", new JsonValueString( utils::Options::get().command_line_string()) );
    doc.set("metadata", jmetadata);
}

} // namespace placement
} // namespace genesis
