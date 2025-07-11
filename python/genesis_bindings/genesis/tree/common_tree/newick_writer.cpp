#include <genesis/placement/placement_tree.hpp>
#include <genesis/tree/common_tree/newick_writer.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/color_writer_plugin.hpp>
#include <genesis/tree/formats/newick/broker.hpp>
#include <genesis/tree/formats/newick/color_writer_plugin.hpp>
#include <genesis/tree/formats/newick/element.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/formats/newick/writer.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/tree/tree_set.hpp>
#include <genesis/utils/color/color.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_output_target.hpp>
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

void bind_genesis_tree_common_tree_newick_writer(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::tree::CommonTreeNewickWriter file:genesis/tree/common_tree/newick_writer.hpp line:367
		pybind11::class_<genesis::tree::CommonTreeNewickWriter, std::shared_ptr<genesis::tree::CommonTreeNewickWriter>, genesis::tree::NewickWriter, genesis::tree::CommonTreeNewickWriterPlugin> cl(M("genesis::tree"), "CommonTreeNewickWriter", "Write default Newick trees, i.e., trees with names and branch lengths.\n\n This class is a convenience wrapper that combines a NewickWriter with a\n CommonTreeNewickWriterPlugin. It is intended to be used for standard use cases, and writes a\n Newick tree from a Tree with CommonNodeData and CommonEdgeData at its nodes and edges.\n\n It is also possible to register additional plugins on top of this class.\n\n Behind the curtain, this class derives from both NewickWriter and CommonTreeNewickWriterPlugin.\n This is a bit ugly, but we use it for simplicity. This allows to use an instance as if it was\n a writer (i.e., call `to_...` functions), but also change the plugin settings in a natural\n way.");
		cl.def( pybind11::init( [](){ return new genesis::tree::CommonTreeNewickWriter(); } ) );
		cl.def( pybind11::init( [](genesis::tree::CommonTreeNewickWriter const &o){ return new genesis::tree::CommonTreeNewickWriter(o); } ) );
		cl.def("assign", (class genesis::tree::CommonTreeNewickWriter & (genesis::tree::CommonTreeNewickWriter::*)(const class genesis::tree::CommonTreeNewickWriter &)) &genesis::tree::CommonTreeNewickWriter::operator=, "C++: genesis::tree::CommonTreeNewickWriter::operator=(const class genesis::tree::CommonTreeNewickWriter &) --> class genesis::tree::CommonTreeNewickWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::tree::ColorWriterPlugin file:genesis/tree/formats/color_writer_plugin.hpp line:81
		pybind11::class_<genesis::tree::ColorWriterPlugin, std::shared_ptr<genesis::tree::ColorWriterPlugin>> cl(M("genesis::tree"), "ColorWriterPlugin", "Base class for creating plugin classes that allow coloring of Tree edges.\n\n The effect of this class on the Tree output is that (if enabled) a color tag will be added to\n each edge.\n\n There are two ways this plugin can be used:\n\n   1. Use `edge_color_vector()` to set a vector of colors for each edge before calling the\n      actual writing function. This can be done by client code that needs a particular coloring\n      of the edges, but can also be used by superclasses to set all edge colors at once.\n   2. Use `set_color()` in the superclass to set the color of individual edges. This is helpful\n      within the `edge_to_element()` function of the superclass. This function is however not\n      implemented in this base class, as it needs knowledge of the actual type of Tree output to\n      work. Thus, this is a function that the plugin classes should provide.\n\n If neither of this is done, no color tags will be written. Color tags can also be deactivated\n by client code using the `enable_color()` option.\n\n Furthermore, with the ignored_color option, by default, all edged that are colored black\n (`Color(0, 0, 0)`) are ignored in the output, i.e., they produce no color tag. See ignored_color\n to set the value to a different color.\n\n For the root node, there is no color tag written, for those reasons:\n\n   * In unrooted trees, the color tags for the nodes next to the root will cover all edges\n     anyway. In other words, there is no \"root edge\" that could get a color.\n   * In rooted trees, the root node has two edges. Those two combined edges are usually\n     interpreted as the one edge where the root is located. So again, there are actual edges\n     that can be colored - no need for an extra root color.\n     Caveat: It is possible to give different colors to those edges. What that means is up to\n     the user.\n\n This class is intended to be used as base for plugin classes that do actual tree output.");
		cl.def( pybind11::init( [](genesis::tree::ColorWriterPlugin const &o){ return new genesis::tree::ColorWriterPlugin(o); } ) );
		cl.def( pybind11::init( [](){ return new genesis::tree::ColorWriterPlugin(); } ) );
		cl.def("edge_colors", (void (genesis::tree::ColorWriterPlugin::*)(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &)) &genesis::tree::ColorWriterPlugin::edge_colors, "Set the edge colors that shall be written to the output.\n\n If this function is called with a vector of size > 0, the edges in the output will be colored\n according to the values given as a parameter. The vector then needs to contain as many\n elements as the tree has edges. The elements need to be indexed using the edge.index() value.\n\n If this function is called with an empty vector, the color printing is reset to not\n print the edge colors that might have been set before.\n\nC++: genesis::tree::ColorWriterPlugin::edge_colors(const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &) --> void", pybind11::arg("color_vector"));
		cl.def("edge_colors", (const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > & (genesis::tree::ColorWriterPlugin::*)() const) &genesis::tree::ColorWriterPlugin::edge_colors, "Return the edge colors that are currently set.\n\nC++: genesis::tree::ColorWriterPlugin::edge_colors() const --> const class std::vector<class genesis::utils::Color, class std::allocator<class genesis::utils::Color> > &", pybind11::return_value_policy::reference_internal);
		cl.def("enable_color", (void (genesis::tree::ColorWriterPlugin::*)(bool)) &genesis::tree::ColorWriterPlugin::enable_color, "Set whether colors tags are written to the output.\n\nC++: genesis::tree::ColorWriterPlugin::enable_color(bool) --> void", pybind11::arg("value"));
		cl.def("enable_color", (bool (genesis::tree::ColorWriterPlugin::*)() const) &genesis::tree::ColorWriterPlugin::enable_color, "Returns whether colors tags are written to the output.\n\nC++: genesis::tree::ColorWriterPlugin::enable_color() const --> bool");
		cl.def("ignored_color", (void (genesis::tree::ColorWriterPlugin::*)(class genesis::utils::Color)) &genesis::tree::ColorWriterPlugin::ignored_color, "Set a color that is used as marker for partially disabling the output of color tags.\n\n All edges that have set a color equal to the ignored color will produce no color tag output.\n This is thus something like [magic pink](https://en.wikipedia.org/wiki/Magic_Pink), where\n all parts of an image are rendered transparent when originially colored in pink.\n\n By default, we do not use an ignored color. If this function is called (which sets a color),\n also use_ignored_color() is set to `true`, so that the color is actually used.\n\nC++: genesis::tree::ColorWriterPlugin::ignored_color(class genesis::utils::Color) --> void", pybind11::arg("value"));
		cl.def("ignored_color", (class genesis::utils::Color (genesis::tree::ColorWriterPlugin::*)() const) &genesis::tree::ColorWriterPlugin::ignored_color, "Return the currently set ignored color. See the setter for more information.\n\nC++: genesis::tree::ColorWriterPlugin::ignored_color() const --> class genesis::utils::Color");
		cl.def("use_ignored_color", (void (genesis::tree::ColorWriterPlugin::*)(bool)) &genesis::tree::ColorWriterPlugin::use_ignored_color, "Set whether to use the ignored_color().\n\n If this option is set to `true`, the color that is set via ignored_color() is not written\n as a color attribute to the output format.\n\nC++: genesis::tree::ColorWriterPlugin::use_ignored_color(bool) --> void", pybind11::arg("value"));
		cl.def("use_ignored_color", (bool (genesis::tree::ColorWriterPlugin::*)() const) &genesis::tree::ColorWriterPlugin::use_ignored_color, "Return whether currently an ignored color is used.\n\nC++: genesis::tree::ColorWriterPlugin::use_ignored_color() const --> bool");
		cl.def("assign", (class genesis::tree::ColorWriterPlugin & (genesis::tree::ColorWriterPlugin::*)(const class genesis::tree::ColorWriterPlugin &)) &genesis::tree::ColorWriterPlugin::operator=, "C++: genesis::tree::ColorWriterPlugin::operator=(const class genesis::tree::ColorWriterPlugin &) --> class genesis::tree::ColorWriterPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
	}
	{ // genesis::tree::NewickColorWriterPlugin file:genesis/tree/formats/newick/color_writer_plugin.hpp line:58
		pybind11::class_<genesis::tree::NewickColorWriterPlugin, std::shared_ptr<genesis::tree::NewickColorWriterPlugin>, genesis::tree::ColorWriterPlugin> cl(M("genesis::tree"), "NewickColorWriterPlugin", "Plugin class for Newick output that allows coloring of edges.\n\n The effect of this class on the Newick output is that (if enabled) a color tag comment will be\n added to each Newick element like this: `[&!color=#%c0ffee]`.\n\n For more information, see ColorWriterPlugin class.");
		cl.def( pybind11::init( [](){ return new genesis::tree::NewickColorWriterPlugin(); } ) );
		cl.def( pybind11::init( [](genesis::tree::NewickColorWriterPlugin const &o){ return new genesis::tree::NewickColorWriterPlugin(o); } ) );
		cl.def("assign", (class genesis::tree::NewickColorWriterPlugin & (genesis::tree::NewickColorWriterPlugin::*)(const class genesis::tree::NewickColorWriterPlugin &)) &genesis::tree::NewickColorWriterPlugin::operator=, "C++: genesis::tree::NewickColorWriterPlugin::operator=(const class genesis::tree::NewickColorWriterPlugin &) --> class genesis::tree::NewickColorWriterPlugin &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("color_tag_prefix", (void (genesis::tree::NewickColorWriterPlugin::*)(const std::string &)) &genesis::tree::NewickColorWriterPlugin::color_tag_prefix, "Set the prefix string that is used within the Newick comment before the actual\n color value.\n\n By default, this string is set to `&!color=`, which is for example used by FigTree to mark\n color in trees. This will result in a Newick comment like `[&!color=#%c0ffee]`.\n\nC++: genesis::tree::NewickColorWriterPlugin::color_tag_prefix(const std::string &) --> void", pybind11::arg("prefix"));
		cl.def("color_tag_prefix", (std::string (genesis::tree::NewickColorWriterPlugin::*)() const) &genesis::tree::NewickColorWriterPlugin::color_tag_prefix, "Get the currently set prefix string. See the setter for more information.\n\nC++: genesis::tree::NewickColorWriterPlugin::color_tag_prefix() const --> std::string");
		cl.def("color_tag_suffix", (void (genesis::tree::NewickColorWriterPlugin::*)(const std::string &)) &genesis::tree::NewickColorWriterPlugin::color_tag_suffix, "Set the suffix string that is used within the Newick comment after the actual\n color value.\n\n By default, this string is empty. See the setter color_tag_prefix() for more information.\n\nC++: genesis::tree::NewickColorWriterPlugin::color_tag_suffix(const std::string &) --> void", pybind11::arg("suffix"));
		cl.def("color_tag_suffix", (std::string (genesis::tree::NewickColorWriterPlugin::*)() const) &genesis::tree::NewickColorWriterPlugin::color_tag_suffix, "Get the currently set suffix string. See the setter for more information.\n\nC++: genesis::tree::NewickColorWriterPlugin::color_tag_suffix() const --> std::string");
		cl.def("prepare_writing", (void (genesis::tree::NewickColorWriterPlugin::*)(const class genesis::tree::Tree &, class genesis::tree::NewickBroker &) const) &genesis::tree::NewickColorWriterPlugin::prepare_writing, "C++: genesis::tree::NewickColorWriterPlugin::prepare_writing(const class genesis::tree::Tree &, class genesis::tree::NewickBroker &) const --> void", pybind11::arg("tree"), pybind11::arg("broker"));
		cl.def("edge_to_element", (void (genesis::tree::NewickColorWriterPlugin::*)(const class genesis::tree::TreeEdge &, struct genesis::tree::NewickBrokerElement &) const) &genesis::tree::NewickColorWriterPlugin::edge_to_element, "C++: genesis::tree::NewickColorWriterPlugin::edge_to_element(const class genesis::tree::TreeEdge &, struct genesis::tree::NewickBrokerElement &) const --> void", pybind11::arg("edge"), pybind11::arg("element"));
		cl.def("register_with", (void (genesis::tree::NewickColorWriterPlugin::*)(class genesis::tree::NewickWriter &) const) &genesis::tree::NewickColorWriterPlugin::register_with, "C++: genesis::tree::NewickColorWriterPlugin::register_with(class genesis::tree::NewickWriter &) const --> void", pybind11::arg("writer"));
	}
	{ // genesis::tree::TreeSet file:genesis/tree/tree_set.hpp line:48
		pybind11::class_<genesis::tree::TreeSet, std::shared_ptr<genesis::tree::TreeSet>> cl(M("genesis::tree"), "TreeSet", "");
		cl.def( pybind11::init( [](){ return new genesis::tree::TreeSet(); } ) );
		cl.def( pybind11::init( [](genesis::tree::TreeSet const &o){ return new genesis::tree::TreeSet(o); } ) );
		cl.def("assign", (class genesis::tree::TreeSet & (genesis::tree::TreeSet::*)(const class genesis::tree::TreeSet &)) &genesis::tree::TreeSet::operator=, "C++: genesis::tree::TreeSet::operator=(const class genesis::tree::TreeSet &) --> class genesis::tree::TreeSet &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("swap", (void (genesis::tree::TreeSet::*)(class genesis::tree::TreeSet &)) &genesis::tree::TreeSet::swap, "C++: genesis::tree::TreeSet::swap(class genesis::tree::TreeSet &) --> void", pybind11::arg("other"));
		cl.def("add", [](genesis::tree::TreeSet &o, const class genesis::tree::Tree & a0) -> void { return o.add(a0); }, "", pybind11::arg("tree"));
		cl.def("add", (void (genesis::tree::TreeSet::*)(const class genesis::tree::Tree &, const std::string &)) &genesis::tree::TreeSet::add, "Add a Tree with a name to the TreeSet.\n\n The Tree is copied.\n\nC++: genesis::tree::TreeSet::add(const class genesis::tree::Tree &, const std::string &) --> void", pybind11::arg("tree"), pybind11::arg("name"));
		cl.def("remove_at", (void (genesis::tree::TreeSet::*)(unsigned long)) &genesis::tree::TreeSet::remove_at, "Remove the Tree at a certain index position.\n\n As this function moves Tree%s in the container around, all iterators and pointers to\n the elements of this TreeSet are considered to be invalidated.\n\nC++: genesis::tree::TreeSet::remove_at(unsigned long) --> void", pybind11::arg("index"));
		cl.def("clear", (void (genesis::tree::TreeSet::*)()) &genesis::tree::TreeSet::clear, "Clear the TreeSet and destroy all contained Trees.\n\nC++: genesis::tree::TreeSet::clear() --> void");
		cl.def("name_at", (const std::string & (genesis::tree::TreeSet::*)(unsigned long) const) &genesis::tree::TreeSet::name_at, "C++: genesis::tree::TreeSet::name_at(unsigned long) const --> const std::string &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("names", (const class std::vector<std::string, class std::allocator<std::string > > & (genesis::tree::TreeSet::*)() const) &genesis::tree::TreeSet::names, "C++: genesis::tree::TreeSet::names() const --> const class std::vector<std::string, class std::allocator<std::string > > &", pybind11::return_value_policy::reference_internal);
		cl.def("begin", (class __gnu_cxx::__normal_iterator<class genesis::tree::Tree *, class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > > (genesis::tree::TreeSet::*)()) &genesis::tree::TreeSet::begin, "C++: genesis::tree::TreeSet::begin() --> class __gnu_cxx::__normal_iterator<class genesis::tree::Tree *, class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > >");
		cl.def("end", (class __gnu_cxx::__normal_iterator<class genesis::tree::Tree *, class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > > (genesis::tree::TreeSet::*)()) &genesis::tree::TreeSet::end, "C++: genesis::tree::TreeSet::end() --> class __gnu_cxx::__normal_iterator<class genesis::tree::Tree *, class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > >");
		cl.def("at", (class genesis::tree::Tree & (genesis::tree::TreeSet::*)(unsigned long)) &genesis::tree::TreeSet::at, "C++: genesis::tree::TreeSet::at(unsigned long) --> class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("__getitem__", (class genesis::tree::Tree & (genesis::tree::TreeSet::*)(const unsigned long)) &genesis::tree::TreeSet::operator[], "C++: genesis::tree::TreeSet::operator[](const unsigned long) --> class genesis::tree::Tree &", pybind11::return_value_policy::reference_internal, pybind11::arg("index"));
		cl.def("trees", (const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > & (genesis::tree::TreeSet::*)() const) &genesis::tree::TreeSet::trees, "C++: genesis::tree::TreeSet::trees() const --> const class std::vector<class genesis::tree::Tree, class std::allocator<class genesis::tree::Tree> > &", pybind11::return_value_policy::reference_internal);
		cl.def("empty", (bool (genesis::tree::TreeSet::*)() const) &genesis::tree::TreeSet::empty, "Return whether the TreeSet is empty.\n\nC++: genesis::tree::TreeSet::empty() const --> bool");
		cl.def("size", (unsigned long (genesis::tree::TreeSet::*)() const) &genesis::tree::TreeSet::size, "Return the size of the TreeSet, i.e., the number of stored Tree%s.\n\nC++: genesis::tree::TreeSet::size() const --> unsigned long");
	}
}
