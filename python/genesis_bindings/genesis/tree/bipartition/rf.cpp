#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/bipartition/rf.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/iterator/path.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/math/bitvector.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>
#include <unordered_map>
#include <utility>
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

void bind_genesis_tree_bipartition_rf(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::rf_get_bitvectors(const class genesis::tree::Tree &, const class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > > &) file:genesis/tree/bipartition/rf.hpp line:72
	M("genesis::tree").def("rf_get_bitvectors", (class std::vector<class genesis::utils::Bitvector, class std::allocator<class genesis::utils::Bitvector> > (*)(const class genesis::tree::Tree &, const class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > > &)) &genesis::tree::rf_get_bitvectors, "Get all split Bitvectors for a given Tree.\n\n For each inner edge of the  a Bitvector is produced that contains `true` bits at all\n indices of the tips on one side of the split, using  for getting indices of leaf nodes.\n\n The Bitvector%s are normalized, that is, their first bit is always set to 0. This makes sure\n that the two ways of representing each split result in the same Bitvector.\n\nC++: genesis::tree::rf_get_bitvectors(const class genesis::tree::Tree &, const class std::unordered_map<std::string, unsigned long, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, unsigned long> > > &) --> class std::vector<class genesis::utils::Bitvector, class std::allocator<class genesis::utils::Bitvector> >", pybind11::arg("tree"), pybind11::arg("names"));

	// genesis::tree::rf_get_occurrences(const class genesis::tree::TreeSet &) file:genesis/tree/bipartition/rf.hpp line:90
	M("genesis::tree").def("rf_get_occurrences", (class std::unordered_map<class genesis::utils::Bitvector, class genesis::utils::Bitvector, struct std::hash<genesis::utils::Bitvector>, struct std::equal_to<class genesis::utils::Bitvector>, class std::allocator<struct std::pair<const class genesis::utils::Bitvector, class genesis::utils::Bitvector> > > (*)(const class genesis::tree::TreeSet &)) &genesis::tree::rf_get_occurrences, "Get an occurrence map for each split found in the given TreeSet.\n\n The given  need to contain the same leaf node names. Then, all their splits are\n computed, represented a Bitvector%s of tips. For each such split, another Bitvector is produced\n that is `true` at every tree in  that contains that split.\n\n In other words, the function yields a map from Bitvector%s (keys) that represent splits\n to Bitvector%s (mapped values) that represent occurences of these splits in the given \n The size of the map hence is the number of unique splits in all \n the size of the key Bitvectors is the number of taxa in the trees;\n and the size of the mapped value Bitvectors is the number of trees, that is TreeSet::size().\n\nC++: genesis::tree::rf_get_occurrences(const class genesis::tree::TreeSet &) --> class std::unordered_map<class genesis::utils::Bitvector, class genesis::utils::Bitvector, struct std::hash<genesis::utils::Bitvector>, struct std::equal_to<class genesis::utils::Bitvector>, class std::allocator<struct std::pair<const class genesis::utils::Bitvector, class genesis::utils::Bitvector> > >", pybind11::arg("trees"));

	// genesis::tree::rf_get_occurrences(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &) file:genesis/tree/bipartition/rf.hpp line:104
	M("genesis::tree").def("rf_get_occurrences", (class std::unordered_map<class genesis::utils::Bitvector, class genesis::utils::Bitvector, struct std::hash<genesis::utils::Bitvector>, struct std::equal_to<class genesis::utils::Bitvector>, class std::allocator<struct std::pair<const class genesis::utils::Bitvector, class genesis::utils::Bitvector> > > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &)) &genesis::tree::rf_get_occurrences, "Get an occurrence map for each split found in some Tree%s.\n\n This is a special case of the more general rf_get_occurrences( TreeSet const& trees ) function,\n which takes one additional Tree into account. This  tree gets index 0 in the resulting\n Bitvectors of the mapped values, while all trees in  get their index in the TreeSet plus one.\n\n The function is meant as an accelaration for computing the distance from one Tree to several\n other Tree%s, and is used by rf_distance_absolute( Tree const& lhs, TreeSet const& rhs ).\n\nC++: genesis::tree::rf_get_occurrences(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &) --> class std::unordered_map<class genesis::utils::Bitvector, class genesis::utils::Bitvector, struct std::hash<genesis::utils::Bitvector>, struct std::equal_to<class genesis::utils::Bitvector>, class std::allocator<struct std::pair<const class genesis::utils::Bitvector, class genesis::utils::Bitvector> > >", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::rf_get_occurrences(const class genesis::tree::Tree &, const class genesis::tree::Tree &) file:genesis/tree/bipartition/rf.hpp line:115
	M("genesis::tree").def("rf_get_occurrences", (class std::unordered_map<class genesis::utils::Bitvector, class genesis::utils::Bitvector, struct std::hash<genesis::utils::Bitvector>, struct std::equal_to<class genesis::utils::Bitvector>, class std::allocator<struct std::pair<const class genesis::utils::Bitvector, class genesis::utils::Bitvector> > > (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &)) &genesis::tree::rf_get_occurrences, "Get an occurrence map for each split found in two trees.\n\n This is a special case of the more general rf_get_occurrences( TreeSet const& trees ) function,\n which takes two Tree%s and computes their split occurences.\n\nC++: genesis::tree::rf_get_occurrences(const class genesis::tree::Tree &, const class genesis::tree::Tree &) --> class std::unordered_map<class genesis::utils::Bitvector, class genesis::utils::Bitvector, struct std::hash<genesis::utils::Bitvector>, struct std::equal_to<class genesis::utils::Bitvector>, class std::allocator<struct std::pair<const class genesis::utils::Bitvector, class genesis::utils::Bitvector> > >", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::rf_distance_absolute(const class genesis::tree::TreeSet &) file:genesis/tree/bipartition/rf.hpp line:129
	M("genesis::tree").def("rf_distance_absolute", (class genesis::utils::Matrix<unsigned long> (*)(const class genesis::tree::TreeSet &)) &genesis::tree::rf_distance_absolute, "Compute the pairwise absolute RF (Robinson-Foulds) distance metric between a set of \n\n The function computes the unweighted absolute RF distance.\n\nC++: genesis::tree::rf_distance_absolute(const class genesis::tree::TreeSet &) --> class genesis::utils::Matrix<unsigned long>", pybind11::arg("trees"));

	// genesis::tree::rf_distance_absolute(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &) file:genesis/tree/bipartition/rf.hpp line:138
	M("genesis::tree").def("rf_distance_absolute", (class std::vector<unsigned long, class std::allocator<unsigned long> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &)) &genesis::tree::rf_distance_absolute, "Compute the absolute RF (Robinson-Foulds) distance metric between a given  Tree\n and all of the trees in the  TreeSet.\n\n The function computes the unweighted absolute RF distance.\n This is meant as an accelaration if the pairwise distance is not needed.\n\nC++: genesis::tree::rf_distance_absolute(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &) --> class std::vector<unsigned long, class std::allocator<unsigned long> >", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::rf_distance_absolute(const class genesis::tree::Tree &, const class genesis::tree::Tree &) file:genesis/tree/bipartition/rf.hpp line:145
	M("genesis::tree").def("rf_distance_absolute", (unsigned long (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &)) &genesis::tree::rf_distance_absolute, "Compute the absolute RF (Robinson-Foulds) distance metric between two Tree%s.\n\n The function computes the unweighted absolute RF distance.\n\nC++: genesis::tree::rf_distance_absolute(const class genesis::tree::Tree &, const class genesis::tree::Tree &) --> unsigned long", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::rf_distance_relative(const class genesis::tree::TreeSet &) file:genesis/tree/bipartition/rf.hpp line:161
	M("genesis::tree").def("rf_distance_relative", (class genesis::utils::Matrix<double> (*)(const class genesis::tree::TreeSet &)) &genesis::tree::rf_distance_relative, "Compute the pairwise relative RF (Robinson-Foulds) distance metric between a set of \n\n The function computes the unweighted relative RF distance.\n\n This internally simply uses rf_distance_absolute(), and divides the result properly;\n hence, if both variants are needed (absolute and relative), it might be faster to duplicate\n that normalization code (simply copy from this function), instead of computing the RF distance\n twice, which is what happens if both rf_distance_relative() and rf_distance_absolute() are called.\n\nC++: genesis::tree::rf_distance_relative(const class genesis::tree::TreeSet &) --> class genesis::utils::Matrix<double>", pybind11::arg("trees"));

	// genesis::tree::rf_distance_relative(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &) file:genesis/tree/bipartition/rf.hpp line:175
	M("genesis::tree").def("rf_distance_relative", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &)) &genesis::tree::rf_distance_relative, "Compute the relative RF (Robinson-Foulds) distance metric between a given  Tree\n and all of the trees in the  TreeSet.\n\n The function computes the unweighted relative RF distance.\n This is meant as an accelaration if the pairwise distance is not needed.\n\n This internally simply uses rf_distance_absolute(), and divides the result properly;\n hence, if both variants are needed (absolute and relative), it might be faster to duplicate\n that normalization code (simply copy from this function), instead of computing the RF distance\n twice, which is what happens if both rf_distance_relative() and rf_distance_absolute() are called.\n\nC++: genesis::tree::rf_distance_relative(const class genesis::tree::Tree &, const class genesis::tree::TreeSet &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("lhs"), pybind11::arg("rhs"));

	// genesis::tree::rf_distance_relative(const class genesis::tree::Tree &, const class genesis::tree::Tree &) file:genesis/tree/bipartition/rf.hpp line:187
	M("genesis::tree").def("rf_distance_relative", (double (*)(const class genesis::tree::Tree &, const class genesis::tree::Tree &)) &genesis::tree::rf_distance_relative, "Compute the relative RF (Robinson-Foulds) distance metric between two Tree%s.\n\n The function computes the unweighted relative RF distance.\n\n This internally simply uses rf_distance_absolute(), and divides the result properly;\n hence, if both variants are needed (absolute and relative), it might be faster to duplicate\n that normalization code (simply copy from this function), instead of computing the RF distance\n twice, which is what happens if both rf_distance_relative() and rf_distance_absolute() are called.\n\nC++: genesis::tree::rf_distance_relative(const class genesis::tree::Tree &, const class genesis::tree::Tree &) --> double", pybind11::arg("lhs"), pybind11::arg("rhs"));

	{ // genesis::tree::IteratorPath file:genesis/tree/iterator/path.hpp line:60
		pybind11::class_<genesis::tree::IteratorPath<true>, std::shared_ptr<genesis::tree::IteratorPath<true>>> cl(M("genesis::tree"), "IteratorPath_true_t", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::IteratorPath<true>(); } ) );
		cl.def( pybind11::init<const class genesis::tree::TreeNode &, const class genesis::tree::TreeNode &>(), pybind11::arg("start"), pybind11::arg("finish") );

		cl.def( pybind11::init<const class genesis::tree::TreeLink &, const class genesis::tree::TreeLink &>(), pybind11::arg("start"), pybind11::arg("finish") );

		cl.def( pybind11::init( [](genesis::tree::IteratorPath<true> const &o){ return new genesis::tree::IteratorPath<true>(o); } ) );
		cl.def("assign", (class genesis::tree::IteratorPath<true> & (genesis::tree::IteratorPath<true>::*)(const class genesis::tree::IteratorPath<true> &)) &genesis::tree::IteratorPath<true>::operator=, "C++: genesis::tree::IteratorPath<true>::operator=(const class genesis::tree::IteratorPath<true> &) --> class genesis::tree::IteratorPath<true> &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("dereference", (class genesis::tree::IteratorPath<true> (genesis::tree::IteratorPath<true>::*)()) &genesis::tree::IteratorPath<true>::operator*, "C++: genesis::tree::IteratorPath<true>::operator*() --> class genesis::tree::IteratorPath<true>");
		cl.def("pre_increment", (class genesis::tree::IteratorPath<true> (genesis::tree::IteratorPath<true>::*)()) &genesis::tree::IteratorPath<true>::operator++, "C++: genesis::tree::IteratorPath<true>::operator++() --> class genesis::tree::IteratorPath<true>");
		cl.def("post_increment", (class genesis::tree::IteratorPath<true> (genesis::tree::IteratorPath<true>::*)(int)) &genesis::tree::IteratorPath<true>::operator++, "C++: genesis::tree::IteratorPath<true>::operator++(int) --> class genesis::tree::IteratorPath<true>", pybind11::arg(""));
		cl.def("__eq__", (bool (genesis::tree::IteratorPath<true>::*)(const class genesis::tree::IteratorPath<true> &) const) &genesis::tree::IteratorPath<true>::operator==, "C++: genesis::tree::IteratorPath<true>::operator==(const class genesis::tree::IteratorPath<true> &) const --> bool", pybind11::arg("other"));
		cl.def("__ne__", (bool (genesis::tree::IteratorPath<true>::*)(const class genesis::tree::IteratorPath<true> &) const) &genesis::tree::IteratorPath<true>::operator!=, "C++: genesis::tree::IteratorPath<true>::operator!=(const class genesis::tree::IteratorPath<true> &) const --> bool", pybind11::arg("other"));
		cl.def("is_last_common_ancestor", (bool (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::is_last_common_ancestor, "C++: genesis::tree::IteratorPath<true>::is_last_common_ancestor() const --> bool");
		cl.def("is_lca", (bool (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::is_lca, "C++: genesis::tree::IteratorPath<true>::is_lca() const --> bool");
		cl.def("link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::link, "C++: genesis::tree::IteratorPath<true>::link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::node, "C++: genesis::tree::IteratorPath<true>::node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("edge", (const class genesis::tree::TreeEdge & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::edge, "C++: genesis::tree::IteratorPath<true>::edge() const --> const class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal);
		cl.def("start_link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::start_link, "C++: genesis::tree::IteratorPath<true>::start_link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("start_node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::start_node, "C++: genesis::tree::IteratorPath<true>::start_node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_link", (const class genesis::tree::TreeLink & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::finish_link, "C++: genesis::tree::IteratorPath<true>::finish_link() const --> const class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("finish_node", (const class genesis::tree::TreeNode & (genesis::tree::IteratorPath<true>::*)() const) &genesis::tree::IteratorPath<true>::finish_node, "C++: genesis::tree::IteratorPath<true>::finish_node() const --> const class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
	}
}
