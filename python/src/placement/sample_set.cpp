/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::placement;

PYTHON_EXPORT_CLASS( ::genesis::placement::SampleSet, scope )
{

    // -------------------------------------------------------------------
    //     Class SampleSet
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::placement::SampleSet, std::shared_ptr<::genesis::placement::SampleSet> > ( scope, "SampleSet" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< SampleSet const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "add",
        //     ( void ( ::genesis::placement::SampleSet::* )( Sample && ))( &::genesis::placement::SampleSet::add ),
        //     pybind11::arg("smp"),
        //     get_docstring("void ::genesis::placement::SampleSet::add (Sample && smp)")
        // )
        // .def(
        //     "add",
        //     ( void ( ::genesis::placement::SampleSet::* )( Sample &&, std::string const & ))( &::genesis::placement::SampleSet::add ),
        //     pybind11::arg("smp"),
        //     pybind11::arg("name"),
        //     get_docstring("void ::genesis::placement::SampleSet::add (Sample && smp, std::string const & name)")
        // )
        .def(
            "add",
            ( void ( ::genesis::placement::SampleSet::* )( Sample const & ))( &::genesis::placement::SampleSet::add ),
            pybind11::arg("smp"),
            get_docstring("void ::genesis::placement::SampleSet::add (Sample const & smp)")
        )
        .def(
            "add",
            ( void ( ::genesis::placement::SampleSet::* )( Sample const &, std::string const & ))( &::genesis::placement::SampleSet::add ),
            pybind11::arg("smp"),
            pybind11::arg("name"),
            get_docstring("void ::genesis::placement::SampleSet::add (Sample const & smp, std::string const & name)")
        )
        .def(
            "at",
            ( ::genesis::placement::SampleSet::NamedSample & ( ::genesis::placement::SampleSet::* )( size_t ))( &::genesis::placement::SampleSet::at ),
            pybind11::arg("index"),
            get_docstring("NamedSample & ::genesis::placement::SampleSet::at (size_t index)")
        )
        .def(
            "at",
            ( ::genesis::placement::SampleSet::NamedSample const & ( ::genesis::placement::SampleSet::* )( size_t ) const )( &::genesis::placement::SampleSet::at ),
            pybind11::arg("index"),
            get_docstring("NamedSample const & ::genesis::placement::SampleSet::at (size_t index) const")
        )
        .def(
            "clear",
            ( void ( ::genesis::placement::SampleSet::* )(  ))( &::genesis::placement::SampleSet::clear ),
            get_docstring("void ::genesis::placement::SampleSet::clear ()")
        )
        .def(
            "empty",
            ( bool ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::empty ),
            get_docstring("bool ::genesis::placement::SampleSet::empty () const")
        )
        .def(
            "remove_at",
            ( void ( ::genesis::placement::SampleSet::* )( size_t ))( &::genesis::placement::SampleSet::remove_at ),
            pybind11::arg("index"),
            get_docstring("void ::genesis::placement::SampleSet::remove_at (size_t index)")
        )
        .def(
            "size",
            ( size_t ( ::genesis::placement::SampleSet::* )(  ) const )( &::genesis::placement::SampleSet::size ),
            get_docstring("size_t ::genesis::placement::SampleSet::size () const")
        )
        .def(
            "swap",
            ( void ( ::genesis::placement::SampleSet::* )( SampleSet & ))( &::genesis::placement::SampleSet::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__getitem__",
            ( ::genesis::placement::SampleSet::NamedSample & ( ::genesis::placement::SampleSet::* )( size_t ))( &::genesis::placement::SampleSet::operator[] ),
            pybind11::arg("index"),
            get_docstring("NamedSample & ::genesis::placement::SampleSet::operator[] (size_t index)")
        )
        .def(
            "__getitem__",
            ( ::genesis::placement::SampleSet::NamedSample const & ( ::genesis::placement::SampleSet::* )( size_t ) const )( &::genesis::placement::SampleSet::operator[] ),
            pybind11::arg("index"),
            get_docstring("NamedSample const & ::genesis::placement::SampleSet::operator[] (size_t index) const")
        )
        .def(
            "__str__",
            []( ::genesis::placement::SampleSet const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::placement::SampleSet& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            }
            // ,
            // py::keep_alive<0, 1>()
        )
    ;
}
