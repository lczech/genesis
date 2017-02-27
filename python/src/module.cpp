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
 * @brief Definition of the genesis Python module.
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>
// #include <pybind11/pybind11.h>

// #include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <map>
#include <memory>
#include <stdexcept>
#include <functional>

// =================================================================================================
//     Pybind11 Python Module
// =================================================================================================


typedef std::function< pybind11::module & (std::string const &) > ModuleGetter;

void bind_genesis_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);



// int add(int i, int j) {
//     return i + j;
// }
//
// namespace py = pybind11;
//
// PYBIND11_PLUGIN(genesis) {
//     py::module m("genesis", "pybind11 example plugin");
//
//     m.def("add", &add, "A function which adds two numbers");
//
//     return m.ptr();
// }


PYBIND11_PLUGIN(genesis) {
	std::map <std::string, std::shared_ptr<pybind11::module> > modules;
	ModuleGetter M = [&](std::string const &namespace_) -> pybind11::module & {
		auto it = modules.find(namespace_);
		if( it == modules.end() ) throw std::runtime_error("Attempt to access pybind11::module for namespace " + namespace_ + " before it was created!!!");
		return * it->second;
	};

	modules[""] = std::make_shared<pybind11::module>("genesis", "genesis module");

	std::vector< std::pair<std::string, std::string> > sub_modules {
		{"", "genesis"},
		{"genesis", "placement"},
		{"genesis", "sequence"},
		{"genesis", "taxonomy"},
		{"genesis", "tree"},
		{"genesis", "utils"},
		{"genesis::utils", "(anonymou"},
		{"", "std"},
		{"std", "__detail"},
	};
	for(auto &p : sub_modules ) modules[p.first.size() ? p.first+"::"+p.second : p.second] = std::make_shared<pybind11::module>( modules[p.first]->def_submodule(p.second.c_str(), ("Bindings for " + p.first + "::" + p.second + " namespace").c_str() ) );

    bind_genesis_tree_tree(M);

	return modules[""]->ptr();
}
