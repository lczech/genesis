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

    // bool parse_phylip_sequence(
    //     utils::CountingIstream& input_stream,
    //     Sequence&               sequence
    // ) const;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    void from_stream ( std::istream&      is, SequenceSet& sset ) const;
    void from_file   ( std::string const& fn, SequenceSet& sset ) const;
    void from_string ( std::string const& fs, SequenceSet& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    PhylipReader& label_length( size_t value );
    size_t        label_length() const;

    PhylipReader& to_upper( bool value );
    bool          to_upper() const;

    // PhylipReader& validate_chars( std::string const& chars );
    // std::string   validate_chars() const;
    //
    // bool is_validating() const;
    // utils::CharLookup& valid_char_lookup();

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    // utils::CharLookup lookup_;

    bool   to_upper_ = true;
    size_t label_length_ = 0;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
