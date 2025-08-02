#include <genesis/util/color/color.hpp>
#include <genesis/util/color/list_qualitative.hpp>
#include <genesis/util/color/list_sequential.hpp>
#include <iterator>
#include <memory>
#include <string>
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

void bind_genesis_util_color_list_qualitative(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::color::color_list_accent() file:genesis/util/color/list_qualitative.hpp
    // line:55 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_accent()() file:genesis/util/color/list_qualitative.hpp
    // line:55
    M( "genesis::util::color" )
        .def(
            "color_list_accent",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_accent,
            "Color palette `accent`.\n\n Provides a color palette with 8 colors, 4 pale and 4 "
            "saturated,\n for use with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_accent() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_dark2() file:genesis/util/color/list_qualitative.hpp line:65
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_dark2()() file:genesis/util/color/list_qualitative.hpp
    // line:65
    M( "genesis::util::color" )
        .def(
            "color_list_dark2",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_dark2,
            "Color palette `dark2`.\n\n Provides a color palette with 8 dark colors based on "
            "color_list_set2()\n for use with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_dark2() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_paired() file:genesis/util/color/list_qualitative.hpp
    // line:74 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_paired()() file:genesis/util/color/list_qualitative.hpp
    // line:74
    M( "genesis::util::color" )
        .def(
            "color_list_paired",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_paired,
            "Color palette `paired`.\n\n Provides a color palette with 12 colors in 6 pairs for "
            "use with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_paired() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_pastel1() file:genesis/util/color/list_qualitative.hpp
    // line:84 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_pastel1()() file:genesis/util/color/list_qualitative.hpp
    // line:84
    M( "genesis::util::color" )
        .def(
            "color_list_pastel1",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_pastel1,
            "Color palette `pastel1`.\n\n Provides a color palette with 9 colors based on "
            "color_list_set1()\n for use with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_pastel1() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_pastel2() file:genesis/util/color/list_qualitative.hpp
    // line:94 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_pastel2()() file:genesis/util/color/list_qualitative.hpp
    // line:94
    M( "genesis::util::color" )
        .def(
            "color_list_pastel2",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_pastel2,
            "Color palette `pastel2`.\n\n Provides a color palette with 9 colors based on "
            "color_list_set2()\n for use with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_pastel2() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_set1() file:genesis/util/color/list_qualitative.hpp line:103
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_set1()() file:genesis/util/color/list_qualitative.hpp
    // line:103
    M( "genesis::util::color" )
        .def(
            "color_list_set1",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_set1,
            "Color palette `set1`.\n\n Provides a color palette with 9 easy-to-name colors for use "
            "with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_set1() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_set2() file:genesis/util/color/list_qualitative.hpp line:112
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_set2()() file:genesis/util/color/list_qualitative.hpp
    // line:112
    M( "genesis::util::color" )
        .def(
            "color_list_set2",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_set2,
            "Color palette `set2`.\n\n Provides a color palette with 8 hard-to-name colors for use "
            "with qualitative/categorical data.\n\n \n \n\nC++: "
            "genesis::util::color::color_list_set2() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_set3() file:genesis/util/color/list_qualitative.hpp line:122
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_set3()() file:genesis/util/color/list_qualitative.hpp
    // line:122
    M( "genesis::util::color" )
        .def(
            "color_list_set3",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_set3,
            "Color palette `set3`.\n\n Provides a color palette with 12 colors for use with "
            "qualitative/categorical data.\n The colors are more saturated than the Pastels but "
            "less so than set2().\n\n \n \n\nC++: genesis::util::color::color_list_set3() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::ColorListQualitative file:genesis/util/color/list_qualitative.hpp
    // line:128
    pybind11::enum_<genesis::util::color::ColorListQualitative>(
        M( "genesis::util::color" ), "ColorListQualitative", ""
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
    // file:genesis/util/color/list_qualitative.hpp line:140 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_qualitative(enum
    // genesis::util::color::ColorListQualitative)(enum genesis::util::color::ColorListQualitative)
    // file:genesis/util/color/list_qualitative.hpp line:140
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative",
            ( const class std::vector<
                class genesis::util::color::Color>& (*)( enum genesis::util::color::
                                                             ColorListQualitative ) ) &
                genesis::util::color::color_list_qualitative,
            "C++: genesis::util::color::color_list_qualitative(enum "
            "genesis::util::color::ColorListQualitative) --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_qualitative(const std::string &)
    // file:genesis/util/color/list_qualitative.hpp line:141 function-signature: const class
    // std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_qualitative(const std::string &)(const std::string &)
    // file:genesis/util/color/list_qualitative.hpp line:141
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative",
            ( const class std::vector<class genesis::util::color::Color>& (*)(const std::string&)) &
                genesis::util::color::color_list_qualitative,
            "C++: genesis::util::color::color_list_qualitative(const std::string &) --> const "
            "class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference,
            pybind11::arg( "palette" )
        );

    // genesis::util::color::color_list_qualitative_names()
    // file:genesis/util/color/list_qualitative.hpp line:143 function-signature: class
    // std::vector<std::string > genesis::util::color::color_list_qualitative_names()()
    // file:genesis/util/color/list_qualitative.hpp line:143
    M( "genesis::util::color" )
        .def(
            "color_list_qualitative_names",
            ( class std::vector<std::string >( * )() ) &
                genesis::util::color::color_list_qualitative_names,
            "C++: genesis::util::color::color_list_qualitative_names() --> class "
            "std::vector<std::string >"
        );

    // genesis::util::color::color_list_bupubk() file:genesis/util/color/list_sequential.hpp line:54
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_bupubk()() file:genesis/util/color/list_sequential.hpp
    // line:54
    M( "genesis::util::color" )
        .def(
            "color_list_bupubk",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_bupubk,
            "Color Palette `bupubk`.\n\n Provides a sequential blue color palette of light blue, "
            "purple and black.\n Particularly useful for visualizing trees, as the light blue is "
            "still visible when\n used to color branches.\n\nC++: "
            "genesis::util::color::color_list_bupubk() --> const class std::vector<class "
            "genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_blues() file:genesis/util/color/list_sequential.hpp line:63
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_blues()() file:genesis/util/color/list_sequential.hpp
    // line:63
    M( "genesis::util::color" )
        .def(
            "color_list_blues",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_blues,
            "Color palette `blues`.\n\n Provides a sequential blue color palette of increasing "
            "saturation.\n\n \n \n\nC++: genesis::util::color::color_list_blues() --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_bugn() file:genesis/util/color/list_sequential.hpp line:72
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_bugn()() file:genesis/util/color/list_sequential.hpp line:72
    M( "genesis::util::color" )
        .def(
            "color_list_bugn",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_bugn,
            "Color palette `bugn`.\n\n Provides a sequential blue-green color palette of "
            "increasing saturation.\n\n \n \n\nC++: genesis::util::color::color_list_bugn() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_bupu() file:genesis/util/color/list_sequential.hpp line:81
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_bupu()() file:genesis/util/color/list_sequential.hpp line:81
    M( "genesis::util::color" )
        .def(
            "color_list_bupu",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_bupu,
            "Color palette `bupu`.\n\n Provides a sequential blue-purple color palette of "
            "increasing saturation.\n\n \n \n\nC++: genesis::util::color::color_list_bupu() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_gnbu() file:genesis/util/color/list_sequential.hpp line:90
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_gnbu()() file:genesis/util/color/list_sequential.hpp line:90
    M( "genesis::util::color" )
        .def(
            "color_list_gnbu",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_gnbu,
            "Color palette `gnbu`.\n\n Provides a sequential green-blue color palette of "
            "increasing saturation.\n\n \n \n\nC++: genesis::util::color::color_list_gnbu() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_greens() file:genesis/util/color/list_sequential.hpp line:99
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_greens()() file:genesis/util/color/list_sequential.hpp
    // line:99
    M( "genesis::util::color" )
        .def(
            "color_list_greens",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_greens,
            "Color palette `greens`.\n\n Provides a sequential green color palette of increasing "
            "saturation.\n\n \n \n\nC++: genesis::util::color::color_list_greens() --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_greys() file:genesis/util/color/list_sequential.hpp line:108
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_greys()() file:genesis/util/color/list_sequential.hpp
    // line:108
    M( "genesis::util::color" )
        .def(
            "color_list_greys",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_greys,
            "Color palette `greys`.\n\n Provides a sequential grey color palette of increasing "
            "saturation.\n\n \n \n\nC++: genesis::util::color::color_list_greys() --> const class "
            "std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_oranges() file:genesis/util/color/list_sequential.hpp
    // line:117 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_oranges()() file:genesis/util/color/list_sequential.hpp
    // line:117
    M( "genesis::util::color" )
        .def(
            "color_list_oranges",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_oranges,
            "Color palette `oranges`.\n\n Provides a sequential orange color palette of increasing "
            "saturation.\n\n \n \n\nC++: genesis::util::color::color_list_oranges() --> const "
            "class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_orrd() file:genesis/util/color/list_sequential.hpp line:126
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_orrd()() file:genesis/util/color/list_sequential.hpp
    // line:126
    M( "genesis::util::color" )
        .def(
            "color_list_orrd",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_orrd,
            "Color palette `orrd`.\n\n Provides a sequential orange-red color palette of "
            "increasing saturation.\n\n \n \n\nC++: genesis::util::color::color_list_orrd() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_pubu() file:genesis/util/color/list_sequential.hpp line:135
    // function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_pubu()() file:genesis/util/color/list_sequential.hpp
    // line:135
    M( "genesis::util::color" )
        .def(
            "color_list_pubu",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_pubu,
            "Color palette `pubu`.\n\n Provides a sequential purple-blue color palette of "
            "increasing saturation.\n\n \n \n\nC++: genesis::util::color::color_list_pubu() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );

    // genesis::util::color::color_list_pubugn() file:genesis/util/color/list_sequential.hpp
    // line:144 function-signature: const class std::vector<class genesis::util::color::Color> &
    // genesis::util::color::color_list_pubugn()() file:genesis/util/color/list_sequential.hpp
    // line:144
    M( "genesis::util::color" )
        .def(
            "color_list_pubugn",
            ( const class std::vector<class genesis::util::color::Color>& (*)() ) &
                genesis::util::color::color_list_pubugn,
            "Color palette `pubugn`.\n\n Provides a sequential purple-blue-green color palette of "
            "increasing saturation.\n\n \n \n\nC++: genesis::util::color::color_list_pubugn() --> "
            "const class std::vector<class genesis::util::color::Color> &",
            pybind11::return_value_policy::reference
        );
}
