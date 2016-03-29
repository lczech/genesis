#ifndef GENESIS_BOOST_PYTHON_EXPORT_COMMON_H_
#define GENESIS_BOOST_PYTHON_EXPORT_COMMON_H_

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
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <boost/python.hpp>
// #include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// =================================================================================================
//     Module "genesis" Definitions
// =================================================================================================

const std::string MODULE_NAME = "genesis";

// Dummy for the namespace.
namespace genesis {}

// This header is compiled into the python module, but not used in the library itself. Thus, we can
// include the whole namespace of genesis for ease of use here, without risking name conflicts.
using namespace genesis;

// =================================================================================================
//     Forward Declarations
// =================================================================================================

const char* get_docstring (const std::string& signature);

// =================================================================================================
//     Export Macros
// =================================================================================================

/**
 * @brief Scoped class export macro.
 *
 * The class registration is performed by calling PythonExport::registerClassInitializer. In order
 * to call this when the program is started, we initiate a static object of type
 * RegisterPythonExportClass<classname> with name PythonClassExporterInstance##Classname; the
 * constructor calls (with an intermediate step) the function PythonExportWrapper<T>().
 * The body of the PYTHON_EXPORT_CLASS(classname) { .. } macro is the body of
 * PythonExportWrapper<classname>.
 */
#define PYTHON_EXPORT_SCOPED_CLASS(Classname, Scope, ...)                                   \
    namespace {                                                                             \
        RegisterPythonExportClass<Classname> PythonClassExporterInstance##Classname(Scope); \
    }                                                                                       \
    template<> inline void PythonExportWrapper<Classname>()

/**
 * @brief Macro that takes a Classname and optionally a namespace to which this class shall be
 * exported.
 *
 * If no namespace is provided, the default (global) namespace is used. The zero in the end is a
 * dummy argument to give always at least this as a variadic parameter to the macro call.
 */
#define PYTHON_EXPORT_CLASS(...) PYTHON_EXPORT_SCOPED_CLASS(__VA_ARGS__, "", 0)

/**
* @brief Call this macro inside the initialization function to tell the system that
* another class must be initialized previously (particularly, a base class)
*/
#define PYTHON_REQUIRES_CLASS(Classname) \
    RegisterPythonExportClass<Classname>::PythonExportWrapperDelegator();

/**
 * @brief Scoped function export macro. Works the same as the scoped class export macro.
 */
#define PYTHON_EXPORT_SCOPED_FUNCTIONS(Identifier, Scope, ...)                       \
    class PythonFuncWrapperClass##Identifier;                                        \
    namespace {                                                                      \
        RegisterPythonExportFunction<PythonFuncWrapperClass##Identifier>             \
            PythonFuncExporterInstance##Identifier(Scope);                           \
    }                                                                                \
    template<> inline void PythonExportWrapper<PythonFuncWrapperClass##Identifier>()

/**
 * @brief Function export macro. Works the same as the class export macro.
 */
#define PYTHON_EXPORT_FUNCTIONS(...) PYTHON_EXPORT_SCOPED_FUNCTIONS(__VA_ARGS__, "", 0)

/**
 * @brief Template for export definition function.
 *
 * This wrapper function template is instanciated for each class and contains the python class
 * definition as its body. Thus, we do not provide an implementation here - this will be done in the
 * respective instanciations for each class.
 */
template<typename T> void PythonExportWrapper();

// =================================================================================================
//     Python Export Handler
// =================================================================================================

/**
 * @brief
 */
class PythonExportHandler
{
public:
    /**
     * @brief Return single instance of this class.
     */
    static PythonExportHandler& instance() {
        // Meyers-Singleton
        static PythonExportHandler instance;
        return instance;
    }

    /**
     * @brief Register a Python class initialization function.
     *
     * Functions registered first are called first.
     * Note that therefore, base classes must be registered before
     * derived classes, usually using the PYTHON_REQUIRES_CLASS macro.
     *
     * This function is called by the PYTHON_EXPORT_CLASS macro expansion.
     */
    void register_class_initializer(const std::string& ns, void (*func)()) {
        class_initializers.push_back(std::make_pair(ns, func));
    }

    /**
     * @brief Register a Python free function initialization function.
     */
    void register_func_initializer(const std::string& ns, void (*func)()) {
        func_initializers.push_back(std::make_pair(ns, func));
    }

    /**
     * @brief Main Python initialization function.
     *
     * Must only be called after all initializers have been registered.
     */
    void init_python() {
        namespace bp = boost::python;

        // Export all classes.
        for (auto it = class_initializers.begin(); it != class_initializers.end(); ++it) {
            // Set the current scope to the new sub-module. As long as this variable lives (which is
            // for the body of this loop only), all new exports go into that scope.
            bp::scope current_scope = get_scope(MODULE_NAME + "." + (*it).first);

            // Export into the namespace.
            (*it).second();
        }

        // Export all functions. Comes after the classes, to make sure that all necessary types
        // are known to the functions.
        for (auto it = func_initializers.begin(); it != func_initializers.end(); ++it) {
            bp::scope current_scope = get_scope(MODULE_NAME + "." + (*it).first);
            (*it).second();
        }
    }

private:
    /**
     * @brief Private constructor, does nothing.
     */
    PythonExportHandler() {}

    /**
     * @brief Get the scope object belonging to a namespace.
     *
     * This function returns a python object that can be set as scope for exporting classes to
     * python, e.g. in init_python(). It takes the dot-separated namespace name as input and creates
     * all neccessary scopes recursively.
     *
     * It assumes that the first part of `ns` is always "genesis" (respectively, MODULE_NAME),
     * as a marker for the global namespace. This is neccessary because we cannot store the global
     * namespace (it has no copy constructor, and referencing it is also not possible), so we need
     * this as a way of distinguishing when to return a python object that functions as a scope and
     * when to return the "real" global scope object.
     */
    boost::python::object get_scope (std::string ns)
    {
        namespace bp = boost::python;

        // End of recursion. The second condition also catches the case of an empty ns.
        // Return the global namespace. Unfortunately, we cannot store it in the scopes map, as
        // this is a non-copyable object, so we have to check this extra condition here.
        if (ns == MODULE_NAME || ns == MODULE_NAME + ".") {
            return bp::scope();
        }

        // If we already constructed the scope object, return it.
        if (scopes.count(ns) > 0) {
            return scopes[ns];
        }

        // Split away the last sub-scope, so that we can add it to its parent scope.
        size_t pos = ns.find_last_of(".");
        std::string parent_ns = ns.substr(0, pos);
        std::string child_ns  = ns.substr(pos+1, ns.length());

        // Get the parent scope (recursively, might also create it if neccessary) and set it as the
        // current scope. The child scope that we are about to create will thus be added to it.
        bp::scope parent_scope = get_scope(parent_ns);

        // Create the sub scope into its parent.
        // The following two lines will make both ways for importing work:
        //     * "from genesis.ns import <whatever>"
        //     * "from genesis import ns"
        bp::object py_module (bp::handle<>(bp::borrowed(PyImport_AddModule(ns.c_str()))));
        parent_scope.attr(child_ns.c_str()) = py_module;

        // Add the new scope to the map and return it.
        scopes[ns] = py_module;
        return py_module;
    }

    /**
     * @brief List of initializer functions for classes.
     *
     * We store the python namespace first, and then the initializer function that defines the
     * class definition.
     */
    std::vector<std::pair<std::string, void(*)()>> class_initializers;

    /**
     * @brief List of initializer functions for free functions.
     *
     * We store the python namespace first, and then the initializer function that defines the
     * free function definition.
     */
    std::vector<std::pair<std::string, void(*)()>> func_initializers;

    /**
     * @brief
     */
    std::unordered_map<std::string, boost::python::object> scopes;
};

/**
 * @brief Helper class which is instantiated in order to register a Python class export function.
 *
 * Do not use this class directly, it is used by the PYTHON_... macros.
 *
 * Rules (for experts only):
 * Call RegisterPythonExportClass<Classname>() to register class Classname for export.
 * The export specification is given in the global function PythonExportWrapper<Setname>.
 * That function is free to also export subclasses etc.
 */
template<typename T>
class RegisterPythonExportClass {
public:
    RegisterPythonExportClass(const std::string& ns) {
        PythonExportHandler::instance().register_class_initializer(
            ns,
            &RegisterPythonExportClass<T>::PythonExportWrapperDelegator
        );
    }

    /**
     * @brief This tells the system that another class must be initialized previously
     * (particularly, a base class).
     */
    static void PythonExportWrapperDelegator() {
        static bool hasBeenCalled = false;
        if (!hasBeenCalled) {
            PythonExportWrapper<T>();
            hasBeenCalled = true;
        }
    }
};

/**
 * @brief Helper class which is instantiated in order to register a Python free function
 * export function.
 *
 * Works similar to RegisterPythonExportClass. See there for details.
 */
template<typename T>
class RegisterPythonExportFunction {
public:
    RegisterPythonExportFunction(const std::string& ns) {
        PythonExportHandler::instance().register_func_initializer(
            ns,
            &PythonExportWrapper<T>
        );
    }
};

#endif // include guard
