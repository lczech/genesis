#ifndef GNS_UTILS_JSONDOCUMENT_H_
#define GNS_UTILS_JSONDOCUMENT_H_

/**
 * @brief A collection of classes for working with JSON documents. See JsonDocument for more.
 *
 * @file
 * @ingroup utils
 */

#include <deque>
#include <iomanip>
#include <sstream>
#include <string>
#include <unordered_map>

#include "utils/logging.hh"
#include "utils/utils.hh"

namespace genesis {

// =============================================================================
//     JsonValue
// =============================================================================

/** @brief Interface for JSON values.
 *
 * JSON provides different types of values, as listed in JsonValue::Type. This class serves as a
 * base class for all those types. Each derived class needs at least to implement ToString(), which
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

    static std::string TypeToString (const Type t)
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

    inline std::string TypeToString () const
    {
        return TypeToString(type_);
    }

    inline Type type() const
    {
        return type_;
    }

    inline bool IsNull()
    {
        return type_ == kNull;
    }

    inline bool IsBool()
    {
        return type_ == kBool;
    }

    inline bool IsNumber()
    {
        return type_ == kNumber;
    }

    inline bool IsString()
    {
        return type_ == kString;
    }

    inline bool IsArray()
    {
        return type_ == kArray;
    }

    inline bool IsObject()
    {
        return type_ == kObject;
    }

    virtual std::string ToString() const = 0;

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
            LOG_WARN << "Not a valid JSON null expression: '" << v << "'.";
        }
    }

    inline std::string ToString() const override
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
            LOG_WARN << "Not a valid JSON bool expression: '" << v << "'.";
            value = false;
        }
    }

    inline std::string ToString() const override
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
        // TODO check if the string actually contains a valid number
        value = std::stod(v);
    }

    static void SetPrecision (const int p)
    {
        precision = p;
    }

    inline std::string ToString() const override
    {
        std::stringstream ss;
        ss << std::setprecision(precision) << value;
        return ss.str();
    }

    double     value = 0.0;
    static int precision;
};

// =============================================================================
//     JsonValueString
// =============================================================================

class JsonValueString : public JsonValue
{
public:
    JsonValueString () : JsonValue(kString) {};

    JsonValueString (const std::string& v) : JsonValue(kString), value(v) {};

    inline std::string ToString() const override
    {
        return "\"" + StringEscape(value) + "\"";
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
    virtual ~JsonValueArray() override;
    void clear();

    std::string ToString(const int indent_level) const;
    inline std::string ToString() const override
    {
        return ToString(0);
    }

    typedef std::deque<JsonValue*> ArrayData;
    ArrayData data;
};

// =============================================================================
//     JsonValueObject
// =============================================================================

class JsonValueObject : public JsonValue
{
public:
    JsonValueObject () : JsonValue(kObject) {};
    virtual ~JsonValueObject() override;
    void clear();

    std::string ToString(const int indent_level) const;
    inline std::string ToString() const override
    {
        return ToString(0);
    }

    void Set (const std::string& name, JsonValue* value);
    JsonValue* Get (const std::string& name);

    inline bool Has (const std::string& name) const
    {
        return data.count(name) > 0;
    }

    inline size_t size() const
    {
        return data.size();
    }

    typedef std::unordered_map<std::string, JsonValue*> ObjectData;
    typedef ObjectData::value_type ObjectPair;
    ObjectData data;
};

// =============================================================================
//     JsonDocument
// =============================================================================

// TODO write better accessors, write copy and assign functions for all value types

/**
 *
 *
 * See http://www.json.org/ for the complete specification.
 */
class JsonDocument : public JsonValueObject
{
public:
    JsonDocument () : JsonValueObject() {};
    virtual ~JsonDocument() override;

    bool Validate();

    static int indent;
};

// =============================================================================
//     Converter Functions
// =============================================================================

JsonValueNull*   JsonValueToNull   (JsonValue* v);
JsonValueBool*   JsonValueToBool   (JsonValue* v);
JsonValueNumber* JsonValueToNumber (JsonValue* v);
JsonValueString* JsonValueToString (JsonValue* v);
JsonValueArray*  JsonValueToArray  (JsonValue* v);
JsonValueObject* JsonValueToObject (JsonValue* v);

} // namespace genesis

#endif // include guard
