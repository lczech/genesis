#include <cstdio>
#include <sstream> // __str__

#include <functional>
#include <pybind11/pybind11.h>
#include <string>
#include <pybind11/functional.h>
#include <../python/custom_bindings/extensions/utils/matrix.hpp>
#include <genesis/utils/color/functions.hpp>
#include <../python/custom_bindings/extensions/utils/bitvector.hpp>
#include <../python/custom_bindings/extensions/utils/range.hpp>
#include <../python/custom_bindings/extensions/string_target.hpp>
#include <../python/custom_bindings/extensions/sequence/quality.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_set.hpp>
#include <../python/custom_bindings/extensions/sequence/sequence_dict.hpp>
#include <../python/custom_bindings/extensions/sequence/fasta_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/fastq_input_iterator.hpp>
#include <../python/custom_bindings/extensions/sequence/reference_genome.hpp>
#include <../python/custom_bindings/extensions/taxonomy/taxopath.hpp>
#include <../python/custom_bindings/extensions/taxonomy/iterator.hpp>
#include <../python/custom_bindings/extensions/tree/tree.hpp>
#include <../python/custom_bindings/extensions/tree/functions_tree.hpp>
#include <../python/custom_bindings/extensions/placement/helper.hpp>
#include <pybind11/stl.h>


#ifndef BINDER_PYBIND11_TYPE_CASTER
	#define BINDER_PYBIND11_TYPE_CASTER
	PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)
	PYBIND11_DECLARE_HOLDER_TYPE(T, T*)
	PYBIND11_MAKE_OPAQUE(std::shared_ptr<void>)
#endif

void bind_bits_types_struct_FILE(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // _IO_FILE file:bits/types/struct_FILE.h line:49
		pybind11::class_<_IO_FILE, std::shared_ptr<_IO_FILE>> cl(M(""), "_IO_FILE", "");
		cl.def( pybind11::init( [](){ return new _IO_FILE(); } ) );
		cl.def_readwrite("_flags", &_IO_FILE::_flags);
		cl.def_readwrite("_fileno", &_IO_FILE::_fileno);
		cl.def_readwrite("_flags2", &_IO_FILE::_flags2);
		cl.def_readwrite("_old_offset", &_IO_FILE::_old_offset);
		cl.def_readwrite("_cur_column", &_IO_FILE::_cur_column);
		cl.def_readwrite("_vtable_offset", &_IO_FILE::_vtable_offset);
		cl.def_readwrite("_offset", &_IO_FILE::_offset);
		cl.def_readwrite("__pad5", &_IO_FILE::__pad5);
		cl.def_readwrite("_mode", &_IO_FILE::_mode);
	}
}
