#ifndef GENESIS_SEQUENCE_IO_FASTA_WRITER_H_
#define GENESIS_SEQUENCE_IO_FASTA_WRITER_H_

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
//     Fasta Writer
// =================================================================================================

class FastaWriter
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    FastaWriter()  = default;
    ~FastaWriter() = default;

    FastaWriter( FastaWriter const& ) = default;
    FastaWriter( FastaWriter&& )      = default;

    FastaWriter& operator= ( FastaWriter const& ) = default;
    FastaWriter& operator= ( FastaWriter&& )      = default;

    // ---------------------------------------------------------------------
    //     Writing
    // ---------------------------------------------------------------------

    void        to_stream ( SequenceSet const& sset, std::ostream&      os ) const;
    void        to_file   ( SequenceSet const& sset, std::string const& fn ) const;
    std::string to_string ( SequenceSet const& sset ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    FastaWriter& line_length( size_t value );
    size_t       line_length() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    size_t line_length_ = 80;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
