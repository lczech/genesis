#ifndef GENESIS_UTILS_CORE_INDEXED_LIST_H_
#define GENESIS_UTILS_CORE_INDEXED_LIST_H_

/**
 * @brief Header of IndexedList class.
 *
 * @file
 * @ingroup utils
 */

#include <memory>
#include <vector>

#include "utils/core/std.hpp"

namespace genesis {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template <typename T>
class IndexedListIterator;

// =================================================================================================
//     Indexed List
// =================================================================================================

/**
 * @brief
 */
template <typename T>
class IndexedList
{
public:

    // -------------------------------------------------------------------------
    //     Member Types
    // -------------------------------------------------------------------------

    using value_type      = T;

    using       reference =       value_type&;
    using const_reference = const value_type&;
    using       pointer   =       value_type*;
    using const_pointer   = const value_type*;

    using       iterator  = IndexedListIterator<      value_type>;
    using const_iterator  = IndexedListIterator<const value_type>;

    using size_type       = size_t;

    // -------------------------------------------------------------------------
    //     Constructor and Destructor
    // -------------------------------------------------------------------------

    /**
     * @brief Default constructor.
     */
    IndexedList () {};

    /**
     * @brief Fill constructor with default initialization.
     */
    explicit IndexedList (size_type n)
        : IndexedList(n, value_type())
    {}

    /**
     * @brief Fill constructor.
     */
    IndexedList (size_type n, const value_type& val)
    {
        content_.reserve(n);
        for (size_t i = 0; i < n; ++i) {
            content_.push_back(make_unique<value_type>(val));
        }
    }

    /*
    // range (3)
    template <class InputIterator>
    IndexedList (InputIterator first, InputIterator last);
    */

    // copy (4)
    explicit IndexedList (const IndexedList& x) = delete;

    // move (5)
    // IndexedList (IndexedList&& x);

    // initializer list (6)
    // IndexedList (std::initializer_list<value_type> il);

    // copy (1)
    IndexedList& operator= (const IndexedList& x) = delete;

    // move (2)
    // IndexedList& operator= (IndexedList&& x);

    /**
     * @brief Destructor.
     */
    ~IndexedList() = default;

    // -------------------------------------------------------------------------
    //     Iterators
    // -------------------------------------------------------------------------

    iterator begin() noexcept
    {
        // x
    }

    const_iterator begin() const noexcept
    {
        // x
    }

    iterator end() noexcept
    {
        // x
    }

    const_iterator end() const noexcept
    {
        // x
    }

    const_iterator cbegin() const noexcept
    {
        // x
    }

    const_iterator cend() const noexcept
    {
        // x
    }

    // -------------------------------------------------------------------------
    //     Capacity
    // -------------------------------------------------------------------------

    size_type size() const noexcept
    {
        return content_.size();
    }

    bool empty() const noexcept
    {
        return content_.empty();
    }

    // -------------------------------------------------------------------------
    //     Element Access
    // -------------------------------------------------------------------------

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

    // -------------------------------------------------------------------------
    //     Modifiers
    // -------------------------------------------------------------------------

    void push_back()
    {

    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<std::unique_ptr<value_type>> content_;
};

// =================================================================================================
//     Indexed List Iterator
// =================================================================================================

template <typename T>
class IndexedListIterator
{

};

} // namespace genesis

#endif // include guard
