#include <functional>
#include <genesis/utils/core/algorithm.hpp>
#include <genesis/utils/core/fs.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>, false)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*, false)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_genesis_utils_core_algorithm(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::utils::core::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, struct std::less<double>) file:genesis/utils/core/algorithm.hpp line:311
	// function-signature: class std::vector<unsigned long> genesis::utils::core::stable_sort_indices<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >,std::less<double>>(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, struct std::less<double>)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, struct std::less<double>) file:genesis/utils/core/algorithm.hpp line:311
	M("genesis::utils::core").def("stable_sort_indices", (class std::vector<unsigned long> (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, struct std::less<double>)) &genesis::utils::core::stable_sort_indices<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >,std::less<double>>, "C++: genesis::utils::core::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, struct std::less<double>) --> class std::vector<unsigned long>", pybind11::arg("first"), pybind11::arg("last"), pybind11::arg("comparator"));

	// genesis::utils::core::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/core/algorithm.hpp line:353
	// function-signature: class std::vector<unsigned long> genesis::utils::core::stable_sort_indices<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) file:genesis/utils/core/algorithm.hpp line:353
	M("genesis::utils::core").def("stable_sort_indices", (class std::vector<unsigned long> (*)(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >)) &genesis::utils::core::stable_sort_indices<__gnu_cxx::__normal_iterator<const double *, std::vector<double> >>, "C++: genesis::utils::core::stable_sort_indices(class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >, class __gnu_cxx::__normal_iterator<const double *, class std::vector<double> >) --> class std::vector<unsigned long>", pybind11::arg("first"), pybind11::arg("last"));

	// genesis::utils::core::path_exists(const std::string &) file:genesis/utils/core/fs.hpp line:49
	// function-signature: bool genesis::utils::core::path_exists(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:49
	M("genesis::utils::core").def("path_exists", (bool (*)(const std::string &)) &genesis::utils::core::path_exists, "Return whether a path exists, i.e., is a file or directory.\n\nC++: genesis::utils::core::path_exists(const std::string &) --> bool", pybind11::arg("path"));

	// genesis::utils::core::is_file(const std::string &) file:genesis/utils/core/fs.hpp line:58
	// function-signature: bool genesis::utils::core::is_file(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:58
	M("genesis::utils::core").def("is_file", (bool (*)(const std::string &)) &genesis::utils::core::is_file, "Return true iff the provided path is a (readable) file.\n\n Internally, this function simply returns the value of file_is_readable(), meaning that we also\n check that the file can actually be read. That is semantically a bit different from just stating\n that it is a file... But file system stuff in C++ pre-17 is hard, and this works for now.\n\nC++: genesis::utils::core::is_file(const std::string &) --> bool", pybind11::arg("path"));

	// genesis::utils::core::file_exists(const std::string &) file:genesis/utils/core/fs.hpp line:65
	// function-signature: bool genesis::utils::core::file_exists(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:65
	M("genesis::utils::core").def("file_exists", (bool (*)(const std::string &)) &genesis::utils::core::file_exists, "Return true iff the file exists (and is in fact a file, and not, e.g., a directory).\n\n  std::string const& )\n\nC++: genesis::utils::core::file_exists(const std::string &) --> bool", pybind11::arg("filename"));

	// genesis::utils::core::file_is_readable(const std::string &) file:genesis/utils/core/fs.hpp line:76
	// function-signature: bool genesis::utils::core::file_is_readable(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:76
	M("genesis::utils::core").def("file_is_readable", (bool (*)(const std::string &)) &genesis::utils::core::file_is_readable, "Return whether a file is readable.\n\n For this, the file has to exist, and be accessible.\n Another potential error is that too many files are opened already.\n\n See file_is_readable( std::string const&, std::string& ) for a version of the function that also\n allows to retrieve the error message in cases where the result is `false`.\n\nC++: genesis::utils::core::file_is_readable(const std::string &) --> bool", pybind11::arg("filename"));

	// genesis::utils::core::file_is_readable(const std::string &, std::string &) file:genesis/utils/core/fs.hpp line:84
	// function-signature: bool genesis::utils::core::file_is_readable(const std::string &, std::string &)(const std::string &, std::string &) file:genesis/utils/core/fs.hpp line:84
	M("genesis::utils::core").def("file_is_readable", (bool (*)(const std::string &, std::string &)) &genesis::utils::core::file_is_readable, "Return whether a file is readable, and potentially store the error message.\n\n For this, the file has to exist, and be accessible.\n Another potential error is that too many files are opened already.\n\nC++: genesis::utils::core::file_is_readable(const std::string &, std::string &) --> bool", pybind11::arg("filename"), pybind11::arg("err_str"));

	// genesis::utils::core::file_read(const std::string &, bool) file:genesis/utils/core/fs.hpp line:94
	// function-signature: std::string genesis::utils::core::file_read(const std::string &, bool)(const std::string &, bool) file:genesis/utils/core/fs.hpp line:94
	M("genesis::utils::core").def("file_read", [](const std::string & a0) -> std::string { return genesis::utils::core::file_read(a0); }, "", pybind11::arg("filename"));
	M("genesis::utils::core").def("file_read", (std::string (*)(const std::string &, bool)) &genesis::utils::core::file_read, "Return the contents of a file as a string.\n\n If the parameter  is `true` (default), it is first determined whether the\n file is gzip compressed, and if so, the file is decompressed when reading.\n\n If the file is not readable, the function throws `std::runtime_error`.\n\nC++: genesis::utils::core::file_read(const std::string &, bool) --> std::string", pybind11::arg("filename"), pybind11::arg("detect_compression"));

	// genesis::utils::core::file_read_lines(const std::string &, bool) file:genesis/utils/core/fs.hpp line:107
	// function-signature: class std::vector<std::string > genesis::utils::core::file_read_lines(const std::string &, bool)(const std::string &, bool) file:genesis/utils/core/fs.hpp line:107
	M("genesis::utils::core").def("file_read_lines", [](const std::string & a0) -> std::vector<std::string > { return genesis::utils::core::file_read_lines(a0); }, "", pybind11::arg("filename"));
	M("genesis::utils::core").def("file_read_lines", (class std::vector<std::string > (*)(const std::string &, bool)) &genesis::utils::core::file_read_lines, "Return the contents of a file as a vector of strings, one entry for each line.\n\n If the parameter  is `true` (default), it is first determined whether the\n file is gzip compressed, and if so, the file is decompressed when reading.\n\n If the file is not readable, the function throws `std::runtime_error`.\n\nC++: genesis::utils::core::file_read_lines(const std::string &, bool) --> class std::vector<std::string >", pybind11::arg("filename"), pybind11::arg("detect_compression"));

	// genesis::utils::core::file_write(const std::string &, const std::string &, bool) file:genesis/utils/core/fs.hpp line:120
	// function-signature: void genesis::utils::core::file_write(const std::string &, const std::string &, bool)(const std::string &, const std::string &, bool) file:genesis/utils/core/fs.hpp line:120
	M("genesis::utils::core").def("file_write", [](const std::string & a0, const std::string & a1) -> void { return genesis::utils::core::file_write(a0, a1); }, "", pybind11::arg("content"), pybind11::arg("filename"));
	M("genesis::utils::core").def("file_write", (void (*)(const std::string &, const std::string &, bool)) &genesis::utils::core::file_write, "Write the content of a string to a file.\n\n If the file cannot be written to, the function throws an exception. Also, by default, if the file\n already exists, an exception is thrown.\n See \n\n\n\n\nC++: genesis::utils::core::file_write(const std::string &, const std::string &, bool) --> void", pybind11::arg("content"), pybind11::arg("filename"), pybind11::arg("create_dirs"));

	// genesis::utils::core::file_append(const std::string &, const std::string &, bool) file:genesis/utils/core/fs.hpp line:131
	// function-signature: void genesis::utils::core::file_append(const std::string &, const std::string &, bool)(const std::string &, const std::string &, bool) file:genesis/utils/core/fs.hpp line:131
	M("genesis::utils::core").def("file_append", [](const std::string & a0, const std::string & a1) -> void { return genesis::utils::core::file_append(a0, a1); }, "", pybind11::arg("content"), pybind11::arg("filename"));
	M("genesis::utils::core").def("file_append", (void (*)(const std::string &, const std::string &, bool)) &genesis::utils::core::file_append, "Append the content of a string to a file.\n\n If the file is not writable, the function throws `std::runtime_error`.\n\nC++: genesis::utils::core::file_append(const std::string &, const std::string &, bool) --> void", pybind11::arg("content"), pybind11::arg("filename"), pybind11::arg("create_dirs"));

	// genesis::utils::core::is_dir(const std::string &) file:genesis/utils/core/fs.hpp line:147
	// function-signature: bool genesis::utils::core::is_dir(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:147
	M("genesis::utils::core").def("is_dir", (bool (*)(const std::string &)) &genesis::utils::core::is_dir, "Return true iff the provided path is a directory.\n\n Internally, this function simply return the value of dir_exists(), as this already does the\n needed check. Thus, it is an alias.\n\nC++: genesis::utils::core::is_dir(const std::string &) --> bool", pybind11::arg("path"));

	// genesis::utils::core::dir_exists(const std::string &) file:genesis/utils/core/fs.hpp line:152
	// function-signature: bool genesis::utils::core::dir_exists(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:152
	M("genesis::utils::core").def("dir_exists", (bool (*)(const std::string &)) &genesis::utils::core::dir_exists, "Return true iff the directory exists.\n\nC++: genesis::utils::core::dir_exists(const std::string &) --> bool", pybind11::arg("dir"));

	// genesis::utils::core::dir_create(const std::string &, bool) file:genesis/utils/core/fs.hpp line:161
	// function-signature: void genesis::utils::core::dir_create(const std::string &, bool)(const std::string &, bool) file:genesis/utils/core/fs.hpp line:161
	M("genesis::utils::core").def("dir_create", [](const std::string & a0) -> void { return genesis::utils::core::dir_create(a0); }, "", pybind11::arg("path"));
	M("genesis::utils::core").def("dir_create", (void (*)(const std::string &, bool)) &genesis::utils::core::dir_create, "Create a directory.\n\n If the directory already exists, nothing happens.\n If the path exists, but is not a directory, a `std::runtime_error` is thrown.\n If the creation fails for some other reason, also a `std::runtime_error` is thrown.\n\nC++: genesis::utils::core::dir_create(const std::string &, bool) --> void", pybind11::arg("path"), pybind11::arg("with_parents"));

	// genesis::utils::core::dir_normalize_path(const std::string &) file:genesis/utils/core/fs.hpp line:166
	// function-signature: std::string genesis::utils::core::dir_normalize_path(const std::string &)(const std::string &) file:genesis/utils/core/fs.hpp line:166
	M("genesis::utils::core").def("dir_normalize_path", (std::string (*)(const std::string &)) &genesis::utils::core::dir_normalize_path, "Normalize a dir name, i.e., make sure that the given path ends with exaclty one slash.\n\nC++: genesis::utils::core::dir_normalize_path(const std::string &) --> std::string", pybind11::arg("path"));

	// genesis::utils::core::dir_list_contents(const std::string &, bool, const std::string &) file:genesis/utils/core/fs.hpp line:176
	// function-signature: class std::vector<std::string > genesis::utils::core::dir_list_contents(const std::string &, bool, const std::string &)(const std::string &, bool, const std::string &) file:genesis/utils/core/fs.hpp line:176
	M("genesis::utils::core").def("dir_list_contents", [](const std::string & a0) -> std::vector<std::string > { return genesis::utils::core::dir_list_contents(a0); }, "", pybind11::arg("dir"));
	M("genesis::utils::core").def("dir_list_contents", [](const std::string & a0, bool const & a1) -> std::vector<std::string > { return genesis::utils::core::dir_list_contents(a0, a1); }, "", pybind11::arg("dir"), pybind11::arg("full_path"));
	M("genesis::utils::core").def("dir_list_contents", (class std::vector<std::string > (*)(const std::string &, bool, const std::string &)) &genesis::utils::core::dir_list_contents, "Get a list of files and directories in a directory.\n\n If  is set to `true`, the path of the provided  is prepended to the resulting\n list. If a non-empy  is provided, file names are filtered by this regular expression.\n\n If the directory is not readable, the function throws `std::runtime_error`.\n\nC++: genesis::utils::core::dir_list_contents(const std::string &, bool, const std::string &) --> class std::vector<std::string >", pybind11::arg("dir"), pybind11::arg("full_path"), pybind11::arg("regex"));

	// genesis::utils::core::dir_list_files(const std::string &, bool, const std::string &) file:genesis/utils/core/fs.hpp line:188
	// function-signature: class std::vector<std::string > genesis::utils::core::dir_list_files(const std::string &, bool, const std::string &)(const std::string &, bool, const std::string &) file:genesis/utils/core/fs.hpp line:188
	M("genesis::utils::core").def("dir_list_files", [](const std::string & a0) -> std::vector<std::string > { return genesis::utils::core::dir_list_files(a0); }, "", pybind11::arg("dir"));
	M("genesis::utils::core").def("dir_list_files", [](const std::string & a0, bool const & a1) -> std::vector<std::string > { return genesis::utils::core::dir_list_files(a0, a1); }, "", pybind11::arg("dir"), pybind11::arg("full_path"));
	M("genesis::utils::core").def("dir_list_files", (class std::vector<std::string > (*)(const std::string &, bool, const std::string &)) &genesis::utils::core::dir_list_files, "Get a list of files in a directory.\n\n Behaves the same as dir_list_contents(), except tat only files are listed (i.e., no directories).\n See there for an explanation of the arguments.\n\nC++: genesis::utils::core::dir_list_files(const std::string &, bool, const std::string &) --> class std::vector<std::string >", pybind11::arg("dir"), pybind11::arg("full_path"), pybind11::arg("regex"));

	// genesis::utils::core::dir_list_directories(const std::string &, bool, const std::string &) file:genesis/utils/core/fs.hpp line:200
	// function-signature: class std::vector<std::string > genesis::utils::core::dir_list_directories(const std::string &, bool, const std::string &)(const std::string &, bool, const std::string &) file:genesis/utils/core/fs.hpp line:200
	M("genesis::utils::core").def("dir_list_directories", [](const std::string & a0) -> std::vector<std::string > { return genesis::utils::core::dir_list_directories(a0); }, "", pybind11::arg("dir"));
	M("genesis::utils::core").def("dir_list_directories", [](const std::string & a0, bool const & a1) -> std::vector<std::string > { return genesis::utils::core::dir_list_directories(a0, a1); }, "", pybind11::arg("dir"), pybind11::arg("full_path"));
	M("genesis::utils::core").def("dir_list_directories", (class std::vector<std::string > (*)(const std::string &, bool, const std::string &)) &genesis::utils::core::dir_list_directories, "Get a list of directories in a directory.\n\n Behaves the same as dir_list_contents(), except tat only directories are listed (i.e., no files).\n See there for an explanation of the arguments.\n\nC++: genesis::utils::core::dir_list_directories(const std::string &, bool, const std::string &) --> class std::vector<std::string >", pybind11::arg("dir"), pybind11::arg("full_path"), pybind11::arg("regex"));

}
