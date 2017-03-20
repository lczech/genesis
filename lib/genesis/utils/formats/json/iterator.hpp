#ifndef GENESIS_UTILS_FORMATS_JSON_ITERATOR_H_
#define GENESIS_UTILS_FORMATS_JSON_ITERATOR_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/*
    The code in this and the following source files is a heavily altered adaption of the excellent
    "JSON for Modern C++" library by Niels Lohmann, see https://github.com/nlohmann/json

     * lib/utils/formats/json/document.hpp
     * lib/utils/formats/json/document.cpp
     * lib/utils/formats/json/iterator.hpp

    We adapted the original code by splitting it into different classes, using our naming convention
    and name spaces, removed not needed functionality and wrote our own reading/parsing routines.

    For the files listed above, we need to include the following original license:

    MIT License

    Copyright (c) 2013-2017 Niels Lohmann

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/formats/json/document.hpp"

#include <cassert>
#include <stdexcept>

namespace genesis {
namespace utils {

// =================================================================================================
//     Json Iterator
// =================================================================================================

/**
 * @brief Template for a random access iterator for the @ref JsonDocument class.
 *
 * This class implements a both iterators (iterator and const_iterator) for the @ref JsonDocument
 * class.
 *
 * The class satisfies the following concept requirements:
 * [RandomAccessIterator](http://en.cppreference.com/w/cpp/concept/RandomAccessIterator).
 * The iterator that can be moved to point (forward and backward) to any element in constant time.
 *
 * @note An iterator is called *initialized* when a pointer to a JSON value has been set
 * (e.g., by a constructor or a copy assignment). If the iterator is default-constructed, it is
 * *uninitialized* and most methods are undefined. We use assertions to detect calls on
 * uninitialized iterators.
 *
 * This class is a heavily altered adaption of the excellent
 * [JSON for Modern C++](https://github.com/nlohmann/json) library by Niels Lohmann, see also
 * @link supplement_acknowledgements_code_reuse_json_document Acknowledgements@endlink.
 */
template<typename U>
class JsonIterator : public std::iterator<std::random_access_iterator_tag, U>
{
    // -------------------------------------------------------------------------
    //     Friends and Asserts
    // -------------------------------------------------------------------------

    // Allow JsonDocument to access private members
    friend class JsonDocument;

    // make sure U is JsonDocument or const JsonDocument
    static_assert(
        std::is_same<U, JsonDocument>::value or
        std::is_same<U, const JsonDocument>::value,
        "JsonIterator only accepts (const) JsonDocument."
    );

    // -------------------------------------------------------------------------
    //     Typedefs and Enums
    // -------------------------------------------------------------------------

public:

    using value_type      = typename JsonDocument::value_type;
    using difference_type = typename JsonDocument::difference_type;

    using pointer         = typename std::conditional<std::is_const<U>::value,
        typename JsonDocument::const_pointer,
        typename JsonDocument::pointer
    >::type;
    using reference       = typename std::conditional<std::is_const<U>::value,
        typename JsonDocument::const_reference,
        typename JsonDocument::reference
    >::type;

    using iterator_category = std::bidirectional_iterator_tag;

    // -------------------------------------------------------------------------
    //     Primitive Iterator
    // -------------------------------------------------------------------------

    /**
     * @brief an iterator for primitive JSON types
     *
     * This class models an iterator for primitive JSON types (boolean, number, string). It's only
     * purpose is to allow the iterator/const_iterator classes to "iterate" over primitive values.
     * Internally, the iterator is modeled by a `difference_type` variable. Value begin_value (`0`)
     * models the begin, end_value (`1`) models past the end.
     */
    class PrimitiveIterator
    {
    public:

        // set iterator to a defined beginning
        void set_begin() noexcept
        {
            iterator_ = begin_value;
        }

        // set iterator to a defined past the end
        void set_end() noexcept
        {
            iterator_ = end_value;
        }

        // return whether the iterator can be dereferenced
        constexpr bool is_begin() const noexcept
        {
            return (iterator_ == begin_value);
        }

        // return whether the iterator is at end
        constexpr bool is_end() const noexcept
        {
            return (iterator_ == end_value);
        }

        // return reference to the value to change and compare
        operator difference_type& () noexcept
        {
            return iterator_;
        }

        // return value to compare
        constexpr operator difference_type () const noexcept
        {
            return iterator_;
        }

    private:

        static constexpr difference_type begin_value = 0;
        static constexpr difference_type end_value = begin_value + 1;

        // iterator as signed integer type
        difference_type iterator_ = std::numeric_limits<std::ptrdiff_t>::denorm_min();
    };

    // -------------------------------------------------------------------------
    //     Internal Iterator
    // -------------------------------------------------------------------------

    /**
     * @brief an iterator value
     *
     * @note This structure could easily be a union, but MSVC currently does not allow unions
     * members with complex constructors, see https://github.com/nlohmann/json/pull/105.
     */
    struct InternalIterator
    {
        // iterator for JSON objects
        typename JsonDocument::ObjectType::iterator object_iterator;

        // iterator for JSON arrays
        typename JsonDocument::ArrayType::iterator array_iterator;

        // generic iterator for all other types
        PrimitiveIterator primitive_iterator;

        // create an uninitialized InternalIterator
        InternalIterator() noexcept
            : object_iterator()
            , array_iterator()
            , primitive_iterator()
        {}
    };

    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

public:

    JsonIterator() = default;

    /**
     * @brief Constructor for a given JSON instance.
     */
    explicit JsonIterator( pointer object )
        : object_(object)
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                iterator_.object_iterator = typename JsonDocument::ObjectType::iterator();
                break;
            }

            case JsonDocument::ValueType::kArray: {
                iterator_.array_iterator = typename JsonDocument::ArrayType::iterator();
                break;
            }

            default: {
                iterator_.primitive_iterator = PrimitiveIterator();
                break;
            }
        }
    }

    /**
     * @brief Copy constructor.
     */
    JsonIterator(const JsonIterator& other) noexcept
        : object_(other.object_)
        , iterator_(other.iterator_)
    {}

    /**
     * @brief Copy assignment.
     */
    JsonIterator& operator=(JsonIterator other) noexcept(
        std::is_nothrow_move_constructible<pointer>::value and
        std::is_nothrow_move_assignable<pointer>::value and
        std::is_nothrow_move_constructible<InternalIterator>::value and
        std::is_nothrow_move_assignable<InternalIterator>::value
    ) {
        std::swap(object_, other.object_);
        std::swap(iterator_, other.iterator_);
        return *this;
    }

    // -------------------------------------------------------------------------
    //     Operators
    // -------------------------------------------------------------------------

public:

    /**
     * @brief Return a reference to the value pointed to by the iterator.
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    reference operator*() const
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                assert(iterator_.object_iterator != object_->value_.object->end());
                return iterator_.object_iterator->second;
            }
            case JsonDocument::ValueType::kArray: {
                assert(iterator_.array_iterator != object_->value_.array->end());
                return *iterator_.array_iterator;
            }
            case JsonDocument::ValueType::kNull: {
                throw std::out_of_range("Cannot get value from Json Iterator.");
            }
            default: {
                if( iterator_.primitive_iterator.is_begin() ) {
                    return *object_;
                }

                throw std::out_of_range("Cannot get value from Json Iterator.");
            }
        }
    }

    /**
     * @brief Dereference the Iterator.
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    pointer operator->() const
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                assert(iterator_.object_iterator != object_->value_.object->end());
                return &(iterator_.object_iterator->second);
            }
            case JsonDocument::ValueType::kArray: {
                assert(iterator_.array_iterator != object_->value_.array->end());
                return &*iterator_.array_iterator;
            }
            default: {
                if( iterator_.primitive_iterator.is_begin() ) {
                    return object_;
                }

                throw std::out_of_range("cannot get value");
            }
        }
    }

    /**
     * @brief Post-increment (it++).
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator operator++(int)
    {
        auto result = *this;
        ++(*this);
        return result;
    }

    /**
     * @brief Pre-increment (++it).
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator& operator++()
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                std::advance(iterator_.object_iterator, 1);
                break;
            }
            case JsonDocument::ValueType::kArray: {
                std::advance(iterator_.array_iterator, 1);
                break;
            }
            default: {
                ++iterator_.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /**
     * @brief Post-decrement (it--).
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator operator--(int)
    {
        auto result = *this;
        --(*this);
        return result;
    }

    /**
     * @brief pre-decrement (--it)
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator& operator--()
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                std::advance(iterator_.object_iterator, -1);
                break;
            }
            case JsonDocument::ValueType::kArray: {
                std::advance(iterator_.array_iterator, -1);
                break;
            }
            default: {
                --iterator_.primitive_iterator;
                break;
            }
        }

        return *this;
    }

    /**
     * @brief  comparison: equal
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    bool operator==(const JsonIterator& other) const
    {
        // if objects are not the same, the comparison is undefined
        if( object_ != other.object_ ) {
            throw std::domain_error("Cannot compare Json Iterators of different containers.");
        }

        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                return (iterator_.object_iterator == other.iterator_.object_iterator);
            }
            case JsonDocument::ValueType::kArray: {
                return (iterator_.array_iterator == other.iterator_.array_iterator);
            }
            default: {
                return (iterator_.primitive_iterator == other.iterator_.primitive_iterator);
            }
        }
    }

    /**
     * @brief  comparison: not equal
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    bool operator!=(const JsonIterator& other) const
    {
        return not operator==(other);
    }

    /**
     * @brief  comparison: smaller
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    bool operator<(const JsonIterator& other) const
    {
        // if objects are not the same, the comparison is undefined
        if( object_ != other.object_ ) {
            throw std::domain_error("Cannot compare Json Iterators of different containers.");
        }

        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                throw std::domain_error("Cannot compare order of Json object iterators.");
            }
            case JsonDocument::ValueType::kArray: {
                return (iterator_.array_iterator < other.iterator_.array_iterator);
            }
            default: {
                return (iterator_.primitive_iterator < other.iterator_.primitive_iterator);
            }
        }
    }

    /**
    @brief  comparison: less than or equal
    @pre The iterator is initialized; i.e. `object_ != nullptr`.
    */
    bool operator<=(const JsonIterator& other) const
    {
        return not other.operator < (*this);
    }

    /**
    @brief  comparison: greater than
    @pre The iterator is initialized; i.e. `object_ != nullptr`.
    */
    bool operator>(const JsonIterator& other) const
    {
        return not operator<=(other);
    }

    /**
    @brief  comparison: greater than or equal
    @pre The iterator is initialized; i.e. `object_ != nullptr`.
    */
    bool operator>=(const JsonIterator& other) const
    {
        return not operator<(other);
    }

    /**
     * @brief  add to iterator
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator& operator+=(difference_type i)
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                throw std::domain_error("Cannot use offsets with Json object iterators.");
            }
            case JsonDocument::ValueType::kArray: {
                std::advance(iterator_.array_iterator, i);
                break;
            }
            default: {
                iterator_.primitive_iterator += i;
                break;
            }
        }

        return *this;
    }

    /**
     * @brief  subtract from iterator
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator& operator-=(difference_type i)
    {
        return operator+=(-i);
    }

    /**
     * @brief  add to iterator
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator operator+(difference_type i)
    {
        auto result = *this;
        result += i;
        return result;
    }

    /**
     * @brief  subtract from iterator
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    JsonIterator operator-(difference_type i)
    {
        auto result = *this;
        result -= i;
        return result;
    }

    /**
     * @brief  return difference
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    difference_type operator-(const JsonIterator& other) const
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                throw std::domain_error("Cannot use offsets with Json object iterators.");
            }

            case JsonDocument::ValueType::kArray: {
                return iterator_.array_iterator - other.iterator_.array_iterator;
            }

            default: {
                return iterator_.primitive_iterator - other.iterator_.primitive_iterator;
            }
        }
    }

    /**
     * @brief  access to successor
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    reference operator[](difference_type n) const
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                throw std::domain_error("Cannot use operator[] for Json object iterators.");
            }

            case JsonDocument::ValueType::kArray: {
                return *std::next(iterator_.array_iterator, n);
            }

            case JsonDocument::ValueType::kNull: {
                throw std::out_of_range("Cannot get value from Json Iterator.");
            }

            default: {
                if( iterator_.primitive_iterator == -n ) {
                    return *object_;
                }

                throw std::out_of_range("Cannot get value from Json Iterator.");
            }
        }
    }

    // -------------------------------------------------------------------------
    //     Key Value Access for Objects
    // -------------------------------------------------------------------------

    /**
     * @brief  return the key of an object iterator
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    typename JsonDocument::ObjectType::key_type key() const
    {
        assert(object_ != nullptr);

        if( object_->is_object() ) {
            return iterator_.object_iterator->first;
        }

        throw std::domain_error("Cannot use key() for non-object Json Iterators.");
    }

    /**
     * @brief  return the value of an iterator
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    reference value() const
    {
        return operator*();
    }

    // -------------------------------------------------------------------------
    //     Internal Helpers
    // -------------------------------------------------------------------------

private:

    /**
     * @brief Set the iterator to the first value.
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    void set_begin() noexcept
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                iterator_.object_iterator = object_->value_.object->begin();
                break;
            }
            case JsonDocument::ValueType::kArray: {
                iterator_.array_iterator = object_->value_.array->begin();
                break;
            }
            case JsonDocument::ValueType::kNull: {
                // set to end so begin()==end() is true: null is empty
                iterator_.primitive_iterator.set_end();
                break;
            }
            default: {
                iterator_.primitive_iterator.set_begin();
                break;
            }
        }
    }

    /**
     * @brief Set the iterator past the last value.
     * @pre The iterator is initialized; i.e. `object_ != nullptr`.
     */
    void set_end() noexcept
    {
        assert(object_ != nullptr);

        switch( object_->type_ ) {
            case JsonDocument::ValueType::kObject: {
                iterator_.object_iterator = object_->value_.object->end();
                break;
            }
            case JsonDocument::ValueType::kArray: {
                iterator_.array_iterator = object_->value_.array->end();
                break;
            }
            default: {
                iterator_.primitive_iterator.set_end();
                break;
            }
        }
    }

    // -------------------------------------------------------------------------
    //     Member Data
    // -------------------------------------------------------------------------

private:
    pointer object_ = nullptr;
    InternalIterator iterator_ = InternalIterator();

};

} // namespace utils
} // namespace genesis

#endif // include guard
