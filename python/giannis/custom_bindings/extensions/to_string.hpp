#ifndef CUSTOM_BINDING_TO_STRING_INCLUDED_
#define CUSTOM_BINDING_TO_STRING_INCLUDED_

#include "genesis/utils/io/base_output_target.hpp"
#include "genesis/utils/io/string_output_target.hpp"

#include "../python/custom_bindings/extensions/string_target.hpp"

#include <iostream>

inline void extend_to_string(pybind11::module &handle)
{
    handle.def(
        "to_string",
        []( ::genesis::utils::StringTarget *target_string) -> std::shared_ptr<::genesis::utils::BaseOutputTarget>
        {
            return std::make_shared< ::genesis::utils::StringOutputTarget >( target_string->get() );
        }
    );
}

#endif