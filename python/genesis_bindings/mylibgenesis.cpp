#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>

typedef std::function< pybind11::module & (std::string const &) > ModuleGetter;

void bind_std_type_traits(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_stl_iterator_base_types(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_exception(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_bits_types_struct_FILE(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_stdexcept(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_locale_classes(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_basic_ios_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_istream_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_fstream_tcc(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_random(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_math_bitvector(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_genesis_utils_math_bitvector(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_math_bitvector_helper(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_color_functions(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_containers_range(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_containers_matrix(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_containers_matrix_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_containers_matrix_2(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_tools_char_lookup(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_core_logging(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_core_fs(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_io_input_source(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_text_string(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_text_string_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_io_base_output_target(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_io_gzip_output_target(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_color_names(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_tools_tickmarks(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_svg_helper(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_svg_attributes(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_svg_group(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_svg_color_bar(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_svg_shapes(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_svg_shapes_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_formats_nexus_writer(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_math_statistics(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_math_statistics_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_utils_math_matrix(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind____python_custom_bindings_template_instances_matrix(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_codes(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_counts(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_formats_phylip_reader(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_consensus(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_consensus_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_reference_genome(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_dict(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_functions(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_functions_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_quality(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_formats_fastq_writer(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_stats(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_labels(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_signatures(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_sequence_functions_signatures_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_taxonomy_formats_taxopath_parser(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_taxonomy_taxonomy(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_taxonomy_functions_taxonomy(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_std_genesis_taxonomy_taxopath(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_taxonomy_printers_nested(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_taxonomy_iterator_preorder(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_tree_link(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_common_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_common_tree_newick_reader(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_function_functions(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_taxonomy_functions_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_drawing_layout_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_drawing_heat_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_common_tree_functions(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_function_manipulation(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_function_operators(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_mass_tree_balances(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_mass_tree_phylo_ilr(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_common_tree_newick_writer(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_formats_newick_input_iterator(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_attribute_tree_tree(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_attribute_tree_keyed_newick_reader(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_bipartition_rf(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_iterator_path(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind____python_custom_bindings_template_instances_path(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_tree_iterator_path_set(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind____python_custom_bindings_template_instances_path_set(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_formats_jplace_reader(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_sample(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_function_functions(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_function_functions_1(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_pquery_plain(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind____python_custom_bindings_template_instances_range(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_formats_jplace_writer(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_function_nhd(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_function_epca(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_function_operators(std::function< pybind11::module &(std::string const &namespace_) > &M);
void bind_genesis_placement_simulator_distributions(std::function< pybind11::module &(std::string const &namespace_) > &M);


PYBIND11_MODULE(mylibgenesis, root_module) {
	root_module.doc() = "mylibgenesis module";

	std::map <std::string, pybind11::module> modules;
	ModuleGetter M = [&](std::string const &namespace_) -> pybind11::module & {
		auto it = modules.find(namespace_);
		if( it == modules.end() ) throw std::runtime_error("Attempt to access pybind11::module for namespace " + namespace_ + " before it was created!!!");
		return it->second;
	};

	modules[""] = root_module;

	static std::vector<std::string> const reserved_python_words {"nonlocal", "global", };

	auto mangle_namespace_name(
		[](std::string const &ns) -> std::string {
			if ( std::find(reserved_python_words.begin(), reserved_python_words.end(), ns) == reserved_python_words.end() ) return ns;
			else return ns+'_';
		}
	);

	std::vector< std::pair<std::string, std::string> > sub_modules {
		{"", "genesis"},
		{"genesis", "placement"},
		{"genesis", "sequence"},
		{"genesis", "taxonomy"},
		{"genesis", "tree"},
		{"genesis", "utils"},
		{"", "std"},
	};
	for(auto &p : sub_modules ) modules[p.first.size() ? p.first+"::"+p.second : p.second] = modules[p.first].def_submodule( mangle_namespace_name(p.second).c_str(), ("Bindings for " + p.first + "::" + p.second + " namespace").c_str() );

	//pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");

	bind_std_type_traits(M);
	bind_std_stl_iterator_base_types(M);
	bind_std_exception(M);
	bind_bits_types_struct_FILE(M);
	bind_std_stdexcept(M);
	bind_std_locale_classes(M);
	bind_std_basic_ios_tcc(M);
	bind_std_istream_tcc(M);
	bind_std_fstream_tcc(M);
	bind_std_random(M);
	bind_genesis_utils_math_bitvector(M);
	bind_std_genesis_utils_math_bitvector(M);
	bind_genesis_utils_math_bitvector_helper(M);
	bind_genesis_utils_color_functions(M);
	bind_genesis_utils_containers_range(M);
	bind_genesis_utils_containers_matrix(M);
	bind_genesis_utils_containers_matrix_1(M);
	bind_genesis_utils_containers_matrix_2(M);
	bind_genesis_utils_tools_char_lookup(M);
	bind_genesis_utils_core_logging(M);
	bind_genesis_utils_core_fs(M);
	bind_genesis_utils_io_input_source(M);
	bind_genesis_utils_text_string(M);
	bind_genesis_utils_text_string_1(M);
	bind_genesis_utils_io_base_output_target(M);
	bind_genesis_utils_io_gzip_output_target(M);
	bind_genesis_utils_color_names(M);
	bind_genesis_utils_tools_tickmarks(M);
	bind_genesis_utils_formats_svg_helper(M);
	bind_genesis_utils_formats_svg_attributes(M);
	bind_genesis_utils_formats_svg_group(M);
	bind_genesis_utils_formats_svg_color_bar(M);
	bind_genesis_utils_formats_svg_shapes(M);
	bind_genesis_utils_formats_svg_shapes_1(M);
	bind_genesis_utils_formats_nexus_writer(M);
	bind_genesis_utils_math_statistics(M);
	bind_genesis_utils_math_statistics_1(M);
	bind_genesis_utils_math_matrix(M);
	bind____python_custom_bindings_template_instances_matrix(M);
	bind_genesis_sequence_functions_codes(M);
	bind_genesis_sequence_counts(M);
	bind_genesis_sequence_formats_phylip_reader(M);
	bind_genesis_sequence_functions_consensus(M);
	bind_genesis_sequence_functions_consensus_1(M);
	bind_genesis_sequence_reference_genome(M);
	bind_genesis_sequence_functions_dict(M);
	bind_genesis_sequence_functions_functions(M);
	bind_genesis_sequence_functions_functions_1(M);
	bind_genesis_sequence_functions_quality(M);
	bind_genesis_sequence_formats_fastq_writer(M);
	bind_genesis_sequence_functions_stats(M);
	bind_genesis_sequence_functions_labels(M);
	bind_genesis_sequence_functions_signatures(M);
	bind_genesis_sequence_functions_signatures_1(M);
	bind_genesis_taxonomy_formats_taxopath_parser(M);
	bind_genesis_taxonomy_taxonomy(M);
	bind_genesis_taxonomy_functions_taxonomy(M);
	bind_std_genesis_taxonomy_taxopath(M);
	bind_genesis_taxonomy_printers_nested(M);
	bind_genesis_taxonomy_iterator_preorder(M);
	bind_genesis_tree_tree_link(M);
	bind_genesis_tree_tree(M);
	bind_genesis_tree_common_tree_tree(M);
	bind_genesis_tree_common_tree_newick_reader(M);
	bind_genesis_tree_function_functions(M);
	bind_genesis_taxonomy_functions_tree(M);
	bind_genesis_tree_drawing_layout_tree(M);
	bind_genesis_tree_drawing_heat_tree(M);
	bind_genesis_tree_common_tree_functions(M);
	bind_genesis_tree_function_manipulation(M);
	bind_genesis_tree_function_operators(M);
	bind_genesis_tree_mass_tree_balances(M);
	bind_genesis_tree_mass_tree_phylo_ilr(M);
	bind_genesis_tree_common_tree_newick_writer(M);
	bind_genesis_tree_formats_newick_input_iterator(M);
	bind_genesis_tree_attribute_tree_tree(M);
	bind_genesis_tree_attribute_tree_keyed_newick_reader(M);
	bind_genesis_tree_bipartition_rf(M);
	bind_genesis_tree_iterator_path(M);
	bind____python_custom_bindings_template_instances_path(M);
	bind_genesis_tree_iterator_path_set(M);
	bind____python_custom_bindings_template_instances_path_set(M);
	bind_genesis_placement_formats_jplace_reader(M);
	bind_genesis_placement_sample(M);
	bind_genesis_placement_function_functions(M);
	bind_genesis_placement_function_functions_1(M);
	bind_genesis_placement_pquery_plain(M);
	bind____python_custom_bindings_template_instances_range(M);
	bind_genesis_placement_formats_jplace_writer(M);
	bind_genesis_placement_function_nhd(M);
	bind_genesis_placement_function_epca(M);
	bind_genesis_placement_function_operators(M);
	bind_genesis_placement_simulator_distributions(M);

}
