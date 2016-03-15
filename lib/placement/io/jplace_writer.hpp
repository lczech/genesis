#ifndef GENESIS_PLACEMENT_IO_JPLACE_WRITER_H_
#define GENESIS_PLACEMENT_IO_JPLACE_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup placement
 */

#include <string>
#include <vector>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {
    class JsonDocument;
}

namespace placement {
    class Sample;
    class SampleSet;
}

// =================================================================================================
//     Jplace Writer
// =================================================================================================

namespace placement {

/**
 * @brief Write Jplace data.
 *
 * This class provides facilities for writing Jplace data. It supports to write
 *
 *   * to_file()
 *   * to_string()
 *   * to_document()
 *
 * Exemplary usage:
 *
 *     std::string outfile = "path/to/file.jplace";
 *     Sample map;
 *
 *     JplaceWriter()
 *         .to_file( map, outfile );
 *
 * See JplaceReader for more information on the data format. See Sample for more information
 * on the data structure.
 */
class JplaceWriter
{
public:

    // ---------------------------------------------------------------------
    //     Printing
    // ---------------------------------------------------------------------

    bool        to_file     (const Sample& placements,  const std::string   fn);
    void        to_string   (const Sample& placements,        std::string&  jplace);
    std::string to_string   (const Sample& placements);
    void        to_document (const Sample& placements, utils::JsonDocument& doc);
};

} // namespace placement
} // namespace genesis

#endif // include guard
