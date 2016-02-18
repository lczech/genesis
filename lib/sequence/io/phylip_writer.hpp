#ifndef GENESIS_SEQUENCE_IO_PHYLIP_WRITER_H_
#define GENESIS_SEQUENCE_IO_PHYLIP_WRITER_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <iosfwd>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class SequenceSet;

// =================================================================================================
//     Phylip Writer
// =================================================================================================

class PhylipWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    PhylipWriter()  = default;
    ~PhylipWriter() = default;

    PhylipWriter( PhylipWriter const& ) = default;
    PhylipWriter( PhylipWriter&& )      = default;

    PhylipWriter& operator= ( PhylipWriter const& ) = default;
    PhylipWriter& operator= ( PhylipWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    // void to_stream_interleaved ( SequenceSet const& sset, std::ostream& os ) const;
    // void to_stream_sequential  ( SequenceSet const& sset, std::ostream& os ) const;

    void        to_stream ( SequenceSet const& sset, std::ostream&      os ) const;
    void        to_file   ( SequenceSet const& sset, std::string const& fn ) const;
    std::string to_string ( SequenceSet const& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    PhylipWriter& label_length( size_t value );
    size_t        label_length() const;

    PhylipWriter& line_length( size_t value );
    size_t        line_length() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t line_length_  = 80;
    size_t label_length_ = 0;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
