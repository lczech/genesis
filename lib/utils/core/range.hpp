#ifndef GENESIS_UTILS_CORE_RANGE_H_
#define GENESIS_UTILS_CORE_RANGE_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

namespace genesis {
namespace utils {

// =================================================================================================
//     Range
// =================================================================================================

/**
 * @brief
 */
template <typename IteratorType>
class Range
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using iterator = IteratorType;

    // -------------------------------------------------------------------------
    //     Constructor and Rule of Five
    // -------------------------------------------------------------------------

    Range () = default;

    template <class Container>
    Range ( Container& cont )
        : begin_( cont.begin() )
        , end_(   cont.end() )
    {}

    template <class Container>
    Range ( Container const& cont )
        : begin_( cont.begin() )
        , end_(   cont.end() )
    {}

    Range( iterator begin, iterator end )
        : begin_(begin)
        , end_(end)
    {}

    Range( Range const& ) = default;
    Range( Range&& )      = default;

    Range& operator= ( Range const& ) = default;
    Range& operator= ( Range&& )      = default;

    ~Range() = default;

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin()
    {
        return begin_;
    }

    iterator end()
    {
        return end_;
    }

    /*
    const_iterator cbegin()
    {
        return RangeIterator<const value_type> (*this);
    }

    const_iterator cend()
    {
        return RangeIterator<const value_type> (*this, size());
    }
    */

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

    /*
    reference operator[] (size_type n)
    {
        return *(content_[n]);
    }

    const_reference operator[] (size_type n) const
    {
        return *(content_[n]);
    }

    reference at (size_type n)
    {
        return *(content_.at(n));
    }

    const_reference at (size_type n) const
    {
        return *(content_.at(n));
    }

    reference front()
    {
        return *(content_.front());
    }

    const_reference front() const
    {
        return *(content_.front());
    }

    reference back()
    {
        return *(content_.back());
    }

    const_reference back() const
    {
        return *(content_.back());
    }
    */

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    iterator begin_;
    iterator end_;

};

} // namespace utils
} // namespace genesis

#endif // include guard
