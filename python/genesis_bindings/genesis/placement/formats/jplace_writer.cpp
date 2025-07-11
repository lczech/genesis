#include <genesis/placement/formats/jplace_writer.hpp>
#include <genesis/placement/function/cog.hpp>
#include <genesis/placement/function/emd.hpp>
#include <genesis/placement/function/measures.hpp>
#include <genesis/placement/placement_tree.hpp>
#include <genesis/placement/pquery.hpp>
#include <genesis/placement/pquery/name.hpp>
#include <genesis/placement/pquery/placement.hpp>
#include <genesis/placement/sample.hpp>
#include <genesis/placement/sample_set.hpp>
#include <genesis/tree/common_tree/tree.hpp>
#include <genesis/tree/formats/newick/simple_tree.hpp>
#include <genesis/tree/tree.hpp>
#include <genesis/tree/tree/edge.hpp>
#include <genesis/tree/tree/edge_data.hpp>
#include <genesis/tree/tree/link.hpp>
#include <genesis/tree/tree/node.hpp>
#include <genesis/utils/containers/deref_iterator.hpp>
#include <genesis/utils/containers/matrix.hpp>
#include <genesis/utils/containers/range.hpp>
#include <genesis/utils/io/base_output_target.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_placement_formats_jplace_writer(std::function< pybind11::module &(std::string const &namespace_) > &M)
{
	{ // genesis::placement::JplaceWriter file:genesis/placement/formats/jplace_writer.hpp line:80
		pybind11::class_<genesis::placement::JplaceWriter, std::shared_ptr<genesis::placement::JplaceWriter>> cl(M("genesis::placement"), "JplaceWriter", "Write Jplace data.\n\n This class provides facilities for writing Jplace data to different output targets.\n\n Exemplary usage:\n\n     std::string outfile = \"path/to/file.jplace\";\n     Sample sample;\n\n     JplaceWriter().write( sample, utils::to_file( outfile ));\n\n Furthermore, using to_document(), the Sample can also be transformed into our internal\n Json representaion in form of a utils::JsonDocument that adheres to the Jplace standard.\n This is probably rarely needed, but might be useful nonetheless.\n\n See JplaceReader for more information on the data format. See Sample for more information\n on the data structure.");
		cl.def( pybind11::init( [](){ return new genesis::placement::JplaceWriter(); } ) );
		cl.def( pybind11::init( [](genesis::placement::JplaceWriter const &o){ return new genesis::placement::JplaceWriter(o); } ) );
		cl.def("assign", (class genesis::placement::JplaceWriter & (genesis::placement::JplaceWriter::*)(const class genesis::placement::JplaceWriter &)) &genesis::placement::JplaceWriter::operator=, "C++: genesis::placement::JplaceWriter::operator=(const class genesis::placement::JplaceWriter &) --> class genesis::placement::JplaceWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg(""));
		cl.def("write", (void (genesis::placement::JplaceWriter::*)(const class genesis::placement::Sample &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const) &genesis::placement::JplaceWriter::write, "Write a Sample to an output target, using the Jplace format.\n\n See the output target convenience functions utils::to_file(), utils::to_stream(), and\n utils::to_string() for examples of how to obtain a suitable output target.\n\nC++: genesis::placement::JplaceWriter::write(const class genesis::placement::Sample &, class std::shared_ptr<class genesis::utils::BaseOutputTarget>) const --> void", pybind11::arg("sample"), pybind11::arg("target"));
		cl.def("branch_length_precision", (unsigned long (genesis::placement::JplaceWriter::*)() const) &genesis::placement::JplaceWriter::branch_length_precision, "Get the precision used for printing the `branch_length` floating point numbers\n of the reference Tree of the Sample.\n\nC++: genesis::placement::JplaceWriter::branch_length_precision() const --> unsigned long");
		cl.def("branch_length_precision", (class genesis::placement::JplaceWriter & (genesis::placement::JplaceWriter::*)(unsigned long)) &genesis::placement::JplaceWriter::branch_length_precision, "Set the precision used for printing the `branch_length` floating point numbers\n of the reference Tree of the Sample.\n\nC++: genesis::placement::JplaceWriter::branch_length_precision(unsigned long) --> class genesis::placement::JplaceWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("value"));
		cl.def("program", (const std::string & (genesis::placement::JplaceWriter::*)() const) &genesis::placement::JplaceWriter::program, "Get the currently set \"program\" string to be written to the jplace file.\n\n This is jplace meta-data that can be set to later identify settings used when creating\n the jplace file.\n\nC++: genesis::placement::JplaceWriter::program() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("program", (class genesis::placement::JplaceWriter & (genesis::placement::JplaceWriter::*)(const std::string &)) &genesis::placement::JplaceWriter::program, "Set the \"program\" string to be written to the jplace file.\n\n  program()\n\nC++: genesis::placement::JplaceWriter::program(const std::string &) --> class genesis::placement::JplaceWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("program"));
		cl.def("invocation", (const std::string & (genesis::placement::JplaceWriter::*)() const) &genesis::placement::JplaceWriter::invocation, "Get the currently set \"invocation\" string to be written to the jplace file.\n\n This is jplace meta-data that can be set to later identify settings used when creating\n the jplace file.\n\nC++: genesis::placement::JplaceWriter::invocation() const --> const std::string &", pybind11::return_value_policy::reference_internal);
		cl.def("invocation", (class genesis::placement::JplaceWriter & (genesis::placement::JplaceWriter::*)(const std::string &)) &genesis::placement::JplaceWriter::invocation, "Set the \"invocation\" string to be written to the jplace file.\n\n  invocation()\n\nC++: genesis::placement::JplaceWriter::invocation(const std::string &) --> class genesis::placement::JplaceWriter &", pybind11::return_value_policy::reference_internal, pybind11::arg("invocation"));
	}
	// genesis::placement::center_of_gravity(const class genesis::placement::Sample &, const bool) file:genesis/placement/function/cog.hpp line:72
	M("genesis::placement").def("center_of_gravity", [](const class genesis::placement::Sample & a0) -> std::pair<const class genesis::tree::TreeEdge *, double> { return genesis::placement::center_of_gravity(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("center_of_gravity", (struct std::pair<const class genesis::tree::TreeEdge *, double> (*)(const class genesis::placement::Sample &, const bool)) &genesis::placement::center_of_gravity, "Calculate the Center of Gravity of the placements on a tree.\n\n The center of gravity is the point on the tree where all masses of the placements on the one\n side of it times their distance from the point are equal to this sum on the other side of the\n point. In the following example, the hat `^` marks this point on a line with two placements:\n One has mass 1 and distance 3 from the central point, and one as mass 3 and distance 1,\n so that the product of their mass and distance to the point is the same:\n\n                   3\n                   |\n     1             |\n     |_____________|\n               ^\n\n It is thus like calculating masses and torques on a lever in order to find their physical\n center of mass/gravity.\n\n This calculation is done for the whole tree, with the masses calculated from the\n `like_weight_ratio` and distances in terms of the `branch_length` of the edges and the\n `proximal_length` and (if specificed in the method parameter) the `pendant_length` of the\n placements.\n\nC++: genesis::placement::center_of_gravity(const class genesis::placement::Sample &, const bool) --> struct std::pair<const class genesis::tree::TreeEdge *, double>", pybind11::arg("smp"), pybind11::arg("with_pendant_length"));

	// genesis::placement::center_of_gravity_variance(const class genesis::placement::Sample &, const bool) file:genesis/placement/function/cog.hpp line:87
	M("genesis::placement").def("center_of_gravity_variance", [](const class genesis::placement::Sample & a0) -> double { return genesis::placement::center_of_gravity_variance(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("center_of_gravity_variance", (double (*)(const class genesis::placement::Sample &, const bool)) &genesis::placement::center_of_gravity_variance, "Calculate the variance of the PqueryPlacement%s of a Sample around its Center of Gravity.\n\n The caluclation of the variance is as follows:\n\n \n, where\n the weights \n\n are the `like_weight_ratio`s of the placements.\n\n See center_of_gravity() for more.\n\nC++: genesis::placement::center_of_gravity_variance(const class genesis::placement::Sample &, const bool) --> double", pybind11::arg("smp"), pybind11::arg("with_pendant_length"));

	// genesis::placement::center_of_gravity_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const bool) file:genesis/placement/function/cog.hpp line:98
	M("genesis::placement").def("center_of_gravity_distance", [](const class genesis::placement::Sample & a0, const class genesis::placement::Sample & a1) -> double { return genesis::placement::center_of_gravity_distance(a0, a1); }, "", pybind11::arg("smp_a"), pybind11::arg("smp_b"));
	M("genesis::placement").def("center_of_gravity_distance", (double (*)(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const bool)) &genesis::placement::center_of_gravity_distance, "Calculate the distance between the two Centers of Gravity of two Sample%s.\n\n The distance is measured in branch length units; for the Center of Gravity, see\n center_of_gravity().\n\nC++: genesis::placement::center_of_gravity_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const bool) --> double", pybind11::arg("smp_a"), pybind11::arg("smp_b"), pybind11::arg("with_pendant_length"));

	// genesis::placement::earth_movers_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const double, const bool) file:genesis/placement/function/emd.hpp line:97
	M("genesis::placement").def("earth_movers_distance", [](const class genesis::placement::Sample & a0, const class genesis::placement::Sample & a1) -> double { return genesis::placement::earth_movers_distance(a0, a1); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"));
	M("genesis::placement").def("earth_movers_distance", [](const class genesis::placement::Sample & a0, const class genesis::placement::Sample & a1, const double & a2) -> double { return genesis::placement::earth_movers_distance(a0, a1, a2); }, "", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("p"));
	M("genesis::placement").def("earth_movers_distance", (double (*)(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const double, const bool)) &genesis::placement::earth_movers_distance, "Calculate the earth mover's distance between two Sample%s.\n\n This function interprets the \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::earth_movers_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, const double, const bool) --> double", pybind11::arg("lhs"), pybind11::arg("rhs"), pybind11::arg("p"), pybind11::arg("with_pendant_length"));

	// genesis::placement::earth_movers_distance(const class genesis::placement::SampleSet &, const double, const bool) file:genesis/placement/function/emd.hpp line:116
	M("genesis::placement").def("earth_movers_distance", [](const class genesis::placement::SampleSet & a0) -> genesis::utils::Matrix<double> { return genesis::placement::earth_movers_distance(a0); }, "", pybind11::arg("sample_set"));
	M("genesis::placement").def("earth_movers_distance", [](const class genesis::placement::SampleSet & a0, const double & a1) -> genesis::utils::Matrix<double> { return genesis::placement::earth_movers_distance(a0, a1); }, "", pybind11::arg("sample_set"), pybind11::arg("p"));
	M("genesis::placement").def("earth_movers_distance", (class genesis::utils::Matrix<double> (*)(const class genesis::placement::SampleSet &, const double, const bool)) &genesis::placement::earth_movers_distance, "Calculate the pairwise Earth Movers Distance for all Sample%s in a SampleSet.\n\n The result is a pairwise distance \n\n\n\n\n\n\n\n\n\nC++: genesis::placement::earth_movers_distance(const class genesis::placement::SampleSet &, const double, const bool) --> class genesis::utils::Matrix<double>", pybind11::arg("sample_set"), pybind11::arg("p"), pybind11::arg("with_pendant_length"));

	// genesis::placement::edpl(const class genesis::placement::Pquery &, const class genesis::utils::Matrix<double> &) file:genesis/placement/function/measures.hpp line:85
	M("genesis::placement").def("edpl", (double (*)(const class genesis::placement::Pquery &, const class genesis::utils::Matrix<double> &)) &genesis::placement::edpl, "Calculate the EDPL uncertainty values for a Pquery.\n\n This is the function that does the actual computation. It is used by the other `edpl`\n functions, which first calculate the  matrix before calling this function.\n It is useful to separate these steps in order to avoid duplicate work when calculating the\n edpl for many \n\n\n\n\n\n\n\nC++: genesis::placement::edpl(const class genesis::placement::Pquery &, const class genesis::utils::Matrix<double> &) --> double", pybind11::arg("pquery"), pybind11::arg("node_distances"));

	// genesis::placement::edpl(const class genesis::placement::Sample &, const class genesis::utils::Matrix<double> &) file:genesis/placement/function/measures.hpp line:95
	M("genesis::placement").def("edpl", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::placement::Sample &, const class genesis::utils::Matrix<double> &)) &genesis::placement::edpl, "Calculate the \n\n\n\n\n\n\n\nC++: genesis::placement::edpl(const class genesis::placement::Sample &, const class genesis::utils::Matrix<double> &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("sample"), pybind11::arg("node_distances"));

	// genesis::placement::edpl(const class genesis::placement::Sample &, const class genesis::placement::Pquery &) file:genesis/placement/function/measures.hpp line:107
	M("genesis::placement").def("edpl", (double (*)(const class genesis::placement::Sample &, const class genesis::placement::Pquery &)) &genesis::placement::edpl, "Calculate the EDPL uncertainty values for a Pquery.\n\n See http://matsen.github.io/pplacer/generated_rst/guppy_edpl.html for more information.\n\n This function expects a Pquery and the Sample it belongs to. This is necessary in order to\n get the Tree of the Sample and calculate distances between its Nodes.\n\n \n edpl( Sample const& ) for details.\n\nC++: genesis::placement::edpl(const class genesis::placement::Sample &, const class genesis::placement::Pquery &) --> double", pybind11::arg("sample"), pybind11::arg("pquery"));

	// genesis::placement::edpl(const class genesis::placement::Sample &) file:genesis/placement/function/measures.hpp line:138
	M("genesis::placement").def("edpl", (class std::vector<double, class std::allocator<double> > (*)(const class genesis::placement::Sample &)) &genesis::placement::edpl, "Calculate the expected distance between placement locations (EDPL)\n for all \n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::edpl(const class genesis::placement::Sample &) --> class std::vector<double, class std::allocator<double> >", pybind11::arg("sample"));

	// genesis::placement::pairwise_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, bool) file:genesis/placement/function/measures.hpp line:158
	M("genesis::placement").def("pairwise_distance", [](const class genesis::placement::Sample & a0, const class genesis::placement::Sample & a1) -> double { return genesis::placement::pairwise_distance(a0, a1); }, "", pybind11::arg("smp_a"), pybind11::arg("smp_b"));
	M("genesis::placement").def("pairwise_distance", (double (*)(const class genesis::placement::Sample &, const class genesis::placement::Sample &, bool)) &genesis::placement::pairwise_distance, "Calculate the normalized pairwise distance between all placements of the two Samples.\n\n This method calculates the distance between two Sample%s as the normalized sum of the distances\n between all pairs of \n\n\n\n\n\n\n\n\n\n\n\nC++: genesis::placement::pairwise_distance(const class genesis::placement::Sample &, const class genesis::placement::Sample &, bool) --> double", pybind11::arg("smp_a"), pybind11::arg("smp_b"), pybind11::arg("with_pendant_length"));

	// genesis::placement::variance(const class genesis::placement::Sample &, bool) file:genesis/placement/function/measures.hpp line:198
	M("genesis::placement").def("variance", [](const class genesis::placement::Sample & a0) -> double { return genesis::placement::variance(a0); }, "", pybind11::arg("smp"));
	M("genesis::placement").def("variance", (double (*)(const class genesis::placement::Sample &, bool)) &genesis::placement::variance, "Calculate the variance of the placements on a tree.\n\n The variance is a measure of how far a set of items is spread out in its space\n (http://en.wikipedia.org/wiki/variance). In many cases, it can be measured using the mean of the\n items. However, when considering placements on a tree, this does not truly measure how far they\n are from each other. Thus, this algorithm applies a different method of calculating the variance\n in terms of squared deviations of all items from each other:\n \n\n,\n where \n\n denotes the distance between two placements.\n\n According to the formula above, each pair of placements is evaluated twice, and subsequently\n their distance need to be halfed when being added to the sum of distanaces. Instead of that,\n we calculate the distance for each pair only once, thus are able skip half the calculations, and\n of course skip the division by two.\n\n Furthermore, the normalizing factor \n of the variance usually contains the\n number of elements being processed. However, as the placements are weighted by their\n `like_weight_ratio`, we instead calculate `n` as the sum of the `like_weight_ratio` of all\n placements. In case that for each pquery the ratios of all its placements sum up to 1.0, this\n number will be equal to the number of pqueries (and thus be equal to the usual case of using the\n number of elements). However, as this is not required (placements with small ratio can be\n dropped, so that their sum per pquery is less than 1.0), we cannout simply use the count.\n\nC++: genesis::placement::variance(const class genesis::placement::Sample &, bool) --> double", pybind11::arg("smp"), pybind11::arg("with_pendant_length"));

}
