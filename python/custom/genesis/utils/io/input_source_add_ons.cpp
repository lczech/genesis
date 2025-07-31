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
#include <genesis/utils/io/input_source_add_ons.hpp>

#include <istream>
#include <memory>
#include <sstream>
#include <streambuf>
#include <string_view>
#include <string>
#include <utility>

#include <pybind11/pybind11.h>
#include <pybind11/buffer_info.h>
#include <pybind11/iostream.h>

#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/input_source.hpp>

namespace py  = pybind11;

// ================================================================================================
//   Python Input Stream Buffer
// ================================================================================================

// Streambuf that pulls data from a Python file‐like via its read() method
class PythonInputStreambuf : public std::streambuf
{
public:
    // how many bytes to read per underflow
    static constexpr std::size_t BUFSIZE = 4096 * 1024;

    PythonInputStreambuf(py::object source)
        : source_(std::move(source))
        , buffer_(BUFSIZE)
    {
        // no data in buffer yet
        setg(buffer_.data(), buffer_.data(), buffer_.data());
    }

protected:
    // Called when the input sequence is exhausted
    int_type underflow() override
    {
        if (gptr() < egptr()) {
            return traits_type::to_int_type(*gptr());
        }

        // Make sure we are locked
        py::gil_scoped_acquire acquire;

        // file.read(BUFSIZE)
        py::object chunk = source_.attr("read")(BUFSIZE);
        if (chunk.is_none()) {
            return traits_type::eof();
        }

        // Convert to bytes or str --> string
        std::string data = chunk.cast<std::string>();
        if (data.empty()) {
            return traits_type::eof();
        }

        // copy into our buffer
        std::memcpy(buffer_.data(), data.data(), data.size());
        setg(
            buffer_.data(),
            buffer_.data(),
            buffer_.data() + data.size()
        );

        return traits_type::to_int_type(*gptr());
    }

private:
    py::object        source_;
    std::vector<char> buffer_;
};

// ================================================================================================
//   Add On
// ================================================================================================

void genesis_utils_io_input_source_add_ons(py::module &m)
{
    // ---------------------------------------------------------------------
    //   from_file
    // ---------------------------------------------------------------------

    // The from_file wrapper is a bit complex to get all the lifetime management,
    // automatic conversions, function defaults, etc to work...
    //
    // We want to allow the following:
    //
    //     with open(self.path, 'r') as f:
    //         src = utils.io.from_file(f)
    //
    // For that to work efficiently without copying the whole file content upon opening it,
    // we need to use the buffer protocol, and wrap that, and use a holder,
    // tied in lifetime with the returned object itself, so that the buffer and stream
    // used in the wrapper stay alive while the file is being written to.

    // Helper to own the buffer+stream for the lifetime of the Python object
    struct FileBufferHolder {
        PythonInputStreambuf buf;
        std::istream stream;

        FileBufferHolder(py::object f)
            : buf(f)
            , stream(&buf)
        {}
    };

    // Cache the two Python classes we'll test against:
    py::object Path        = py::module::import("pathlib").attr("Path");
    py::object TextIOBase  = py::module::import("io").attr("TextIOBase");

    // Define the actual function
    m.def("from_file",
        [Path, TextIOBase](py::object src) -> std::shared_ptr<genesis::utils::io::BaseInputSource>
        {
            // 1) string path
            if( py::isinstance<py::str>( src )) {
                return genesis::utils::io::from_file(src.cast<std::string>());
            }
            // 2) pathlib.Path
            if( py::isinstance( src, Path )) {
                std::string p = src.attr("as_posix")().cast<std::string>();
                return genesis::utils::io::from_file(p);
            }
            // 3) Fallback for file‐like objects (read content as stream)
            if( py::hasattr( src, "read" )) {

                // Check if the read attribute is callable
                auto read_attr = src.attr("read");
                if (!PyCallable_Check(read_attr.ptr())) {
                    throw std::invalid_argument(
                        "from_file(): file-like object read() attribute is not callable"
                    );
                }

                // Keep the pythonbuf + istream alive
                auto holder_sp = std::make_shared<FileBufferHolder>(src);

                // Call from_stream on the istream
                auto original_src = genesis::utils::io::from_stream(holder_sp->stream);

                // Bundle them into one control block
                auto joint = std::make_shared<
                    std::pair<
                        std::shared_ptr<genesis::utils::io::BaseInputSource>,
                        std::shared_ptr<FileBufferHolder>
                    >
                >(original_src, holder_sp);

                // Create an aliasing shared_ptr to the BaseInputSource,
                // which shares ownership of both the target and the holder,
                // meaning that Python now owns both objects.
                return std::shared_ptr<genesis::utils::io::BaseInputSource>(
                    joint,
                    joint->first.get()
                );
            }

            // Old way which would have copied the whole file content to memory.
            // if (py::hasattr(src, "read")) {
            //     std::string data = src.attr("read")().cast<std::string>();
            //     return genesis::utils::io::from_string(std::move(data));
            // }
            throw std::invalid_argument(
                "from_file(): expected filename (str or pathlib.Path) or file-like object with read()"
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
        "C++: genesis::utils::io::from_file(const std::string &) --> class std::shared_ptr<class genesis::utils::io::BaseInputSource>"
    );

    // ---------------------------------------------------------------------
    //   from_string
    // ---------------------------------------------------------------------

    // The from_string wrapper is a bit complex to get all the lifetime management,
    // automatic conversions, function defaults, etc to work...
    //
    // We want to allow the following:
    //
    //     data = b"some string"
    //     # or data = some_string.encode('ascii')
    //     source = utils.io.from_string(data)
    //
    // For that to work, we need to use the buffer protocol, and wrap that, and use a holder,
    // tied in lifetime with the returned object itself, so that the buffer and stream
    // used in the wrapper stay alive while the file is being written to.

    // Helper to own the buffer+stream for the lifetime of the Python object
    struct StringBufferHolder {
        py::buffer        buf;
        py::buffer_info   info;
        std::string_view  sv;

        StringBufferHolder(py::buffer b)
            : buf(b)
            , info(b.request())
            , sv(static_cast<const char*>(info.ptr), info.size * info.itemsize)
        {}
    };

    m.def("from_string",
        [](py::object src) -> std::shared_ptr<genesis::utils::io::BaseInputSource>
        {
            // 1) Simple string
            if( py::isinstance<py::str>( src )) {
                // We need this custom binding, as the string conversion creates a temporary string
                // from the py object, and so our StringInputSource cannot point to it.
                // We might want to change this in the future by having an overload of from_string
                // in cpp that takes a string by value instead?!
                return genesis::utils::io::from_string(src.cast<std::string>());
            }
            // 2) Any object exposing the buffer protocol (zero-copy via aliasing shared_ptr)
            if( py::isinstance<py::buffer>( src )) {
                // Create a shared StringBufferHolder
                py::buffer buf_obj = src.cast<py::buffer>();
                auto holder_sp = std::make_shared<StringBufferHolder>(buf_obj);

                // Call the string_view source function
                auto original_src = genesis::utils::io::from_string_view( holder_sp->sv );

                // Bundle source + holder into one control block
                auto joint = std::make_shared<
                    std::pair<
                        std::shared_ptr<genesis::utils::io::BaseInputSource>,
                        std::shared_ptr<StringBufferHolder>
                    >
                >( original_src, holder_sp );

                // Create an aliasing shared_ptr to the BaseInputSource,
                // which shares ownership of both the target and the holder,
                // meaning that Python now owns both objects.
                std::shared_ptr<genesis::utils::io::BaseInputSource> ret(
                    joint,
                    joint->first.get()
                );
                return ret;
            }
            throw std::invalid_argument(
                "from_string(): expected str, or buffer-protocol object"
            );
        },
        py::return_value_policy::reference,
        "Obtain an input source for reading from a `std::string`.\n\n"
        "The input source returned from this function can be used in the reader classes, e.g.,\n"
        "placement::JplaceReader or sequence::FastaReader.\n\n \n See from_file(), from_files(), "
        "from_strings(), and from_stream() for similar\n helper functions for other types of input "
        "sources.\n\nC++: genesis::utils::io::from_string(const std::string &) --> class "
        "std::shared_ptr<class genesis::utils::io::BaseInputSource>"
    );
}
