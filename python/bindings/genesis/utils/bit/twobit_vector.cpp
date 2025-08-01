#include <genesis/utils/bit/twobit_vector.hpp>
#include <genesis/utils/bit/twobit_vector/functions.hpp>
#include <genesis/utils/bit/twobit_vector_add_ons.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
#include <string>

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

void bind_genesis_utils_bit_twobit_vector(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::utils::bit::TwobitVector file:genesis/utils/bit/twobit_vector.hpp line:51
        pybind11::class_<
            genesis::utils::bit::TwobitVector,
            std::shared_ptr<genesis::utils::bit::TwobitVector>>
            cl( M( "genesis::utils::bit" ),
                "TwobitVector",
                "Represent a nucleotide sequence (of `ACGT` characters) as a vector of two-bit "
                "entries.\n\n This is a memory-efficient representation of an arbitrary length "
                "nucleotide sequence, using\n the usual conversion `A == 00`, `C == 01`, `G == "
                "10`, and `T == 11`.\n Use from_nucleic_acids() and to_nucleic_acids() to convert "
                "from and to a string.\n See also IteratorSubstitutions, IteratorInsertions, and "
                "IteratorDeletions for iterators that\n yield all single position differences "
                "(\"neighborhood\" or \"mirovariants\") of a given sequence." );
        // function-signature: genesis::utils::bit::TwobitVector::TwobitVector()()
        // file:genesis/utils/bit/twobit_vector.hpp line:110
        cl.def( pybind11::init( []() { return new genesis::utils::bit::TwobitVector(); } ) );
        // function-signature: genesis::utils::bit::TwobitVector::TwobitVector(unsigned
        // long)(unsigned long) file:genesis/utils/bit/twobit_vector.hpp line:115
        cl.def( pybind11::init<unsigned long>(), pybind11::arg( "size" ) );

        // function-signature: genesis::utils::bit::TwobitVector::TwobitVector(const class
        // genesis::utils::bit::TwobitVector &)(const class genesis::utils::bit::TwobitVector &)
        // file:genesis/utils/bit/twobit_vector.hpp line:119
        cl.def( pybind11::init( []( genesis::utils::bit::TwobitVector const& o ) {
            return new genesis::utils::bit::TwobitVector( o );
        } ) );

        pybind11::enum_<genesis::utils::bit::TwobitVector::ValueType>(
            cl,
            "ValueType",
            "Value Type enumeration for the elements of a TwobitVector.\n\n The values 0-3 are "
            "named `A`, `C`, `G` and `T`, respectively, in order to ease the\n use with nucleotide "
            "sequences.\n\n The underlying value of the enum is WordType, so that a cast does not "
            "need to\n convert internally."
        )
            .value( "A", genesis::utils::bit::TwobitVector::ValueType::A )
            .value( "C", genesis::utils::bit::TwobitVector::ValueType::C )
            .value( "G", genesis::utils::bit::TwobitVector::ValueType::G )
            .value( "T", genesis::utils::bit::TwobitVector::ValueType::T );

        // function-signature: class genesis::utils::bit::TwobitVector &
        // genesis::utils::bit::TwobitVector::operator=(const class
        // genesis::utils::bit::TwobitVector &)(const class genesis::utils::bit::TwobitVector &)
        // file:genesis/utils/bit/twobit_vector.hpp line:122
        cl.def(
            "assign",
            ( class genesis::utils::bit::TwobitVector &
              (genesis::utils::bit::TwobitVector::*)(const class genesis::utils::bit::TwobitVector&)
            ) & genesis::utils::bit::TwobitVector::operator=,
            "C++: genesis::utils::bit::TwobitVector::operator=(const class "
            "genesis::utils::bit::TwobitVector &) --> class genesis::utils::bit::TwobitVector &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: unsigned long genesis::utils::bit::TwobitVector::size() const()
        // file:genesis/utils/bit/twobit_vector.hpp line:134
        cl.def(
            "size",
            ( unsigned long ( genesis::utils::bit::TwobitVector::* )() const ) &
                genesis::utils::bit::TwobitVector::size,
            "Return the size of the vector, that is, how many values (of type ValueType)\n it "
            "currently holds.\n\nC++: genesis::utils::bit::TwobitVector::size() const --> unsigned "
            "long"
        );
        // function-signature: unsigned long genesis::utils::bit::TwobitVector::data_size() const()
        // file:genesis/utils/bit/twobit_vector.hpp line:140
        cl.def(
            "data_size",
            ( unsigned long ( genesis::utils::bit::TwobitVector::* )() const ) &
                genesis::utils::bit::TwobitVector::data_size,
            "Return the number of words (of type WordType) that are used to store the values\n in "
            "the vector.\n\nC++: genesis::utils::bit::TwobitVector::data_size() const --> unsigned "
            "long"
        );
        // function-signature: enum genesis::utils::bit::TwobitVector::ValueType
        // genesis::utils::bit::TwobitVector::get(unsigned long) const(unsigned long)
        // file:genesis/utils/bit/twobit_vector.hpp line:145
        cl.def(
            "get",
            ( enum genesis::utils::bit::TwobitVector::
                  ValueType( genesis::utils::bit::TwobitVector::* )( unsigned long ) const ) &
                genesis::utils::bit::TwobitVector::get,
            "Get the value at an  in the vector.\n\nC++: "
            "genesis::utils::bit::TwobitVector::get(unsigned long) const --> enum "
            "genesis::utils::bit::TwobitVector::ValueType",
            pybind11::arg( "index" )
        );
        // function-signature: enum genesis::utils::bit::TwobitVector::ValueType
        // genesis::utils::bit::TwobitVector::operator[](unsigned long) const(unsigned long)
        // file:genesis/utils/bit/twobit_vector.hpp line:150
        cl.def(
            "__getitem__",
            ( enum genesis::utils::bit::TwobitVector::
                  ValueType( genesis::utils::bit::TwobitVector::* )( unsigned long ) const ) &
                genesis::utils::bit::TwobitVector::operator[],
            "Get the value at an  in the vector.\n\nC++: "
            "genesis::utils::bit::TwobitVector::operator[](unsigned long) const --> enum "
            "genesis::utils::bit::TwobitVector::ValueType",
            pybind11::arg( "index" )
        );
        // function-signature: unsigned long & genesis::utils::bit::TwobitVector::data_at(unsigned
        // long)(unsigned long) file:genesis/utils/bit/twobit_vector.hpp line:166
        cl.def(
            "data_at",
            ( unsigned long& (genesis::utils::bit::TwobitVector::*)(unsigned long)) &
                genesis::utils::bit::TwobitVector::data_at,
            "Return a single word of the vector.\n\n This is useful for external functions that "
            "want to directly work on the underlying bit\n representation.\n\nC++: "
            "genesis::utils::bit::TwobitVector::data_at(unsigned long) --> unsigned long &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "index" )
        );
        // function-signature: unsigned long genesis::utils::bit::TwobitVector::hash() const()
        // file:genesis/utils/bit/twobit_vector.hpp line:174
        cl.def(
            "hash",
            ( unsigned long ( genesis::utils::bit::TwobitVector::* )() const ) &
                genesis::utils::bit::TwobitVector::hash,
            "Calculate a hash value of the vector, based on its size() and the xor of\n all its "
            "words.\n\n This is a simple function, but might just be enough for using it in a "
            "hashmap.\n\nC++: genesis::utils::bit::TwobitVector::hash() const --> unsigned long"
        );
        // function-signature: bool genesis::utils::bit::TwobitVector::operator==(const class
        // genesis::utils::bit::TwobitVector &) const(const class genesis::utils::bit::TwobitVector
        // &) file:genesis/utils/bit/twobit_vector.hpp line:180
        cl.def(
            "__eq__",
            ( bool( genesis::utils::bit::TwobitVector::* )( const class genesis::utils::bit::
                                                                TwobitVector& ) const ) &
                genesis::utils::bit::TwobitVector::operator==,
            "C++: genesis::utils::bit::TwobitVector::operator==(const class "
            "genesis::utils::bit::TwobitVector &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: bool genesis::utils::bit::TwobitVector::operator!=(const class
        // genesis::utils::bit::TwobitVector &) const(const class genesis::utils::bit::TwobitVector
        // &) file:genesis/utils/bit/twobit_vector.hpp line:181
        cl.def(
            "__ne__",
            ( bool( genesis::utils::bit::TwobitVector::* )( const class genesis::utils::bit::
                                                                TwobitVector& ) const ) &
                genesis::utils::bit::TwobitVector::operator!=,
            "C++: genesis::utils::bit::TwobitVector::operator!=(const class "
            "genesis::utils::bit::TwobitVector &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: bool genesis::utils::bit::TwobitVector::validate() const()
        // file:genesis/utils/bit/twobit_vector.hpp line:189
        cl.def(
            "validate",
            ( bool( genesis::utils::bit::TwobitVector::* )() const ) &
                genesis::utils::bit::TwobitVector::validate,
            "Validation function that checks some basic invariants.\n\n This is mainly useful in "
            "testing. The function checks whether the vector is correctly\n sized and contains "
            "zero padding at its end.\n\nC++: genesis::utils::bit::TwobitVector::validate() const "
            "--> bool"
        );
        // function-signature: void genesis::utils::bit::TwobitVector::set(unsigned long, enum
        // genesis::utils::bit::TwobitVector::ValueType)(unsigned long, enum
        // genesis::utils::bit::TwobitVector::ValueType) file:genesis/utils/bit/twobit_vector.hpp
        // line:198
        cl.def(
            "set",
            ( void( genesis::utils::bit::TwobitVector::* )(
                unsigned long, enum genesis::utils::bit::TwobitVector::ValueType
            ) ) &
                genesis::utils::bit::TwobitVector::set,
            "Set a value at a position in the vector.\n\nC++: "
            "genesis::utils::bit::TwobitVector::set(unsigned long, enum "
            "genesis::utils::bit::TwobitVector::ValueType) --> void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::bit::TwobitVector::insert_at(unsigned long, enum
        // genesis::utils::bit::TwobitVector::ValueType)(unsigned long, enum
        // genesis::utils::bit::TwobitVector::ValueType) file:genesis/utils/bit/twobit_vector.hpp
        // line:205
        cl.def(
            "insert_at",
            ( void( genesis::utils::bit::TwobitVector::* )(
                unsigned long, enum genesis::utils::bit::TwobitVector::ValueType
            ) ) &
                genesis::utils::bit::TwobitVector::insert_at,
            "Insert a value at a position.\n\n The size() is increased by one.\n\nC++: "
            "genesis::utils::bit::TwobitVector::insert_at(unsigned long, enum "
            "genesis::utils::bit::TwobitVector::ValueType) --> void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::utils::bit::TwobitVector::remove_at(unsigned
        // long)(unsigned long) file:genesis/utils/bit/twobit_vector.hpp line:212
        cl.def(
            "remove_at",
            ( void( genesis::utils::bit::TwobitVector::* )( unsigned long ) ) &
                genesis::utils::bit::TwobitVector::remove_at,
            "Remove the value at a position.\n\n The size() is decreased by one.\n\nC++: "
            "genesis::utils::bit::TwobitVector::remove_at(unsigned long) --> void",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::utils::bit::TwobitVector::clear()()
        // file:genesis/utils/bit/twobit_vector.hpp line:217
        cl.def(
            "clear",
            ( void( genesis::utils::bit::TwobitVector::* )() ) &
                genesis::utils::bit::TwobitVector::clear,
            "Clear the vector, so that it contains no data.\n\nC++: "
            "genesis::utils::bit::TwobitVector::clear() --> void"
        );

        genesis_utils_bit_twobit_vector_add_ons( cl );
    }
    // genesis::utils::bit::translate_from_nucleic_acid(char)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:51 function-signature: enum
    // genesis::utils::bit::TwobitVector::ValueType
    // genesis::utils::bit::translate_from_nucleic_acid(char)(char)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:51
    M( "genesis::utils::bit" )
        .def(
            "translate_from_nucleic_acid",
            ( enum genesis::utils::bit::TwobitVector::ValueType( * )( char ) ) &
                genesis::utils::bit::translate_from_nucleic_acid,
            "Translate a char into TwobitVector::ValueType.\n\n Valid chars are `A`, `C`, `G`, "
            "`T`, and their lower case variants.\n\nC++: "
            "genesis::utils::bit::translate_from_nucleic_acid(char) --> enum "
            "genesis::utils::bit::TwobitVector::ValueType",
            pybind11::arg( "site" )
        );

    // genesis::utils::bit::translate_to_nucleic_acid(enum
    // genesis::utils::bit::TwobitVector::ValueType)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:58 function-signature: char
    // genesis::utils::bit::translate_to_nucleic_acid(enum
    // genesis::utils::bit::TwobitVector::ValueType)(enum
    // genesis::utils::bit::TwobitVector::ValueType)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:58
    M( "genesis::utils::bit" )
        .def(
            "translate_to_nucleic_acid",
            ( char ( * )( enum genesis::utils::bit::TwobitVector::ValueType ) ) &
                genesis::utils::bit::translate_to_nucleic_acid,
            "Translate a TwobitVector::ValueType into its char representation.\n\n This gives one "
            "of the values `A`, `C`, `G` and `T`.\n\nC++: "
            "genesis::utils::bit::translate_to_nucleic_acid(enum "
            "genesis::utils::bit::TwobitVector::ValueType) --> char",
            pybind11::arg( "value" )
        );

    // genesis::utils::bit::from_nucleic_acids(const std::string &)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:63 function-signature: class
    // genesis::utils::bit::TwobitVector genesis::utils::bit::from_nucleic_acids(const std::string
    // &)(const std::string &) file:genesis/utils/bit/twobit_vector/functions.hpp line:63
    M( "genesis::utils::bit" )
        .def(
            "from_nucleic_acids",
            ( class genesis::utils::bit::TwobitVector( * )( const std::string& ) ) &
                genesis::utils::bit::from_nucleic_acids,
            "Turn a string of nucleic acids into a TwobitVector.\n\nC++: "
            "genesis::utils::bit::from_nucleic_acids(const std::string &) --> class "
            "genesis::utils::bit::TwobitVector",
            pybind11::arg( "sequence" )
        );

    // genesis::utils::bit::to_nucleic_acids(const class genesis::utils::bit::TwobitVector &)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:68 function-signature: std::string
    // genesis::utils::bit::to_nucleic_acids(const class genesis::utils::bit::TwobitVector &)(const
    // class genesis::utils::bit::TwobitVector &) file:genesis/utils/bit/twobit_vector/functions.hpp
    // line:68
    M( "genesis::utils::bit" )
        .def(
            "to_nucleic_acids",
            ( std::string( * )( const class genesis::utils::bit::TwobitVector& ) ) &
                genesis::utils::bit::to_nucleic_acids,
            "Turn a TwobitVector into its string representation of nucleic acids.\n\nC++: "
            "genesis::utils::bit::to_nucleic_acids(const class genesis::utils::bit::TwobitVector "
            "&) --> std::string",
            pybind11::arg( "vec" )
        );

    // genesis::utils::bit::bitstring(const class genesis::utils::bit::TwobitVector &)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:76 function-signature: std::string
    // genesis::utils::bit::bitstring(const class genesis::utils::bit::TwobitVector &)(const class
    // genesis::utils::bit::TwobitVector &) file:genesis/utils/bit/twobit_vector/functions.hpp
    // line:76
    M( "genesis::utils::bit" )
        .def(
            "bitstring",
            ( std::string( * )( const class genesis::utils::bit::TwobitVector& ) ) &
                genesis::utils::bit::bitstring,
            "Return a string with a bit-representation of a TwobitVector.\n\n It returns the words "
            "of the vector with bits in the order of the underlying integer type.\n This is mainly "
            "useful for debugging and testing.\n\nC++: genesis::utils::bit::bitstring(const class "
            "genesis::utils::bit::TwobitVector &) --> std::string",
            pybind11::arg( "vec" )
        );

    // genesis::utils::bit::bitstring(const unsigned long &)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:84 function-signature: std::string
    // genesis::utils::bit::bitstring(const unsigned long &)(const unsigned long &)
    // file:genesis/utils/bit/twobit_vector/functions.hpp line:84
    M( "genesis::utils::bit" )
        .def(
            "bitstring",
            ( std::string( * )( const unsigned long& ) ) & genesis::utils::bit::bitstring,
            "Return a string with a bit-representation of a TwobitVector::WordType.\n\n It returns "
            "the word with bits in the order of the underlying integer type.\n This is mainly "
            "useful for debugging and testing.\n\nC++: genesis::utils::bit::bitstring(const "
            "unsigned long &) --> std::string",
            pybind11::arg( "vec" )
        );
}
