#ifndef CUSTOM_BINDING_TO_BYTEARRAY_INCLUDED_
#define CUSTOM_BINDING_TO_BYTEARRAY_INCLUDED_

#include "genesis/utils/io/base_output_target.hpp"
#include <pybind11/pytypes.h>

#include <sstream>
#include <string>

// =================================================================================================
//     Bytearray Output Target
// =================================================================================================

namespace genesis {
namespace utils {

/**
 * @brief Output target for writing data to a Python bytearray.
 */
class BytearrayOutputTarget final : public BaseOutputTarget
{
public:

    // -------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------

    /**
     * @brief Construct the output target to a string that is modified.
     */
    explicit BytearrayOutputTarget(pybind11::bytearray& target )
        : target_( target )
    {}

    BytearrayOutputTarget( BytearrayOutputTarget const& ) = delete;
    BytearrayOutputTarget( BytearrayOutputTarget&& )      = delete;

    BytearrayOutputTarget& operator= ( BytearrayOutputTarget const& ) = delete;
    BytearrayOutputTarget& operator= ( BytearrayOutputTarget&& )      = delete;

    ~BytearrayOutputTarget() override
    {
        //target_ = stream_.str();
        // or?!
        target_ = pybind11::bytearray( stream_.str() );
    }

    // -------------------------------------------------------------
    //     Overloaded Internal Members
    // -------------------------------------------------------------

private:

    /**
     * @brief Override of the output stream function.
     */
    std::ostream& out_stream_() override
    {
        return stream_;
    }

    /**
     * @brief Override of the target name funtion. Returns "output bytearray".
     */
    std::string target_name_() const override
    {
        return "output bytearray";
    }

    /**
     * @brief Override of the target string funtion. Returns "bytearray".
     */
    std::string target_string_() const override
    {
        return "bytearray";
    }

    // -------------------------------------------------------------
    //     Member Variables
    // -------------------------------------------------------------

    pybind11::bytearray& target_;
    std::ostringstream stream_;
};

/**
 * @brief Obtain an output target for writing to a Python bytearray.
 */
inline std::shared_ptr<BaseOutputTarget> to_bytearray(
    pybind11::bytearray& target_string
) {
    return std::make_shared< BytearrayOutputTarget >( target_string );
}

}
}

#endif
