/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS( sequence_functions_codes_export, ::genesis::sequence, scope )
{

    scope.def(
        "nucleic_acid_ambiguity_code",
        ( char ( * )( std::string ))( &::genesis::sequence::nucleic_acid_ambiguity_code ),
            pybind11::arg("codes"),
        get_docstring("char ::genesis::sequence::nucleic_acid_ambiguity_code (std::string codes)")
    );

    scope.def(
        "amino_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::amino_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::amino_acid_text_colors ()")
    );

    scope.def(
        "nucleic_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::nucleic_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::nucleic_acid_text_colors ()")
    );

    scope.def(
        "amino_acid_colors",
        ( std::map< char, utils::Color > ( * )(  ))( &::genesis::sequence::amino_acid_colors ),
        get_docstring("std::map< char, utils::Color > ::genesis::sequence::amino_acid_colors ()")
    );

    scope.def(
        "nucleic_acid_colors",
        ( std::map< char, utils::Color > ( * )(  ))( &::genesis::sequence::nucleic_acid_colors ),
        get_docstring("std::map< char, utils::Color > ::genesis::sequence::nucleic_acid_colors ()")
    );

    scope.def(
        "amino_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_all ()")
    );

    scope.def(
        "amino_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_degenerated ()")
    );

    scope.def(
        "amino_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_plain ()")
    );

    scope.def(
        "amino_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_undetermined ()")
    );

    scope.def(
        "nucleic_acid_ambiguities",
        ( std::string ( * )( char ))( &::genesis::sequence::nucleic_acid_ambiguities ),
            pybind11::arg("code"),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_ambiguities (char code)")
    );

    scope.def(
        "nucleic_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_all ()")
    );

    scope.def(
        "nucleic_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_degenerated ()")
    );

    scope.def(
        "nucleic_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_plain ()")
    );

    scope.def(
        "nucleic_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_undetermined ()")
    );

    scope.def(
        "translate_amino_acid",
        ( std::string ( * )( char ))( &::genesis::sequence::translate_amino_acid ),
            pybind11::arg("code"),
        get_docstring("std::string ::genesis::sequence::translate_amino_acid (char code)")
    );

    scope.def(
        "translate_nucleic_acid",
        ( std::string ( * )( char ))( &::genesis::sequence::translate_nucleic_acid ),
            pybind11::arg("code"),
        get_docstring("std::string ::genesis::sequence::translate_nucleic_acid (char code)")
    );
}
