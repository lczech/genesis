/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2025 Lucas Czech

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

#include <custom/utils/extra_bindings.hpp>

#include <sstream>
#include <string>
#include <string_view>

#include <pybind11/pybind11.h>
#include <pybind11/buffer_info.h>
#include <pybind11/iostream.h>

#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/input_source.hpp>

namespace py  = pybind11;

void genesis_binder_utils_io_input_source(py::module &m)
{
    m.def("from_file",
    [](py::object src) -> std::shared_ptr<genesis::utils::BaseInputSource>
    {
         // cache the two Python classes we'll test against:
        py::object Path        = py::module::import("pathlib").attr("Path");
        py::object TextIOBase  = py::module::import("io").attr("TextIOBase");

        // 1) string path
        if (py::isinstance<py::str>(src)) {
            return genesis::utils::from_file(src.cast<std::string>());
        }
        // 2) pathlib.Path
        if (py::isinstance(src, Path)) {
            std::string p = src.attr("as_posix")().cast<std::string>();
            return genesis::utils::from_file(p);
        }
        // 3) Any object exposing the buffer protocol
        if (py::isinstance<py::buffer>(src)) {
            // grab raw pointer + size without an extra py::bytes
            py::buffer buf = src.cast<py::buffer>();
            auto info = buf.request();
            auto ptr  = static_cast<const char*>(info.ptr);
            size_t n  = info.size * info.itemsize;
            std::string_view sv(ptr, n);
            return genesis::utils::from_string_view(sv);
        }
        // 4) Fallback for file‐like objects (read entire content once)
        if (py::hasattr(src, "read")) {
            std::string data = src.attr("read")().cast<std::string>();
            return genesis::utils::from_string(std::move(data));
        }
        throw std::invalid_argument(
            "from_file(): expected str, pathlib.Path, buffer-protocol object, or file-like object"
        );
    },
    py::return_value_policy::reference,
    "Obtain an input source for reading from a file.\n\n"
    "The input source returned from this function can be used in the reader classes, e.g.,\n"
    "placement::JplaceReader or sequence::FastaReader. It is first determined whether the\n"
    "file is gzip compressed, and if so, a transparent decompression layer is added.\n"
    "That means, gzip-compressed files can be decompressed automatically and on the fly.\n\n"
    "See from_files(), from_string(), from_strings(), and from_stream() for similar\n"
    "helper functions for other types of input sources.\n\n"
    "C++: genesis::utils::from_file(const std::string &) --> class std::shared_ptr<class genesis::utils::BaseInputSource>"
    );
}
