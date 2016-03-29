/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

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
