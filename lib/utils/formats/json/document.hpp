#ifndef GENESIS_UTILS_FORMATS_JSON_DOCUMENT_H_
#define GENESIS_UTILS_FORMATS_JSON_DOCUMENT_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

/**
 * @brief A collection of classes for working with JSON documents. See JsonDocument for more.
 *
 * @file
 * @ingroup utils
 */

#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "utils/core/logging.hpp"

namespace genesis {
namespace utils {

// =============================================================================
//     JsonValue
// =============================================================================

/** @brief Interface for JSON values.
 *
 * JSON provides different types of values, as listed in JsonValue::Type. This class serves as a
 * base class for all those types. Each derived class needs at least to implement to_string(), which
 * returns the JSON string representation of its content. Also, derived classes need to make sure to
 * call the base constructor in order to set their type correctly.
 */
class JsonValue
{
public:
    enum Type {
        kNull,
        kBool,
        kNumber,
        kString,
        kArray,
        kObject
    };

    static std::string type_to_string (const Type t)
    {
        switch (t) {
            case kNull   : return "Null";
            case kBool   : return "Bool";
            case kNumber : return "Number";
            case kString : return "String";
            case kArray  : return "Array";
            case kObject : return "Object";
            default      : return "Unknown";
        }
    }

    inline std::string type_to_string () const
    {
        return type_to_string(type_);
    }

    inline Type type() const
    {
        return type_;
    }

    inline bool is_null()
    {
        return type_ == kNull;
    }

    inline bool is_bool()
    {
        return type_ == kBool;
    }

    inline bool is_number()
    {
        return type_ == kNumber;
    }

    inline bool is_string()
    {
        return type_ == kString;
    }

    inline bool is_array()
    {
        return type_ == kArray;
    }

    inline bool is_object()
    {
        return type_ == kObject;
    }

    virtual std::string to_string()     const = 0;

    virtual ~JsonValue() {};

protected:
    /**
     * @brief Protected constructor that allows derived classes to set their value type.
     *
     * Storing the type of each object as a special member saves the issue of using run time type
     * inference, making it faster and easier to work with objects of different types.
     */
    JsonValue (const Type type) : type_(type) {};

    /** @brief Stores the type of an object. Set by the constructor. */
    const Type type_;
};

// =============================================================================
//     JsonValueNull
// =============================================================================

class JsonValueNull : public JsonValue
{
public:
    JsonValueNull() : JsonValue(kNull) {};

    JsonValueNull (const std::string& v) : JsonValue(kNull)
    {
        if (v.compare("null") != 0) {
            throw std::runtime_error( "Not a valid JSON null expression: '" + v + "'." );
        }
    }

    inline std::string to_string() const override
    {
        return "null";
    }
};

// =============================================================================
//     JsonValueBool
// =============================================================================

class JsonValueBool : public JsonValue
{
public:
    JsonValueBool () : JsonValue(kBool), value(false) {};

    JsonValueBool (const bool v) : JsonValue(kBool), value(v) {};

    JsonValueBool (const std::string& v) : JsonValue(kBool)
    {
        if (v.compare("true") == 0) {
            value = true;
        } else if (v.compare("false") == 0) {
            value = false;
        } else {
            throw std::runtime_error( "Not a valid JSON bool expression: '" + v + "'." );
        }
    }

    inline std::string to_string() const override
    {
        return value ? "true" : "false";
    }

    bool value;
};

// =============================================================================
//     JsonValueNumber
// =============================================================================

class JsonValueNumber : public JsonValue
{
public:
    JsonValueNumber () : JsonValue(kNumber) {}

    JsonValueNumber (const double v) : JsonValue(kNumber), value(v) {}

    JsonValueNumber (const std::string& v) : JsonValue(kNumber)
    {
        value = std::stod(v);
    }

    inline std::string to_string() const override
    {
        // use string stream instead of std::to_string() to have better conversion of the number
        // (e.g., avoid trailing zeros when not needed).
        std::ostringstream out;
        out << value;
        return out.str();
    }

    double value = 0.0;
};

// =============================================================================
//     JsonValueString
// =============================================================================

class JsonValueString : public JsonValue
{
public:
    JsonValueString () : JsonValue(kString) {};

    JsonValueString (const std::string& v) : JsonValue(kString), value(v) {};

    inline std::string to_string() const override
    {
        return value;
    }

    std::string value;
};

// =============================================================================
//     JsonValueArray
// =============================================================================

class JsonValueArray : public JsonValue
{
public:
    JsonValueArray () : JsonValue(kArray) {};
    virtual ~JsonValueArray() override
    {
        clear();
    }

    // ---------------------------------------------------------------------
    //     Accessors and Iterators
    // ---------------------------------------------------------------------

    typedef std::vector<JsonValue*> ArrayData;

    /**
     * @brief Iterator type to access the array values.
     *
     * This iterator allows to use a loop like this:
     *
     *     JsonValueArray a;
     *     for (JsonValueArray::iterator t = a.begin(); t != a.end(); ++t) {
     *         std::cout << t->to_string() << std::endl;
     *     }
     * %
     */
    typedef ArrayData::iterator       iterator;

    /** @brief Const version of the iterator. */
    typedef ArrayData::const_iterator const_iterator;

    /**
     * @brief Returns an iterator to the beginning of the array.
     *
     * This is used for the iterator and also allows to use range based
     * looping over the array values:
     *
     *     JsonValueArray a;
     *     for (JsonValue* v : a) {
     *         std::cout << v->to_string() << std::endl;
     *     }
     * %
     */
    inline iterator begin()
    {
        return data_.begin();
    }

    /** @brief Const version of begin(). */
    inline const_iterator cbegin() const
    {
        return data_.cbegin();
    }

    /** @brief Returns an iterator to the end of the array. */
    inline iterator end()
    {
        return data_.end();
    }

    /** @brief Const version of end(). */
    inline const_iterator cend() const
    {
        return data_.cend();
    }

    /**
     * @brief Provides index based array access to the array.
     *
     * This also allows to iterate over them using:
     *
     *     JsonValueArray a;
     *     for (size_t i = 0; i < a.size(); ++i) {
     *        JsonValue* v = a[i];
     *        std::cout << v->to_string() << std::endl;
     *     }
     *
     * Caveat: this operator does no boundary check. If you need this check,
     * use at() instead.
     */
    inline JsonValue* operator[](const std::size_t index) const
    {
        return data_[index];
    }

    /**
     * @brief Provides index based array access to the array, doing a
     * boundary check first.
     *
     * In out of bounds cases, a `nullptr` is returned.
     */
    inline JsonValue* at(const std::size_t index) const
    {
        if (index < data_.size()) {
            return data_[index];
        } else {
            return nullptr;
        }
    }

    /**
     * @brief Returns whether the array is empty.
     */
    inline bool empty() const
    {
        return data_.empty();
    }

    /**
     * @brief Returns the number of values in the array.
     */
    inline size_t size() const
    {
        return data_.size();
    }

    // ---------------------------------------------------------------------
    //     Mutators
    // ---------------------------------------------------------------------

    inline void push_back(JsonValue* value)
    {
        data_.push_back(value);
    }

    /**
     * @brief Alias of push_back().
     */
    inline void add (JsonValue* value)
    {
        data_.push_back(value);
    }

    inline void pop_back()
    {
        data_.pop_back();
    }

    /**
     * @brief Clears all values, as if the array was newly created.
     */
    inline void clear()
    {
        for (JsonValue* v : data_) {
            delete v;
        }
        ArrayData().swap(data_);
    }

    // ---------------------------------------------------------------------
    //     Other Members
    // ---------------------------------------------------------------------

    inline std::string to_string() const override
    {
        return "(Json Array)";
    }

protected:
    ArrayData data_;
};

// =============================================================================
//     JsonValueObject
// =============================================================================

class JsonValueObject : public JsonValue
{
public:
    JsonValueObject () : JsonValue(kObject) {};
    virtual ~JsonValueObject() override
    {
        clear();
    }

    // ---------------------------------------------------------------------
    //     Accessors and Iterators
    // ---------------------------------------------------------------------

    typedef std::unordered_map<std::string, JsonValue*> ObjectData;
    typedef ObjectData::value_type ObjectPair;

    /**
     * @brief Iterator type to access the object values.
     *
     * This iterator allows to use a loop like this:
     *
     *     JsonValueObject o;
     *     for (JsonValueObject::iterator t = o.begin(); t != o.end(); ++t) {
     *         std::cout << t->to_string() << std::endl;
     *     }
     * %
     */
    typedef ObjectData::iterator       iterator;

    /** @brief Const version of the iterator. */
    typedef ObjectData::const_iterator const_iterator;

    /**
     * @brief Returns an iterator to the beginning of the object.
     *
     * This is used for the iterator and also allows to use range based
     * looping over the object values:
     *
     *     JsonValueObject o;
     *     for (JsonValueObject::ObjectPair p : o) {
     *         std::cout << p.first << " : " << p.second->to_string() << std::endl;
     *     }
     * %
     */
    inline iterator begin()
    {
        return data_.begin();
    }

    /** @brief Const version of begin(). */
    inline const_iterator cbegin() const
    {
        return data_.cbegin();
    }

    /** @brief Returns an iterator to the end of the object. */
    inline iterator end()
    {
        return data_.end();
    }

    /** @brief Const version of end(). */
    inline const_iterator cend() const
    {
        return data_.cend();
    }

    /**
     * @brief Provides index based array access to the object, doing a
     * boundary check first. This is an alias for get().
     *
     * In out of bounds cases, a `nullptr` is returned.
     */
    inline JsonValue* at(const std::string& name) const
    {
        return get(name);
    }

    /**
     * @brief Returns whether the object is empty.
     */
    inline bool empty() const
    {
        return data_.empty();
    }

    /**
     * @brief Returns the number of values in the object.
     */
    inline size_t size() const
    {
        return data_.size();
    }

    /**
     * @brief Returns true iff the object contains a certain key.
     */
    inline bool has (const std::string& name) const
    {
        return data_.count(name) > 0;
    }

    /**
     * @brief Returns the value of a certain key if present in the object, `nullptr` otherwise.
     */
    inline JsonValue* get (const std::string& name) const
    {
        if (has(name)) {
            return data_.at(name);
        } else {
            return nullptr;
        }
    }

    // ---------------------------------------------------------------------
    //     Mutators
    // ---------------------------------------------------------------------

    /**
     * @brief Clears all values, as if the object was newly created.
     */
    inline void clear()
    {
        for (ObjectPair v : data_) {
            delete v.second;
        }
        ObjectData().swap(data_);
    }

    /**
     * @brief Clears the value of a specific key; returns true iff this key existed.
     */
    inline bool erase(const std::string name)
    {
        return data_.erase(name) > 0;
    }

    /**
     * @brief Sets the value for a certain key.
     */
    inline void set (const std::string& name, JsonValue* value)
    {
        data_[name] = value;
    }

    // ---------------------------------------------------------------------
    //     Other Members
    // ---------------------------------------------------------------------

    inline std::string to_string() const override
    {
        return "(Json Object)";
    }

protected:
    ObjectData data_;
};

// =============================================================================
//     JsonDocument
// =============================================================================

// TODO write copy ctor and assign op functions for all value types
// TODO add insert to object and array that takes a whole other object/array and makes a deep copy

/**
 *
 *
 * See http://www.json.org/ for the complete specification.
 */
class JsonDocument : public JsonValueObject
{
public:
    JsonDocument () : JsonValueObject() {};
    virtual ~JsonDocument() override
    {
        clear();
    }

    inline std::string to_string() const override
    {
        return "(Json Document)";
    }

    bool validate();

    inline std::string dump() const
    {
        return "(Json Document)";
    }
};

// =============================================================================
//     Converter Functions
// =============================================================================

// TODO const correctness is heavily violated here!
JsonValueNull*   json_value_to_null   (const JsonValue* v);
JsonValueBool*   json_value_to_bool   (const JsonValue* v);
JsonValueNumber* json_value_to_number (const JsonValue* v);
JsonValueString* json_value_to_string (const JsonValue* v);
JsonValueArray*  json_value_to_array  (const JsonValue* v);
JsonValueObject* json_value_to_object (const JsonValue* v);

} // namespace utils
} // namespace genesis

#endif // include guard
