#include <genesis/util/bit/twobit_vector.hpp>
#include <genesis/util/bit/twobit_vector/iterator_deletions.hpp>
#include <genesis/util/bit/twobit_vector/iterator_insertions.hpp>
#include <genesis/util/bit/twobit_vector/iterator_substitutions.hpp>
#include <genesis/util/bit/twobit_vector_add_ons.hpp>
#include <sstream> // __str__

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

void bind_genesis_util_bit_twobit_vector_iterator_deletions(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::bit::IteratorDeletions
      // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:59
        pybind11::class_<
            genesis::util::bit::IteratorDeletions,
            std::shared_ptr<genesis::util::bit::IteratorDeletions>>
            cl( M( "genesis::util::bit" ),
                "IteratorDeletions",
                "Take a TwobitVector sequence and iterate over all possible single-position "
                "deletions\n of its characters.\n\n In each step, this iterator yields a "
                "TwobitVector with a single deletion compared to the\n original, iterating through "
                "all positions in the sequence.\n\n Use iterate_deletions() to obtain instances of "
                "this class that can be used in a range-based\n for loop for convenience." );
        // function-signature: genesis::util::bit::IteratorDeletions::IteratorDeletions()()
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:75
        cl.def( pybind11::init( []() { return new genesis::util::bit::IteratorDeletions(); } ) );
        // function-signature: genesis::util::bit::IteratorDeletions::IteratorDeletions(const class
        // genesis::util::bit::TwobitVector &)(const class genesis::util::bit::TwobitVector &)
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:83
        cl.def(
            pybind11::init<const class genesis::util::bit::TwobitVector&>(),
            pybind11::arg( "vector" )
        );

        // function-signature: genesis::util::bit::IteratorDeletions::IteratorDeletions(const class
        // genesis::util::bit::IteratorDeletions &)(const class
        // genesis::util::bit::IteratorDeletions &)
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:98
        cl.def( pybind11::init( []( genesis::util::bit::IteratorDeletions const& o ) {
            return new genesis::util::bit::IteratorDeletions( o );
        } ) );
        // function-signature: class genesis::util::bit::IteratorDeletions &
        // genesis::util::bit::IteratorDeletions::operator=(const class
        // genesis::util::bit::IteratorDeletions &)(const class
        // genesis::util::bit::IteratorDeletions &)
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:101
        cl.def(
            "assign",
            ( class genesis::util::bit::IteratorDeletions &
              (genesis::util::bit::IteratorDeletions::*)(const class genesis::util::bit::
                                                             IteratorDeletions&)) &
                genesis::util::bit::IteratorDeletions::operator=,
            "C++: genesis::util::bit::IteratorDeletions::operator=(const class "
            "genesis::util::bit::IteratorDeletions &) --> class "
            "genesis::util::bit::IteratorDeletions &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: const class genesis::util::bit::TwobitVector &
        // genesis::util::bit::IteratorDeletions::operator*()()
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:108
        cl.def(
            "dereference",
            ( const class genesis::util::bit::
                  TwobitVector& (genesis::util::bit::IteratorDeletions::*)() ) &
                genesis::util::bit::IteratorDeletions::operator*,
            "C++: genesis::util::bit::IteratorDeletions::operator*() --> const class "
            "genesis::util::bit::TwobitVector &",
            pybind11::return_value_policy::copy
        );
        // function-signature: const class genesis::util::bit::TwobitVector *
        // genesis::util::bit::IteratorDeletions::operator->()()
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:113
        cl.def(
            "arrow",
            ( const class genesis::util::bit::
                  TwobitVector* (genesis::util::bit::IteratorDeletions::*)() ) &
                genesis::util::bit::IteratorDeletions::operator->,
            "C++: genesis::util::bit::IteratorDeletions::operator->() --> const class "
            "genesis::util::bit::TwobitVector *",
            pybind11::return_value_policy::copy
        );
        // function-signature: class genesis::util::bit::IteratorDeletions &
        // genesis::util::bit::IteratorDeletions::operator++()()
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:118
        cl.def(
            "pre_increment",
            ( class genesis::util::bit::IteratorDeletions &
              (genesis::util::bit::IteratorDeletions::*)() ) &
                genesis::util::bit::IteratorDeletions::operator++,
            "C++: genesis::util::bit::IteratorDeletions::operator++() --> class "
            "genesis::util::bit::IteratorDeletions &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: class genesis::util::bit::IteratorDeletions
        // genesis::util::bit::IteratorDeletions::operator++(int)(int)
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:158
        cl.def(
            "post_increment",
            ( class genesis::util::bit::
                  IteratorDeletions( genesis::util::bit::IteratorDeletions::* )( int ) ) &
                genesis::util::bit::IteratorDeletions::operator++,
            "C++: genesis::util::bit::IteratorDeletions::operator++(int) --> class "
            "genesis::util::bit::IteratorDeletions",
            pybind11::arg( "" )
        );
        // function-signature: bool genesis::util::bit::IteratorDeletions::operator==(const class
        // genesis::util::bit::IteratorDeletions &) const(const class
        // genesis::util::bit::IteratorDeletions &)
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:165
        cl.def(
            "__eq__",
            ( bool( genesis::util::bit::IteratorDeletions::* )( const class genesis::util::bit::
                                                                    IteratorDeletions& ) const ) &
                genesis::util::bit::IteratorDeletions::operator==,
            "C++: genesis::util::bit::IteratorDeletions::operator==(const class "
            "genesis::util::bit::IteratorDeletions &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: bool genesis::util::bit::IteratorDeletions::operator!=(const class
        // genesis::util::bit::IteratorDeletions &) const(const class
        // genesis::util::bit::IteratorDeletions &)
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:170
        cl.def(
            "__ne__",
            ( bool( genesis::util::bit::IteratorDeletions::* )( const class genesis::util::bit::
                                                                    IteratorDeletions& ) const ) &
                genesis::util::bit::IteratorDeletions::operator!=,
            "C++: genesis::util::bit::IteratorDeletions::operator!=(const class "
            "genesis::util::bit::IteratorDeletions &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: unsigned long genesis::util::bit::IteratorDeletions::position()
        // const() file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:182
        cl.def(
            "position",
            ( unsigned long ( genesis::util::bit::IteratorDeletions::* )() const ) &
                genesis::util::bit::IteratorDeletions::position,
            "Get the position that is currently deleted.\n\nC++: "
            "genesis::util::bit::IteratorDeletions::position() const --> unsigned long"
        );
        // function-signature: unsigned long genesis::util::bit::IteratorDeletions::hash() const()
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:190
        cl.def(
            "hash",
            ( unsigned long ( genesis::util::bit::IteratorDeletions::* )() const ) &
                genesis::util::bit::IteratorDeletions::hash,
            "Get the hash value of the current vector.\n\nC++: "
            "genesis::util::bit::IteratorDeletions::hash() const --> unsigned long"
        );
        // function-signature: const class genesis::util::bit::TwobitVector &
        // genesis::util::bit::IteratorDeletions::vector() const()
        // file:genesis/util/bit/twobit_vector/iterator_deletions.hpp line:198
        cl.def(
            "vector",
            ( const class genesis::util::bit::
                  TwobitVector& (genesis::util::bit::IteratorDeletions::*)() const ) &
                genesis::util::bit::IteratorDeletions::vector,
            "Get the current vector.\n\nC++: genesis::util::bit::IteratorDeletions::vector() const "
            "--> const class genesis::util::bit::TwobitVector &",
            pybind11::return_value_policy::copy
        );

        genesis_util_bit_iterator_add_ons( cl );
    }
    { // genesis::util::bit::IteratorInsertions
      // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:76
        pybind11::class_<
            genesis::util::bit::IteratorInsertions,
            std::shared_ptr<genesis::util::bit::IteratorInsertions>>
            cl( M( "genesis::util::bit" ),
                "IteratorInsertions",
                "Take a TwobitVector sequence and iterate over all possible single-position "
                "insertions\n into the sequence.\n\n In each step, this iterator yields a "
                "TwobitVector with a single insertion compared to the\n original, iterating "
                "through all positions in the sequence, and through all four possible\n insertions "
                "at that position.\n\n Example:\n\n     Original:\n     CAT\n\n     Iterations:\n  "
                "   ACAT, CCAT, GCAT, TCAT,  // insert in front\n     CAAT, CCAT, CGAT, CTAT,  // "
                "insert after first character\n     CAAT, CACT, CAGT, CATT,  // insert after "
                "second character\n     CATA, CATC, CATG, CATT   // insert after third "
                "character\n\n Note that this does not filter out duplicates that can occur due to "
                "repeated characters in the\n sequence. If this needs to be avoided, either a "
                "downstream filter (such as a hashmap) needs to\n be used, or the algorithm used "
                "here needs to be adapted.\n\n Use iterate_insertions() to obtain instances of "
                "this class that can be used in a range-based\n for loop for convenience." );
        // function-signature: genesis::util::bit::IteratorInsertions::IteratorInsertions()()
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:93
        cl.def( pybind11::init( []() { return new genesis::util::bit::IteratorInsertions(); } ) );
        // function-signature: genesis::util::bit::IteratorInsertions::IteratorInsertions(const
        // class genesis::util::bit::TwobitVector &)(const class genesis::util::bit::TwobitVector &)
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:101
        cl.def(
            pybind11::init<const class genesis::util::bit::TwobitVector&>(),
            pybind11::arg( "vector" )
        );

        // function-signature: genesis::util::bit::IteratorInsertions::IteratorInsertions(const
        // class genesis::util::bit::IteratorInsertions &)(const class
        // genesis::util::bit::IteratorInsertions &)
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:115
        cl.def( pybind11::init( []( genesis::util::bit::IteratorInsertions const& o ) {
            return new genesis::util::bit::IteratorInsertions( o );
        } ) );
        // function-signature: class genesis::util::bit::IteratorInsertions &
        // genesis::util::bit::IteratorInsertions::operator=(const class
        // genesis::util::bit::IteratorInsertions &)(const class
        // genesis::util::bit::IteratorInsertions &)
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:118
        cl.def(
            "assign",
            ( class genesis::util::bit::IteratorInsertions &
              (genesis::util::bit::IteratorInsertions::*)(const class genesis::util::bit::
                                                              IteratorInsertions&)) &
                genesis::util::bit::IteratorInsertions::operator=,
            "C++: genesis::util::bit::IteratorInsertions::operator=(const class "
            "genesis::util::bit::IteratorInsertions &) --> class "
            "genesis::util::bit::IteratorInsertions &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: const class genesis::util::bit::TwobitVector &
        // genesis::util::bit::IteratorInsertions::operator*()()
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:125
        cl.def(
            "dereference",
            ( const class genesis::util::bit::
                  TwobitVector& (genesis::util::bit::IteratorInsertions::*)() ) &
                genesis::util::bit::IteratorInsertions::operator*,
            "C++: genesis::util::bit::IteratorInsertions::operator*() --> const class "
            "genesis::util::bit::TwobitVector &",
            pybind11::return_value_policy::copy
        );
        // function-signature: const class genesis::util::bit::TwobitVector *
        // genesis::util::bit::IteratorInsertions::operator->()()
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:130
        cl.def(
            "arrow",
            ( const class genesis::util::bit::
                  TwobitVector* (genesis::util::bit::IteratorInsertions::*)() ) &
                genesis::util::bit::IteratorInsertions::operator->,
            "C++: genesis::util::bit::IteratorInsertions::operator->() --> const class "
            "genesis::util::bit::TwobitVector *",
            pybind11::return_value_policy::copy
        );
        // function-signature: class genesis::util::bit::IteratorInsertions &
        // genesis::util::bit::IteratorInsertions::operator++()()
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:135
        cl.def(
            "pre_increment",
            ( class genesis::util::bit::IteratorInsertions &
              (genesis::util::bit::IteratorInsertions::*)() ) &
                genesis::util::bit::IteratorInsertions::operator++,
            "C++: genesis::util::bit::IteratorInsertions::operator++() --> class "
            "genesis::util::bit::IteratorInsertions &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: class genesis::util::bit::IteratorInsertions
        // genesis::util::bit::IteratorInsertions::operator++(int)(int)
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:207
        cl.def(
            "post_increment",
            ( class genesis::util::bit::
                  IteratorInsertions( genesis::util::bit::IteratorInsertions::* )( int ) ) &
                genesis::util::bit::IteratorInsertions::operator++,
            "C++: genesis::util::bit::IteratorInsertions::operator++(int) --> class "
            "genesis::util::bit::IteratorInsertions",
            pybind11::arg( "" )
        );
        // function-signature: bool genesis::util::bit::IteratorInsertions::operator==(const class
        // genesis::util::bit::IteratorInsertions &) const(const class
        // genesis::util::bit::IteratorInsertions &)
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:214
        cl.def(
            "__eq__",
            ( bool( genesis::util::bit::IteratorInsertions::* )( const class genesis::util::bit::
                                                                     IteratorInsertions& ) const ) &
                genesis::util::bit::IteratorInsertions::operator==,
            "C++: genesis::util::bit::IteratorInsertions::operator==(const class "
            "genesis::util::bit::IteratorInsertions &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: bool genesis::util::bit::IteratorInsertions::operator!=(const class
        // genesis::util::bit::IteratorInsertions &) const(const class
        // genesis::util::bit::IteratorInsertions &)
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:219
        cl.def(
            "__ne__",
            ( bool( genesis::util::bit::IteratorInsertions::* )( const class genesis::util::bit::
                                                                     IteratorInsertions& ) const ) &
                genesis::util::bit::IteratorInsertions::operator!=,
            "C++: genesis::util::bit::IteratorInsertions::operator!=(const class "
            "genesis::util::bit::IteratorInsertions &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: unsigned long genesis::util::bit::IteratorInsertions::position()
        // const() file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:231
        cl.def(
            "position",
            ( unsigned long ( genesis::util::bit::IteratorInsertions::* )() const ) &
                genesis::util::bit::IteratorInsertions::position,
            "Get the position that is currently deleted.\n\nC++: "
            "genesis::util::bit::IteratorInsertions::position() const --> unsigned long"
        );
        // function-signature: unsigned long genesis::util::bit::IteratorInsertions::hash() const()
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:239
        cl.def(
            "hash",
            ( unsigned long ( genesis::util::bit::IteratorInsertions::* )() const ) &
                genesis::util::bit::IteratorInsertions::hash,
            "Get the hash value of the current vector.\n\nC++: "
            "genesis::util::bit::IteratorInsertions::hash() const --> unsigned long"
        );
        // function-signature: const class genesis::util::bit::TwobitVector &
        // genesis::util::bit::IteratorInsertions::vector() const()
        // file:genesis/util/bit/twobit_vector/iterator_insertions.hpp line:247
        cl.def(
            "vector",
            ( const class genesis::util::bit::
                  TwobitVector& (genesis::util::bit::IteratorInsertions::*)() const ) &
                genesis::util::bit::IteratorInsertions::vector,
            "Get the current vector.\n\nC++: genesis::util::bit::IteratorInsertions::vector() "
            "const --> const class genesis::util::bit::TwobitVector &",
            pybind11::return_value_policy::copy
        );

        genesis_util_bit_iterator_add_ons( cl );
    }
    { // genesis::util::bit::IteratorSubstitutions
      // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:60
        pybind11::class_<
            genesis::util::bit::IteratorSubstitutions,
            std::shared_ptr<genesis::util::bit::IteratorSubstitutions>>
            cl( M( "genesis::util::bit" ),
                "IteratorSubstitutions",
                "Take a TwobitVector sequence and iterate over all possible single-position "
                "substitutions\n of its characters.\n\n In each step, this iterator yields a "
                "TwobitVector with a single substitution compared to the\n original, iterating "
                "through all positions in the sequence, and all three possible substitutions\n at "
                "that position.\n\n Use iterate_substitutions() to obtain instances of this class "
                "that can be used in a range-based\n for loop for convenience." );
        // function-signature: genesis::util::bit::IteratorSubstitutions::IteratorSubstitutions()()
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:77
        cl.def( pybind11::init( []() { return new genesis::util::bit::IteratorSubstitutions(); } )
        );
        // function-signature:
        // genesis::util::bit::IteratorSubstitutions::IteratorSubstitutions(const class
        // genesis::util::bit::TwobitVector &)(const class genesis::util::bit::TwobitVector &)
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:85
        cl.def(
            pybind11::init<const class genesis::util::bit::TwobitVector&>(),
            pybind11::arg( "vector" )
        );

        // function-signature:
        // genesis::util::bit::IteratorSubstitutions::IteratorSubstitutions(const class
        // genesis::util::bit::IteratorSubstitutions &)(const class
        // genesis::util::bit::IteratorSubstitutions &)
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:99
        cl.def( pybind11::init( []( genesis::util::bit::IteratorSubstitutions const& o ) {
            return new genesis::util::bit::IteratorSubstitutions( o );
        } ) );
        // function-signature: class genesis::util::bit::IteratorSubstitutions &
        // genesis::util::bit::IteratorSubstitutions::operator=(const class
        // genesis::util::bit::IteratorSubstitutions &)(const class
        // genesis::util::bit::IteratorSubstitutions &)
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:102
        cl.def(
            "assign",
            ( class genesis::util::bit::IteratorSubstitutions &
              (genesis::util::bit::IteratorSubstitutions::*)(const class genesis::util::bit::
                                                                 IteratorSubstitutions&)) &
                genesis::util::bit::IteratorSubstitutions::operator=,
            "C++: genesis::util::bit::IteratorSubstitutions::operator=(const class "
            "genesis::util::bit::IteratorSubstitutions &) --> class "
            "genesis::util::bit::IteratorSubstitutions &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: const class genesis::util::bit::TwobitVector &
        // genesis::util::bit::IteratorSubstitutions::operator*()()
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:109
        cl.def(
            "dereference",
            ( const class genesis::util::bit::
                  TwobitVector& (genesis::util::bit::IteratorSubstitutions::*)() ) &
                genesis::util::bit::IteratorSubstitutions::operator*,
            "C++: genesis::util::bit::IteratorSubstitutions::operator*() --> const class "
            "genesis::util::bit::TwobitVector &",
            pybind11::return_value_policy::copy
        );
        // function-signature: const class genesis::util::bit::TwobitVector *
        // genesis::util::bit::IteratorSubstitutions::operator->()()
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:114
        cl.def(
            "arrow",
            ( const class genesis::util::bit::
                  TwobitVector* (genesis::util::bit::IteratorSubstitutions::*)() ) &
                genesis::util::bit::IteratorSubstitutions::operator->,
            "C++: genesis::util::bit::IteratorSubstitutions::operator->() --> const class "
            "genesis::util::bit::TwobitVector *",
            pybind11::return_value_policy::copy
        );
        // function-signature: class genesis::util::bit::IteratorSubstitutions &
        // genesis::util::bit::IteratorSubstitutions::operator++()()
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:119
        cl.def(
            "pre_increment",
            ( class genesis::util::bit::IteratorSubstitutions &
              (genesis::util::bit::IteratorSubstitutions::*)() ) &
                genesis::util::bit::IteratorSubstitutions::operator++,
            "C++: genesis::util::bit::IteratorSubstitutions::operator++() --> class "
            "genesis::util::bit::IteratorSubstitutions &",
            pybind11::return_value_policy::reference_internal
        );
        // function-signature: class genesis::util::bit::IteratorSubstitutions
        // genesis::util::bit::IteratorSubstitutions::operator++(int)(int)
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:181
        cl.def(
            "post_increment",
            ( class genesis::util::bit::
                  IteratorSubstitutions( genesis::util::bit::IteratorSubstitutions::* )( int ) ) &
                genesis::util::bit::IteratorSubstitutions::operator++,
            "C++: genesis::util::bit::IteratorSubstitutions::operator++(int) --> class "
            "genesis::util::bit::IteratorSubstitutions",
            pybind11::arg( "" )
        );
        // function-signature: bool genesis::util::bit::IteratorSubstitutions::operator==(const
        // class genesis::util::bit::IteratorSubstitutions &) const(const class
        // genesis::util::bit::IteratorSubstitutions &)
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:188
        cl.def(
            "__eq__",
            ( bool( genesis::util::bit::IteratorSubstitutions::* )( const class genesis::util::bit::
                                                                        IteratorSubstitutions& )
                  const ) &
                genesis::util::bit::IteratorSubstitutions::operator==,
            "C++: genesis::util::bit::IteratorSubstitutions::operator==(const class "
            "genesis::util::bit::IteratorSubstitutions &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: bool genesis::util::bit::IteratorSubstitutions::operator!=(const
        // class genesis::util::bit::IteratorSubstitutions &) const(const class
        // genesis::util::bit::IteratorSubstitutions &)
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:193
        cl.def(
            "__ne__",
            ( bool( genesis::util::bit::IteratorSubstitutions::* )( const class genesis::util::bit::
                                                                        IteratorSubstitutions& )
                  const ) &
                genesis::util::bit::IteratorSubstitutions::operator!=,
            "C++: genesis::util::bit::IteratorSubstitutions::operator!=(const class "
            "genesis::util::bit::IteratorSubstitutions &) const --> bool",
            pybind11::arg( "other" )
        );
        // function-signature: unsigned long genesis::util::bit::IteratorSubstitutions::position()
        // const() file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:205
        cl.def(
            "position",
            ( unsigned long ( genesis::util::bit::IteratorSubstitutions::* )() const ) &
                genesis::util::bit::IteratorSubstitutions::position,
            "Get the position that is currently deleted.\n\nC++: "
            "genesis::util::bit::IteratorSubstitutions::position() const --> unsigned long"
        );
        // function-signature: unsigned long genesis::util::bit::IteratorSubstitutions::hash()
        // const() file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:213
        cl.def(
            "hash",
            ( unsigned long ( genesis::util::bit::IteratorSubstitutions::* )() const ) &
                genesis::util::bit::IteratorSubstitutions::hash,
            "Get the hash value of the current vector.\n\nC++: "
            "genesis::util::bit::IteratorSubstitutions::hash() const --> unsigned long"
        );
        // function-signature: const class genesis::util::bit::TwobitVector &
        // genesis::util::bit::IteratorSubstitutions::vector() const()
        // file:genesis/util/bit/twobit_vector/iterator_substitutions.hpp line:221
        cl.def(
            "vector",
            ( const class genesis::util::bit::
                  TwobitVector& (genesis::util::bit::IteratorSubstitutions::*)() const ) &
                genesis::util::bit::IteratorSubstitutions::vector,
            "Get the current vector.\n\nC++: genesis::util::bit::IteratorSubstitutions::vector() "
            "const --> const class genesis::util::bit::TwobitVector &",
            pybind11::return_value_policy::copy
        );

        genesis_util_bit_iterator_add_ons( cl );
    }
}
