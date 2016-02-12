#ifndef GENESIS_SEQUENCE_IO_FASTA_INPUT_ITERATOR_H_
#define GENESIS_SEQUENCE_IO_FASTA_INPUT_ITERATOR_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include "sequence/sequence.hpp"
#include "utils/io/counting_istream.hpp"

#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace genesis {
namespace sequence {

// =================================================================================================
//     Fasta Input Iterator
// =================================================================================================

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
        if( eof() ) {
            return false;
        }

        // Readability.
        auto& it = input_stream_;

        // Check beginning of sequence.
        if( it.current() != '>' ) {
            throw std::runtime_error(
                "Malformed fasta file: Expecting '>' at beginning of sequence at " + it.at() + "."
            );
        }
        assert( *it == '>' );
        ++it;

        // Parse label.
        sequence_.label().clear();
        while( it && (*it != '\n') && (*it != ' ') ) {
            sequence_.label() += *it;
            ++it;
        }

        // Check for unexpected end of file.
        if( it.eof() ) {
            throw std::runtime_error(
                "Malformed fasta file: Expecting a sequence after the label line at "
                + it.at() + "."
            );
        }
        assert( *it == '\n' || *it == ' ' );

        // Parse metadata.
        sequence_.metadata().clear();
        if( *it == ' ' ) {
            ++it;
            while( it && *it != '\n' ) {
                sequence_.metadata() += *it;
                ++it;
            }
        }

        // Skip comments.
        while( it && *it == ';' ) {
            while( it && *it != '\n' ) {
                ++it;
            }
        }

        // Check for unexpected end of file.
        if( it.eof() || *it == '>' ) {
            throw std::runtime_error(
                "Malformed fasta file: Expecting a sequence after the label line at "
                + it.at() + "."
            );
        }
        assert( *it == '\n' );
        ++it;

        // Parse sequence. At every beginning of the outer loop, we are at a line start.
        sequence_.sites().clear();
        while( it ) {
            assert( it.column() == 1 );

            if( *it == '>' ) {
                break;
            }
            while( it && *it != '\n' ) {
                sequence_.sites() += *it;
                ++it;
            }
            if( it ) {
                assert( *it == '\n' );
                ++it;
            }
        }

        return true;
    }

    bool eof() const
    {
        return input_stream_.eof();
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
