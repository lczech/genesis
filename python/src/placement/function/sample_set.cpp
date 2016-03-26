/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_FUNCTIONS(placement_function_sample_set_export, "placement")
{

    boost::python::def(
        "find_sample",
        ( Sample * ( * )( SampleSet &, std::string const & ))( &::genesis::placement::find_sample ),
        ( boost::python::arg("sset"), boost::python::arg("name") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("Sample * ::genesis::placement::find_sample (SampleSet & sset, std::string const & name)")
    );

    boost::python::def(
        "merge_all",
        ( Sample ( * )( SampleSet const & ))( &::genesis::placement::merge_all ),
        ( boost::python::arg("sset") ),
        get_docstring("Sample ::genesis::placement::merge_all (SampleSet const & sset)")
    );

    boost::python::def(
        "find_sample",
        ( Sample const * ( * )( SampleSet const &, std::string const & ))( &::genesis::placement::find_sample ),
        ( boost::python::arg("sset"), boost::python::arg("name") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>(),
        get_docstring("Sample const * ::genesis::placement::find_sample (SampleSet const & sset, std::string const & name)")
    );

    boost::python::def(
        "all_identical_trees",
        ( bool ( * )( SampleSet const & ))( &::genesis::placement::all_identical_trees ),
        ( boost::python::arg("sset") ),
        get_docstring("bool ::genesis::placement::all_identical_trees (SampleSet const & sset)")
    );

    boost::python::def(
        "operator<<",
        ( std::ostream & ( * )( std::ostream &, SampleSet const & ))( &::genesis::placement::operator<< ),
        ( boost::python::arg("out"), boost::python::arg("sset") ),
        boost::python::return_value_policy<boost::python::reference_existing_object>()
    );

    boost::python::def(
        "tree_set",
        ( tree::TreeSet< PlacementTree > ( * )( SampleSet const & ))( &::genesis::placement::tree_set ),
        ( boost::python::arg("sset") ),
        get_docstring("tree::TreeSet< PlacementTree > ::genesis::placement::tree_set (SampleSet const & sset)")
    );
}
