#include <functional>
#include <genesis/util/color/color.hpp>
#include <genesis/util/color/helper.hpp>
#include <genesis/util/color/list_diverging.hpp>
#include <genesis/util/color/list_misc.hpp>
#include <genesis/util/color/list_qualitative.hpp>
#include <genesis/util/color/list_sequential.hpp>
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

    // genesis::util::color::ColorListDiverging file:genesis/util/color/list_diverging.hpp line:62
    pybind11::enum_<genesis::util::color::ColorListDiverging>(
        M( "genesis::util::color" ),
        "ColorListDiverging",
        "Divering color palettes.\n\n List of the diverging color palettes offered by "
        "color_list_diverging().\n\n These color palette are adapted from ColorBrewer at "
        "https://github.com/axismaps/colorbrewer and\n "
        "https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` "
        "values.\n The original ColorBrewer color specifications and designs were developed by "
        "Cynthia Brewer\n (http://colorbrewer.org/), while their gnuplot equivalets are authored "
        "by Anna Schneider,\n and published under the [Apache-2.0 "
        "license](http://www.apache.org/licenses/LICENSE-2.0).\n See also our \n\n\n\n\n"
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
    // file:genesis/util/color/list_diverging.hpp line:134 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_diverging(enum
    // genesis::util::color::ColorListDiverging)(enum genesis::util::color::ColorListDiverging)
    // file:genesis/util/color/list_diverging.hpp line:134
    M( "genesis::util::color" )
        .def(
            "color_list_diverging",
            ( const class std::vector<class genesis::util::color::
                                          Color>& (*)( enum genesis::util::color::ColorListDiverging
            ) ) &
                genesis::util::color::color_list_diverging,
            "Get a diverging color palette.\n\n See ColorListDiverging for the available "
            "ones.\n\nC++: genesis::util::color::color_list_diverging(enum "
            "genesis::util::color::ColorListDiverging) --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_diverging(const std::string &)
    // file:genesis/util/color/list_diverging.hpp line:143 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_diverging(const std::string &)(const std::string &)
    // file:genesis/util/color/list_diverging.hpp line:143
    M( "genesis::util::color" )
        .def(
            "color_list_diverging",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_diverging,
            "Get a diverging color palette by its name.\n\n The names used here are the same as in "
            "ColorListDiverging, but without the leading `k`,\n i.e., using their original name "
            "(case insensitive).\n For instance, ColorListDiverging::kSpectral can be obtained "
            "here simply as `\"spectral\"`.\n\nC++: "
            "genesis::util::color::color_list_diverging(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_diverging_names() file:genesis/util/color/list_diverging.hpp
    // line:150 function-signature: class std::vector<std::string >
    // genesis::util::color::color_list_diverging_names()()
    // file:genesis/util/color/list_diverging.hpp line:150
    M( "genesis::util::color" )
        .def(
            "color_list_diverging_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_diverging_names,
            "Get the list of all available diverging color palettes as strings.\n\n See also "
            "ColorListDiverging for the corresponding enum.\n\nC++: "
            "genesis::util::color::color_list_diverging_names() --> class std::vector<std::string >"
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

    // genesis::util::color::ColorListQualitative file:genesis/util/color/list_qualitative.hpp
    // line:62
    pybind11::enum_<genesis::util::color::ColorListQualitative>(
        M( "genesis::util::color" ),
        "ColorListQualitative",
        "Qualitative color palettes.\n\n List of the qualitative color palettes offered by "
        "color_list_qualitative().\n\n These color palette are adapted from ColorBrewer at "
        "https://github.com/axismaps/colorbrewer and\n "
        "https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` "
        "values.\n The original ColorBrewer color specifications and designs were developed by "
        "Cynthia Brewer\n (http://colorbrewer.org/), while their gnuplot equivalets are authored "
        "by Anna Schneider,\n and published under the [Apache-2.0 "
        "license](http://www.apache.org/licenses/LICENSE-2.0).\n See also our \n\n\n\n\n"
    )
        .value( "kAccent", genesis::util::color::ColorListQualitative::kAccent )
        .value( "kDark2", genesis::util::color::ColorListQualitative::kDark2 )
        .value( "kPaired", genesis::util::color::ColorListQualitative::kPaired )
        .value( "kPastel1", genesis::util::color::ColorListQualitative::kPastel1 )
        .value( "kPastel2", genesis::util::color::ColorListQualitative::kPastel2 )
        .value( "kSet1", genesis::util::color::ColorListQualitative::kSet1 )
        .value( "kSet2", genesis::util::color::ColorListQualitative::kSet2 )
        .value( "kSet3", genesis::util::color::ColorListQualitative::kSet3 );

    ;

    // genesis::util::color::color_list_qualitative(enum genesis::util::color::ColorListQualitative)
    // file:genesis/util/color/list_qualitative.hpp line:131 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_qualitative(enum
    // genesis::util::color::ColorListQualitative)(enum genesis::util::color::ColorListQualitative)
    // file:genesis/util/color/list_qualitative.hpp line:131
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative",
            ( const class std::vector<
                class genesis::util::color::Color>& (*)( enum genesis::util::color::
                                                             ColorListQualitative ) ) &
                genesis::util::color::color_list_qualitative,
            "Get a qualitative color palette.\n\n See ColorListQualitative for the available "
            "ones.\n\nC++: genesis::util::color::color_list_qualitative(enum "
            "genesis::util::color::ColorListQualitative) --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_qualitative(const std::string &)
    // file:genesis/util/color/list_qualitative.hpp line:140 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_qualitative(const std::string &)(const std::string &)
    // file:genesis/util/color/list_qualitative.hpp line:140
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_qualitative,
            "Get a qualitative color palette by its name.\n\n The names used here are the same as "
            "in ColorListQualitative, but without the leading `k`,\n i.e., using their original "
            "name (case insensitive).\n For instance, ColorListQualitative::kAccent can be "
            "obtained here simply as `\"accent\"`.\n\nC++: "
            "genesis::util::color::color_list_qualitative(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_qualitative_names()
    // file:genesis/util/color/list_qualitative.hpp line:147 function-signature: class
    // std::vector<std::string > genesis::util::color::color_list_qualitative_names()()
    // file:genesis/util/color/list_qualitative.hpp line:147
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_qualitative_names,
            "Get the list of all available qualitative color palettes as strings.\n\n See also "
            "ColorListQualitative for the corresponding enum.\n\nC++: "
            "genesis::util::color::color_list_qualitative_names() --> class "
            "std::vector<std::string >"
        );

    // genesis::util::color::ColorListSequential file:genesis/util/color/list_sequential.hpp line:69
    pybind11::enum_<genesis::util::color::ColorListSequential>(
        M( "genesis::util::color" ),
        "ColorListSequential",
        "Sequential color palettes.\n\n List of the sequential color palettes offered by "
        "color_list_sequential().\n The available palettes come from difference sources, credited "
        "below.\n\n Some color palettes are adapted from ColorBrewer at "
        "https://github.com/axismaps/colorbrewer and\n "
        "https://github.com/Gnuplotting/gnuplot-palettes by converting the colors to RGB `double` "
        "values.\n The original ColorBrewer color specifications and designs were developed by "
        "Cynthia Brewer\n (http://colorbrewer.org/), while their gnuplot equivalets are authored "
        "by Anna Schneider,\n and published under the [Apache-2.0 "
        "license](http://www.apache.org/licenses/LICENSE-2.0).\n See also our "
        "\n\n\n\n\n\n\n\n\n\n\n"
    )
        .value( "kBupubk", genesis::util::color::ColorListSequential::kBupubk )
        .value( "kBlues", genesis::util::color::ColorListSequential::kBlues )
        .value( "kBugn", genesis::util::color::ColorListSequential::kBugn )
        .value( "kBupu", genesis::util::color::ColorListSequential::kBupu )
        .value( "kGnbu", genesis::util::color::ColorListSequential::kGnbu )
        .value( "kGreens", genesis::util::color::ColorListSequential::kGreens )
        .value( "kGreys", genesis::util::color::ColorListSequential::kGreys )
        .value( "kOranges", genesis::util::color::ColorListSequential::kOranges )
        .value( "kOrrd", genesis::util::color::ColorListSequential::kOrrd )
        .value( "kPubu", genesis::util::color::ColorListSequential::kPubu )
        .value( "kPubugn", genesis::util::color::ColorListSequential::kPubugn )
        .value( "kPurd", genesis::util::color::ColorListSequential::kPurd )
        .value( "kPurples", genesis::util::color::ColorListSequential::kPurples )
        .value( "kRdpu", genesis::util::color::ColorListSequential::kRdpu )
        .value( "kReds", genesis::util::color::ColorListSequential::kReds )
        .value( "kYlgn", genesis::util::color::ColorListSequential::kYlgn )
        .value( "kYlgnbu", genesis::util::color::ColorListSequential::kYlgnbu )
        .value( "kYlorbr", genesis::util::color::ColorListSequential::kYlorbr )
        .value( "kYlorrd", genesis::util::color::ColorListSequential::kYlorrd )
        .value( "kHeat", genesis::util::color::ColorListSequential::kHeat )
        .value( "kMagma", genesis::util::color::ColorListSequential::kMagma )
        .value( "kInferno", genesis::util::color::ColorListSequential::kInferno )
        .value( "kPlasma", genesis::util::color::ColorListSequential::kPlasma )
        .value( "kViridis", genesis::util::color::ColorListSequential::kViridis );

    ;
}
