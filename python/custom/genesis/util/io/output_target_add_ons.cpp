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
#include <genesis/util/io/output_target_add_ons.hpp>

#include <memory>
#include <ostream>
#include <sstream>
#include <streambuf>
#include <string_view>
#include <string>
#include <utility>
#include <vector>

#include <pybind11/pybind11.h>
#include <pybind11/buffer_info.h>
#include <pybind11/iostream.h>

#include <genesis/util/io/base_output_target.hpp>
#include <genesis/util/io/output_target.hpp>

namespace py  = pybind11;

// ================================================================================================
//   Python Output Target Buffer
// ================================================================================================

// Streambuf that accumulates chars and writes them as Python bytes to dest.write(...)
class PythonOutputStreambuf : public std::streambuf
{
public:
    static constexpr std::size_t BUFSIZE = 65536; // 2^16

    PythonOutputStreambuf(py::object dest)
        : dest_(std::move(dest))
        , buffer_(BUFSIZE)
        , python_valid_(true)
    {
        // Check if dest is text or binary at construction time
        try {
            py::gil_scoped_acquire acquire;
            is_text_ = py::isinstance(dest_, py::module::import("io").attr("TextIOBase"));
        } catch (...) {
            // Default to binary if we cannot determine from the object
            is_text_ = false;
        }

        // set the put area to our buffer
        setp(buffer_.data(), buffer_.data() + buffer_.size());
    }

    ~PythonOutputStreambuf() override
    {
        // Only sync if Python is still valid
        if (python_valid_ && Py_IsInitialized()) {
            try {
                py::gil_scoped_acquire acquire;
                sync();
            } catch (py::error_already_set& e) {
                // prevent std::terminate and log it as unraisable
                // https://pybind11.readthedocs.io/en/stable/advanced/classes.html?utm_source=chatgpt.com#destructors-that-call-python
                e.discard_as_unraisable("PythonOutputStreambuf::~PythonOutputStreambuf");
            }
        }
    }

    // Mark Python as invalid (to be called before Python shutdown)
    void invalidate_python() {
        python_valid_ = false;
    }

protected:

    // Flush the buffer to Python dest.write(bytes)
    int sync() override
    {
        if (!python_valid_ || !Py_IsInitialized()) {
            return -1;
        }

        std::ptrdiff_t n = pptr() - pbase();
        int result = 0;
        if (n > 0) {
            try {
                py::gil_scoped_acquire acquire;

                if (is_text_) {
                    // For text mode, we need to ensure valid UTF-8.
                    // Convert to string_view first to avoid potential issues.
                    std::string_view sv(buffer_.data(), size_t(n));
                    dest_.attr("write")(py::str(sv.data(), sv.size()));
                } else {
                    // For binary mode, use bytes
                    dest_.attr("write")(py::bytes(buffer_.data(), size_t(n)));
                }

                // Flush Python internal buffers
                if (py::hasattr(dest_, "flush")) {
                    dest_.attr("flush")();
                }
            } catch (const py::error_already_set &e) {
                // Log the error if possible, then clear and signal failure
                PyErr_Clear();
                result = -1;
            } catch (...) {
                // Handle any other exceptions
                result = -1;
            }
        }

        // reset pointers to the buffer
        setp(buffer_.data(), buffer_.data() + buffer_.size());
        return result;
    }

    // Called when the buffer is full or on explicit overflow
    int_type overflow(int_type ch) override
    {
        // Edge cases
        if (!python_valid_ || !Py_IsInitialized()) {
            return traits_type::eof();
        }
        if (traits_type::eq_int_type(ch, traits_type::eof())) {
            return traits_type::not_eof(ch);
        }

        // Flush the full buffer
        if (sync() != 0) {
            return traits_type::eof();
        }

        // Now there is room for one char
        *pptr() = traits_type::to_char_type(ch);
        pbump(1);
        return ch;
    }

private:
    py::object        dest_;
    std::vector<char> buffer_;
    bool              is_text_;
    bool              python_valid_;  // Track if Python objects are still valid
};

// ================================================================================================
//   Add On
// ================================================================================================

void genesis_util_io_output_target_add_ons(py::module &m)
{
    // ---------------------------------------------------------------------
    //   Buffer Holder
    // ---------------------------------------------------------------------

    // Helper to own the buffer+stream for the lifetime of the Python object
    struct BufferHolder {
        PythonOutputStreambuf buf;
        std::ostream stream;

        // Keep a reference to the original BaseOutputTarget to ensure proper destruction order
        std::shared_ptr<genesis::util::io::BaseOutputTarget> target_ref;

        BufferHolder(py::object dest)
            : buf(dest)
            , stream(&buf)
        {}

        ~BufferHolder()
        {
            // Explicitly reset the target first to ensure GzipOStream is destroyed
            // before our stream/buffer
            target_ref.reset();

            // Now invalidate Python access
            buf.invalidate_python();

            // The stream and buf will be destroyed in reverse order of construction
        }

        void set_target(std::shared_ptr<genesis::util::io::BaseOutputTarget> target)
        {
            target_ref = target;
        }
    };

    // Helper lambda that uses the above PythonOutputStreambuf, held in a BufferHolder,
    // and wraps this such that we get a BaseOutputTarget for file-like stream,
    // such as io.StringIO and io.BytesIO
    auto make_python_buffer_target_ = [](
        py::object dest,
        genesis::util::io::GzipCompressionLevel compression_level
    ) -> std::shared_ptr<genesis::util::io::BaseOutputTarget> {

        // Check if the write attribute is present and callable
        auto write_attr = dest.attr("write");
        if( ! py::hasattr( dest, "write" ) || ! PyCallable_Check( write_attr.ptr() )) {
            throw std::invalid_argument(
                "Object does not have callable write() attribute "
                "and can hence not be used as an output target"
            );
        }

        // Allocate holder on heap so it outlives this lambda.
        auto holder_sp = std::make_shared<BufferHolder>(dest);

        // Build the actual C++ target using the holder stream.
        auto original_target = genesis::util::io::to_stream(
            holder_sp->stream,
            compression_level
        );

        // Give the holder a reference to the target so it controls destruction order.
        // This is important, as otherwise the destruction order leads to a segfault
        // when closing gzip streams from file-like objects...
        holder_sp->set_target(original_target);

        // Create a custom shared_ptr that ensures the holder outlives the target
        std::shared_ptr<genesis::util::io::BaseOutputTarget> ret(
            original_target.get(),
            [holder_sp, original_target](genesis::util::io::BaseOutputTarget*) mutable
            {
                // Custom deleter: first reset the original target (this destroys GzipOStream)
                original_target.reset();
                // Then let the holder be destroyed (this destroys our stream/buffer)
                holder_sp.reset();
                // Note: we don't delete the pointer since original_target owns it
            }
        );

        return ret;

        // Alternative to the above: Do not use a stream, and just re-open the file
        // with the to_file function instead. Not as nice, but kept here for reference.

        // If we are compressing, re-open by filename instead of using a stream.
        // Otherwise, we are running into segfauls and nastyness trying trying to sync
        // the buffers here with the python, while also managing their lifetime correctly...
        // if( compression_level != genesis::util::io::GzipCompressionLevel::kNoCompression ) {
        //     if( !py::hasattr(dest, "name") || !py::isinstance<py::str>(dest.attr("name")) ) {
        //         throw std::invalid_argument(
        //             "to_file(): cannot write compressed stream to file-like object "
        //             "that does not have a name attribute"
        //         );
        //     }
        //     std::string path = dest.attr("name").cast<std::string>();
        //     return genesis::util::io::to_file(
        //         path,
        //         compression_level,
        //         false
        //     );
        // }
    };

    // ---------------------------------------------------------------------
    //   to_file
    // ---------------------------------------------------------------------

    // The to_file wrapper is a bit complex to get all the lifetime management,
    // automatic conversions, function defaults, etc to work...
    //
    // We want to allow the following:
    //
    //     with open(self.path, 'w', encoding='utf-8') as f:
    //         target = util.io.to_file(f)
    //
    // For that to work, we need to use the `write` attribute, and wrap that, and use a holder,
    // tied in lifetime with the returned object itself, so that the buffer and stream
    // used in the wrapper stay alive while the file is being written to.
    //
    // Furthermore, we need to take care of the GzipCompressionLevel argument:
    // We cannot just use a default here, as Binder places custom add ons at the top
    // of the namespace, see https://github.com/RosettaCommons/binder/issues/86
    // So instead of a default arg, we have to define the function twice, once with
    // and once without the compression_level argument. To avoid too much code duplication,
    // we use another function to wrap the main binding first, and reuse it for binding.

    // Define the binding lambda function.
    auto Path = py::module::import("pathlib").attr("Path");
    auto to_file_binder_ = [Path, make_python_buffer_target_](
        py::object dest,
        genesis::util::io::GzipCompressionLevel compression_level,
        bool auto_adjust_filename = true
    ) -> std::shared_ptr<genesis::util::io::BaseOutputTarget> {
        // 1) str --> filesystem
        if (py::isinstance<py::str>(dest)) {
            return genesis::util::io::to_file(
                dest.cast<std::string>(),
                compression_level,
                auto_adjust_filename
            );
        }
        // 2) pathlib.Path --> filesystem
        if (py::isinstance(dest, Path)) {
            auto path = dest.attr("as_posix")().cast<std::string>();
            return genesis::util::io::to_file(
                path,
                compression_level,
                auto_adjust_filename
            );
        }
        // 3) file-like --> wrap in std::ostream
        if (py::hasattr(dest, "write")) {
            return make_python_buffer_target_( dest, compression_level );
        }
        throw std::invalid_argument(
            "to_file(): expected filename (str or pathlib.Path) or "
            "file-like object with write() such as io.StringIO or io.BytesIO"
        );
    };

    // Doc string. Needs to be updated at some point with the correct doxygen content.
    static std::string const to_file_doc =
        "Obtain an output target for writing to a file.\n\n The output target returned from "
        "this function can be used in the writer classes, e.g.,\n placement::JplaceWriter or "
        "sequence::FastaWriter.\n\n If  is set to a compression level other than\n "
        "::GzipCompressionLevel::kNoCompression, the output is compressed using\n gzip. We "
        "recommend to use ::GzipCompressionLevel::kDefaultCompression%.\n\n Furthermore, if  "
        "is set to `true` (default), the file name is\n adjusted according to the compression "
        "setting: If compression is used, the file name\n is appended by the `.gz` extension, "
        "if this is not alreay present. (For completeness,\n the oppositve also works: If the "
        "file name ends in `.gz`, but no compression is chosen, the `.gz`\n extension is "
        "removed.)\n\n If the file cannot be written to, the function throws an exception.\n "
        "Also, by default, if the file already exists, an exception is thrown. See\n "
        "\n\n\n\n\n\n\n\nC++: genesis::util::io::to_file(const std::string &, enum "
        "genesis::util::io::GzipCompressionLevel, bool) --> class std::shared_ptr<class "
        "genesis::util::io::BaseOutputTarget>"
    ;

    // Generate a binding that takes a compression_level
    m.def("to_file",
        to_file_binder_,
        // [to_file_binder_](
        //     py::object dest,
        //     genesis::util::io::GzipCompressionLevel compression_level,
        //     bool auto_adjust_filename = true
        // ) -> std::shared_ptr<genesis::util::io::BaseOutputTarget> {
        //     return to_file_binder_( dest, compression_level, auto_adjust_filename );
        // },
        py::arg("dest"),
        py::arg("compression_level"),
        py::arg("auto_adjust_filename") = true,
        to_file_doc.c_str()
    );

    // Generate a binding that defaults the compression_level
    m.def("to_file",
        [to_file_binder_](
            py::object dest
        ) -> std::shared_ptr<genesis::util::io::BaseOutputTarget> {
            return to_file_binder_(
                dest,
                genesis::util::io::GzipCompressionLevel::kNoCompression
            );
        },
        py::arg("dest"),
        to_file_doc.c_str()
    );

    // ---------------------------------------------------------------------
    //   to_string
    // ---------------------------------------------------------------------

    // Bind the default string target that can be retrieved later
    m.def("to_string",
        []() -> std::shared_ptr<genesis::util::io::BaseOutputTarget>
        {
            return genesis::util::io::to_string();
        },
        "Create an in-memory string output target; later call .get() or str() "
        "on this target to retrieve the string"
    );

    // Bind a string target for io.StringIO, io.BytesIO, etc
    m.def("to_string",
        [make_python_buffer_target_](
            py::object dest
        ) -> std::shared_ptr<genesis::util::io::BaseOutputTarget> {
            // file-like --> wrap in std::ostream
            if (py::hasattr(dest, "write")) {
                return make_python_buffer_target_(
                    dest,
                    genesis::util::io::GzipCompressionLevel::kNoCompression
                );
            }
            throw std::invalid_argument(
                "to_string(): expected file-like object with write() "
                "such as io.StringIO or io.BytesIO as argument"
            );
        },
        py::arg("dest"),
        "Create an in-memory string output target taking a file-like object with a write() "
        "attribute, such as io.StringIO or io.BytesIO to write to."
    );
}
