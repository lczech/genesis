/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_CLASS( ::genesis::sequence::Sequence, scope )
{

    // -------------------------------------------------------------------
    //     Class Sequence
    // -------------------------------------------------------------------

    pybind11::class_< ::genesis::sequence::Sequence, std::shared_ptr<::genesis::sequence::Sequence> > ( scope, "Sequence" )
        .def(
            pybind11::init<  >()
        )
        .def(
            pybind11::init< std::string const &, std::string const & >(),
            pybind11::arg("label"),
            pybind11::arg("sites")
        )
        .def(
            pybind11::init< std::string const &, std::string const &, std::string const & >(),
            pybind11::arg("label"),
            pybind11::arg("metadata"),
            pybind11::arg("sites")
        )
        .def(
            pybind11::init< Sequence const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        // .def(
        //     "cbegin",
        //     ( const_iterator ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::cbegin )
        // )
        // .def(
        //     "cend",
        //     ( const_iterator ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::cend )
        // )
        .def(
            "clear",
            ( void ( ::genesis::sequence::Sequence::* )(  ))( &::genesis::sequence::Sequence::clear )
        )
        .def(
            "label",
            ( std::string const & ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::label )
        )
        .def(
            "label",
            ( void ( ::genesis::sequence::Sequence::* )( std::string const & ))( &::genesis::sequence::Sequence::label ),
            pybind11::arg("value")
        )
        .def(
            "length",
            ( size_t ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::length )
        )
        .def(
            "metadata",
            ( std::string const & ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::metadata )
        )
        .def(
            "metadata",
            ( void ( ::genesis::sequence::Sequence::* )( std::string const & ))( &::genesis::sequence::Sequence::metadata ),
            pybind11::arg("value")
        )
        .def(
            "site_at",
            ( char ( ::genesis::sequence::Sequence::* )( size_t ) const )( &::genesis::sequence::Sequence::site_at ),
            pybind11::arg("index")
        )
        .def(
            "sites",
            ( std::string & ( ::genesis::sequence::Sequence::* )(  ))( &::genesis::sequence::Sequence::sites )
        )
        .def(
            "sites",
            ( std::string const & ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::sites )
        )
        // .def(
        //     "sites",
        //     ( void ( ::genesis::sequence::Sequence::* )( std::string && ))( &::genesis::sequence::Sequence::sites ),
        //     pybind11::arg("value")
        // )
        .def(
            "sites",
            ( void ( ::genesis::sequence::Sequence::* )( std::string const & ))( &::genesis::sequence::Sequence::sites ),
            pybind11::arg("value")
        )
        .def(
            "size",
            ( size_t ( ::genesis::sequence::Sequence::* )(  ) const )( &::genesis::sequence::Sequence::size )
        )
        .def(
            "swap",
            ( void ( ::genesis::sequence::Sequence::* )( Sequence & ))( &::genesis::sequence::Sequence::swap ),
            pybind11::arg("other")
        )

        // Operators

        .def(
            "__getitem__",
            ( char ( ::genesis::sequence::Sequence::* )( size_t ) const )( &::genesis::sequence::Sequence::operator[] ),
            pybind11::arg("index")
        )
        .def(
            "__str__",
            []( ::genesis::sequence::Sequence const& obj ) -> std::string {
                std::ostringstream s;
                s << obj;
                return s.str();
            }
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::sequence::Sequence& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            pybind11::keep_alive<0, 1>()
        )
    ;
}
