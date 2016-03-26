/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <python/src/common.hpp>

#include "lib/genesis.hpp"

using namespace ::genesis::sequence;

PYTHON_EXPORT_FUNCTIONS(sequence_codes_export, "sequence")
{

    boost::python::def(
        "amino_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::amino_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::amino_acid_text_colors ()")
    );

    boost::python::def(
        "nucleic_acid_text_colors",
        ( std::map< char, std::string > ( * )(  ))( &::genesis::sequence::nucleic_acid_text_colors ),
        get_docstring("std::map< char, std::string > ::genesis::sequence::nucleic_acid_text_colors ()")
    );

    boost::python::def(
        "amino_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_all ()")
    );

    boost::python::def(
        "amino_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_degenerated ()")
    );

    boost::python::def(
        "amino_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_plain ()")
    );

    boost::python::def(
        "amino_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::amino_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::amino_acid_codes_undetermined ()")
    );

    boost::python::def(
        "nucleic_acid_codes_all",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_all ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_all ()")
    );

    boost::python::def(
        "nucleic_acid_codes_degenerated",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_degenerated ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_degenerated ()")
    );

    boost::python::def(
        "nucleic_acid_codes_plain",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_plain ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_plain ()")
    );

    boost::python::def(
        "nucleic_acid_codes_undetermined",
        ( std::string ( * )(  ))( &::genesis::sequence::nucleic_acid_codes_undetermined ),
        get_docstring("std::string ::genesis::sequence::nucleic_acid_codes_undetermined ()")
    );

    boost::python::def(
        "translate_amino_acid",
        ( std::string ( * )( char ))( &::genesis::sequence::translate_amino_acid ),
        ( boost::python::arg("code") ),
        get_docstring("std::string ::genesis::sequence::translate_amino_acid (char code)")
    );

    boost::python::def(
        "translate_nucleic_acid",
        ( std::string ( * )( char ))( &::genesis::sequence::translate_nucleic_acid ),
        ( boost::python::arg("code") ),
        get_docstring("std::string ::genesis::sequence::translate_nucleic_acid (char code)")
    );
}
