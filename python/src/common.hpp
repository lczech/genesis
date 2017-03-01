#ifndef GENESIS_PYTHON_COMMON_H_
#define GENESIS_PYTHON_COMMON_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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

#include <pybind11/pybind11.h>
#include <pybind11/cast.h>
#include <pybind11/operators.h>
#include <pybind11/stl.h>

#include <cassert>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <iostream>

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

// Helper to concat __LINE__ with other stuff
#define CONCAT( a, b ) a ## b
#define TOKENIZE( a, b ) CONCAT( a, b )

/**
 * @brief Class export macro.
 *
 * The class registration is performed by calling RegisterPythonExportClass::register_class_initializer.
 * In order to call this when the program is started, we initiate a static object of type
 * RegisterPythonExportClass<classname> with name PythonClassExporterInstance##FULL_NAME; the
 * constructor calls (with an intermediate step) the function PythonExportWrapper<T>().
 * The body of the PYTHON_EXPORT_CLASS(classname) { .. } macro is the body of
 * PythonExportWrapper<classname>.
 */
#define PYTHON_EXPORT_CLASS( FULL_NAME, OUT_SCOPE )                                   \
    namespace {                                                                             \
        RegisterPythonExportClass<FULL_NAME> TOKENIZE( PythonClassExporterInstance, __LINE__ )( #FULL_NAME ); \
    }                                                                                       \
    template<> inline void PythonExportWrapper<FULL_NAME>( pybind11::module& OUT_SCOPE )

/**
* @brief Call this macro inside the initialization function to tell the system that
* another class must be initialized previously (particularly, a base class)
*/
#define PYTHON_REQUIRES_CLASS(FULL_NAME, OUT_SCOPE) \
    RegisterPythonExportClass<FULL_NAME>::PythonExportWrapperDelegator( pybind11::module& OUT_SCOPE );

/**
 * @brief Function export macro. Works the same as the class export macro.
 */
#define PYTHON_EXPORT_FUNCTIONS( IDENTIFIER, SCOPE, OUT_SCOPE )                       \
    class PythonFuncWrapperClass##IDENTIFIER;                                        \
    namespace {                                                                      \
        RegisterPythonExportFunction<PythonFuncWrapperClass##IDENTIFIER>             \
            PythonFuncExporterInstance##IDENTIFIER( #SCOPE );                           \
    }                                                                                \
    template<> inline void PythonExportWrapper<PythonFuncWrapperClass##IDENTIFIER>( pybind11::module& OUT_SCOPE )

/**
 * @brief Template for export definition function.
 *
 * This wrapper function template is instanciated for each class and contains the python class
 * definition as its body. Thus, we do not provide an implementation here - this will be done in the
 * respective instanciations for each class.
 */
template<typename T> void PythonExportWrapper( pybind11::module& scope );

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
    void register_class_initializer(const std::string& ns, void (*func)( pybind11::module& )) {
        auto parent_scope = split_last_scope_name(ns).first;
        class_initializers.push_back(std::make_pair( parent_scope, func ));
    }

    /**
     * @brief Register a Python free function initialization function.
     */
    void register_func_initializer(const std::string& ns, void (*func)( pybind11::module& )) {
        func_initializers.push_back(std::make_pair( ns, func ));
    }

    /**
     * @brief Main Python initialization function.
     *
     * Must only be called after all initializers have been registered.
     */
    pybind11::module& init_python() {
        // Export all classes.
        for (auto it = class_initializers.begin(); it != class_initializers.end(); ++it) {
            // Set the current scope to the new sub-module. As long as this variable lives (which is
            // for the body of this loop only), all new exports go into that scope.
            auto& current_scope = get_scope( (*it).first );

            // Export into the namespace.
            (*it).second( current_scope );
        }

        // Export all functions. Comes after the classes, to make sure that all necessary types
        // are known to the functions.
        for (auto it = func_initializers.begin(); it != func_initializers.end(); ++it) {
            auto& current_scope = get_scope( (*it).first );
            (*it).second( current_scope );
        }

        return get_scope( "::" + MODULE_NAME );
    }

private:
    /**
     * @brief Private constructor, inits the scopes with the module namespace.
     */
    PythonExportHandler() {
        scopes[ "::" + MODULE_NAME ] = std::make_shared<pybind11::module>(
            MODULE_NAME.c_str(), ("Bindings for " + MODULE_NAME + " namespace").c_str()
        );
    }

    std::pair<std::string, std::string> split_last_scope_name( std::string ns )
    {
        // Check some border cases.
        if( ns == "" || ns == MODULE_NAME ) {
            ns = "::" + MODULE_NAME;
        }
        assert( ! ns.empty() );

        // Always use fully-qualified namespaces, starting with ::
        if( ns[0] != ':' ) {
            ns = "::" + ns;
        }

        // Split away the last sub-scope, so that we can add it to its parent scope.
        auto pos = ns.find_last_of( ":" );
        if( pos == std::string::npos || pos == ns.length() - 1 || pos == 0 || ns[pos-1] != ':' ) {
            throw std::runtime_error( "Invalid scope: '" + ns + "'." );
        }
        std::string parent_ns = ns.substr(0, pos-1);
        std::string child_ns  = ns.substr(pos+1, ns.length());

        return { parent_ns, child_ns };
    }

    /**
     * @brief Get the scope object belonging to a namespace.
     *
     * This function returns a python object that can be set as scope for exporting classes to
     * python, e.g. in init_python(). It takes the dot-separated namespace name as input and creates
     * all neccessary scopes recursively.
     */
    pybind11::module& get_scope( std::string ns )
    {
        // If we already constructed the scope object, return it.
        if( scopes.count(ns) > 0 ) {
            return *scopes[ns];
        }

        // Get the partent and chile scope.
        auto pc_scopes = split_last_scope_name(ns);

        // Get the parent scope (recursively, might also create it if neccessary) and set it as the
        // current scope. The child scope that we are about to create will thus be added to it.
        pybind11::module& parent_scope = get_scope(pc_scopes.first);

        // Add the new scope to the map and return it.
        scopes[ns] = std::make_shared<pybind11::module>(
            parent_scope.def_submodule(
                pc_scopes.second.c_str(), ("Bindings for " + ns + " namespace").c_str()
            )
        );
        return *scopes[ns];
    }

    /**
     * @brief List of initializer functions for classes.
     *
     * We store the python namespace first, and then the initializer function that defines the
     * class definition.
     */
    std::vector< std::pair<std::string, void(*)( pybind11::module& )> > class_initializers;

    /**
     * @brief List of initializer functions for free functions.
     *
     * We store the python namespace first, and then the initializer function that defines the
     * free function definition.
     */
    std::vector< std::pair<std::string, void(*)( pybind11::module& )> > func_initializers;

    /**
     * @brief
     */
    std::map< std::string, std::shared_ptr<pybind11::module> > scopes;
};

// =================================================================================================
//     Python Export Registry Classes
// =================================================================================================

/**
 * @brief Helper class which is instantiated in order to register a Python class export function.
 *
 * Do not use this class directly, it is used by the PYTHON_... macros.
 *
 * Rules (for experts only):
 * Call RegisterPythonExportClass<Classname>() to register class Classname for export.
 * The export specification is given in the global function PythonExportWrapper<T>.
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
    static void PythonExportWrapperDelegator( pybind11::module& scope ) {
        static bool hasBeenCalled = false;
        if (!hasBeenCalled) {
            PythonExportWrapper<T>( scope );
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
