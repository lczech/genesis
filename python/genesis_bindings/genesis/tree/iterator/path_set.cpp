#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/iterator/path_set.hpp>
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

void bind_genesis_tree_iterator_path_set(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::IteratorPathSet file:genesis/tree/iterator/path_set.hpp line:96
		pybind11::class_<genesis::tree::IteratorPathSet<true>, std::shared_ptr<genesis::tree::IteratorPathSet<true>>> cl(M("genesis::tree"), "IteratorPathSet_true_t", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::IteratorPathSet<true>(); } ) );
		cl.def( pybind11::init<const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &>(), pybind11::arg("start"), pybind11::arg("finish"), pybind11::arg("lca") );

		cl.def( pybind11::init<const class genesis::tree::TreeLink &, const class genesis::tree::TreeLink &, const class genesis::tree::TreeLink &>(), pybind11::arg("start"), pybind11::arg("finish"), pybind11::arg("lca") );

		cl.def( pybind11::init( [](genesis::tree::IteratorPathSet<true> const &o){ return new genesis::tree::IteratorPathSet<true>(o); } ) );
		cl.def("dereference", (class genesis::tree::IteratorPathSet<true> (genesis::tree::IteratorPathSet<true>::*)()) &genesis::tree::IteratorPathSet<true>::operator*, "C++: genesis::tree::IteratorPathSet<true>::operator*() --> class genesis::tree::IteratorPathSet<true>");
		cl.def("pre_increment", (class genesis::tree::IteratorPathSet<true> (genesis::tree::IteratorPathSet<true>::*)()) &genesis::tree::IteratorPathSet<true>::operator++, "C++: genesis::tree::IteratorPathSet<true>::operator++() --> class genesis::tree::IteratorPathSet<true>");
		cl.def("post_increment", (class genesis::tree::IteratorPathSet<true> (genesis::tree::IteratorPathSet<true>::*)(int)) &genesis::tree::IteratorPathSet<true>::operator++, "C++: genesis::tree::IteratorPathSet<true>::operator++(int) --> class genesis::tree::IteratorPathSet<true>", pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::tree::IteratorPathSet<true>::*)(const class genesis::tree::IteratorPathSet<true> &) const) &genesis::tree::IteratorPathSet<true>::operator==, "C++: genesis::tree::IteratorPathSet<true>::operator==(const class genesis::tree::IteratorPathSet<true> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::tree::IteratorPathSet<true>::*)(const class genesis::tree::IteratorPathSet<true> &) const) &genesis::tree::IteratorPathSet<true>::operator!=, "C++: genesis::tree::IteratorPathSet<true>::operator!=(const class genesis::tree::IteratorPathSet<true> &) const --> bool", pybind11::arg("other"));
		cl.def("is_last_common_ancestor", (bool (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::is_last_common_ancestor, "C++: genesis::tree::IteratorPathSet<true>::is_last_common_ancestor() const --> bool");
		cl.def("is_lca", (bool (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::is_lca, "C++: genesis::tree::IteratorPathSet<true>::is_lca() const --> bool");
		cl.def("link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::link, "C++: genesis::tree::IteratorPathSet<true>::link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::node, "C++: genesis::tree::IteratorPathSet<true>::node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("edge", (const class genesis::tree::TreeEdge & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::edge, "C++: genesis::tree::IteratorPathSet<true>::edge() const --> const class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("start_link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::start_link, "C++: genesis::tree::IteratorPathSet<true>::start_link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("start_node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::start_node, "C++: genesis::tree::IteratorPathSet<true>::start_node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::finish_link, "C++: genesis::tree::IteratorPathSet<true>::finish_link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::finish_node, "C++: genesis::tree::IteratorPathSet<true>::finish_node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("lca_link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::lca_link, "C++: genesis::tree::IteratorPathSet<true>::lca_link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("lca_node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<true>::*)() const) &genesis::tree::IteratorPathSet<true>::lca_node, "C++: genesis::tree::IteratorPathSet<true>::lca_node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
	}
	{ // genesis::tree::IteratorPathSet file:genesis/tree/iterator/path_set.hpp line:96
		pybind11::class_<genesis::tree::IteratorPathSet<false>, std::shared_ptr<genesis::tree::IteratorPathSet<false>>> cl(M("genesis::tree"), "IteratorPathSet_false_t", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::IteratorPathSet<false>(); } ) );
		cl.def( pybind11::init<class genesis::tree::TreeNode &, class genesis::tree::TreeNode &, class genesis::tree::TreeNode &>(), pybind11::arg("start"), pybind11::arg("finish"), pybind11::arg("lca") );

		cl.def( pybind11::init<class genesis::tree::TreeLink &, class genesis::tree::TreeLink &, class genesis::tree::TreeLink &>(), pybind11::arg("start"), pybind11::arg("finish"), pybind11::arg("lca") );

		cl.def( pybind11::init( [](genesis::tree::IteratorPathSet<false> const &o){ return new genesis::tree::IteratorPathSet<false>(o); } ) );
		cl.def("dereference", (class genesis::tree::IteratorPathSet<false> (genesis::tree::IteratorPathSet<false>::*)()) &genesis::tree::IteratorPathSet<false>::operator*, "C++: genesis::tree::IteratorPathSet<false>::operator*() --> class genesis::tree::IteratorPathSet<false>");
		cl.def("pre_increment", (class genesis::tree::IteratorPathSet<false> (genesis::tree::IteratorPathSet<false>::*)()) &genesis::tree::IteratorPathSet<false>::operator++, "C++: genesis::tree::IteratorPathSet<false>::operator++() --> class genesis::tree::IteratorPathSet<false>");
		cl.def("post_increment", (class genesis::tree::IteratorPathSet<false> (genesis::tree::IteratorPathSet<false>::*)(int)) &genesis::tree::IteratorPathSet<false>::operator++, "C++: genesis::tree::IteratorPathSet<false>::operator++(int) --> class genesis::tree::IteratorPathSet<false>", pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::tree::IteratorPathSet<false>::*)(const class genesis::tree::IteratorPathSet<false> &) const) &genesis::tree::IteratorPathSet<false>::operator==, "C++: genesis::tree::IteratorPathSet<false>::operator==(const class genesis::tree::IteratorPathSet<false> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::tree::IteratorPathSet<false>::*)(const class genesis::tree::IteratorPathSet<false> &) const) &genesis::tree::IteratorPathSet<false>::operator!=, "C++: genesis::tree::IteratorPathSet<false>::operator!=(const class genesis::tree::IteratorPathSet<false> &) const --> bool", pybind11::arg("other"));
		cl.def("is_last_common_ancestor", (bool (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::is_last_common_ancestor, "C++: genesis::tree::IteratorPathSet<false>::is_last_common_ancestor() const --> bool");
		cl.def("is_lca", (bool (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::is_lca, "C++: genesis::tree::IteratorPathSet<false>::is_lca() const --> bool");
		cl.def("link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::link, "C++: genesis::tree::IteratorPathSet<false>::link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::node, "C++: genesis::tree::IteratorPathSet<false>::node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("edge", (class genesis::tree::TreeEdge & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::edge, "C++: genesis::tree::IteratorPathSet<false>::edge() const --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("start_link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::start_link, "C++: genesis::tree::IteratorPathSet<false>::start_link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("start_node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::start_node, "C++: genesis::tree::IteratorPathSet<false>::start_node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::finish_link, "C++: genesis::tree::IteratorPathSet<false>::finish_link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::finish_node, "C++: genesis::tree::IteratorPathSet<false>::finish_node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("lca_link", (class genesis::tree::TreeLink & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::lca_link, "C++: genesis::tree::IteratorPathSet<false>::lca_link() const --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("lca_node", (class genesis::tree::TreeNode & (genesis::tree::IteratorPathSet<false>::*)() const) &genesis::tree::IteratorPathSet<false>::lca_node, "C++: genesis::tree::IteratorPathSet<false>::lca_node() const --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
	}
	// genesis::tree::path_set(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) file:genesis/tree/iterator/path_set.hpp line:317
	M("genesis::tree").def("path_set", (class genesis::utils::Range<class genesis::tree::IteratorPathSet<true> > (*)(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &)) &genesis::tree::path_set<genesis::tree::TreeNode>, "C++: genesis::tree::path_set(const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &) --> class genesis::utils::Range<class genesis::tree::IteratorPathSet<true> >", pybind11::arg("start"), pybind11::arg("finish"), pybind11::arg("lca"));

	// genesis::tree::path_set(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) file:genesis/tree/iterator/path_set.hpp line:327
	M("genesis::tree").def("path_set", (class genesis::utils::Range<class genesis::tree::IteratorPathSet<false> > (*)(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &, class genesis::tree::TreeNode &)) &genesis::tree::path_set<genesis::tree::TreeNode>, "C++: genesis::tree::path_set(class genesis::tree::TreeNode &, class genesis::tree::TreeNode &, class genesis::tree::TreeNode &) --> class genesis::utils::Range<class genesis::tree::IteratorPathSet<false> >", pybind11::arg("start"), pybind11::arg("finish"), pybind11::arg("lca"));

}
