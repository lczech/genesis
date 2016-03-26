/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_NewickColorWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class NewickColorWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using NewickColorWriterMixinType = NewickColorWriterMixin<typename Base>;

    boost::python::class_< NewickColorWriterMixinType > ( name.c_str() )

        // Public Member Functions

        .def(
            "color_tag_prefix",
            ( std::string ( NewickColorWriterMixinType::* )(  ))( &NewickColorWriterMixinType::color_tag_prefix ),
            get_docstring("std::string ::genesis::tree::NewickColorWriterMixin::color_tag_prefix ()")
        )
        .def(
            "color_tag_prefix",
            ( void ( NewickColorWriterMixinType::* )( std::string ))( &NewickColorWriterMixinType::color_tag_prefix ),
            ( boost::python::arg("prefix") ),
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
            ( boost::python::arg("suffix") ),
            get_docstring("void ::genesis::tree::NewickColorWriterMixin::color_tag_suffix (std::string suffix)")
        )
    ;
}
