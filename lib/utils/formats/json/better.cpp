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

     * lib/utils/formats/json/better.hpp
     * lib/utils/formats/json/better.cpp
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

#include "utils/formats/json/better.hpp"

#include "utils/formats/json/iterator.hpp"
#include "utils/core/logging.hpp"

#include<algorithm>

namespace genesis {
namespace utils {

// =================================================================================================
//     Constructors and Rule of Five
// =================================================================================================

// ---------------------------------------------------------------------
//     Initializer List Constructor
// ---------------------------------------------------------------------

JsonBetter::JsonBetter(
    std::initializer_list<JsonBetter> init,
    bool type_deduction,
    ValueType manual_type
) {

    // Check if each element is an array with two elements, whose first element is a string
    bool is_an_object = std::all_of(
        init.begin(),
        init.end(),
        []( JsonBetter const& element ) {
            return element.is_array() and element.size() == 2 and element[0].is_string();
        }
    );

    // Adjust type if type deduction is not wanted
    if( not type_deduction ) {
        // if array is wanted, do not create an object though possible
        if( manual_type == ValueType::kArray ) {
            is_an_object = false;
        }

        // if object is wanted but impossible, throw an exception
        if( manual_type == ValueType::kObject and not is_an_object ) {
            throw std::domain_error( "Invalid initializer list for creating Json object ." );
        }
    }

    if( is_an_object ) {
        // the initializer list is a list of pairs -> create object
        type_  = ValueType::kObject;
        value_ = ValueType::kObject;

        std::for_each(
            init.begin(),
            init.end(),
            [this] ( JsonBetter const& element ) {
                value_.object->emplace( *(element[0].value_.string), element[1] );
            }
        );

    } else {
        // the initializer list describes an array -> create array
        type_ = ValueType::kArray;
        value_.array = create<ArrayType>( init );
    }

    assert_invariant();
}

// ---------------------------------------------------------------------
//     Copy Constructor
// ---------------------------------------------------------------------

JsonBetter::JsonBetter( JsonBetter const& other )
    : type_( other.type_ )
{
    // Check that the passed value is valid
    other.assert_invariant();

    switch ( type_ ) {
        case ValueType::kObject: {
            value_ = *other.value_.object;
            break;
        }
        case ValueType::kArray: {
            value_ = *other.value_.array;
            break;
        }
        case ValueType::kString: {
            value_ = *other.value_.string;
            break;
        }
        case ValueType::kBoolean: {
            value_ = other.value_.boolean;
            break;
        }
        case ValueType::kNumberFloat: {
            value_ = other.value_.number_float;
            break;
        }
        case ValueType::kNumberSigned: {
            value_ = other.value_.number_signed;
            break;
        }
        case ValueType::kNumberUnsigned: {
            value_ = other.value_.number_unsigned;
            break;
        }
        default: {
            break;
        }
    }

    assert_invariant();
}

// ---------------------------------------------------------------------
//     Move Constructor
// ---------------------------------------------------------------------

JsonBetter::JsonBetter( JsonBetter&& other )
    : type_(  std::move( other.type_  ))
    , value_( std::move( other.value_ ))
{
    // Check that passed value is valid
    other.assert_invariant();

    // Invalidate payload
    other.type_  = ValueType::kNull;
    other.value_ = {};

    assert_invariant();
}

// ---------------------------------------------------------------------
//     Copy Assignment
// ---------------------------------------------------------------------

JsonBetter& JsonBetter::operator = ( JsonBetter other )
{
    // check that passed value is valid
    other.assert_invariant();

    // Use copy swap idion (with copy being done in the signature).
    using std::swap;
    swap( type_,  other.type_  );
    swap( value_, other.value_ );

    assert_invariant();
    return *this;
}

// ---------------------------------------------------------------------
//     Destructor
// ---------------------------------------------------------------------

JsonBetter::~JsonBetter()
{
    assert_invariant();

    switch( type_ ) {
        case ValueType::kArray: {
            destroy<ArrayType>( value_.array );
            break;
        }
        case ValueType::kObject: {
            destroy<ObjectType>( value_.object );
            break;
        }
        case ValueType::kString: {
            destroy<StringType>( value_.string );
            break;
        }
        default: {
            // Nothing to do for other types.
            break;
        }
    }
}

// =================================================================================================
//     Capacity
// =================================================================================================

bool JsonBetter::empty() const
{
    switch( type_ ) {
        case ValueType::kNull: {
            // null values are empty
            return true;
        }
        case ValueType::kArray: {
            // delegate call to ArrayType::empty()
            return value_.array->empty();
        }
        case ValueType::kObject: {
            // delegate call to ObjectType::empty()
            return value_.object->empty();
        }
        default: {
            // all other types are nonempty
            return false;
        }
    }
}

size_t JsonBetter::size() const
{
    switch( type_ ) {
        case ValueType::kNull: {
            // null values are empty
            return 0;
        }
        case ValueType::kArray: {
            // delegate call to ArrayType::size()
            return value_.array->size();
        }
        case ValueType::kObject: {
            // delegate call to ObjectType::size()
            return value_.object->size();
        }
        default: {
            // all other types have size 1
            return 1;
        }
    }
}

size_t JsonBetter::max_size() const
{
    switch( type_ ) {
        case ValueType::kArray: {
            // delegate call to ArrayType::max_size()
            return value_.array->max_size();
        }
        case ValueType::kObject: {
            // delegate call to ObjectType::max_size()
            return value_.object->max_size();
        }
        default: {
            // all other types have max_size() == size()
            return size();
        }
    }
}

// =================================================================================================
//     Value Access
// =================================================================================================

JsonBetter::ArrayType& JsonBetter::get_array()
{
    if( not is_array() ) {
        throw std::domain_error( "Cannot use get_array() with " + type_name() + "." );
    }
    return *value_.array;
}

JsonBetter::ArrayType const& JsonBetter::get_array() const
{
    if( not is_array() ) {
        throw std::domain_error( "Cannot use get_array() with " + type_name() + "." );
    }
    return *value_.array;
}

JsonBetter::ObjectType& JsonBetter::get_object()
{
    if( not is_object() ) {
        throw std::domain_error( "Cannot use get_object() with " + type_name() + "." );
    }
    return *value_.object;
}

JsonBetter::ObjectType const& JsonBetter::get_object() const
{
    if( not is_object() ) {
        throw std::domain_error( "Cannot use get_object() with " + type_name() + "." );
    }
    return *value_.object;
}

JsonBetter::StringType& JsonBetter::get_string()
{
    if( not is_string() ) {
        throw std::domain_error( "Cannot use get_string() with " + type_name() + "." );
    }
    return *value_.string;
}

JsonBetter::StringType const& JsonBetter::get_string() const
{
    if( not is_string() ) {
        throw std::domain_error( "Cannot use get_string() with " + type_name() + "." );
    }
    return *value_.string;
}

JsonBetter::BooleanType& JsonBetter::get_boolean()
{
    if( not is_boolean() ) {
        throw std::domain_error( "Cannot use get_boolean() with " + type_name() + "." );
    }
    return value_.boolean;
}

JsonBetter::BooleanType JsonBetter::get_boolean() const
{
    if( not is_boolean() ) {
        throw std::domain_error( "Cannot use get_boolean() with " + type_name() + "." );
    }
    return value_.boolean;
}

JsonBetter::NumberFloatType& JsonBetter::get_number_float()
{
    if( not is_number_float() ) {
        throw std::domain_error( "Cannot use get_number_float() with " + type_name() + "." );
    }
    return value_.number_float;
}

JsonBetter::NumberFloatType JsonBetter::get_number_float() const
{
    if( not is_number_float() ) {
        throw std::domain_error( "Cannot use get_number_float() with " + type_name() + "." );
    }
    return value_.number_float;
}

JsonBetter::NumberSignedType& JsonBetter::get_number_signed()
{
    if( not is_number_signed() ) {
        throw std::domain_error( "Cannot use get_number_signed() with " + type_name() + "." );
    }
    return value_.number_signed;
}

JsonBetter::NumberSignedType JsonBetter::get_number_signed() const
{
    if( not is_number_signed() ) {
        throw std::domain_error( "Cannot use get_number_signed() with " + type_name() + "." );
    }
    return value_.number_signed;
}

JsonBetter::NumberUnsignedType& JsonBetter::get_number_unsigned()
{
    if( not is_number_unsigned() ) {
        throw std::domain_error( "Cannot use get_number_unsigned() with " + type_name() + "." );
    }
    return value_.number_unsigned;
}

JsonBetter::NumberUnsignedType JsonBetter::get_number_unsigned() const
{
    if( not is_number_unsigned() ) {
        throw std::domain_error( "Cannot use get_number_unsigned() with " + type_name() + "." );
    }
    return value_.number_unsigned;
}

// =================================================================================================
//     Element Access
// =================================================================================================

JsonBetter& JsonBetter::at( size_t index )
{
    // This code is also copied to the const version of the function.
    if( is_array() ) {
        try{
            return value_.array->at( index );
        } catch( ... ) {
            throw std::out_of_range(
                "Array index " + std::to_string( index ) + " is out of range."
            );
        }
    } else {
        throw std::domain_error( "Cannot use at() with " + type_name() );
    }
}

JsonBetter const& JsonBetter::at( size_t index ) const
{
    // Copied from non-const version of the function.
    if( is_array() ) {
        try{
            return value_.array->at( index );
        } catch( ... ) {
            throw std::out_of_range(
                "Array index " + std::to_string( index ) + " is out of range."
            );
        }
    } else {
        throw std::domain_error( "Cannot use at() with " + type_name() );
    }
}

JsonBetter& JsonBetter::at( typename ObjectType::key_type const& key )
{
    // This code is also copied to the const version of the function.
    if( is_object() ) {
        try{
            return value_.object->at( key );
        } catch( ... ) {
            throw std::out_of_range(
                "Invalid key '" + key + "' for object access."
            );
        }
    } else {
        throw std::domain_error( "Cannot use at() with " + type_name() );
    }
}

JsonBetter const& JsonBetter::at( typename ObjectType::key_type const& key ) const
{
    // Copied from non-const version of the function.
    if( is_object() ) {
        try{
            return value_.object->at( key );
        } catch( ... ) {
            throw std::out_of_range(
                "Invalid key '" + key + "' for object access."
            );
        }
    } else {
        throw std::domain_error( "Cannot use at() with " + type_name() );
    }
}

JsonBetter& JsonBetter::operator [] ( size_t index )
{
    // Implicitly convert null value to an empty array
    if( is_null( )) {
        type_ = ValueType::kArray;
        value_.array = create<ArrayType>();
        assert_invariant();
    }

    if( is_array() ) {
        // fill up array with null values if given idx is outside range
        if( index >= value_.array->size() ) {
            value_.array->insert(
                value_.array->end(),
                 index - value_.array->size() + 1,
                 JsonBetter()
             );
        }

        return value_.array->operator[]( index );
    } else {
        throw std::domain_error( "Cannot use operator[] with " + type_name() );
    }
}

JsonBetter const& JsonBetter::operator [] ( size_t index ) const
{
    if( is_array() ) {
        return value_.array->operator[]( index );
    } else {
        throw std::domain_error( "Cannot use operator[] with " + type_name() );
    }
}

JsonBetter& JsonBetter::operator [] ( typename ObjectType::key_type const& key )
{
    // implicitly convert null value to an empty object
    if( is_null() ) {
        type_ = ValueType::kObject;
        value_.object = create<ObjectType>();
        assert_invariant();
    }

    // operator[] only works for objects
    if( is_object() ) {
        return value_.object->operator[]( key );
    } else {
        throw std::domain_error( "Cannot use operator[] with " + type_name() );
    }
}

JsonBetter const& JsonBetter::operator [] ( typename ObjectType::key_type const& key ) const
{
    // const operator[] only works for objects
    if( is_object() ) {
        assert(value_.object->find(key) != value_.object->end());
        return value_.object->find(key)->second;
    } else {
        throw std::domain_error( "Cannot use operator[] with " + type_name() );
    }
}

// =================================================================================================
//     Iterators
// =================================================================================================

JsonBetter::iterator JsonBetter::begin() noexcept
{
    iterator result(this);
    result.set_begin();
    return result;
}

JsonBetter::const_iterator JsonBetter::begin() const noexcept
{
    return cbegin();
}

JsonBetter::const_iterator JsonBetter::cbegin() const noexcept
{
    const_iterator result(this);
    result.set_begin();
    return result;
}

JsonBetter::iterator JsonBetter::end() noexcept
{
    iterator result(this);
    result.set_end();
    return result;
}

JsonBetter::const_iterator JsonBetter::end() const noexcept
{
    return cend();
}

JsonBetter::const_iterator JsonBetter::cend() const noexcept
{
    const_iterator result(this);
    result.set_end();
    return result;
}

// =================================================================================================
//     Modifiers
// =================================================================================================

void JsonBetter::clear()
{
    switch( type_ ) {
        case ValueType::kNumberSigned: {
            value_.number_signed = 0;
            break;
        }
        case ValueType::kNumberUnsigned: {
            value_.number_unsigned = 0;
            break;
        }
        case ValueType::kNumberFloat: {
            value_.number_float = 0.0;
            break;
        }
        case ValueType::kBoolean: {
            value_.boolean = false;
            break;
        }
        case ValueType::kString: {
            value_.string->clear();
            break;
        }
        case ValueType::kArray: {
            value_.array->clear();
            break;
        }
        case ValueType::kObject: {
            value_.object->clear();
            break;
        }
        default: {
            break;
        }
    }
}

void JsonBetter::push_back( JsonBetter&& val )
{
    // push_back only works for null objects or arrays
    if( not(is_null() or is_array()) ) {
        throw std::domain_error("Cannot use push_back() with " + type_name());
    }

    // transform null object into an array
    if( is_null() ) {
        type_ = ValueType::kArray;
        value_ = ValueType::kArray;
        assert_invariant();
    }

    // add element to array (move semantics)
    value_.array->push_back(std::move(val));
    // invalidate object
    val.type_ = ValueType::kNull;
}

void JsonBetter::push_back( JsonBetter const& val )
{
    // push_back only works for null objects or arrays
    if( not(is_null() or is_array()) ) {
        throw std::domain_error("cannot use push_back() with " + type_name());
    }

    // transform null object into an array
    if( is_null() ) {
        type_ = ValueType::kArray;
        value_ = ValueType::kArray;
        assert_invariant();
    }

    // add element to array
    value_.array->push_back(val);
}

void JsonBetter::push_back( typename ObjectType::value_type const& val )
{
    // push_back only works for null objects or objects
    if( not(is_null() or is_object()) ) {
        throw std::domain_error("Cannot use push_back() with " + type_name());
    }

    // transform null object into an object
    if( is_null() ) {
        type_ = ValueType::kObject;
        value_ = ValueType::kObject;
        assert_invariant();
    }

    // add element to array
    value_.object->insert(val);
}

// =================================================================================================
//     Lexicographical Comparison Operators
// =================================================================================================

bool operator==( JsonBetter::const_reference lhs, JsonBetter::const_reference rhs ) noexcept
{
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    // Shorthands
    using ValueType          = JsonBetter::ValueType;
    using NumberFloatType    = JsonBetter::NumberFloatType;
    using NumberSignedType   = JsonBetter::NumberSignedType;

    if( lhs_type == rhs_type ) {
        switch( lhs_type ) {
            case ValueType::kArray: {
                return *lhs.value_.array == *rhs.value_.array;
            }
            case ValueType::kObject: {
                return *lhs.value_.object == *rhs.value_.object;
            }
            case ValueType::kNull: {
                return true;
            }
            case ValueType::kString: {
                return *lhs.value_.string == *rhs.value_.string;
            }
            case ValueType::kBoolean: {
                return lhs.value_.boolean == rhs.value_.boolean;
            }
            case ValueType::kNumberSigned: {
                return lhs.value_.number_signed == rhs.value_.number_signed;
            }
            case ValueType::kNumberUnsigned: {
                return lhs.value_.number_unsigned == rhs.value_.number_unsigned;
            }
            case ValueType::kNumberFloat: {
                return lhs.value_.number_float == rhs.value_.number_float;
            }
            default: {
                return false;
            }
        }
    }
    else if( lhs_type == ValueType::kNumberSigned and rhs_type == ValueType::kNumberFloat ) {
        return static_cast<NumberFloatType>(lhs.value_.number_signed) == rhs.value_.number_float;
    }
    else if( lhs_type == ValueType::kNumberFloat and rhs_type == ValueType::kNumberSigned ) {
        return lhs.value_.number_float == static_cast<NumberFloatType>(rhs.value_.number_signed);
    }
    else if( lhs_type == ValueType::kNumberUnsigned and rhs_type == ValueType::kNumberFloat ) {
        return static_cast<NumberFloatType>(lhs.value_.number_unsigned) == rhs.value_.number_float;
    }
    else if( lhs_type == ValueType::kNumberFloat and rhs_type == ValueType::kNumberUnsigned ) {
        return lhs.value_.number_float == static_cast<NumberFloatType>(rhs.value_.number_unsigned);
    }
    else if( lhs_type == ValueType::kNumberUnsigned and rhs_type == ValueType::kNumberSigned ) {
        return static_cast<NumberSignedType>(lhs.value_.number_unsigned) == rhs.value_.number_signed;
    }
    else if( lhs_type == ValueType::kNumberSigned and rhs_type == ValueType::kNumberUnsigned ) {
        return lhs.value_.number_signed == static_cast<NumberSignedType>(rhs.value_.number_unsigned);
    }

    return false;
}

bool operator<(JsonBetter::const_reference lhs, JsonBetter::const_reference rhs) noexcept
{
    const auto lhs_type = lhs.type();
    const auto rhs_type = rhs.type();

    // Shorthands
    using ValueType          = JsonBetter::ValueType;
    using NumberFloatType    = JsonBetter::NumberFloatType;
    using NumberSignedType   = JsonBetter::NumberSignedType;

    if( lhs_type == rhs_type ) {
        switch( lhs_type ) {
            case ValueType::kArray: {
                return *lhs.value_.array < *rhs.value_.array;
            }
            case ValueType::kObject: {
                return *lhs.value_.object < *rhs.value_.object;
            }
            case ValueType::kNull: {
                return false;
            }
            case ValueType::kString: {
                return *lhs.value_.string < *rhs.value_.string;
            }
            case ValueType::kBoolean: {
                return lhs.value_.boolean < rhs.value_.boolean;
            }
            case ValueType::kNumberSigned: {
                return lhs.value_.number_signed < rhs.value_.number_signed;
            }
            case ValueType::kNumberUnsigned: {
                return lhs.value_.number_unsigned < rhs.value_.number_unsigned;
            }
            case ValueType::kNumberFloat: {
                return lhs.value_.number_float < rhs.value_.number_float;
            }
            default: {
                return false;
            }
        }
    }
    else if( lhs_type == ValueType::kNumberSigned and rhs_type == ValueType::kNumberFloat ) {
        return static_cast<NumberFloatType>(lhs.value_.number_signed) < rhs.value_.number_float;
    }
    else if( lhs_type == ValueType::kNumberFloat and rhs_type == ValueType::kNumberSigned ) {
        return lhs.value_.number_float < static_cast<NumberFloatType>(rhs.value_.number_signed);
    }
    else if( lhs_type == ValueType::kNumberUnsigned and rhs_type == ValueType::kNumberFloat ) {
        return static_cast<NumberFloatType>(lhs.value_.number_unsigned) < rhs.value_.number_float;
    }
    else if( lhs_type == ValueType::kNumberFloat and rhs_type == ValueType::kNumberUnsigned ) {
        return lhs.value_.number_float < static_cast<NumberFloatType>(rhs.value_.number_unsigned);
    }
    else if( lhs_type == ValueType::kNumberSigned and rhs_type == ValueType::kNumberUnsigned ) {
        return lhs.value_.number_signed < static_cast<NumberSignedType>(rhs.value_.number_unsigned);
    }
    else if( lhs_type == ValueType::kNumberUnsigned and rhs_type == ValueType::kNumberSigned ) {
        return static_cast<NumberSignedType>(lhs.value_.number_unsigned) < rhs.value_.number_signed;
    }

    // We only reach this line if we cannot compare values. In that case,
    // we compare types. Note we have to call the operator explicitly,
    // because MSVC has problems otherwise.
    return operator<(lhs_type, rhs_type);
}

bool operator<(const JsonBetter::ValueType lhs, const JsonBetter::ValueType rhs) noexcept
{
    // Use the order of the ValueType enum!
    static constexpr std::array<uint8_t, 8> order = {{
            0, // null
            4, // array
            3, // object
            5, // string
            1, // boolean
            2, // float
            2, // signed
            2, // unsigned
        }
    };
    return order[static_cast<std::size_t>(lhs)] < order[static_cast<std::size_t>(rhs)];
}

// =================================================================================================
//     Convenience Functions
// =================================================================================================

std::string JsonBetter::type_name() const
{
    switch( type_ ) {
        case ValueType::kNull: {
            return "null";
        }
        case ValueType::kArray: {
            return "array";
        }
        case ValueType::kObject: {
            return "object";
        }
        case ValueType::kString: {
            return "string";
        }
        case ValueType::kBoolean: {
            return "boolean";
        }
        case ValueType::kNumberFloat: {
            return "float";
        }
        case ValueType::kNumberSigned: {
            return "signed integer";
        }
        case ValueType::kNumberUnsigned: {
            return "unsigned integer";
        }
        default: {
            assert( false );
        }
    };
}

} // namespace utils
} // namespace genesis
