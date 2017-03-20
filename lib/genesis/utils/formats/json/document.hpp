#ifndef GENESIS_UTILS_FORMATS_JSON_DOCUMENT_H_
#define GENESIS_UTILS_FORMATS_JSON_DOCUMENT_H_

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

#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Forward Declarations
// =================================================================================================

template<typename U>
class JsonIterator;

class JsonDocument;

bool operator==(JsonDocument const& lhs, JsonDocument const& rhs) noexcept;
bool operator==(JsonDocument const& v, std::nullptr_t) noexcept;
bool operator==(std::nullptr_t, JsonDocument const& v) noexcept;
bool operator!=(JsonDocument const& lhs, JsonDocument const& rhs) noexcept;
bool operator!=(JsonDocument const& v, std::nullptr_t) noexcept;
bool operator!=(std::nullptr_t, JsonDocument const& v) noexcept;
bool operator<(JsonDocument const& lhs, JsonDocument const& rhs) noexcept;
bool operator<=(JsonDocument const& lhs, JsonDocument const& rhs) noexcept;
bool operator>(JsonDocument const& lhs, JsonDocument const& rhs) noexcept;
bool operator>=(JsonDocument const& lhs, JsonDocument const& rhs) noexcept;

// =================================================================================================
//     JsonValue
// =================================================================================================

/**
 * @brief Store a Json value of any kind.
 *
 * This class is a heavily altered adaption of the excellent
 * [JSON for Modern C++](https://github.com/nlohmann/json) library by Niels Lohmann, see also
 * @link supplement_acknowledgements_code_reuse_json_document Acknowledgements@endlink.
 */
class JsonDocument
{
    // ---------------------------------------------------------------------------------------------
    //     Friends and Asserts
    // ---------------------------------------------------------------------------------------------

    friend class JsonIterator<JsonDocument>;
    friend class JsonIterator<JsonDocument const>;

public:

    // ---------------------------------------------------------------------------------------------
    //     Tyepdefs and Enums
    // ---------------------------------------------------------------------------------------------

    using value_type         = JsonDocument;
    using reference          = JsonDocument&;
    using const_reference    = JsonDocument const&;

    using pointer            = JsonDocument*;
    using const_pointer      = JsonDocument const*;

    using iterator           = JsonIterator<JsonDocument>;
    using const_iterator     = JsonIterator<JsonDocument const>;

    using difference_type    = std::ptrdiff_t;
    using size_type          = std::size_t;

    using ArrayType          = std::vector< JsonDocument >;
    using ObjectType         = std::map< std::string, JsonDocument >;
    using StringType         = std::string;

    using BooleanType        = bool;
    using NumberFloatType    = double;
    using NumberSignedType   = std::int64_t;
    using NumberUnsignedType = std::uint64_t;

    enum class ValueType : uint8_t
    {
        kNull,

        kArray,
        kObject,
        kString,

        kBoolean,
        kNumberFloat,
        kNumberSigned,
        kNumberUnsigned
    };

    // ---------------------------------------------------------------------------------------------
    //     Value Union
    // ---------------------------------------------------------------------------------------------

private:

    union ValueUnion
    {
        ArrayType*         array;
        ObjectType*        object;
        StringType*        string;

        BooleanType        boolean;
        NumberFloatType    number_float;
        NumberSignedType   number_signed;
        NumberUnsignedType number_unsigned;

        ValueUnion() = default;

        ValueUnion( ValueType t )
        {
            switch( t ) {
                case ValueType::kNull: {
                    break;
                }
                case ValueType::kArray: {
                    array = create<ArrayType>();
                    break;
                }
                case ValueType::kObject: {
                    object = create<ObjectType>();
                    break;
                }
                case ValueType::kString: {
                    string = create<StringType>("");
                    break;
                }
                case ValueType::kBoolean: {
                    boolean = false;
                    break;
                }
                case ValueType::kNumberFloat: {
                    number_float = 0.0;
                    break;
                }
                case ValueType::kNumberSigned: {
                    number_signed = 0;
                    break;
                }
                case ValueType::kNumberUnsigned: {
                    number_unsigned = 0;
                    break;
                }
                default: {
                    throw std::runtime_error( "Invalid Json Value Type." );
                }
            }
        }

        ValueUnion( ArrayType const&v )
            : array( create<ArrayType>(v) )
        {}

        ValueUnion( ObjectType const& v )
            : object( create<ObjectType>(v) )
        {}

        ValueUnion( StringType const& v )
            : string( create<StringType>(v) )
        {}

        ValueUnion( BooleanType v )
            : boolean(v)
        {}

        ValueUnion( NumberFloatType v )
            : number_float(v)
        {}

        ValueUnion( NumberSignedType v )
            : number_signed(v)
        {}

        ValueUnion( NumberUnsignedType v )
            : number_unsigned(v)
        {}
    };

    // ---------------------------------------------------------------------------------------------
    //     Constructors and and Rule of Five
    // ---------------------------------------------------------------------------------------------

public:

    // -------------------------------------------------------------------------
    //     Basic Constructors
    // -------------------------------------------------------------------------

    /**
     * @brief Create a null object.
     *
     * Create a `null` JSON value. It either takes a null pointer as parameter (explicitly creating
     * `null`) or no parameter (implicitly creating `null`).
     */
    JsonDocument( std::nullptr_t = nullptr )
        : JsonDocument( ValueType::kNull )
    {
        assert_invariant();
    }

    /**
     * @brief Create an empty value of the given `type`.
     */
    JsonDocument( ValueType type )
        // We use the ValueUnion( ValueType t ) constructor to init the value according to type.
        : type_(  type )
        , value_( type )
    {
        assert_invariant();
    }

    /**
     * @brief Create an array, explicitly.
     */
    JsonDocument( ArrayType const& v )
        : type_(  ValueType::kArray )
        , value_( v )
    {
        assert_invariant();
    }

    /**
     * @brief Create an object, explicitly.
     */
    JsonDocument( ObjectType const& v )
        : type_(  ValueType::kObject )
        , value_( v )
    {
        assert_invariant();
    }

    /**
     * @brief Create a string, explicitly.
     */
    JsonDocument( StringType const& v )
        : type_(  ValueType::kString )
        , value_( v )
    {
        assert_invariant();
    }

    /**
     * @brief Create a string, explicitly, using a char pointer.
     */
    JsonDocument( typename StringType::value_type const* v )
        : type_(  ValueType::kString )
        , value_( StringType(v) )
    {
        assert_invariant();
    }

    /**
     * @brief Create a boolean, explicitly.
     */
    JsonDocument( BooleanType v )
        : type_(  ValueType::kBoolean )
        , value_( v )
    {
        assert_invariant();
    }

    /**
     * @brief Create a signed integer number (explicit).
     */
    template<typename T, typename std::enable_if<
        not (std::is_same<T, int>::value) and
        std::is_same<T, NumberSignedType>::value,
    int>::type = 0>
    JsonDocument(const NumberSignedType val)
        : type_(ValueType::kNumberSigned)
        , value_(val)
    {
        assert_invariant();
    }

    /**
     * @brief Create a signed integer number from an enum type (explicit).
     */
    JsonDocument( int const val )
        : type_(ValueType::kNumberSigned)
        , value_( static_cast<NumberSignedType>(val) )
    {
        assert_invariant();
    }

    /**
     * @brief Create an unsigned integer number (explicit).
     */
    template<typename T, typename std::enable_if<
        not (std::is_same<T, int>::value) and
        std::is_same<T, NumberUnsignedType>::value,
    int>::type = 0>
    JsonDocument(const NumberUnsignedType val) noexcept
        : type_(ValueType::kNumberUnsigned), value_(val)
    {
        assert_invariant();
    }

    /**
     * @brief Create a floating-point number (explicit).
     *
     * [RFC 7159](http://www.rfc-editor.org/rfc/rfc7159.txt), section 6 disallows NaN values:
     * > Numeric values that cannot be represented in the grammar below (such as Infinity and NaN)
     * > are not permitted.
     * In case the parameter @a val is not a number, a JSON null value is created instead.
     */
    JsonDocument(const NumberFloatType val)
        : type_(  ValueType::kNumberFloat )
        , value_( val )
    {
        // replace infinity and NAN by null
        if( not std::isfinite(val) ) {
            type_  = ValueType::kNull;
            value_ = ValueUnion();
        }

        assert_invariant();
    }

    // -------------------------------------------------------------------------
    //     Advanced Constructors
    // -------------------------------------------------------------------------

    /**
     * @brief Create a container (array or object) from an initializer list.
     */
    JsonDocument(
        std::initializer_list<JsonDocument> init,
        bool type_deduction = true,
        ValueType manual_type = ValueType::kArray
    );

    /**
     * @brief Construct an array with `n` many copies of `val`.
     */
    JsonDocument( size_t n, JsonDocument const& val )
        : type_( ValueType::kArray )
    {
        value_.array = create<ArrayType>( n, val );
        assert_invariant();
    }

    template<
        class CompatibleArrayTypeT, typename std::enable_if<
            not std::is_same<CompatibleArrayTypeT, typename JsonDocument::iterator>::value and
            not std::is_same<CompatibleArrayTypeT, typename JsonDocument::const_iterator>::value and
            // not std::is_same<CompatibleArrayTypeT, typename JsonDocument_t::reverse_iterator>::value and
            // not std::is_same<CompatibleArrayTypeT, typename JsonDocument_t::const_reverse_iterator>::value and
            not std::is_same<CompatibleArrayTypeT, typename ArrayType::iterator>::value and
            not std::is_same<CompatibleArrayTypeT, typename ArrayType::const_iterator>::value and
            std::is_constructible<JsonDocument, typename CompatibleArrayTypeT::value_type>::value, int>::type = 0
    >
    JsonDocument(const CompatibleArrayTypeT& val)
        : type_(ValueType::kArray)
    {
        using std::begin;
        using std::end;
        value_.array = create<ArrayType>(begin(val), end(val));
        assert_invariant();
    }

    /**
     * @brief Create an object (implicit).
     *
     * Create an object JSON value with a given content. This constructor allows any type
     * @a CompatibleObjectType that can be used to construct values of type @ref ObjectType.
     */
    template<class CompatibleObjectType,
        typename std::enable_if<
            std::is_constructible<
                typename ObjectType::key_type, typename CompatibleObjectType::key_type
            >::value and
            std::is_constructible<
                JsonDocument, typename CompatibleObjectType::mapped_type
            >::value,
            int
        >::type = 0
    >
    JsonDocument( CompatibleObjectType const& val )
        : type_( ValueType::kObject )
    {
        using std::begin;
        using std::end;
        value_.object = create<ObjectType>( begin(val), end(val) );
        assert_invariant();
    }

    /**
     * @brief Create a string (implicit).
     *
     * Create a string JSON value with a given content with a type which is compatible to
     * @ref StringType, for instance `std::string`.
     */
    template<class CompatibleStringType, typename std::enable_if<
        std::is_constructible<StringType, CompatibleStringType>::value, int>::type = 0
    >
    JsonDocument(const CompatibleStringType& val)
        : JsonDocument( StringType(val) )
    {
        assert_invariant();
    }

    // -------------------------------------------------------------------------
    //     Rule of Five
    // -------------------------------------------------------------------------

    /**
     * @brief Copy constructor.
     */
    JsonDocument( JsonDocument const& other );

    /**
     * @brief Move constructor.
     */
    JsonDocument( JsonDocument&& other );

    /**
     * @brief Copy assignment.
     */
    JsonDocument& operator = ( JsonDocument other );

    /**
     * @brief Destructor.
     */
    ~JsonDocument();

    // -------------------------------------------------------------------------
    //     Factory Method Construction
    // -------------------------------------------------------------------------

    /**
    * @brief Explicitly create an array from an initializer list.
    */
    static JsonDocument array(
        std::initializer_list<JsonDocument> init = std::initializer_list<JsonDocument>()
    ) {
        return JsonDocument( init, false, ValueType::kArray );
    }

    /**
     * @brief Explicitly create an object from an initializer list.
     */
    static JsonDocument object(
        std::initializer_list<JsonDocument> init = std::initializer_list<JsonDocument>()
    ) {
        return JsonDocument( init, false, ValueType::kObject );
    }

    /**
     * @brief Explicitly create a string.
     */
    static JsonDocument string(
        std::string const& init
    ) {
        return JsonDocument( init );
    }

    /**
     * @brief Explicitly create a boolean.
     */
    static JsonDocument boolean(
        BooleanType value
    ) {
        auto result = JsonDocument( ValueType::kBoolean );
        result.value_ = value;
        return result;
    }

    /**
     * @brief Explicitly create a float number.
     */
    static JsonDocument number_float(
        NumberFloatType value
    ) {
        auto result = JsonDocument( ValueType::kNumberFloat );
        result.value_ = value;
        return result;
    }

    /**
     * @brief Explicitly create a signed number.
     */
    static JsonDocument number_signed(
        NumberSignedType value
    ) {
        auto result = JsonDocument( ValueType::kNumberSigned );
        result.value_ = value;
        return result;
    }

    /**
     * @brief Explicitly create an unsigned number.
     */
    static JsonDocument number_unsigned(
        NumberUnsignedType value
    ) {
        auto result = JsonDocument( ValueType::kNumberUnsigned );
        result.value_ = value;
        return result;
    }

    // ---------------------------------------------------------------------------------------------
    //     Type Inspection
    // ---------------------------------------------------------------------------------------------

public:

    /**
     * @brief Return the type of the JSON value.
     */
    ValueType type() const
    {
        return type_;
    }

    /**
     * @brief Return true iff the JSON type is primitive (string, number, boolean, or null).
     */
    bool is_primitive() const
    {
        return is_null() or is_string() or is_boolean() or is_number();
    }

    /**
     * @brief Return true iff the JSON type is structured (array or object).
     */
    bool is_structured() const
    {
        return is_array() or is_object();
    }

    /**
     * @brief Return true iff the JSON value is null.
     */
    bool is_null() const
    {
        return type_ == ValueType::kNull;
    }

    /**
     * @brief Return true iff the JSON value is an array.
     */
    bool is_array() const
    {
        return type_ == ValueType::kArray;
    }

    /**
     * @brief Return true iff the JSON value is an object.
     */
    bool is_object() const
    {
        return type_ == ValueType::kObject;
    }

    /**
     * @brief Return true iff the JSON value is a string.
     */
    bool is_string() const
    {
        return type_ == ValueType::kString;
    }

    /**
     * @brief Return true iff the JSON value is a boolean.
     */
    bool is_boolean() const
    {
        return type_ == ValueType::kBoolean;
    }

    /**
     * @brief Return true iff the JSON value is a number, i.e., a float or signed or unsigned
     * integer.
     */
    bool is_number() const
    {
        return is_number_float() or is_number_integer();
    }

    /**
     * @brief Return true iff the JSON value is a float number.
     */
    bool is_number_float() const
    {
        return type_ == ValueType::kNumberFloat;
    }

    /**
     * @brief Return true iff the JSON value is a signed or unsigned integer number.
     */
    bool is_number_integer() const
    {
        return is_number_signed() or is_number_unsigned();
    }

    /**
     * @brief Return true iff the JSON value is a signed integer number.
     */
    bool is_number_signed() const
    {
        return type_ == ValueType::kNumberSigned;
    }

    /**
     * @brief Return true iff the JSON value is an unsigned integer number.
     */
    bool is_number_unsigned() const
    {
        return type_ == ValueType::kNumberUnsigned;
    }

    // ---------------------------------------------------------------------------------------------
    //     Capacity
    // ---------------------------------------------------------------------------------------------

public:

    bool empty() const;
    size_t size() const;
    size_t max_size() const;

    // ---------------------------------------------------------------------------------------------
    //     Value Access
    // ---------------------------------------------------------------------------------------------

public:

    ArrayType& get_array();
    ArrayType const& get_array() const;

    ObjectType& get_object();
    ObjectType const& get_object() const;

    StringType& get_string();
    StringType const& get_string() const;

    BooleanType& get_boolean();
    BooleanType get_boolean() const;

    NumberFloatType& get_number_float();
    NumberFloatType get_number_float() const;

    NumberSignedType& get_number_signed();
    NumberSignedType get_number_signed() const;

    NumberUnsignedType& get_number_unsigned();
    NumberUnsignedType get_number_unsigned() const;

    template<typename T>
    T get_number() const
    {
        if( is_number_float() ) {
            return value_.number_float;
        } else if( is_number_signed() ) {
            return value_.number_signed;
        } else if( is_number_unsigned() ) {
            return value_.number_unsigned;
        } else {
            throw std::domain_error( "Cannot use get_number<T>() with " + type_name() + "." );
        }
    }

    // ---------------------------------------------------------------------------------------------
    //     Element Access
    // ---------------------------------------------------------------------------------------------

public:

    JsonDocument& at( size_t index );
    JsonDocument const& at( size_t index ) const;

    JsonDocument& at( typename ObjectType::key_type const& key );
    JsonDocument const& at( typename ObjectType::key_type const& key ) const;

    JsonDocument& operator [] ( size_t index );
    JsonDocument const& operator [] ( size_t index ) const;

    JsonDocument& operator [] ( typename ObjectType::key_type const& key );
    JsonDocument const& operator [] ( typename ObjectType::key_type const& key ) const;

    // ---------------------------------------------------------------------------------------------
    //     Lookup
    // ---------------------------------------------------------------------------------------------

    /**
     * @brief Find an element in a JSON object.
     *
     * Finds an element in a JSON object with key equivalent to @a key. If the element is not found
     * or the JSON value is not an object, end() is returned.
     */
    iterator find( typename JsonDocument::ObjectType::key_type key );

    /**
     * @copydoc find(typename JsonDocument::ObjectType::key_type)
     */
    const_iterator find(typename JsonDocument::ObjectType::key_type key) const;

    /**
     * @brief Return the number of occurrences of a key in a JSON object.
     *
     * As we use a std::map for objects, this value is either 0 (key not found) or 1 (key found).
     * For non-object types, the return value is always 0.
     */
    size_type count(typename ObjectType::key_type key) const;

    // ---------------------------------------------------------------------------------------------
    //     Iterators
    // ---------------------------------------------------------------------------------------------

public:

    /**
     * @brief Return an iterator to the first element.
     */
    iterator begin() noexcept;

    /**
     * @copydoc JsonDocument::cbegin()
     */
    const_iterator begin() const noexcept;

    /**
     * @brief Return a const iterator to the first element.
     */
    const_iterator cbegin() const noexcept;

    /**
     * @brief Return an iterator to one past the last element.
     */
    iterator end() noexcept;

    /**
     * @copydoc JsonDocument::cend()
     */
    const_iterator end() const noexcept;

    /**
     * @brief Return a const iterator to one past the last element.
     */
    const_iterator cend() const noexcept;

    // ---------------------------------------------------------------------------------------------
    //     Modifiers
    // ---------------------------------------------------------------------------------------------

public:

    /**
     * @brief Clears the content of a JSON value and resets it to the default value as
     * if @ref JsonDocument(ValueType) would have been called.
     *
     * The values are cleared as follows:
     *
     * Value type  | initial value
     * ----------- | -------------
     * null        | `null`
     * boolean     | `false`
     * string      | `""`
     * number      | `0`
     * object      | `{}`
     * array       | `[]`
     *
     * In order to complete eliminate a Json value, set it to `nullptr` instead.
     */
    void clear();

    /**
     * @brief Add a Json value to an array.
     *
     * Appends the given element @a val to the end of the JSON value. If the function is called on
     * a JSON null value, an empty array is created before appending
     * @a val.void push_back( JsonDocument&& val );
     */
    void push_back( JsonDocument&& val );

    /**
     * @copydoc push_back(JsonDocument&&)
     */
    void push_back( JsonDocument const& val );

    /**
     * @brief Add an object to an object.
     *
     * Inserts the given element @a val to the JSON object. If the function is called on a JSON
     * null value, an empty object is created before inserting @a val.
     */
    void push_back( typename ObjectType::value_type const& val );

    template<class... Args>
    void emplace_back( Args&& ... args )
    {
        // emplace_back only works for null objects or arrays
        if( not(is_null() or is_array()) ) {
            throw std::domain_error("Cannot use emplace_back() with " + type_name());
        }

        // transform null object into an array
        if( is_null() ) {
            type_  = ValueType::kArray;
            value_ = ValueType::kArray;
            assert_invariant();
        }

        // add element to array (perfect forwarding)
        value_.array->emplace_back( std::forward<Args>(args)... );
    }

    // ---------------------------------------------------------------------------------------------
    //     Lexicographical Comparison Operators
    // ---------------------------------------------------------------------------------------------

public:

    /**
     * @brief Compare equal.
     *
     * Compares two JSON values for equality according to the following rules:
     *
     *  * Two JSON values are equal if (1) they are from the same type and (2) their stored values
     *    are the same.
     *  * Integer and floating-point numbers are automatically converted before comparison.
     *    Floating-point numbers are compared indirectly: two floating-point numbers `f1` and `f2`
     *    are considered equal if neither `f1 > f2` nor `f2 > f1` holds.
     *  * Two JSON null values are equal.
     * %
     */
    friend bool operator==(const_reference lhs, const_reference rhs) noexcept;

    /**
     * @brief Compare equal.
     *
     * The functions compares the given JSON value against a null pointer. As the null pointer can
     * be used to initialize a JSON value to null, a comparison of JSON value @a v with a null
     * pointer should be equivalent to call `v.is_null()`.
     */
    friend bool operator==(const_reference v, std::nullptr_t) noexcept
    {
        return v.is_null();
    }

    /**
     * @copydoc operator==(const_reference v, std::nullptr_t)
     */
    friend bool operator==(std::nullptr_t, const_reference v) noexcept
    {
        return v.is_null();
    }

    /**
     * @brief Compare not equal.
     *
     * See operator==(const_reference lhs, const_reference rhs) for details.
     */
    friend bool operator!=(const_reference lhs, const_reference rhs) noexcept
    {
        return not (lhs == rhs);
    }

    /**
     * @brief Compare not equal.
     *
     * See operator==(const_reference v, std::nullptr_t) for details.
     */
    friend bool operator!=(const_reference v, std::nullptr_t) noexcept
    {
        return not v.is_null();
    }

    /**
     * @brief Compare not equal.
     *
     * See operator==(const_reference v, std::nullptr_t) for details.
     */
    friend bool operator!=(std::nullptr_t, const_reference v) noexcept
    {
        return not v.is_null();
    }

    /**
     * @brief Compare less than.
     *
     * Compares whether one JSON value @a lhs is less than another JSON value @a rhs according to
     * the following rules:
     *
     *  * If @a lhs and @a rhs have the same type, the values are compared using the default `<`
     *    operator.
     *  * Integer and floating-point numbers are automatically converted before comparison
     *  * In case @a lhs and @a rhs have different types, the values are ignored and the order of
     *    the types is considered, see @ref operator<(const JsonDocument::ValueType, const JsonDocument::ValueType).
     * %
     */
    friend bool operator<(const_reference lhs, const_reference rhs) noexcept;

    /**
     * @brief Compare less than or equal.
     *
     * Compares whether one JSON value @a lhs is less than or equal to another JSON value by
     * calculating `not (rhs < lhs)`.
     */
    friend bool operator<=(const_reference lhs, const_reference rhs) noexcept
    {
        return not (rhs < lhs);
    }

    /**
     * @brief Compare greater than.
     *
     * Compares whether one JSON value @a lhs is greater than another JSON value by calculating
     * `not (lhs <= rhs)`.
     */
    friend bool operator>(const_reference lhs, const_reference rhs) noexcept
    {
        return not (lhs <= rhs);
    }

    /**
     * @brief Compare greater than or equal.
     *
     * Compares whether one JSON value @a lhs is greater than or equal to another
     * JSON value by calculating `not (lhs < rhs)`.
     */
    friend bool operator>=(const_reference lhs, const_reference rhs) noexcept
    {
        return not (lhs < rhs);
    }

private:

    /**
     * @brief Comparison of Json types (instead of values).
     *
     * Returns an ordering that is similar to Python:
     *  * order: null < boolean < number < object < array < string
     *  * furthermore, each type is not smaller than itself
     * %
     */
    friend bool operator<(const ValueType lhs, const ValueType rhs) noexcept;

    // ---------------------------------------------------------------------------------------------
    //     Convenience Functions
    // ---------------------------------------------------------------------------------------------

public:

    std::string type_name() const;

    // ---------------------------------------------------------------------------------------------
    //     Helper Functions
    // ---------------------------------------------------------------------------------------------

private:

    /**
     * @brief Helper for exception-safe object creation.
     */
    template<typename T, typename... Args>
    static T* create( Args&& ... args )
    {
        // Avoid memory leaks.
        std::allocator<T> alloc;
        auto deleter = [&](T * object)
        {
            alloc.deallocate(object, 1);
        };
        std::unique_ptr<T, decltype(deleter)> object(alloc.allocate(1), deleter);
        alloc.construct(object.get(), std::forward<Args>(args)...);
        assert(object != nullptr);
        return object.release();

        // auto object = new T(std::forward<Args>(args)...);
        // assert( object != nullptr );
        // return object;
    }

    template<typename T>
    static void destroy( T* ptr )
    {
        std::allocator<T> alloc;
        alloc.destroy(ptr);
        alloc.deallocate(ptr, 1);

        // delete ptr;
    }

    void assert_invariant() const
    {
        assert( type_ != ValueType::kArray  or value_.array  != nullptr );
        assert( type_ != ValueType::kObject or value_.object != nullptr );
        assert( type_ != ValueType::kString or value_.string != nullptr );
    }

    // ---------------------------------------------------------------------------------------------
    //     Data Members
    // ---------------------------------------------------------------------------------------------

private:

    ValueType  type_  = ValueType::kNull;
    ValueUnion value_ = {};

};

} // namespace utils
} // namespace genesis

#endif // include guard
