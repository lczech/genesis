#ifndef CUSTOM_BINDING_STRING_WRAPPER_INCLUDED_
#define CUSTOM_BINDING_STRING_WRAPPER_INCLUDED_

#include "genesis/utils/io/base_output_target.hpp"
#include "genesis/utils/io/output_target.hpp"
#include "genesis/utils/io/string_output_target.hpp"

#include "genesis/sequence/formats/fasta_output_iterator.hpp"

#include <pybind11/pytypes.h>

#include <sstream>
#include <string>

namespace genesis {
namespace utils {

/**
 * @brief Helper class to use to_string() and StringOutputTarget in Python.
 *
 * Python has immutable strings, so, we cannot write to them with our references as usual.
 * Instead, we write to this class, which wraps the string on the C++ side, and
 * access it later from Python.
 */
struct StringTarget
{
public:
    // -------------------------------------------------------------------------
    //     Constructors and Rule of Five
    // -------------------------------------------------------------------------

    StringTarget()
    {
        target_ = ::genesis::utils::to_string( data_ );
    }

    // -------------------------------------------------------------------------
    //     Accessors
    // -------------------------------------------------------------------------

    std::string& get()
    {
        target_->flush();
        return data_;
    }

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::string data_;
    std::shared_ptr<::genesis::utils::BaseOutputTarget> target_;

};

}
}

inline void extend_string_target(pybind11::module &handle)
{
    pybind11::class_<::genesis::utils::StringTarget>(handle, "StringTarget")
     .def(pybind11::init())
     .def("get", &genesis::utils::StringTarget::get);

    handle.def(
        "to_string",
        []( ::genesis::utils::StringTarget *target_string) -> std::shared_ptr<::genesis::utils::StringOutputTarget>
        {
            return std::make_shared< ::genesis::utils::StringOutputTarget >( target_string->get() );
        }
        ,
        pybind11::return_value_policy::move
    );
}

template<class T>
inline void extend_fasta_output_iterator( pybind11::class_<T, std::shared_ptr<T> >& cl )
{
    cl.def(
        pybind11::init( [](genesis::utils::StringTarget *target_string)
        {
            return new genesis::sequence::FastaOutputIterator( genesis::utils::to_string( target_string->get() ) );
        }),
        pybind11::return_value_policy::move
    );
}

#endif
