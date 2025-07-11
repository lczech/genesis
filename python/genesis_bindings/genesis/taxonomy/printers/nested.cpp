#include <functional>
#include <genesis/taxonomy/iterator/levelorder.hpp>
#include <genesis/taxonomy/iterator/postorder.hpp>
#include <genesis/taxonomy/iterator/preorder.hpp>
#include <genesis/taxonomy/printers/nested.hpp>
#include <genesis/taxonomy/taxon.hpp>
#include <genesis/taxonomy/taxon_data.hpp>
#include <genesis/taxonomy/taxonomy.hpp>
#include <ios>
#include <iterator>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>

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

void bind_genesis_taxonomy_printers_nested(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::taxonomy::PrinterNested file:genesis/taxonomy/printers/nested.hpp line:53
		pybind11::class_<genesis::taxonomy::PrinterNested, std::shared_ptr<genesis::taxonomy::PrinterNested>> cl(M("genesis::taxonomy"), "PrinterNested", "Simple printer class for Taxonomy.");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::PrinterNested(); } ) );
		cl.def( pybind11::init( [](genesis::taxonomy::PrinterNested const &o){ return new genesis::taxonomy::PrinterNested(o); } ) );
		cl.def("assign", (class genesis::taxonomy::PrinterNested & (genesis::taxonomy::PrinterNested::*)(const class genesis::taxonomy::PrinterNested &)) &genesis::taxonomy::PrinterNested::operator=, "C++: genesis::taxonomy::PrinterNested::operator=(const class genesis::taxonomy::PrinterNested &) --> class genesis::taxonomy::PrinterNested &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("print", (void (genesis::taxonomy::PrinterNested::*)(std::ostream &, const class genesis::taxonomy::Taxonomy &) const) &genesis::taxonomy::PrinterNested::print, "C++: genesis::taxonomy::PrinterNested::print(std::ostream &, const class genesis::taxonomy::Taxonomy &) const --> void", pybind11::arg("out"), pybind11::arg("tax"));
		cl.def("print", (std::string (genesis::taxonomy::PrinterNested::*)(const class genesis::taxonomy::Taxonomy &) const) &genesis::taxonomy::PrinterNested::print, "C++: genesis::taxonomy::PrinterNested::print(const class genesis::taxonomy::Taxonomy &) const --> std::string", pybind11::arg("tax"));
		cl.def("__call__", (std::string (genesis::taxonomy::PrinterNested::*)(const class genesis::taxonomy::Taxonomy &) const) &genesis::taxonomy::PrinterNested::operator(), "C++: genesis::taxonomy::PrinterNested::operator()(const class genesis::taxonomy::Taxonomy &) const --> std::string", pybind11::arg("tax"));
		cl.def("line_limit", (class genesis::taxonomy::PrinterNested & (genesis::taxonomy::PrinterNested::*)(int)) &genesis::taxonomy::PrinterNested::line_limit, "C++: genesis::taxonomy::PrinterNested::line_limit(int) --> class genesis::taxonomy::PrinterNested &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("line_limit", (int (genesis::taxonomy::PrinterNested::*)() const) &genesis::taxonomy::PrinterNested::line_limit, "C++: genesis::taxonomy::PrinterNested::line_limit() const --> int");
		cl.def("depth_limit", (class genesis::taxonomy::PrinterNested & (genesis::taxonomy::PrinterNested::*)(int)) &genesis::taxonomy::PrinterNested::depth_limit, "C++: genesis::taxonomy::PrinterNested::depth_limit(int) --> class genesis::taxonomy::PrinterNested &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("depth_limit", (int (genesis::taxonomy::PrinterNested::*)() const) &genesis::taxonomy::PrinterNested::depth_limit, "C++: genesis::taxonomy::PrinterNested::depth_limit() const --> int");
		cl.def("print_ranks", (class genesis::taxonomy::PrinterNested & (genesis::taxonomy::PrinterNested::*)(bool)) &genesis::taxonomy::PrinterNested::print_ranks, "C++: genesis::taxonomy::PrinterNested::print_ranks(bool) --> class genesis::taxonomy::PrinterNested &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("print_ranks", (bool (genesis::taxonomy::PrinterNested::*)() const) &genesis::taxonomy::PrinterNested::print_ranks, "C++: genesis::taxonomy::PrinterNested::print_ranks() const --> bool");
		cl.def("print_ids", (class genesis::taxonomy::PrinterNested & (genesis::taxonomy::PrinterNested::*)(bool)) &genesis::taxonomy::PrinterNested::print_ids, "C++: genesis::taxonomy::PrinterNested::print_ids(bool) --> class genesis::taxonomy::PrinterNested &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("print_ids", (bool (genesis::taxonomy::PrinterNested::*)() const) &genesis::taxonomy::PrinterNested::print_ids, "C++: genesis::taxonomy::PrinterNested::print_ids() const --> bool");
	}
	// genesis::taxonomy::levelorder_for_each(class genesis::taxonomy::Taxonomy &, class std::function<void (class genesis::taxonomy::Taxon &)>, bool) file:genesis/taxonomy/iterator/levelorder.hpp line:58
	M("genesis::taxonomy").def("levelorder_for_each", [](class genesis::taxonomy::Taxonomy & a0, class std::function<void (class genesis::taxonomy::Taxon &)> const & a1) -> void { return genesis::taxonomy::levelorder_for_each(a0, a1); }, "", pybind11::arg("tax"), pybind11::arg("fn"));
	M("genesis::taxonomy").def("levelorder_for_each", (void (*)(class genesis::taxonomy::Taxonomy &, class std::function<void (class genesis::taxonomy::Taxon &)>, bool)) &genesis::taxonomy::levelorder_for_each, "Apply a function to all taxa of the Taxonomy, traversing it in levelorder.\n\n The given Taxonomy is traversed in levelorder (i.e., breadth-first search). If\n `include_inner_taxa` is set to true (default), the provided functional is called for all\n \n\n\n\n\n\n\nC++: genesis::taxonomy::levelorder_for_each(class genesis::taxonomy::Taxonomy &, class std::function<void (class genesis::taxonomy::Taxon &)>, bool) --> void", pybind11::arg("tax"), pybind11::arg("fn"), pybind11::arg("include_inner_taxa"));

	// genesis::taxonomy::levelorder_for_each(const class genesis::taxonomy::Taxonomy &, class std::function<void (const class genesis::taxonomy::Taxon &)>, bool) file:genesis/taxonomy/iterator/levelorder.hpp line:92
	M("genesis::taxonomy").def("levelorder_for_each", [](const class genesis::taxonomy::Taxonomy & a0, class std::function<void (const class genesis::taxonomy::Taxon &)> const & a1) -> void { return genesis::taxonomy::levelorder_for_each(a0, a1); }, "", pybind11::arg("tax"), pybind11::arg("fn"));
	M("genesis::taxonomy").def("levelorder_for_each", (void (*)(const class genesis::taxonomy::Taxonomy &, class std::function<void (const class genesis::taxonomy::Taxon &)>, bool)) &genesis::taxonomy::levelorder_for_each, "Apply a function to all taxa of the Taxonomy, traversing it in levelorder.\n\n The given Taxonomy is traversed in levelorder (i.e., breadth-first search). If\n `include_inner_taxa` is set to true (default), the provided functional is called for all\n \n\n\n\n\n\n\nC++: genesis::taxonomy::levelorder_for_each(const class genesis::taxonomy::Taxonomy &, class std::function<void (const class genesis::taxonomy::Taxon &)>, bool) --> void", pybind11::arg("tax"), pybind11::arg("fn"), pybind11::arg("include_inner_taxa"));

	// genesis::taxonomy::postorder_for_each(class genesis::taxonomy::Taxonomy &, class std::function<void (class genesis::taxonomy::Taxon &)>, bool) file:genesis/taxonomy/iterator/postorder.hpp line:57
	M("genesis::taxonomy").def("postorder_for_each", [](class genesis::taxonomy::Taxonomy & a0, class std::function<void (class genesis::taxonomy::Taxon &)> const & a1) -> void { return genesis::taxonomy::postorder_for_each(a0, a1); }, "", pybind11::arg("tax"), pybind11::arg("fn"));
	M("genesis::taxonomy").def("postorder_for_each", (void (*)(class genesis::taxonomy::Taxonomy &, class std::function<void (class genesis::taxonomy::Taxon &)>, bool)) &genesis::taxonomy::postorder_for_each, "Apply a function to all taxa of the Taxonomy, traversing it in postorder.\n\n The given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If\n `include_inner_taxa` is set to true (default), the provided functional is called for all\n \n\n\n\n\n\n\nC++: genesis::taxonomy::postorder_for_each(class genesis::taxonomy::Taxonomy &, class std::function<void (class genesis::taxonomy::Taxon &)>, bool) --> void", pybind11::arg("tax"), pybind11::arg("fn"), pybind11::arg("include_inner_taxa"));

	// genesis::taxonomy::postorder_for_each(const class genesis::taxonomy::Taxonomy &, class std::function<void (const class genesis::taxonomy::Taxon &)>, bool) file:genesis/taxonomy/iterator/postorder.hpp line:81
	M("genesis::taxonomy").def("postorder_for_each", [](const class genesis::taxonomy::Taxonomy & a0, class std::function<void (const class genesis::taxonomy::Taxon &)> const & a1) -> void { return genesis::taxonomy::postorder_for_each(a0, a1); }, "", pybind11::arg("tax"), pybind11::arg("fn"));
	M("genesis::taxonomy").def("postorder_for_each", (void (*)(const class genesis::taxonomy::Taxonomy &, class std::function<void (const class genesis::taxonomy::Taxon &)>, bool)) &genesis::taxonomy::postorder_for_each, "Apply a function to all taxa of the Taxonomy, traversing it in postorder.\n\n The given Taxonomy is traversed in postorder (i.e., a variant of depth-first search). If\n `include_inner_taxa` is set to true (default), the provided functional is called for all\n \n\n\n\n\n\n\nC++: genesis::taxonomy::postorder_for_each(const class genesis::taxonomy::Taxonomy &, class std::function<void (const class genesis::taxonomy::Taxon &)>, bool) --> void", pybind11::arg("tax"), pybind11::arg("fn"), pybind11::arg("include_inner_taxa"));

	{ // genesis::taxonomy::IteratorPreorder file:genesis/taxonomy/iterator/preorder.hpp line:103
		pybind11::class_<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>, std::shared_ptr<genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>>> cl(M("genesis::taxonomy"), "IteratorPreorder_const_genesis_taxonomy_Taxonomy_const_genesis_taxonomy_Taxon_t", "");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>(); } ) );
		cl.def( pybind11::init<const class genesis::taxonomy::Taxonomy &>(), pybind11::arg("taxonomy") );

		cl.def( pybind11::init( [](genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon> const &o){ return new genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>(o); } ) );
		cl.def("assign", (class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> & (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)(const class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &)) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator=, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator=(const class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &) --> class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("dereference", (class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)()) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator*, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator*() --> class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>");
		cl.def("pre_increment", (class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)()) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator++, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator++() --> class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>");
		cl.def("post_increment", (class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)(int)) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator++, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator++(int) --> class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon>", pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)(const class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &) const) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator==, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator==(const class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)(const class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &) const) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator!=, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::operator!=(const class genesis::taxonomy::IteratorPreorder<const class genesis::taxonomy::Taxonomy, const class genesis::taxonomy::Taxon> &) const --> bool", pybind11::arg("other"));
		cl.def("taxon", (const class genesis::taxonomy::Taxon & (genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy,const genesis::taxonomy::Taxon>::*)() const) &genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::taxon, "C++: genesis::taxonomy::IteratorPreorder<const genesis::taxonomy::Taxonomy, const genesis::taxonomy::Taxon>::taxon() const --> const class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal);
	}
	{ // genesis::taxonomy::IteratorPreorder file:genesis/taxonomy/iterator/preorder.hpp line:103
		pybind11::class_<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>, std::shared_ptr<genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>>> cl(M("genesis::taxonomy"), "IteratorPreorder_genesis_taxonomy_Taxonomy_genesis_taxonomy_Taxon_t", "");
		cl.def( pybind11::init( [](){ return new genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>(); } ) );
		cl.def( pybind11::init<class genesis::taxonomy::Taxonomy &>(), pybind11::arg("taxonomy") );

		cl.def( pybind11::init( [](genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon> const &o){ return new genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>(o); } ) );
		cl.def("assign", (class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> & (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)(const class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &)) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator=, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator=(const class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &) --> class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("dereference", (class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)()) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator*, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator*() --> class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>");
		cl.def("pre_increment", (class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)()) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator++, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator++() --> class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>");
		cl.def("post_increment", (class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)(int)) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator++, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator++(int) --> class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon>", pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)(const class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &) const) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator==, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator==(const class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)(const class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &) const) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator!=, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::operator!=(const class genesis::taxonomy::IteratorPreorder<class genesis::taxonomy::Taxonomy, class genesis::taxonomy::Taxon> &) const --> bool", pybind11::arg("other"));
		cl.def("taxon", (class genesis::taxonomy::Taxon & (genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy,genesis::taxonomy::Taxon>::*)() const) &genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::taxon, "C++: genesis::taxonomy::IteratorPreorder<genesis::taxonomy::Taxonomy, genesis::taxonomy::Taxon>::taxon() const --> class genesis::taxonomy::Taxon &", pybind11::return_value_policy::reference_internal);
	}
}
