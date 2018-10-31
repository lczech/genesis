/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Point>
void PythonExportClass_::genesis::utils::Kmeans(std::string name)
{

    // -------------------------------------------------------------------
    //     Class Kmeans
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using KmeansType = Kmeans<typename Point>;

    pybind11::class_< KmeansType, std::shared_ptr<KmeansType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< Kmeans const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "assignments",
            ( Kmeans & ( KmeansType::* )( std::vector< size_t > const & ))( &KmeansType::assignments ),
            pybind11::arg("value")
        )
        .def(
            "assignments",
            ( std::vector< size_t > const & ( KmeansType::* )(  ) const )( &KmeansType::assignments )
        )
        .def(
            "centroids",
            ( Kmeans & ( KmeansType::* )( std::vector< Point > const & ))( &KmeansType::centroids ),
            pybind11::arg("value")
        )
        .def(
            "centroids",
            ( std::vector< Point > const & ( KmeansType::* )(  ) const )( &KmeansType::centroids )
        )
        .def(
            "clear",
            ( void ( KmeansType::* )(  ))( &KmeansType::clear )
        )
        .def(
            "cluster_info",
            ( KmeansClusteringInfo ( KmeansType::* )( std::vector< Point > const & ) const )( &KmeansType::cluster_info ),
            pybind11::arg("data")
        )
        .def(
            "cluster_sizes",
            ( std::vector< size_t > ( KmeansType::* )(  ) const )( &KmeansType::cluster_sizes )
        )
        .def(
            "initialization_strategy",
            ( Kmeans & ( KmeansType::* )( KmeansInitializationStrategy ))( &KmeansType::initialization_strategy ),
            pybind11::arg("value")
        )
        .def(
            "initialization_strategy",
            ( KmeansInitializationStrategy ( KmeansType::* )(  ) const )( &KmeansType::initialization_strategy )
        )
        .def(
            "max_iterations",
            ( Kmeans & ( KmeansType::* )( size_t ))( &KmeansType::max_iterations ),
            pybind11::arg("value")
        )
        .def(
            "max_iterations",
            ( size_t ( KmeansType::* )(  ) const )( &KmeansType::max_iterations )
        )
        .def(
            "run",
            ( size_t ( KmeansType::* )( std::vector< Point > const &, size_t const ))( &KmeansType::run ),
            pybind11::arg("data"),
            pybind11::arg("k")
        )
    ;
}
