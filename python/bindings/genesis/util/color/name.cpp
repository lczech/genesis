#include <genesis/util/color/color.hpp>
#include <genesis/util/color/name.hpp>
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

void bind_genesis_util_color_name(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    // genesis::util::color::is_color_name(const std::string &) file:genesis/util/color/name.hpp
    // line:52 function-signature: bool genesis::util::color::is_color_name(const std::string
    // &)(const std::string &) file:genesis/util/color/name.hpp line:52
    M( "genesis::util::color" )
        .def(
            "is_color_name",
            ( bool ( * )( const std::string& ) ) & genesis::util::color::is_color_name,
            "Return `true` if the name represents one of the named colors offered by genesis,\n "
            "which is (currently) a shortcut for is_color_name_web() and "
            "is_color_name_xkcd().\n\nC++: genesis::util::color::is_color_name(const std::string "
            "&) --> bool",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_from_name(const std::string &) file:genesis/util/color/name.hpp
    // line:58 function-signature: class genesis::util::color::Color
    // genesis::util::color::color_from_name(const std::string &)(const std::string &)
    // file:genesis/util/color/name.hpp line:58
    M( "genesis::util::color" )
        .def(
            "color_from_name",
            ( class genesis::util::color::Color( * )( const std::string& ) ) &
                genesis::util::color::color_from_name,
            "Return the color represented by the given name,\n which is (currently) a shortcut for "
            "color_from_name_web() and color_from_name_xkcd().\n\nC++: "
            "genesis::util::color::color_from_name(const std::string &) --> class "
            "genesis::util::color::Color",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_palette_web() file:genesis/util/color/name.hpp line:64
    // function-signature: class std::vector<class genesis::util::color::Color>
    // genesis::util::color::color_palette_web()() file:genesis/util/color/name.hpp line:64
    M( "genesis::util::color" )
        .def(
            "color_palette_web",
            ( class std::vector<class genesis::util::color::Color>( * )() ) &
                genesis::util::color::color_palette_web,
            "C++: genesis::util::color::color_palette_web() --> class std::vector<class "
            "genesis::util::color::Color>"
        );

    // genesis::util::color::is_color_name_web(const std::string &) file:genesis/util/color/name.hpp
    // line:71 function-signature: bool genesis::util::color::is_color_name_web(const std::string
    // &)(const std::string &) file:genesis/util/color/name.hpp line:71
    M( "genesis::util::color" )
        .def(
            "is_color_name_web",
            ( bool ( * )( const std::string& ) ) & genesis::util::color::is_color_name_web,
            "Return true iff the given name is a named web color.\n\n Names are compared only by "
            "their alnum chars, and the letter case is ignored.\n\nC++: "
            "genesis::util::color::is_color_name_web(const std::string &) --> bool",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_from_name_web(const std::string &)
    // file:genesis/util/color/name.hpp line:79 function-signature: class
    // genesis::util::color::Color genesis::util::color::color_from_name_web(const std::string
    // &)(const std::string &) file:genesis/util/color/name.hpp line:79
    M( "genesis::util::color" )
        .def(
            "color_from_name_web",
            ( class genesis::util::color::Color( * )( const std::string& ) ) &
                genesis::util::color::color_from_name_web,
            "Retrieve a named web color by name.\n\n Names are compared only by their alnum chars, "
            "and the letter case is ignored.\n If the color name does not exist, an "
            "`std::invalid_argument` exception is thrown.\n\nC++: "
            "genesis::util::color::color_from_name_web(const std::string &) --> class "
            "genesis::util::color::Color",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_palette_xkcd() file:genesis/util/color/name.hpp line:85
    // function-signature: class std::vector<class genesis::util::color::Color>
    // genesis::util::color::color_palette_xkcd()() file:genesis/util/color/name.hpp line:85
    M( "genesis::util::color" )
        .def(
            "color_palette_xkcd",
            ( class std::vector<class genesis::util::color::Color>( * )() ) &
                genesis::util::color::color_palette_xkcd,
            "C++: genesis::util::color::color_palette_xkcd() --> class std::vector<class "
            "genesis::util::color::Color>"
        );

    // genesis::util::color::is_color_name_xkcd(const std::string &)
    // file:genesis/util/color/name.hpp line:93 function-signature: bool
    // genesis::util::color::is_color_name_xkcd(const std::string &)(const std::string &)
    // file:genesis/util/color/name.hpp line:93
    M( "genesis::util::color" )
        .def(
            "is_color_name_xkcd",
            ( bool ( * )( const std::string& ) ) & genesis::util::color::is_color_name_xkcd,
            "Return true iff the given name is a named xkcd color.\n\n Names are compared only by "
            "their alnum chars, and the letter case is ignored.\n See color_from_name_xkcd() for "
            "details on this color list.\n\nC++: genesis::util::color::is_color_name_xkcd(const "
            "std::string &) --> bool",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_from_name_xkcd(const std::string &)
    // file:genesis/util/color/name.hpp line:107 function-signature: class
    // genesis::util::color::Color genesis::util::color::color_from_name_xkcd(const std::string
    // &)(const std::string &) file:genesis/util/color/name.hpp line:107
    M( "genesis::util::color" )
        .def(
            "color_from_name_xkcd",
            ( class genesis::util::color::Color( * )( const std::string& ) ) &
                genesis::util::color::color_from_name_xkcd,
            "Retrieve a named xkcd color by name.\n\n Names are compared only by their alnum "
            "chars, and the letter case is ignored.\n If the color name does not exist, an "
            "`std::invalid_argument` exception is thrown.\n\n The colors are taken from an "
            "[https://xkcd.com/](xkcd) color survey.\n They were published under Public Domain, "
            "http://creativecommons.org/publicdomain/zero/1.0/.\n See https://xkcd.com/color/rgb/ "
            "and https://blog.xkcd.com/2010/05/03/color-survey-results/\n for details. See also "
            "our\n \n\n\n\nC++: genesis::util::color::color_from_name_xkcd(const std::string &) "
            "--> class genesis::util::color::Color",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_palette_lego() file:genesis/util/color/name.hpp line:113
    // function-signature: class std::vector<class genesis::util::color::Color>
    // genesis::util::color::color_palette_lego()() file:genesis/util/color/name.hpp line:113
    M( "genesis::util::color" )
        .def(
            "color_palette_lego",
            ( class std::vector<class genesis::util::color::Color>( * )() ) &
                genesis::util::color::color_palette_lego,
            "C++: genesis::util::color::color_palette_lego() --> class std::vector<class "
            "genesis::util::color::Color>"
        );

    // genesis::util::color::is_color_name_lego(const std::string &)
    // file:genesis/util/color/name.hpp line:121 function-signature: bool
    // genesis::util::color::is_color_name_lego(const std::string &)(const std::string &)
    // file:genesis/util/color/name.hpp line:121
    M( "genesis::util::color" )
        .def(
            "is_color_name_lego",
            ( bool ( * )( const std::string& ) ) & genesis::util::color::is_color_name_lego,
            "Return true iff the given name is a named LEGO color.\n\n Names are compared only by "
            "their alnum chars, and the letter case is ignored.\n See color_from_name_lego() for "
            "details on this color list.\n\nC++: genesis::util::color::is_color_name_lego(const "
            "std::string &) --> bool",
            pybind11::arg( "name" )
        );

    // genesis::util::color::color_from_name_lego(const std::string &)
    // file:genesis/util/color/name.hpp line:136 function-signature: class
    // genesis::util::color::Color genesis::util::color::color_from_name_lego(const std::string
    // &)(const std::string &) file:genesis/util/color/name.hpp line:136
    M( "genesis::util::color" )
        .def(
            "color_from_name_lego",
            ( class genesis::util::color::Color( * )( const std::string& ) ) &
                genesis::util::color::color_from_name_lego,
            "Retrieve a named LEGO color by name.\n\n Names are compared only by their alnum "
            "chars, and the letter case is ignored.\n If the color name does not exist, an "
            "`std::invalid_argument` exception is thrown.\n\n The colors are taken from the "
            "`colors.csv.gz` table of the LEGO Catalog Database at\n "
            "https://rebrickable.com/downloads/ on 2024-06-17. They were published with the "
            "note\n\n  > You can use these files for any purpose.\n\n which we interpret as the "
            "table being in the public domain.\n\nC++: "
            "genesis::util::color::color_from_name_lego(const std::string &) --> class "
            "genesis::util::color::Color",
            pybind11::arg( "name" )
        );
}
