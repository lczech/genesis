#include <algorithm>
#include <functional>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>

using ModuleGetter = std::function< pybind11::module&( std::string const& ) >;

void bind_genesis_utils_core_fs(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_core_fs_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_core_info(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_core_info_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_core_options(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_core_version(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_io_base_input_source(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_io_file_input_source(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_io_gzip_stream(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_char(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_binomial(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_compensated_sum(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_correlation(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_random(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_statistics(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_statistics_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_base64(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_string(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_string_1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_string_2(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_string_3(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_convert(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_style(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_text_table(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_tools_date_time(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_tools_hash_sha1(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_tools_hash_sha256(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_tools_tickmarks(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
);
void bind_genesis_utils_math_compensated_sum_bind(
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
        { "genesis", "utils" },
        { "genesis::utils", "core" },
        { "genesis::utils", "io" },
        { "genesis::utils", "math" },
        { "genesis::utils", "text" },
    };
    for( auto& p : sub_modules )
        modules[p.first.empty() ? p.second : p.first + "::" + p.second] =
            modules[p.first].def_submodule(
                mangle_namespace_name( p.second ).c_str(),
                ( "Bindings for " + p.first + "::" + p.second + " namespace" ).c_str()
            );

    // pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");

    bind_genesis_utils_core_fs( M );
    bind_genesis_utils_core_fs_1( M );
    bind_genesis_utils_core_info( M );
    bind_genesis_utils_core_info_1( M );
    bind_genesis_utils_core_options( M );
    bind_genesis_utils_core_version( M );
    bind_genesis_utils_io_base_input_source( M );
    bind_genesis_utils_io_file_input_source( M );
    bind_genesis_utils_io_gzip_stream( M );
    bind_genesis_utils_text_char( M );
    bind_genesis_utils_math_binomial( M );
    bind_genesis_utils_math_compensated_sum( M );
    bind_genesis_utils_math_correlation( M );
    bind_genesis_utils_math_random( M );
    bind_genesis_utils_math_statistics( M );
    bind_genesis_utils_math_statistics_1( M );
    bind_genesis_utils_text_base64( M );
    bind_genesis_utils_text_string( M );
    bind_genesis_utils_text_string_1( M );
    bind_genesis_utils_text_string_2( M );
    bind_genesis_utils_text_string_3( M );
    bind_genesis_utils_text_convert( M );
    bind_genesis_utils_text_style( M );
    bind_genesis_utils_text_table( M );
    bind_genesis_utils_tools_date_time( M );
    bind_genesis_utils_tools_hash_sha1( M );
    bind_genesis_utils_tools_hash_sha256( M );
    bind_genesis_utils_tools_tickmarks( M );
    bind_genesis_utils_math_compensated_sum_bind( M );
}
