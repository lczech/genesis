#ifndef GNS_UTILS_JSONDOCUMENT_H_
#define GNS_UTILS_JSONDOCUMENT_H_

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <deque>
#include <iomanip>
#include <sstream>
#include <string>

namespace genesis {

// =============================================================================
//     JsonValue
// =============================================================================

/** @brief Interface for JSON values.
 *
 * JSON provides different types of values, as listed in JsonValue::Type.
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

    inline std::string TypeToString ()
    {
        return TypeToString(type_);
    }

    virtual std::string ToString() = 0;

protected:
    /**
     * @brief Protected constructor that allows derived classes to set their value type.
     *
     * Storing the type of each object as a special member saves the issue of using
     * run time type inference, making it easier to work with objects of different type.
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

    inline std::string ToString()
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

    inline std::string ToString()
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

    JsonValueNumber (const std::string v) : JsonValue(kNumber)
    {
        value = std::stod(v);
    }

    static void SetPrecision (const int p)
    {
        precision = p;
    }

    inline std::string ToString()
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

    JsonValueString (const std::string v) : JsonValue(kString), value(v) {};

    inline std::string ToString()
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

    std::string ToString();

    std::deque<JsonValue> value;
};

// =============================================================================
//     JsonValueObject
// =============================================================================

class JsonValueObject : public JsonValue
{
public:
    JsonValueObject () : JsonValue(kObject) {}

    std::string ToString() {return "";};

    void clear();

    void AddValue (std::string name, JsonValue& value);

private:

};

// =============================================================================
//     JsonDocument
// =============================================================================

class JsonDocument : JsonValueObject
{

};

} // namespace genesis

#endif // include guard
