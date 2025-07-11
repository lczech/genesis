#include <functional>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/broker.hpp>
#include <genesis/tree/formats/newick/element.hpp>
#include <genesis/tree/formats/newick/reader.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/function/operators.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree/node_data.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_input_source.hpp>
#include <genesis/utils/io/input_stream.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <ostream>
#include <sstream> // __str__
#include <streambuf>
#include <string>
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

void bind_genesis_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	// genesis::tree::validate_topology(const class genesis::tree::Tree &) file:genesis/tree/tree.hpp line:60
	M("genesis::tree").def("validate_topology", (bool (*)(const class genesis::tree::Tree &)) &genesis::tree::validate_topology, "C++: genesis::tree::validate_topology(const class genesis::tree::Tree &) --> bool", pybind11::arg("tree"));

	{ // genesis::tree::Tree file:genesis/tree/tree.hpp line:97
		pybind11::class_<genesis::tree::Tree, std::shared_ptr<genesis::tree::Tree>> cl(M("genesis::tree"), "Tree", "Class for representing phylogenetic trees.\n\n A tree in this implementation consists of three types of elements: Links, Nodes and Edges.\n The topoloty of the tree is completely described by the links, while nodes and edges add the\n capability to store data on the tree.\n\n Data in the Tree is not directly stored in the elements (Links, Nodes, Edges) of the Tree.\n Instead, data belonging to nodes and edges can be stored in their data pointers. For this,\n the data classes need to derive from BaseNodeData and BaseEdgeData, respectively.\n\n Additional information concerning the whole tree, as well as special algorithms working on the\n tree and its data are also not direct members of the tree - for reasons of flexibility\n and extensibility: Instead of adding more and more data and algorithms to the tree, we create\n new classes that encapsulate a tree and all the other needed code to work with it.\n See the Sample class for an example.\n\n Thus, the tree itself only contains the needed information to store and work with a topology.\n\n This class has several redundant ways of representing the same information. On the one hand, this\n makes using a Tree easy, as one can chose the representaion of data that best fits a given task.\n On the other hand, maintaining all those invariants when changing the tree topology is tedious.\n Here is a (comprehensive?) list of invariants of this class:\n\n  *  The indices in all three arrays (nodes(), links() and edges()) have to match the index\n     integers stored in those elements: `node_at(i).index() == i`.\n  *  The link that is stored in a node has to be the one pointing towards the root.\n  *  The primary link of an edge has to point towards the root, the secondary away from it.\n\n Those invariants are established when the Tree is constructed.");
		cl.def( pybind11::init( [](){ return new genesis::tree::Tree(); } ) );
		cl.def( pybind11::init( [](genesis::tree::Tree const &o){ return new genesis::tree::Tree(o); } ) );
		cl.def("assign", (class genesis::tree::Tree & (genesis::tree::Tree::*)(const class genesis::tree::Tree &)) &genesis::tree::Tree::operator=, "Assignment operator.\n\n See Tree copy constructor for more information.\n\nC++: genesis::tree::Tree::operator=(const class genesis::tree::Tree &) --> class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal, pybind11::arg("other"));
		cl.def("clone_topology", (class genesis::tree::Tree (genesis::tree::Tree::*)() const) &genesis::tree::Tree::clone_topology, "Return a Tree with the same topology, but without any data.\n\n All data pointers of the nodes and edges of the returned tree are `nullptr`.\n\nC++: genesis::tree::Tree::clone_topology() const --> class genesis::tree::Tree");
		cl.def("swap", (void (genesis::tree::Tree::*)(class genesis::tree::Tree &)) &genesis::tree::Tree::swap, "Swap.\n\nC++: genesis::tree::Tree::swap(class genesis::tree::Tree &) --> void", pybind11::arg("other"));
		cl.def("clear", (void (genesis::tree::Tree::*)()) &genesis::tree::Tree::clear, "Deletes all data of the tree, including all links, nodes and edges.\n\n This functions results in an empty tree.\n\nC++: genesis::tree::Tree::clear() --> void");
		cl.def("empty", (bool (genesis::tree::Tree::*)() const) &genesis::tree::Tree::empty, "Return whether the Tree is empty (i.e., has no nodes, edges and links).\n\nC++: genesis::tree::Tree::empty() const --> bool");
		cl.def("link_at", (class genesis::tree::TreeLink & (genesis::tree::Tree::*)(unsigned long)) &genesis::tree::Tree::link_at, "Return the TreeLink at a certain index.\n\nC++: genesis::tree::Tree::link_at(unsigned long) --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("node_at", (class genesis::tree::TreeNode & (genesis::tree::Tree::*)(unsigned long)) &genesis::tree::Tree::node_at, "Return the TreeNode at a certain index.\n\nC++: genesis::tree::Tree::node_at(unsigned long) --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("edge_at", (class genesis::tree::TreeEdge & (genesis::tree::Tree::*)(unsigned long)) &genesis::tree::Tree::edge_at, "Return the TreeEdge at a certain index.\n\nC++: genesis::tree::Tree::edge_at(unsigned long) --> class genesis::tree::TreeEdge &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("link_count", (unsigned long (genesis::tree::Tree::*)() const) &genesis::tree::Tree::link_count, "Return the number of TreeLink%s of the Tree.\n\nC++: genesis::tree::Tree::link_count() const --> unsigned long");
		cl.def("node_count", (unsigned long (genesis::tree::Tree::*)() const) &genesis::tree::Tree::node_count, "Return the number of TreeNode%s of the Tree.\n\nC++: genesis::tree::Tree::node_count() const --> unsigned long");
		cl.def("edge_count", (unsigned long (genesis::tree::Tree::*)() const) &genesis::tree::Tree::edge_count, "Return the number of TreeEdge%s of the Tree.\n\nC++: genesis::tree::Tree::edge_count() const --> unsigned long");
		cl.def("root_link", (class genesis::tree::TreeLink & (genesis::tree::Tree::*)()) &genesis::tree::Tree::root_link, "Return the TreeLink at the current root of the Tree.\n\nC++: genesis::tree::Tree::root_link() --> class genesis::tree::TreeLink &", pybind11::return_value_policy::reference_internal);
		cl.def("root_node", (class genesis::tree::TreeNode & (genesis::tree::Tree::*)()) &genesis::tree::Tree::root_node, "Return the TreeNode at the current root of the Tree.\n\nC++: genesis::tree::Tree::root_node() --> class genesis::tree::TreeNode &", pybind11::return_value_policy::reference_internal);
		cl.def("reset_root_link", (class genesis::tree::Tree & (genesis::tree::Tree::*)(class genesis::tree::TreeLink *)) &genesis::tree::Tree::reset_root_link, "Reset the link that is considered to be the root of the Tree.\n\n This function is meant for tree manipulation functions. Use with care!\n\n Caveat: This function simply sets the link, but does not change any other properties of the tree.\n Particularly the correct primary/secondary order of TreeEdge%s and primary links of TreeNode%s\n needs to be maintained manually when using this function! Otherwise, we end up with an invalid\n Tree that breaks its invariants!\n\nC++: genesis::tree::Tree::reset_root_link(class genesis::tree::TreeLink *) --> class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal, pybind11::arg("root_link"));

		cl.def("__str__", [](genesis::tree::Tree const &o) -> std::string { std::ostringstream s; genesis::tree::operator<<(s, o); return s.str(); } );

		extend_tree(cl);
	}
	{ // genesis::tree::NewickReader file:genesis/tree/formats/newick/reader.hpp line:67
		pybind11::class_<genesis::tree::NewickReader, std::shared_ptr<genesis::tree::NewickReader>> cl(M("genesis::tree"), "NewickReader", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::NewickReader(); } ) );
		cl.def( pybind11::init( [](genesis::tree::NewickReader const &o){ return new genesis::tree::NewickReader(o); } ) );
		cl.def_readwrite("prepare_reading_plugins", &genesis::tree::NewickReader::prepare_reading_plugins);
		cl.def_readwrite("finish_reading_plugins", &genesis::tree::NewickReader::finish_reading_plugins);
		cl.def_readwrite("create_node_data_plugin", &genesis::tree::NewickReader::create_node_data_plugin);
		cl.def_readwrite("create_edge_data_plugin", &genesis::tree::NewickReader::create_edge_data_plugin);
		cl.def_readwrite("element_to_node_plugins", &genesis::tree::NewickReader::element_to_node_plugins);
		cl.def_readwrite("element_to_edge_plugins", &genesis::tree::NewickReader::element_to_edge_plugins);
		cl.def("assign", (class genesis::tree::NewickReader & (genesis::tree::NewickReader::*)(const class genesis::tree::NewickReader &)) &genesis::tree::NewickReader::operator=, "C++: genesis::tree::NewickReader::operator=(const class genesis::tree::NewickReader &) --> class genesis::tree::NewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("read", (class genesis::tree::Tree (genesis::tree::NewickReader::*)(class std::shared_ptr<class genesis::utils::BaseInputSource>) const) &genesis::tree::NewickReader::read, "Read a single Tree from an input source containing a Newick tree.\n\n Use functions such as utils::from_file() and utils::from_string() to conveniently\n get an input source that can be used here.\n\nC++: genesis::tree::NewickReader::read(class std::shared_ptr<class genesis::utils::BaseInputSource>) const --> class genesis::tree::Tree", pybind11::arg("source"));
		cl.def("read", [](genesis::tree::NewickReader const &o, class std::shared_ptr<class genesis::utils::BaseInputSource> const & a0, class genesis::tree::TreeSet & a1) -> void { return o.read(a0, a1); }, "", pybind11::arg("source"), pybind11::arg("target"));
		cl.def("read", (void (genesis::tree::NewickReader::*)(class std::shared_ptr<class genesis::utils::BaseInputSource>, class genesis::tree::TreeSet &, const std::string &) const) &genesis::tree::NewickReader::read, "Add Tree%s to a TreeSet from an input source containing a list of Newick trees.\n\n These trees can either be named or unnamed, using this syntax:\n\n     Tree_A = (...);\n     'Tree B'=(...);\n     (...);\n\n where the first two lines are named trees and the third line is an unnamed tree.\n The trees do not have to be on distinct lines of the input, as whitespaces are completely\n stripped anyway. However, they are required to end with a semicolon `;`.\n\n In case of unnamed trees, a `default_name` can be provided, which will be appended by a counter\n that counts up all unnamed trees. For example, `default_name == \"tree_\"` results in trees\n named \"tree_0\", \"tree_1\" etc. If no default name is given, the trees will simply be named\n using the counter itself.\n\n The Trees are added to the TreeSet. That means, Trees that already exist in the\n TreeSet are kept. Thus, this function can be used to add additional Trees to the set.\n\n Use functions such as utils::from_file() and utils::from_string() to conveniently\n get an input source that can be used here.\n\nC++: genesis::tree::NewickReader::read(class std::shared_ptr<class genesis::utils::BaseInputSource>, class genesis::tree::TreeSet &, const std::string &) const --> void", pybind11::arg("source"), pybind11::arg("target"), pybind11::arg("default_name"));
		cl.def("read", [](genesis::tree::NewickReader const &o, class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > > const & a0, class genesis::tree::TreeSet & a1) -> void { return o.read(a0, a1); }, "", pybind11::arg("sources"), pybind11::arg("target"));
		cl.def("read", (void (genesis::tree::NewickReader::*)(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >, class genesis::tree::TreeSet &, const std::string &) const) &genesis::tree::NewickReader::read, "Fill a TreeSet from a list of input sources containing Newick trees.\n\n See \n\n\n\nC++: genesis::tree::NewickReader::read(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >, class genesis::tree::TreeSet &, const std::string &) const --> void", pybind11::arg("sources"), pybind11::arg("target"), pybind11::arg("default_name"));
		cl.def("read", [](genesis::tree::NewickReader const &o, class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > > const & a0) -> genesis::tree::TreeSet { return o.read(a0); }, "", pybind11::arg("sources"));
		cl.def("read", (class genesis::tree::TreeSet (genesis::tree::NewickReader::*)(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >, const std::string &) const) &genesis::tree::NewickReader::read, "Return a TreeSet from a list of input sources containing Newick trees.\n\n See \n\n\n\nC++: genesis::tree::NewickReader::read(class std::vector<class std::shared_ptr<class genesis::utils::BaseInputSource>, class std::allocator<class std::shared_ptr<class genesis::utils::BaseInputSource> > >, const std::string &) const --> class genesis::tree::TreeSet", pybind11::arg("sources"), pybind11::arg("default_name"));
		cl.def("enable_tags", (class genesis::tree::NewickReader & (genesis::tree::NewickReader::*)(bool)) &genesis::tree::NewickReader::enable_tags, "Set whether Newick tags are enabled for reading.\n\n Newick tags are an inofficial extension to the Newick format. They consist of values in curly\n braces, for example `{value}` and can occur where node labels are allowed in Newick.\n\n For example:\n\n     (( A{0}, B{1} )D{3}, C{4} )R{5};\n\n Thus, they are used similarly to the way Newick comments are often (mis-)used to annotate a\n tree with additional information about the nodes and edges.\n\n They are for example used in the `jplace` format, see placement::JplaceReader for details.\n\n If this option is set to `true`, such tags are stored in NewickBrokerElement::tags. If it is\n `false` (default), any string that has the form of Newick tags will simply be treated as part\n of the node name.\n\nC++: genesis::tree::NewickReader::enable_tags(bool) --> class genesis::tree::NewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("enable_tags", (bool (genesis::tree::NewickReader::*)() const) &genesis::tree::NewickReader::enable_tags, "Return whether currently Newick tags are enabled.\n\n See enable_tags( bool ) for details.\n\nC++: genesis::tree::NewickReader::enable_tags() const --> bool");
		cl.def("stop_after_semicolon", (class genesis::tree::NewickReader & (genesis::tree::NewickReader::*)(bool)) &genesis::tree::NewickReader::stop_after_semicolon, "Set whether reading a single tree stops after the semicolon that finishes a Newick\n tree.\n\n When reading a single Newick tree, it is possible that there is input after the semicolon.\n If this input is just Newick comments, this is allowed. However, other input might indicate\n an error in the tree. Using this function, the behaviour of the reading can be controlled.\n\n If set to `false` (default), reading continues until the end of the input is reached. This\n is mostly wanted when reading e.g., a complete file. If then input contains invalid data\n (non-comments) after the semicolon, an execption is thrown.\n\n If set to `true`, reading stops after the semicolon. This is useful if the Newick tree is\n part of some other file, e.g., Nexus. In this case, we simply want to stop and continue\n parsing the rest of the input as Nexus data.\n\nC++: genesis::tree::NewickReader::stop_after_semicolon(bool) --> class genesis::tree::NewickReader &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("stop_after_semicolon", (bool (genesis::tree::NewickReader::*)() const) &genesis::tree::NewickReader::stop_after_semicolon, "Return whether currently reading stops after the  semicolon that finishes a Newick\n tree.\n\n See stop_after_semicolon( bool ) for details.\n\nC++: genesis::tree::NewickReader::stop_after_semicolon() const --> bool");
		cl.def("parse_single_tree", (class genesis::tree::Tree (genesis::tree::NewickReader::*)(class genesis::utils::InputStream &) const) &genesis::tree::NewickReader::parse_single_tree, "Parse a single tree. Depending on stop_after_semicolon(), stop after the semicolon\n or continue until the end of the input, checking if there are only comments.\n\nC++: genesis::tree::NewickReader::parse_single_tree(class genesis::utils::InputStream &) const --> class genesis::tree::Tree", pybind11::arg("input_stream"));
		cl.def("parse_multiple_trees", (void (genesis::tree::NewickReader::*)(class genesis::utils::InputStream &, class genesis::tree::TreeSet &, const std::string &) const) &genesis::tree::NewickReader::parse_multiple_trees, "Parse until the end of the stream and add all Tree%s to the TreeSet.\n\nC++: genesis::tree::NewickReader::parse_multiple_trees(class genesis::utils::InputStream &, class genesis::tree::TreeSet &, const std::string &) const --> void", pybind11::arg("input_stream"), pybind11::arg("tree_set"), pybind11::arg("default_name"));
		cl.def("parse_named_tree", (struct std::pair<std::string, class genesis::tree::Tree> (genesis::tree::NewickReader::*)(class genesis::utils::InputStream &) const) &genesis::tree::NewickReader::parse_named_tree, "Parse one named tree, i.e., a tree as described\n \n\n\n\nC++: genesis::tree::NewickReader::parse_named_tree(class genesis::utils::InputStream &) const --> struct std::pair<std::string, class genesis::tree::Tree>", pybind11::arg("input_stream"));
		cl.def("broker_to_tree", (class genesis::tree::Tree (genesis::tree::NewickReader::*)(const class genesis::tree::NewickBroker &) const) &genesis::tree::NewickReader::broker_to_tree, "Build a Tree from a NewickBroker.\n\n This is mainly an internal function, but can be used by other functions that have\n a NewickBroker and want to convert it into an actual Tree object.\n\nC++: genesis::tree::NewickReader::broker_to_tree(const class genesis::tree::NewickBroker &) const --> class genesis::tree::Tree", pybind11::arg("broker"));
		cl.def("broker_to_tree_destructive", (class genesis::tree::Tree (genesis::tree::NewickReader::*)(class genesis::tree::NewickBroker &) const) &genesis::tree::NewickReader::broker_to_tree_destructive, "Build a Tree from a NewickBroker.\n\n Same as broker_to_tree(), but destroys the NewickBroker while building the Tree.\n This saves memory (~50%), and is mainly intended as an internal function for reading.\n\nC++: genesis::tree::NewickReader::broker_to_tree_destructive(class genesis::tree::NewickBroker &) const --> class genesis::tree::Tree", pybind11::arg("broker"));
	}
}
