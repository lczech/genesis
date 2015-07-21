/**
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

// #include <boost/python.hpp>
// #include <boost/python/suite/indexing/vector_indexing_suite.hpp>

// =================================================================================================
//     Boost Python Module
// =================================================================================================

BOOST_PYTHON_MODULE(genesis)
{
    // Show genesis docstrings, python signature, but not c++ signature.
    boost::python::docstring_options doc_options(true, true, false);

    PythonExportHandler::instance().init_python();
}
