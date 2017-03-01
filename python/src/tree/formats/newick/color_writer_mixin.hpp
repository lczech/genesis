/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::tree::NewickColorWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class NewickColorWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using NewickColorWriterMixinType = NewickColorWriterMixin<typename Base>;

    pybind11::class_< NewickColorWriterMixinType, std::shared_ptr<NewickColorWriterMixinType> > ( scope, name.c_str() )

        // Public Member Functions

        .def(
            "color_tag_prefix",
            ( std::string ( NewickColorWriterMixinType::* )(  ))( &NewickColorWriterMixinType::color_tag_prefix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterMixin::color_tag_prefix ()")
        )
        .def(
            "color_tag_prefix",
            ( void ( NewickColorWriterMixinType::* )( std::string ))( &NewickColorWriterMixinType::color_tag_prefix ),
            pybind11::arg("prefix"),
            get_docstring("void ::genesis::tree::NewickColorWriterMixin::color_tag_prefix (std::string prefix)")
        )
        .def(
            "color_tag_suffix",
            ( std::string ( NewickColorWriterMixinType::* )(  ))( &NewickColorWriterMixinType::color_tag_suffix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterMixin::color_tag_suffix ()")
        )
        .def(
            "color_tag_suffix",
            ( void ( NewickColorWriterMixinType::* )( std::string ))( &NewickColorWriterMixinType::color_tag_suffix ),
            pybind11::arg("suffix"),
            get_docstring("void ::genesis::tree::NewickColorWriterMixin::color_tag_suffix (std::string suffix)")
        )
    ;
}
