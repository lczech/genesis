/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

template <typename Tree>
void PythonExportClass_Bipartition(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Bipartition
    // -------------------------------------------------------------------

    using namespace ::genesis::tree;

    using BipartitionType = Bipartition<typename Tree>;

    boost::python::class_< BipartitionType > ( name.c_str(), boost::python::init< size_t >(( boost::python::arg("num_leaves") )) )

        // Public Member Functions

        .def(
            "invert",
            ( void ( BipartitionType::* )(  ))( &BipartitionType::invert )
        )
        .def(
            "link",
            ( LinkType * ( BipartitionType::* )(  ))( &BipartitionType::link ),
            boost::python::return_value_policy<boost::python::reference_existing_object>()
        )
    ;
}
