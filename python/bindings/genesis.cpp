#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>

using ModuleGetter = std::function< pybind11::module&( std::string const& ) >;

void bind_genesis_util_core_info(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_core_info_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_bit_bitvector(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_bit_bitvector_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_bit_bitvector_function_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_core_options(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_core_fs(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_io_base_input_source(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_io_file_input_source(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_io_gzip(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_io_base_output_target(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_bit_bitvector_operator(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_bit_twobit_vector(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_bit_twobit_vector_iterator_deletions(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_color(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_helper(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_list_qualitative(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_list_sequential(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_name(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_char(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_string(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_string_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_string_2(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_string_3(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_normalization(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_tool_tickmarks(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_norm_boundary(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_color_norm_logarithmic(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_tool_date_time(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_core_version(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_io_function(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_math_binomial(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_math_ranking(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_math_moments(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_math_statistic(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_base64(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_convert(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_style(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_text_table(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_tool_geodesy_geo_coordinate(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_tool_hash_sha1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_tool_hash_sha256(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_tool_timer(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_container_bitpacked_vector_instances(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_util_math_compensated_sum_instances(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);

PYBIND11_MODULE( genesis, root_module )
{
    root_module.doc() = "genesis module";

    std::map<std::string, pybind11::module> modules;
    ModuleGetter M = [&]( std::string const& namespace_ ) -> pybind11::module& {
        auto it = modules.find( namespace_ );
        if( it == modules.end() )
            throw std::runtime_error(
                "Attempt to access pybind11::module for namespace " + namespace_ +
                " before it was created!!!"
            );
        return it->second;
    };

    modules[""] = root_module;

    static std::vector<std::string> const reserved_python_words{
        "nonlocal",
        "global",
    };

    auto mangle_namespace_name( []( std::string const& ns ) -> std::string {
        if( std::find( reserved_python_words.begin(), reserved_python_words.end(), ns ) ==
            reserved_python_words.end() )
            return ns;
        return ns + '_';
    } );

    std::vector< std::pair<std::string, std::string> > sub_modules{
        { "", "genesis" },
        { "genesis", "util" },
        { "genesis::util", "bit" },
        { "genesis::util", "color" },
        { "genesis::util", "container" },
        { "genesis::util", "core" },
        { "genesis::util", "io" },
        { "genesis::util", "math" },
        { "genesis::util", "text" },
    };
    for( auto& p : sub_modules )
        modules[p.first.empty() ? p.second : p.first + "::" + p.second] =
            modules[p.first].def_submodule(
                mangle_namespace_name( p.second ).c_str(),
                ( "Bindings for " + p.first + "::" + p.second + " namespace" ).c_str()
            );

    // pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");

    bind_genesis_util_core_info( M );
    bind_genesis_util_core_info_1( M );
    bind_genesis_util_bit_bitvector( M );
    bind_genesis_util_bit_bitvector_function( M );
    bind_genesis_util_bit_bitvector_function_1( M );
    bind_genesis_util_core_options( M );
    bind_genesis_util_core_fs( M );
    bind_genesis_util_io_base_input_source( M );
    bind_genesis_util_io_file_input_source( M );
    bind_genesis_util_io_gzip( M );
    bind_genesis_util_io_base_output_target( M );
    bind_genesis_util_bit_bitvector_operator( M );
    bind_genesis_util_bit_twobit_vector( M );
    bind_genesis_util_bit_twobit_vector_iterator_deletions( M );
    bind_genesis_util_color_color( M );
    bind_genesis_util_color_function( M );
    bind_genesis_util_color_helper( M );
    bind_genesis_util_color_list_qualitative( M );
    bind_genesis_util_color_list_sequential( M );
    bind_genesis_util_color_name( M );
    bind_genesis_util_text_char( M );
    bind_genesis_util_text_string( M );
    bind_genesis_util_text_string_1( M );
    bind_genesis_util_text_string_2( M );
    bind_genesis_util_text_string_3( M );
    bind_genesis_util_color_normalization( M );
    bind_genesis_util_tool_tickmarks( M );
    bind_genesis_util_color_norm_boundary( M );
    bind_genesis_util_color_norm_logarithmic( M );
    bind_genesis_util_tool_date_time( M );
    bind_genesis_util_core_version( M );
    bind_genesis_util_io_function( M );
    bind_genesis_util_math_binomial( M );
    bind_genesis_util_math_ranking( M );
    bind_genesis_util_math_moments( M );
    bind_genesis_util_math_statistic( M );
    bind_genesis_util_text_base64( M );
    bind_genesis_util_text_convert( M );
    bind_genesis_util_text_style( M );
    bind_genesis_util_text_table( M );
    bind_genesis_util_tool_geodesy_geo_coordinate( M );
    bind_genesis_util_tool_hash_sha1( M );
    bind_genesis_util_tool_hash_sha256( M );
    bind_genesis_util_tool_timer( M );
    bind_genesis_util_container_bitpacked_vector_instances( M );
    bind_genesis_util_math_compensated_sum_instances( M );
}
