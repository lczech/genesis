#ifndef GENESIS_SEQUENCE_IO_FASTA_READER_H_
#define GENESIS_SEQUENCE_IO_FASTA_READER_H_

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

class FastaInputIterator;
class SequenceSet;

// =================================================================================================
//     Fasta Reader
// =================================================================================================

class FastaReader
{
public:

    // ---------------------------------------------------------------------
    //     Constructor and Rule of Five
    // ---------------------------------------------------------------------

    FastaReader()  = default;
    ~FastaReader() = default;

    FastaReader( FastaReader const& ) = default;
    FastaReader( FastaReader&& )      = default;

    FastaReader& operator= ( FastaReader const& ) = default;
    FastaReader& operator= ( FastaReader&& )      = default;

    // ---------------------------------------------------------------------
    //     Reading
    // ---------------------------------------------------------------------

    void from_stream ( std::istream&      is, SequenceSet& sset ) const;
    void from_file   ( std::string const& fn, SequenceSet& sset ) const;
    void from_string ( std::string const& fs, SequenceSet& sset ) const;

    // ---------------------------------------------------------------------
    //     Iteration
    // ---------------------------------------------------------------------

    FastaInputIterator iterate_stream ( std::istream&      is ) const;
    FastaInputIterator iterate_file   ( std::string const& fn ) const;
    FastaInputIterator iterate_string ( std::string const& fs ) const;

    // ---------------------------------------------------------------------
    //     Properties
    // ---------------------------------------------------------------------

    FastaReader& to_upper( bool value );
    bool         to_upper() const;

    // ---------------------------------------------------------------------
    //     Members
    // ---------------------------------------------------------------------

private:

    bool to_upper_ = true;

};

} // namespace sequence
} // namespace genesis

#endif // include guard
