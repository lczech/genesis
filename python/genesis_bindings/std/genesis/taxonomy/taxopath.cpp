#include <genesis/taxonomy/taxopath.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <vector>

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

void bind_std_genesis_taxonomy_taxopath(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // std::hash file:genesis/taxonomy/taxopath.hpp line:396
		pybind11::class_<std::hash<genesis::taxonomy::Taxopath>, std::shared_ptr<std::hash<genesis::taxonomy::Taxopath>>> cl(M("std"), "hash_genesis_taxonomy_Taxopath_t", "Hash function for Taxopath.\n\n We just forward to the hash of the internal data of a Taxopath, that is, we hash its strings.");
		cl.def( pybind11::init( [](){ return new std::hash<genesis::taxonomy::Taxopath>(); } ) );
		cl.def("__call__", (unsigned long (std::hash<genesis::taxonomy::Taxopath>::*)(const class genesis::taxonomy::Taxopath &) const) &std::hash<genesis::taxonomy::Taxopath>::operator(), "C++: std::hash<genesis::taxonomy::Taxopath>::operator()(const class genesis::taxonomy::Taxopath &) const --> unsigned long", pybind11::arg("t"));
	}
}
