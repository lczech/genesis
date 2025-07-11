#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/iterator/path.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/containers/range.hpp>
#include <memory>
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

void bind_genesis_tree_iterator_path(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::IteratorPath file:genesis/tree/iterator/path.hpp line:60
		pybind11::class_<genesis::tree::IteratorPath<false>, std::shared_ptr<genesis::tree::IteratorPath<false>>> cl(M("genesis::tree"), "IteratorPath_false_t", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::IteratorPath<false>(); } ) );
		cl.def( pybind11::init<class genesis::tree::TreeNode &, class genesis::tree::TreeNode &>(), pybind11::arg("start"), pybind11::arg("finish") );

		cl.def( pybind11::init<class genesis::tree::TreeLink &, class genesis::tree::TreeLink &>(), pybind11::arg("start"), pybind11::arg("finish") );

		cl.def( pybind11::init( [](genesis::tree::IteratorPath<false> const &o){ return new genesis::tree::IteratorPath<false>(o); } ) );
		cl.def("assign", (class genesis::tree::IteratorPath<false> & (genesis::tree::IteratorPath<false>::*)(const class genesis::tree::IteratorPath<false> &)) &genesis::tree::IteratorPath<false>::operator=, "C++: genesis::tree::IteratorPath<false>::operator=(const class genesis::tree::IteratorPath<false> &) --> class genesis::tree::IteratorPath<false> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("dereference", (class genesis::tree::IteratorPath<false> (genesis::tree::IteratorPath<false>::*)()) &genesis::tree::IteratorPath<false>::operator*, "C++: genesis::tree::IteratorPath<false>::operator*() --> class genesis::tree::IteratorPath<false>");
		cl.def("pre_increment", (class genesis::tree::IteratorPath<false> (genesis::tree::IteratorPath<false>::*)()) &genesis::tree::IteratorPath<false>::operator++, "C++: genesis::tree::IteratorPath<false>::operator++() --> class genesis::tree::IteratorPath<false>");
		cl.def("post_increment", (class genesis::tree::IteratorPath<false> (genesis::tree::IteratorPath<false>::*)(int)) &genesis::tree::IteratorPath<false>::operator++, "C++: genesis::tree::IteratorPath<false>::operator++(int) --> class genesis::tree::IteratorPath<false>", pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::tree::IteratorPath<false>::*)(const class genesis::tree::IteratorPath<false> &) const) &genesis::tree::IteratorPath<false>::operator==, "C++: genesis::tree::IteratorPath<false>::operator==(const class genesis::tree::IteratorPath<false> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::tree::IteratorPath<false>::*)(const class genesis::tree::IteratorPath<false> &) const) &genesis::tree::IteratorPath<false>::operator!=, "C++: genesis::tree::IteratorPath<false>::operator!=(const class genesis::tree::IteratorPath<false> &) const --> bool", pybind11::arg("other"));
		cl.def("is_last_common_ancestor", (bool (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::is_last_common_ancestor, "C++: genesis::tree::IteratorPath<false>::is_last_common_ancestor() const --> bool");
		cl.def("is_lca", (bool (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::is_lca, "C++: genesis::tree::IteratorPath<false>::is_lca() const --> bool");
		cl.def("link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::link, "C++: genesis::tree::IteratorPath<false>::link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::node, "C++: genesis::tree::IteratorPath<false>::node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("edge", (class genesis::tree::TreeEdge & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::edge, "C++: genesis::tree::IteratorPath<false>::edge() const --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("start_link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::start_link, "C++: genesis::tree::IteratorPath<false>::start_link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("start_node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::start_node, "C++: genesis::tree::IteratorPath<false>::start_node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::finish_link, "C++: genesis::tree::IteratorPath<false>::finish_link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPath<false>::*)() const) &genesis::tree::IteratorPath<false>::finish_node, "C++: genesis::tree::IteratorPath<false>::finish_node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
	}
	// genesis::tree::path(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) file:genesis/tree/iterator/path.hpp line:337
	M("genesis::tree").def("path", (class genesis::utils::Range<class genesis::tree::IteratorPath<true> > (*)(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)) &genesis::tree::path<genesis::tree::TreeNode>, "C++: genesis::tree::path(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) --> class genesis::utils::Range<class genesis::tree::IteratorPath<true> >", pybind11::arg("start"), pybind11::arg("finish"));

	// genesis::tree::path(const class genesis::tree::TreeLink &, const class genesis::tree::TreeLink &) file:genesis/tree/iterator/path.hpp line:337
	M("genesis::tree").def("path", (class genesis::utils::Range<class genesis::tree::IteratorPath<true> > (*)(const class genesis::tree::TreeLink &, const class genesis::tree::TreeLink &)) &genesis::tree::path<genesis::tree::TreeLink>, "C++: genesis::tree::path(const class genesis::tree::TreeLink &, const class genesis::tree::TreeLink &) --> class genesis::utils::Range<class genesis::tree::IteratorPath<true> >", pybind11::arg("start"), pybind11::arg("finish"));

	// genesis::tree::path(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) file:genesis/tree/iterator/path.hpp line:347
	M("genesis::tree").def("path", (class genesis::utils::Range<class genesis::tree::IteratorPath<false> > (*)(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)) &genesis::tree::path<genesis::tree::TreeNode>, "C++: genesis::tree::path(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) --> class genesis::utils::Range<class genesis::tree::IteratorPath<false> >", pybind11::arg("start"), pybind11::arg("finish"));

	// genesis::tree::path(class genesis::tree::TreeLink &, class genesis::tree::TreeLink &) file:genesis/tree/iterator/path.hpp line:347
	M("genesis::tree").def("path", (class genesis::utils::Range<class genesis::tree::IteratorPath<false> > (*)(class genesis::tree::TreeLink &, class genesis::tree::TreeLink &)) &genesis::tree::path<genesis::tree::TreeLink>, "C++: genesis::tree::path(class genesis::tree::TreeLink &, class genesis::tree::TreeLink &) --> class genesis::utils::Range<class genesis::tree::IteratorPath<false> >", pybind11::arg("start"), pybind11::arg("finish"));

}
