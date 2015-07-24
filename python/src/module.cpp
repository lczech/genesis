/**
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

// #include <boost/python/suite/indexing/vector_indexing_suite.hpp>

// =================================================================================================
//     Boost Python Module
// =================================================================================================

BOOST_PYTHON_MODULE(genesis)
{
    namespace bp = boost::python;

    // Show genesis docstrings, python signature, but not c++ signature.
    bp::docstring_options doc_options(true, true, false);

    // Specify that this module is actually a package.
    bp::object package = bp::scope();
    package.attr("__path__") = MODULE_NAME;

    // Call all export functions.
    PythonExportHandler::instance().init_python();
}
