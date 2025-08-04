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

    // genesis::util::color::ColorListDiverging file:genesis/util/color/list_diverging.hpp line:63
    pybind11::enum_<genesis::util::color::ColorListDiverging>(
        M( "genesis::util::color" ),
        "ColorListDiverging",
        "Divering color palettes.\n\n List of the diverging color palettes offered by "
        "color_list_diverging(). A list of all color\n palettes offered here by their name can "
        "also be obtained from color_list_diverging_names().\n\n These color palette are adapted "
        "from ColorBrewer at https://github.com/axismaps/colorbrewer and\n "
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
        .value( "kSpectral", genesis::util::color::ColorListDiverging::kSpectral )
        .value( "kThermisDiverging", genesis::util::color::ColorListDiverging::kThermisDiverging );

    ;

    // genesis::util::color::color_list_diverging(enum genesis::util::color::ColorListDiverging)
    // file:genesis/util/color/list_diverging.hpp line:151 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_diverging(enum
    // genesis::util::color::ColorListDiverging)(enum genesis::util::color::ColorListDiverging)
    // file:genesis/util/color/list_diverging.hpp line:151
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
    // file:genesis/util/color/list_diverging.hpp line:162 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_diverging(const std::string &)(const std::string &)
    // file:genesis/util/color/list_diverging.hpp line:162
    M( "genesis::util::color" )
        .def(
            "color_list_diverging",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_diverging,
            "Get a diverging color palette by its name.\n\n The names used here are the same as in "
            "ColorListDiverging, but without the leading `k`,\n i.e., using their original name "
            "(case insensitive and ignoring all non-alnum characters).\n A list of all color "
            "palettes offered here by their name can also be obtained from\n "
            "color_list_diverging_names().\n For instance, ColorListDiverging::kSpectral can be "
            "obtained here simply as `\"spectral\"`.\n\nC++: "
            "genesis::util::color::color_list_diverging(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_diverging_names() file:genesis/util/color/list_diverging.hpp
    // line:170 function-signature: class std::vector<std::string >
    // genesis::util::color::color_list_diverging_names()()
    // file:genesis/util/color/list_diverging.hpp line:170
    M( "genesis::util::color" )
        .def(
            "color_list_diverging_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_diverging_names,
            "Get the list of all available diverging color palettes as strings.\n\n See also "
            "ColorListDiverging for the corresponding enum. Use the color list names provided "
            "here\n as arguments for color_list_diverging( std::string const& ).\n\nC++: "
            "genesis::util::color::color_list_diverging_names() --> class std::vector<std::string >"
        );

    // genesis::util::color::color_list_nextstrain(unsigned long)
    // file:genesis/util/color/list_misc.hpp line:60 function-signature: class std::vector<class
    // genesis::util::color::Color> genesis::util::color::color_list_nextstrain(unsigned
    // long)(unsigned long) file:genesis/util/color/list_misc.hpp line:60
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
            "that many entries anyway?!\n\n See ColorListMisc::kNextstrain for details on the "
            "palette.\n\nC++: genesis::util::color::color_list_nextstrain(unsigned long) --> class "
            "std::vector<class genesis::util::color::Color>",
            pybind11::arg( "n" )
        );

    // genesis::util::color::ColorListMisc file:genesis/util/color/list_misc.hpp line:74
    pybind11::enum_<genesis::util::color::ColorListMisc>(
        M( "genesis::util::color" ),
        "ColorListMisc",
        "List of miscellaneous color palettes.\n\n List of the diverging color palettes offered by "
        "color_list_misc(). A list of all color palettes\n offered here by their name can also be "
        "obtained from color_list_misc_names().\n\n For further color palettes, see "
        "ColorListQualitative, ColorListSequential, and ColorListDiverging."
    )
        .value( "kNextstrain", genesis::util::color::ColorListMisc::kNextstrain )
        .value( "kThermisCyclic", genesis::util::color::ColorListMisc::kThermisCyclic );

    ;

    // genesis::util::color::color_list_misc(enum genesis::util::color::ColorListMisc)
    // file:genesis/util/color/list_misc.hpp line:117 function-signature: const class
    // std::vector<class genesis::util::color::Color> & genesis::util::color::color_list_misc(enum
    // genesis::util::color::ColorListMisc)(enum genesis::util::color::ColorListMisc)
    // file:genesis/util/color/list_misc.hpp line:117
    M( "genesis::util::color" )
        .def(
            "color_list_misc",
            ( const class std::vector<
                class genesis::util::color::Color>& (*)( enum genesis::util::color::ColorListMisc )
            ) & genesis::util::color::color_list_misc,
            "Get a miscellaneous color palette.\n\n See ColorListMisc for the available "
            "ones.\n\nC++: genesis::util::color::color_list_misc(enum "
            "genesis::util::color::ColorListMisc) --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_misc(const std::string &)
    // file:genesis/util/color/list_misc.hpp line:128 function-signature: const class
    // std::vector<class genesis::util::color::Color> & genesis::util::color::color_list_misc(const
    // std::string &)(const std::string &) file:genesis/util/color/list_misc.hpp line:128
    M( "genesis::util::color" )
        .def(
            "color_list_misc",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_misc,
            "Get a miscellaneous color palette by its name.\n\n The names used here are the same "
            "as in ColorListMisc, but without the leading `k`,\n i.e., using their original name "
            "(case insensitive and ignoring all non-alnum characters).\n A list of all color "
            "palettes offered here by their name can also be obtained from\n "
            "color_list_misc_names().\n For instance, ColorListMisc::kThermisCyclic can be "
            "obtained here simply as `\"ThermisCyclic\"`.\n\nC++: "
            "genesis::util::color::color_list_misc(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_misc_names() file:genesis/util/color/list_misc.hpp line:136
    // function-signature: class std::vector<std::string >
    // genesis::util::color::color_list_misc_names()() file:genesis/util/color/list_misc.hpp
    // line:136
    M( "genesis::util::color" )
        .def(
            "color_list_misc_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_misc_names,
            "Get the list of all available miscellaneous color palettes as strings.\n\n See also "
            "ColorListMisc for the corresponding enum. Use the color list names provided here\n as "
            "arguments for color_list_misc( std::string const& ).\n\nC++: "
            "genesis::util::color::color_list_misc_names() --> class std::vector<std::string >"
        );

    // genesis::util::color::ColorListQualitative file:genesis/util/color/list_qualitative.hpp
    // line:63
    pybind11::enum_<genesis::util::color::ColorListQualitative>(
        M( "genesis::util::color" ),
        "ColorListQualitative",
        "Qualitative color palettes.\n\n List of the qualitative color palettes offered by "
        "color_list_qualitative(). A list of all color\n palettes offered here by their name can "
        "also be obtained from color_list_qualitative_names().\n\n These color palette are adapted "
        "from ColorBrewer at https://github.com/axismaps/colorbrewer and\n "
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
    // file:genesis/util/color/list_qualitative.hpp line:132 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_qualitative(enum
    // genesis::util::color::ColorListQualitative)(enum genesis::util::color::ColorListQualitative)
    // file:genesis/util/color/list_qualitative.hpp line:132
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
    // file:genesis/util/color/list_qualitative.hpp line:143 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_qualitative(const std::string &)(const std::string &)
    // file:genesis/util/color/list_qualitative.hpp line:143
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_qualitative,
            "Get a qualitative color palette by its name.\n\n The names used here are the same as "
            "in ColorListQualitative, but without the leading `k`,\n i.e., using their original "
            "name (case insensitive and ignoring all non-alnum characters).\n A list of all color "
            "palettes offered here by their name can also be obtained from\n "
            "color_list_qualitative_names().\n For instance, ColorListQualitative::kAccent can be "
            "obtained here simply as `\"accent\"`.\n\nC++: "
            "genesis::util::color::color_list_qualitative(const std::string &) --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_qualitative_names()
    // file:genesis/util/color/list_qualitative.hpp line:151 function-signature: class
    // std::vector<std::string > genesis::util::color::color_list_qualitative_names()()
    // file:genesis/util/color/list_qualitative.hpp line:151
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_qualitative_names,
            "Get the list of all available qualitative color palettes as strings.\n\n See also "
            "ColorListQualitative for the corresponding enum. Use the color list names provided "
            "here\n as arguments for color_list_qualitative( std::string const& ).\n\nC++: "
            "genesis::util::color::color_list_qualitative_names() --> class "
            "std::vector<std::string >"
        );

    // genesis::util::color::ColorListSequential file:genesis/util/color/list_sequential.hpp line:70
    pybind11::enum_<genesis::util::color::ColorListSequential>(
        M( "genesis::util::color" ),
        "ColorListSequential",
        "Sequential color palettes.\n\n List of the sequential color palettes offered by "
        "color_list_sequential(). A list of all color\n palettes offered here by their name can "
        "also be obtained from color_list_sequential_names().\n The available palettes come from "
        "difference sources, credited below.\n\n Some color palettes are adapted from ColorBrewer "
        "at https://github.com/axismaps/colorbrewer and\n "
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
        .value( "kViridis", genesis::util::color::ColorListSequential::kViridis )
        .value( "kThermisGreen", genesis::util::color::ColorListSequential::kThermisGreen )
        .value( "kThermisRed", genesis::util::color::ColorListSequential::kThermisRed );

    ;
}
