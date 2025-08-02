#include <functional>
#include <genesis/util/color/color.hpp>
#include <genesis/util/color/helper.hpp>
#include <genesis/util/color/list_diverging.hpp>
#include <genesis/util/color/list_misc.hpp>
#include <genesis/util/color/norm_boundary.hpp>
#include <genesis/util/color/norm_diverging.hpp>
#include <iterator>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <functional>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>

#ifndef BINDER_PYBIND11_TYPE_CASTER
#    define BINDER_PYBIND11_TYPE_CASTER
PYBIND11_DECLARE_HOLDER_TYPE( T, std::shared_ptr<T>, false )
PYBIND11_DECLARE_HOLDER_TYPE( T, T*, false )
PYBIND11_MAKE_OPAQUE( std::shared_ptr<void> )
#endif

void bind_genesis_util_color_helper(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationDiverging &, unsigned long)
    // file:genesis/util/color/helper.hpp line:73 function-signature: class std::map<double,
    // std::string > genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationDiverging &, unsigned long)(const class
    // genesis::util::color::ColorNormalizationDiverging &, unsigned long)
    // file:genesis/util/color/helper.hpp line:73
    M( "genesis::util::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::util::color::ColorNormalizationDiverging&, unsigned long
            ) ) &
                genesis::util::color::color_tickmarks,
            "C++: genesis::util::color::color_tickmarks(const class "
            "genesis::util::color::ColorNormalizationDiverging &, unsigned long) --> class "
            "std::map<double, std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );

    // genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationBoundary &, unsigned long)
    // file:genesis/util/color/helper.hpp line:74 function-signature: class std::map<double,
    // std::string > genesis::util::color::color_tickmarks(const class
    // genesis::util::color::ColorNormalizationBoundary &, unsigned long)(const class
    // genesis::util::color::ColorNormalizationBoundary &, unsigned long)
    // file:genesis/util/color/helper.hpp line:74
    M( "genesis::util::color" )
        .def(
            "color_tickmarks",
            ( class std::map<double, std::string >( * )(
                const class genesis::util::color::ColorNormalizationBoundary&, unsigned long
            ) ) &
                genesis::util::color::color_tickmarks,
            "C++: genesis::util::color::color_tickmarks(const class "
            "genesis::util::color::ColorNormalizationBoundary &, unsigned long) --> class "
            "std::map<double, std::string >",
            pybind11::arg( "norm" ),
            pybind11::arg( "num_ticks" )
        );

    // genesis::util::color::color_list_brbg() file:genesis/util/color/list_diverging.hpp line:54
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_brbg()() file:genesis/util/color/list_diverging.hpp line:54
    M( "genesis::util::color" )
        .def(
            "color_list_brbg",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_brbg,
            "Color palette `brbg`.\n\n Provides a diverging color palette with brown low, white "
            "middle, and blue-green high.\n\n \n \n\nC++: genesis::util::color::color_list_brbg() "
            "--> const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_piyg() file:genesis/util/color/list_diverging.hpp line:63
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_piyg()() file:genesis/util/color/list_diverging.hpp line:63
    M( "genesis::util::color" )
        .def(
            "color_list_piyg",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_piyg,
            "Color palette `piyg`.\n\n Provides a diverging color palette with pink low, white "
            "middle, and yellow-green high.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_piyg() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_prgn() file:genesis/util/color/list_diverging.hpp line:72
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_prgn()() file:genesis/util/color/list_diverging.hpp line:72
    M( "genesis::util::color" )
        .def(
            "color_list_prgn",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_prgn,
            "Color palette `prgn`.\n\n Provides a diverging color palette with purple low, white "
            "middle, and green high.\n\n \n \n\nC++: genesis::util::color::color_list_prgn() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_puor() file:genesis/util/color/list_diverging.hpp line:81
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_puor()() file:genesis/util/color/list_diverging.hpp line:81
    M( "genesis::util::color" )
        .def(
            "color_list_puor",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_puor,
            "Color palette `puor`.\n\n Provides a diverging color palette with orange low, white "
            "middle, and purple high.\n\n \n \n\nC++: genesis::util::color::color_list_puor() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_rdbu() file:genesis/util/color/list_diverging.hpp line:90
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_rdbu()() file:genesis/util/color/list_diverging.hpp line:90
    M( "genesis::util::color" )
        .def(
            "color_list_rdbu",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_rdbu,
            "Color palette `rdbu`.\n\n Provides a diverging color palette with red low, white "
            "middle, and blue high.\n\n \n \n\nC++: genesis::util::color::color_list_rdbu() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_rdgy() file:genesis/util/color/list_diverging.hpp line:99
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_rdgy()() file:genesis/util/color/list_diverging.hpp line:99
    M( "genesis::util::color" )
        .def(
            "color_list_rdgy",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_rdgy,
            "Color palette `rdgy`.\n\n Provides a diverging color palette with red low, white "
            "middle, and grey high.\n\n \n \n\nC++: genesis::util::color::color_list_rdgy() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_rdylbu() file:genesis/util/color/list_diverging.hpp line:108
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_rdylbu()() file:genesis/util/color/list_diverging.hpp
    // line:108
    M( "genesis::util::color" )
        .def(
            "color_list_rdylbu",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_rdylbu,
            "Color palette `rdylbu`.\n\n Provides a diverging color palette with red low, pale "
            "yellow middle, and blue high.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_rdylbu() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_rdylgn() file:genesis/util/color/list_diverging.hpp line:117
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_rdylgn()() file:genesis/util/color/list_diverging.hpp
    // line:117
    M( "genesis::util::color" )
        .def(
            "color_list_rdylgn",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_rdylgn,
            "Color palette `rdylgn`.\n\n Provides a diverging color palette with red low, pale "
            "yellow middle, and green high.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_rdylgn() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_spectral() file:genesis/util/color/list_diverging.hpp
    // line:131 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_spectral()() file:genesis/util/color/list_diverging.hpp
    // line:131
    M( "genesis::util::color" )
        .def(
            "color_list_spectral",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_spectral,
            "Color palette `spectral`.\n\n Provides a diverging rainbow color palette with red "
            "low, pale yellow middle, and blue high.\n\n This ColorBrewer color palette is adapted "
            "from https://github.com/axismaps/colorbrewer and\n "
            "https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB "
            "`double` values.\n The original ColorBrewer color specifications and designs were "
            "developed by Cynthia Brewer\n (http://colorbrewer.org/), while their gnuplot "
            "equivalets are authored by Anna Schneider,\n and published under the [Apache-2.0 "
            "license](http://www.apache.org/licenses/LICENSE-2.0).\n See also our \n\n\n\nC++: "
            "genesis::util::color::color_list_spectral() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::ColorListDiverging file:genesis/util/color/list_diverging.hpp line:137
    pybind11::enum_<genesis::util::color::ColorListDiverging>(
        M( "genesis::util::color" ), "ColorListDiverging", ""
    )
        .value( "kBrbg", genesis::util::color::ColorListDiverging::kBrbg )
        .value( "kPiyg", genesis::util::color::ColorListDiverging::kPiyg )
        .value( "kPrgn", genesis::util::color::ColorListDiverging::kPrgn )
        .value( "kPuor", genesis::util::color::ColorListDiverging::kPuor )
        .value( "kRdbu", genesis::util::color::ColorListDiverging::kRdbu )
        .value( "kRdgy", genesis::util::color::ColorListDiverging::kRdgy )
        .value( "kRdylbu", genesis::util::color::ColorListDiverging::kRdylbu )
        .value( "kRdylgn", genesis::util::color::ColorListDiverging::kRdylgn )
        .value( "kSpectral", genesis::util::color::ColorListDiverging::kSpectral );

    ;

    // genesis::util::color::color_list_diverging(enum genesis::util::color::ColorListDiverging)
    // file:genesis/util/color/list_diverging.hpp line:150 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_diverging(enum
    // genesis::util::color::ColorListDiverging)(enum genesis::util::color::ColorListDiverging)
    // file:genesis/util/color/list_diverging.hpp line:150
    M( "genesis::util::color" )
        .def(
            "color_list_diverging",
            ( const class std::vector<class genesis::util::color::
                                          Color>& (*)( enum genesis::util::color::ColorListDiverging
            ) ) &
                genesis::util::color::color_list_diverging,
            "C++: genesis::util::color::color_list_diverging(enum "
            "genesis::util::color::ColorListDiverging) --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_diverging(const std::string &)
    // file:genesis/util/color/list_diverging.hpp line:151 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_diverging(const std::string &)(const std::string &)
    // file:genesis/util/color/list_diverging.hpp line:151
    M( "genesis::util::color" )
        .def(
            "color_list_diverging",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_diverging,
            "C++: genesis::util::color::color_list_diverging(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_diverging_names() file:genesis/util/color/list_diverging.hpp
    // line:153 function-signature: class std::vector<std::string >
    // genesis::util::color::color_list_diverging_names()()
    // file:genesis/util/color/list_diverging.hpp line:153
    M( "genesis::util::color" )
        .def(
            "color_list_diverging_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_diverging_names,
            "C++: genesis::util::color::color_list_diverging_names() --> class "
            "std::vector<std::string >"
        );

    // genesis::util::color::color_list_nextstrain() file:genesis/util/color/list_misc.hpp line:65
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_nextstrain()() file:genesis/util/color/list_misc.hpp line:65
    M( "genesis::util::color" )
        .def(
            "color_list_nextstrain",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_nextstrain,
            "Color palette from the the Nextstrain build for novel coronavirus (nCoV) at\n "
            "https://nextstrain.org/ncov\n\n The palette starts at purple and blue, progresses via "
            "green and yellow, towards orange and red.\n This function returns a palette with 256 "
            "entries.\n The original palette also has a reduced set of colors when using < 16 "
            "entries. We made these available\n via the \n\n\n\n\n\n\n\n\n\n\n\n\nC++: "
            "genesis::util::color::color_list_nextstrain() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_nextstrain(unsigned long)
    // file:genesis/util/color/list_misc.hpp line:86 function-signature: class std::vector<class
    // genesis::util::color::Color> genesis::util::color::color_list_nextstrain(unsigned
    // long)(unsigned long) file:genesis/util/color/list_misc.hpp line:86
    M( "genesis::util::color" )
        .def(
            "color_list_nextstrain",
            ( class std::vector<class genesis::util::color::Color>( * )( unsigned long ) ) &
                genesis::util::color::color_list_nextstrain,
            "Color palette from the the Nextstrain build for novel coronavirus (nCoV) at\n "
            "https://nextstrain.org/ncov\n\n The original palette has a reduced set of colors when "
            "using < 16 entries. This function makes\n these reduced color sets available when "
            "using `n < 16`. All values above that use the full\n palette (with 256) and "
            "interpolate to the requested number of entries. NB: The original palette\n continues "
            "to 500 colors as well, but the differences between adjacent entries are "
            "diminishing,\n so that our approach of interpolation when requesting a palette with "
            "more than 256 entries\n yields highly similar entries. Also, who needs a palette with "
            "that many entries anyway?!\n\n This color palette is adapted from the Nextstrain "
            "build for novel coronavirus (nCoV) at\n https://nextstrain.org/ncov, using their "
            "color scheme\n "
            "https://github.com/nextstrain/ncov/blob/master/config/color_schemes.tsv\n and "
            "converting parts of it to RGB `double` values.\n The repository at "
            "https://github.com/nextstrain/ncov is published under the\n [MIT "
            "license](https://opensource.org/licenses/MIT).\n See also our \n\n\n\nC++: "
            "genesis::util::color::color_list_nextstrain(unsigned long) --> class "
            "std::vector<class genesis::util::color::Color>",
            pybind11::arg( "n" )
        );

    // genesis::util::color::ColorListMisc file:genesis/util/color/list_misc.hpp line:92
    pybind11::enum_<genesis::util::color::ColorListMisc>(
        M( "genesis::util::color" ), "ColorListMisc", ""
    )
        .value( "kNextstrain", genesis::util::color::ColorListMisc::kNextstrain );

    ;

    // genesis::util::color::color_list_misc(enum genesis::util::color::ColorListMisc)
    // file:genesis/util/color/list_misc.hpp line:97 function-signature: const class
    // std::vector<class genesis::util::color::Color> & genesis::util::color::color_list_misc(enum
    // genesis::util::color::ColorListMisc)(enum genesis::util::color::ColorListMisc)
    // file:genesis/util/color/list_misc.hpp line:97
    M( "genesis::util::color" )
        .def(
            "color_list_misc",
            ( const class std::vector<
                class genesis::util::color::Color>& (*)( enum genesis::util::color::ColorListMisc )
            ) & genesis::util::color::color_list_misc,
            "C++: genesis::util::color::color_list_misc(enum genesis::util::color::ColorListMisc) "
            "--> const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_misc(const std::string &)
    // file:genesis/util/color/list_misc.hpp line:98 function-signature: const class
    // std::vector<class genesis::util::color::Color> & genesis::util::color::color_list_misc(const
    // std::string &)(const std::string &) file:genesis/util/color/list_misc.hpp line:98
    M( "genesis::util::color" )
        .def(
            "color_list_misc",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_misc,
            "C++: genesis::util::color::color_list_misc(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_misc_names() file:genesis/util/color/list_misc.hpp line:100
    // function-signature: class std::vector<std::string >
    // genesis::util::color::color_list_misc_names()() file:genesis/util/color/list_misc.hpp
    // line:100
    M( "genesis::util::color" )
        .def(
            "color_list_misc_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_misc_names,
            "C++: genesis::util::color::color_list_misc_names() --> class std::vector<std::string >"
        );
}
