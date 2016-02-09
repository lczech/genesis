#ifndef GENESIS_SEQUENCE_SEQUENCE_SET_H_
#define GENESIS_SEQUENCE_SEQUENCE_SET_H_

/**
 * @brief
 *
 * @file
 * @ingroup sequence
 */

#include <vector>

#include "sequence/sequence.hpp"

namespace genesis {
namespace sequence {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

class Sequence;

// =================================================================================================
//     SequenceSet
// =================================================================================================

class SequenceSet
{
public:

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

    typedef std::vector<Sequence>::iterator       iterator;
    typedef std::vector<Sequence>::const_iterator const_iterator;

    typedef Sequence&       reference;
    typedef Sequence const& const_reference;

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    SequenceSet() = default;
    ~SequenceSet() = default;

    SequenceSet( SequenceSet const& ) = default;
    SequenceSet( SequenceSet&& )      = default;

    SequenceSet& operator= ( SequenceSet const& ) = default;
    SequenceSet& operator= ( SequenceSet&& )      = default;

    void swap( SequenceSet& other )
    {
        using std::swap;
        swap( sequences_, other.sequences_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    size_t size() const;

    reference       operator[] (size_t index);
    const_reference operator[] (size_t index) const;

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void push_back( Sequence const& s );
    void push_back( Sequence &&     s );

    void clear();

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    const_iterator cbegin() const;
    const_iterator cend() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<Sequence> sequences_;
};

} // namespace sequence
} // namespace genesis

#endif // include guard
