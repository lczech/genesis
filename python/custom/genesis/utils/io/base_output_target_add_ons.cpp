/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2023-2025 Giannis Reppas and Lucas Czech

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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

// Custom file in the python/custom dir
#include <genesis/utils/io/base_output_target_add_ons.hpp>

#include <string>

#include <pybind11/pybind11.h>

#include <genesis/utils/io/base_output_target.hpp>
#include <genesis/utils/io/output_target.hpp>

namespace py  = pybind11;

void genesis_utils_io_base_output_target_add_ons(
    pybind11::class_<
        genesis::utils::io::BaseOutputTarget,
        std::shared_ptr<genesis::utils::io::BaseOutputTarget>
    > &cl
) {
    // ---------------------------------------------------------------------
    //   Context Protocol
    // ---------------------------------------------------------------------

    // Support for Python context manager protocol, so that `with ...` works.
    // Except for flushing upon exit, we do not need to do much to enable this.

    // __enter__ returns self
    cl.def("__enter__",
        []( std::shared_ptr<genesis::utils::io::BaseOutputTarget> self )
            -> std::shared_ptr<genesis::utils::io::BaseOutputTarget>
        {
            return self;
        },
        py::return_value_policy::reference_internal,
        "Enter the context for `with` statements in the context manager protocol"
    );

    // __exit__ closes and returns False to propagate exceptions
    cl.def("__exit__",
        []( std::shared_ptr<genesis::utils::io::BaseOutputTarget> self,
           py::object /*exc_type*/,
           py::object /*exc_value*/,
           py::object /*traceback*/
        ) {
            // Make sure all content is written
            if( self ) {
                self->flush();
            }

            // Return false => any exception is reraised
            return false;
        },
        "Exit the context for `with` statements in the context manager protocol"
    );

    // ---------------------------------------------------------------------
    //   String Output Target
    // ---------------------------------------------------------------------

    // Support for Python string conversion, checking that the target is in fact a string target,
    // and throwing an exception otherwise - users are not supposed to call the string functions
    // on non-string targets. We offer this as an explicit getter, and implicit via str(...).

    // Avoid code duplication by re-using a lambda.
    auto string_target_getter = [](
        std::shared_ptr<genesis::utils::io::BaseOutputTarget> self
    ) -> std::string {
        // Downcast to the string-target type
        if( self ) {
            // Call the get() of the shared pointer to get the underlying raw pointer.
            auto s = dynamic_cast<genesis::utils::io::StringOutputTarget*>( self.get() );
            if( s ) {
                // Call the get() method of StringOutputTarget to get the string.
                return s->get();
            }
        }
        throw std::runtime_error("Cannot get string. Object is not a string output target");
    };

    // Add the two variants of string access for the target
    cl.def("get",
        string_target_getter,
        "Get the contents of the in-memory string target"
    );
    cl.def("__str__",
        string_target_getter,
        "Get the contents of the in-memory string target"
    );
}
