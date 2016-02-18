#ifndef GENESIS_SEQUENCE_IO_FASTA_INPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_IO_FASTA_INPUT_ITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/io/fasta_parser.hpp"
#include "sequence/sequence.hpp"
#include "utils/io/counting_istream.hpp"

#include <iosfwd>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fasta Input Iterator
// =================================================================================================

/**
 * @brief Iterate a input stream and parse it as fasta sequences.
 *
 * This class allows to iterate over an input stream, iterpreting it as fasta sequences, and
 * yielding one such sequence per iteration step. This is useful for processing large files without
 * having to keep them fully in memory.
 *
 * Example:
 *
 *     auto it = FastaInputIterator( some_istream );
 *     while( it.extract_sequence() ) {
 *         std::cout << it->length() << "\n";
 *     }
 *
 * See parse_fasta_sequence() for a description of the expected format.
 */
class FastaInputIterator
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using self_type = FastaInputIterator;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    FastaInputIterator()
        : input_stream_()
    {}

    explicit FastaInputIterator(std::istream& in)
        : input_stream_(in)
    {}

    ~FastaInputIterator() = default;

    FastaInputIterator(self_type const&) = default;
    FastaInputIterator(self_type&&)      = default;

    self_type& operator= (self_type const&) = default;
    self_type& operator= (self_type&&)      = default;

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    Sequence const& operator * ()
    {
        return sequence_;
    }

    Sequence const* operator -> ()
    {
        return &sequence_;
    }

    Sequence get() const
    {
        return sequence_;
    }

    // -------------------------------------------------------------------------
    //     Iteration
    // -------------------------------------------------------------------------

    bool extract_sequence()
    {
        return parse_fasta_sequence( input_stream_, sequence_ );
    }

    bool eos() const
    {
        return input_stream_.eos();
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    utils::CountingIstream input_stream_;
    Sequence               sequence_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
