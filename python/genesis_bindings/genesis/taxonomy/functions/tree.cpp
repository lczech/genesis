#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/taxonomy/functions/tree.hpp>
#include <genesis/taxonomy/taxon.hpp>
#include <genesis/taxonomy/taxon_data.hpp>
#include <genesis/taxonomy/taxonomy.hpp>
#include <genesis/taxonomy/taxopath.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <iterator>
#include <memory>
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

void bind_genesis_taxonomy_functions_tree(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::taxonomy::taxonomy_to_tree(const class genesis::taxonomy::Taxonomy &, bool, bool, int) file:genesis/taxonomy/functions/tree.hpp line:75
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0); }, "", pybind11::arg("taxonomy"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0, bool const & a1) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1); }, "", pybind11::arg("taxonomy"), pybind11::arg("keep_singleton_inner_nodes"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0, bool const & a1, bool const & a2) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1, a2); }, "", pybind11::arg("taxonomy"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"));
	M("genesis::taxonomy").def("taxonomy_to_tree", (class genesis::tree::Tree (*)(const class genesis::taxonomy::Taxonomy &, bool, bool, int)) &genesis::taxonomy::taxonomy_to_tree, "Turn a Taxonomy into a (possibly multifurcating) Tree.\n\n A Taxonomy is a hierarchy that can be interpreted as a rooted tree. Using this function, such\n a tree is created and returned. It can be used to construct a taxonomic constraint tree\n for tree inference.\n\n It might happen that a taxonomic path goes down several levels with just one taxon at each level.\n This would create inner nodes in the tree that just connect two other nodes, that is, nodes that\n do not furcate at all. Many downstream programs might have problems with such trees.\n By default, such nodes are collapsed.  can be used to include these\n inner nodes in the tree, instead of immediately adding their children.\n\n Furthermore, a Taxonomy contains names at every level, while a Tree usually does not contain\n inner node names. Thus, inner node are not named by default. Use \n to still set the inner taxonomic labels in the tree.\n\n Lastly,  can be used to only turn the first few levels (starting at 0) of the\n Taxonomy to the tree, and stopping after that. By default, the whole Taxonomy (all levels)\n is turned into a Tree.\n\nC++: genesis::taxonomy::taxonomy_to_tree(const class genesis::taxonomy::Taxonomy &, bool, bool, int) --> class genesis::tree::Tree", pybind11::arg("taxonomy"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"), pybind11::arg("max_level"));

	// genesis::taxonomy::taxonomy_to_tree(const class genesis::taxonomy::Taxonomy &, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > &, bool, bool, int, bool) file:genesis/taxonomy/functions/tree.hpp line:106
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a1) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1); }, "", pybind11::arg("taxonomy"), pybind11::arg("extra_taxa"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a1, bool const & a2) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1, a2); }, "", pybind11::arg("taxonomy"), pybind11::arg("extra_taxa"), pybind11::arg("keep_singleton_inner_nodes"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a1, bool const & a2, bool const & a3) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1, a2, a3); }, "", pybind11::arg("taxonomy"), pybind11::arg("extra_taxa"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class genesis::taxonomy::Taxonomy & a0, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a1, bool const & a2, bool const & a3, int const & a4) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1, a2, a3, a4); }, "", pybind11::arg("taxonomy"), pybind11::arg("extra_taxa"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"), pybind11::arg("max_level"));
	M("genesis::taxonomy").def("taxonomy_to_tree", (class genesis::tree::Tree (*)(const class genesis::taxonomy::Taxonomy &, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > &, bool, bool, int, bool)) &genesis::taxonomy::taxonomy_to_tree, "Turn a Taxonomy into a (possibly multifurcating) Tree, and allow to add extra tips to it.\n\n This is similar to the general version of this function, see\n \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxonomy_to_tree(const class genesis::taxonomy::Taxonomy &, const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > &, bool, bool, int, bool) --> class genesis::tree::Tree", pybind11::arg("taxonomy"), pybind11::arg("extra_taxa"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"), pybind11::arg("max_level"), pybind11::arg("add_extra_taxa_parents"));

	// genesis::taxonomy::taxonomy_to_tree(const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > &, bool, bool, int) file:genesis/taxonomy/functions/tree.hpp line:128
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a0) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0); }, "", pybind11::arg("taxon_map"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a0, bool const & a1) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1); }, "", pybind11::arg("taxon_map"), pybind11::arg("keep_singleton_inner_nodes"));
	M("genesis::taxonomy").def("taxonomy_to_tree", [](const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > & a0, bool const & a1, bool const & a2) -> genesis::tree::Tree { return genesis::taxonomy::taxonomy_to_tree(a0, a1, a2); }, "", pybind11::arg("taxon_map"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"));
	M("genesis::taxonomy").def("taxonomy_to_tree", (class genesis::tree::Tree (*)(const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > &, bool, bool, int)) &genesis::taxonomy::taxonomy_to_tree, "Turn a list of Taxa into a (possibly multifurcating) Tree.\n\n The function is a simplified version of\n \n\n\n\n\n\n\n\n\n\n\nC++: genesis::taxonomy::taxonomy_to_tree(const class std::unordered_map<std::string, class genesis::taxonomy::Taxopath, struct std::hash<std::string>, struct std::equal_to<std::string >, class std::allocator<struct std::pair<const std::string, class genesis::taxonomy::Taxopath> > > &, bool, bool, int) --> class genesis::tree::Tree", pybind11::arg("taxon_map"), pybind11::arg("keep_singleton_inner_nodes"), pybind11::arg("keep_inner_node_names"), pybind11::arg("max_level"));

}
