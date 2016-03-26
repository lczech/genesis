/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Base>
void PythonExportClass_DefaultTreeNewickWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNewickWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreeNewickWriterMixinType = DefaultTreeNewickWriterMixin<typename Base>;

    boost::python::class_< DefaultTreeNewickWriterMixinType > ( name.c_str() )

        // Public Member Functions

        .def(
            "enable_branch_lengths",
            ( bool ( DefaultTreeNewickWriterMixinType::* )(  ))( &DefaultTreeNewickWriterMixinType::enable_branch_lengths )
        )
        .def(
            "enable_branch_lengths",
            ( void ( DefaultTreeNewickWriterMixinType::* )( bool ))( &DefaultTreeNewickWriterMixinType::enable_branch_lengths ),
            ( boost::python::arg("value") )
        )
        .def(
            "enable_names",
            ( bool ( DefaultTreeNewickWriterMixinType::* )(  ))( &DefaultTreeNewickWriterMixinType::enable_names )
        )
        .def(
            "enable_names",
            ( void ( DefaultTreeNewickWriterMixinType::* )( bool ))( &DefaultTreeNewickWriterMixinType::enable_names ),
            ( boost::python::arg("value") )
        )
    ;
}
