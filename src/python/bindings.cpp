/**
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>

#include "genesis.hpp"

// forward declarations of all exported classes
void BoostPythonExport_Bitvector();

BOOST_PYTHON_MODULE(genesis)
{
    using namespace boost::python;
    using namespace genesis;

    BoostPythonExport_Bitvector();

    //~ class_<Bitvector>("Bitvector", init<size_t>())
        //~ .def("Get",  &Bitvector::Get)
        //~ .def("Set",  (void(Bitvector::*)(size_t))       &Bitvector::Set)
        //~ .def("Set",  (void(Bitvector::*)(size_t, bool)) &Bitvector::Set)
        //~ .def("Dump", &Bitvector::Dump)
    //~ ;

    //~ class_<SequenceSet>("SequenceSet")
        //~ .def("Dump",  &SequenceSet::Dump)
    //~ ;
}
