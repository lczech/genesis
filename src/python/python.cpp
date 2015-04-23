/**
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

//~ #ifdef PYTHON

#include <boost/python.hpp>

//~ #include "genesis.hpp"
#include "utils/bitvector.hpp"

BOOST_PYTHON_MODULE(genesis)
{
    using namespace boost::python;
    using namespace genesis;

    class_<Bitvector>("Bitvector", init<size_t>())
        .def("Get",  &Bitvector::Get)
        .def("Set",  (void(Bitvector::*)(size_t, bool)) &Bitvector::Set)
        .def("Set",  (void(Bitvector::*)(size_t)) &Bitvector::Set)
        .def("Dump", &Bitvector::Dump)
    ;
}

//~ #endif
