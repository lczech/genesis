#ifndef GENESIS_SEQUENCE_IO_PHYLIP_READER_H_
#define GENESIS_SEQUENCE_IO_PHYLIP_READER_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "utils/tools/char_lookup.hpp"

#include <iosfwd>
#include <string>
#include <utility>

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

namespace utils {

class CountingIstream;

} // namespace utils

namespace sequence {

class SequenceSet;
class Sequence;

// =================================================================================================
//     Phylip Reader
// =================================================================================================

class PhylipReader
{
public:

    // ---------------------------------------------------------------------
    //     Types and Enums
    // ---------------------------------------------------------------------

    /**
     * @brief Enum to distinguish between the different file variants of Phylip.
     */
    enum class Mode
    {
        kSequential,
        kInterleaved,
        kAutomatic
    };

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    PhylipReader();
    ~PhylipReader() = default;

    PhylipReader( PhylipReader const& ) = default;
    PhylipReader( PhylipReader&& )      = default;

    PhylipReader& operator= ( PhylipReader const& ) = default;
    PhylipReader& operator= ( PhylipReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Parsing
    // ---------------------------------------------------------------------

    std::pair<size_t, size_t> parse_phylip_header(
        utils::CountingIstream& it
    ) const;

    std::string parse_phylip_label(
        utils::CountingIstream& it
    ) const;

    std::string parse_phylip_sequence_line(
        utils::CountingIstream& it
    ) const;

    void parse_phylip_interleaved(
        utils::CountingIstream& it,
        SequenceSet& sset
    ) const;

    void parse_phylip_sequential(
        utils::CountingIstream& it,
        SequenceSet& sset
    ) const;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    void from_stream ( std::istream&      is, SequenceSet& sset ) const;
    void from_file   ( std::string const& fn, SequenceSet& sset ) const;
    void from_string ( std::string const& fs, SequenceSet& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    PhylipReader& mode( Mode value );
    Mode          mode() const;

    PhylipReader& label_length( size_t value );
    size_t        label_length() const;

    PhylipReader& to_upper( bool value );
    bool          to_upper() const;

    PhylipReader& validate_chars( std::string const& chars );
    std::string   validate_chars() const;

    bool is_validating() const;
    utils::CharLookup& valid_char_lookup();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    Mode              mode_         = Mode::kSequential;
    size_t            label_length_ = 0;
    bool              to_upper_     = true;
    utils::CharLookup lookup_;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
