/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Base>
void PythonExportClass_::genesis::tree::DefaultTreeNewickWriterMixin(std::string name)
{

    // -------------------------------------------------------------------
    //     Class DefaultTreeNewickWriterMixin
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using DefaultTreeNewickWriterMixinType = DefaultTreeNewickWriterMixin<typename Base>;

    pybind11::class_< DefaultTreeNewickWriterMixinType, std::shared_ptr<DefaultTreeNewickWriterMixinType> > ( scope, name.c_str() )

        // Public Member Functions

        .def(
            "enable_branch_lengths",
            ( bool ( DefaultTreeNewickWriterMixinType::* )(  ))( &DefaultTreeNewickWriterMixinType::enable_branch_lengths )
        )
        .def(
            "enable_branch_lengths",
            ( void ( DefaultTreeNewickWriterMixinType::* )( bool ))( &DefaultTreeNewickWriterMixinType::enable_branch_lengths ),
            pybind11::arg("value")
        )
        .def(
            "enable_names",
            ( bool ( DefaultTreeNewickWriterMixinType::* )(  ))( &DefaultTreeNewickWriterMixinType::enable_names )
        )
        .def(
            "enable_names",
            ( void ( DefaultTreeNewickWriterMixinType::* )( bool ))( &DefaultTreeNewickWriterMixinType::enable_names ),
            pybind11::arg("value")
        )
    ;
}
