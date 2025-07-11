#include <../python/custom_bindings/template_instances/preorder.hpp>
#include <../python/custom_bindings/template_instances/range.hpp>

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

void bind____python_custom_bindings_template_instances_range(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// template_range_iteratorpreorder_taxonomy_taxon_t() file:../python/custom_bindings/template_instances/range.hpp line:12
	M("").def("template_range_iteratorpreorder_taxonomy_taxon_t", (void (*)()) &template_range_iteratorpreorder_taxonomy_taxon_t, "C++: template_range_iteratorpreorder_taxonomy_taxon_t() --> void");

	// template_preorder_taxonomy_t() file:../python/custom_bindings/template_instances/preorder.hpp line:4
	M("").def("template_preorder_taxonomy_t", (void (*)()) &template_preorder_taxonomy_t, "C++: template_preorder_taxonomy_t() --> void");

}
