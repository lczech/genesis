#include <genesis/util/container/bitpacked_vector.hpp>
#include <genesis/util/container/bitpacked_vector_instances.hpp>
#include <iterator>
#include <memory>
#include <sstream> // __str__
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

void bind_genesis_util_container_bitpacked_vector_instances(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::container::BitpackedVector
      // file:genesis/util/container/bitpacked_vector_instances.hpp line:37
        pybind11::class_<
            genesis::util::container::BitpackedVector<unsigned char, unsigned char>,
            std::shared_ptr<
                genesis::util::container::BitpackedVector<unsigned char, unsigned char>>>
            cl( M( "genesis::util::container" ), "BitpackedVector_uint8_t", "" );
        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // char>::BitpackedVector()() file:genesis/util/container/bitpacked_vector.hpp line:90
        cl.def( pybind11::init( []() {
            return new genesis::util::container::BitpackedVector<unsigned char, unsigned char>();
        } ) );
        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // char>::BitpackedVector(unsigned long, unsigned long)(unsigned long, unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:96
        cl.def(
            pybind11::init<unsigned long, unsigned long>(),
            pybind11::arg( "size" ),
            pybind11::arg( "bit_width" )
        );

        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // char>::BitpackedVector(const class genesis::util::container::BitpackedVector<uint8_t>
        // &)(const class genesis::util::container::BitpackedVector<uint8_t> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:118
        cl.def( pybind11::init(
            []( genesis::util::container::BitpackedVector<unsigned char, unsigned char> const& o ) {
                return new genesis::util::container::BitpackedVector<unsigned char, unsigned char>(
                    o
                );
            }
        ) );
        // function-signature: class genesis::util::container::BitpackedVector<uint8_t> &
        // genesis::util::container::BitpackedVector<unsigned char>::operator=(const class
        // genesis::util::container::BitpackedVector<uint8_t> &)(const class
        // genesis::util::container::BitpackedVector<uint8_t> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:121
        cl.def(
            "assign",
            ( class genesis::util::container::BitpackedVector<uint8_t> &
              (genesis::util::container::BitpackedVector<
                  unsigned char,
                  unsigned char>::*)(const class genesis::util::container::
                                         BitpackedVector<uint8_t>&)) &
                genesis::util::container::BitpackedVector<unsigned char>::operator=,
            "C++: genesis::util::container::BitpackedVector<unsigned char>::operator=(const class "
            "genesis::util::container::BitpackedVector<uint8_t> &) --> class "
            "genesis::util::container::BitpackedVector<uint8_t> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: unsigned char genesis::util::container::BitpackedVector<unsigned
        // char>::get(unsigned long) const(unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:131
        cl.def(
            "get",
            ( unsigned char ( genesis::util::container::
                                  BitpackedVector<unsigned char, unsigned char>::* )( unsigned long
            ) const ) &
                genesis::util::container::BitpackedVector<unsigned char>::get,
            "Retrieve an element from the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<unsigned char>::get(unsigned long) const "
            "--> unsigned char",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::util::container::BitpackedVector<unsigned
        // char>::set(unsigned long, unsigned char)(unsigned long, unsigned char)
        // file:genesis/util/container/bitpacked_vector.hpp line:170
        cl.def(
            "set",
            ( void( genesis::util::container::BitpackedVector<unsigned char, unsigned char>::* )(
                unsigned long, unsigned char
            ) ) &
                genesis::util::container::BitpackedVector<unsigned char>::set,
            "Set an element in the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<unsigned char>::set(unsigned long, unsigned "
            "char) --> void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<unsigned
        // char>::size() const() file:genesis/util/container/bitpacked_vector.hpp line:217
        cl.def(
            "size",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned char, unsigned char>::* )() const ) &
                genesis::util::container::BitpackedVector<unsigned char>::size,
            "C++: genesis::util::container::BitpackedVector<unsigned char>::size() const --> "
            "unsigned long"
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<unsigned
        // char>::bit_width() const() file:genesis/util/container/bitpacked_vector.hpp line:222
        cl.def(
            "bit_width",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned char, unsigned char>::* )() const ) &
                genesis::util::container::BitpackedVector<unsigned char>::bit_width,
            "C++: genesis::util::container::BitpackedVector<unsigned char>::bit_width() const --> "
            "unsigned long"
        );
        // function-signature: const class std::vector<unsigned char> &
        // genesis::util::container::BitpackedVector<unsigned char>::data() const()
        // file:genesis/util/container/bitpacked_vector.hpp line:227
        cl.def(
            "data",
            ( const class std::vector<unsigned char>& (genesis::util::container::BitpackedVector<
                                                       unsigned char,
                                                       unsigned char>::*)() const ) &
                genesis::util::container::BitpackedVector<unsigned char>::data,
            "C++: genesis::util::container::BitpackedVector<unsigned char>::data() const --> const "
            "class std::vector<unsigned char> &",
            pybind11::return_value_policy::reference_internal
        );
    }
    { // genesis::util::container::BitpackedVector
      // file:genesis/util/container/bitpacked_vector_instances.hpp line:38
        pybind11::class_<
            genesis::util::container::BitpackedVector<unsigned short, unsigned short>,
            std::shared_ptr<
                genesis::util::container::BitpackedVector<unsigned short, unsigned short>>>
            cl( M( "genesis::util::container" ), "BitpackedVector_uint16_t", "" );
        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // short>::BitpackedVector()() file:genesis/util/container/bitpacked_vector.hpp line:90
        cl.def( pybind11::init( []() {
            return new genesis::util::container::BitpackedVector<unsigned short, unsigned short>();
        } ) );
        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // short>::BitpackedVector(unsigned long, unsigned long)(unsigned long, unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:96
        cl.def(
            pybind11::init<unsigned long, unsigned long>(),
            pybind11::arg( "size" ),
            pybind11::arg( "bit_width" )
        );

        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // short>::BitpackedVector(const class genesis::util::container::BitpackedVector<uint16_t>
        // &)(const class genesis::util::container::BitpackedVector<uint16_t> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:118
        cl.def( pybind11::init( []( genesis::util::container::
                                        BitpackedVector<unsigned short, unsigned short> const& o ) {
            return new genesis::util::container::BitpackedVector<unsigned short, unsigned short>( o
            );
        } ) );
        // function-signature: class genesis::util::container::BitpackedVector<uint16_t> &
        // genesis::util::container::BitpackedVector<unsigned short>::operator=(const class
        // genesis::util::container::BitpackedVector<uint16_t> &)(const class
        // genesis::util::container::BitpackedVector<uint16_t> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:121
        cl.def(
            "assign",
            ( class genesis::util::container::BitpackedVector<uint16_t> &
              (genesis::util::container::BitpackedVector<
                  unsigned short,
                  unsigned short>::*)(const class genesis::util::container::
                                          BitpackedVector<uint16_t>&)) &
                genesis::util::container::BitpackedVector<unsigned short>::operator=,
            "C++: genesis::util::container::BitpackedVector<unsigned short>::operator=(const class "
            "genesis::util::container::BitpackedVector<uint16_t> &) --> class "
            "genesis::util::container::BitpackedVector<uint16_t> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: unsigned short genesis::util::container::BitpackedVector<unsigned
        // short>::get(unsigned long) const(unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:131
        cl.def(
            "get",
            ( unsigned short ( genesis::util::container::BitpackedVector<
                               unsigned short,
                               unsigned short>::* )( unsigned long ) const ) &
                genesis::util::container::BitpackedVector<unsigned short>::get,
            "Retrieve an element from the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<unsigned short>::get(unsigned long) const "
            "--> unsigned short",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::util::container::BitpackedVector<unsigned
        // short>::set(unsigned long, unsigned short)(unsigned long, unsigned short)
        // file:genesis/util/container/bitpacked_vector.hpp line:170
        cl.def(
            "set",
            ( void( genesis::util::container::BitpackedVector<unsigned short, unsigned short>::* )(
                unsigned long, unsigned short
            ) ) &
                genesis::util::container::BitpackedVector<unsigned short>::set,
            "Set an element in the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<unsigned short>::set(unsigned long, "
            "unsigned short) --> void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<unsigned
        // short>::size() const() file:genesis/util/container/bitpacked_vector.hpp line:217
        cl.def(
            "size",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned short, unsigned short>::* )() const ) &
                genesis::util::container::BitpackedVector<unsigned short>::size,
            "C++: genesis::util::container::BitpackedVector<unsigned short>::size() const --> "
            "unsigned long"
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<unsigned
        // short>::bit_width() const() file:genesis/util/container/bitpacked_vector.hpp line:222
        cl.def(
            "bit_width",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned short, unsigned short>::* )() const ) &
                genesis::util::container::BitpackedVector<unsigned short>::bit_width,
            "C++: genesis::util::container::BitpackedVector<unsigned short>::bit_width() const --> "
            "unsigned long"
        );
        // function-signature: const class std::vector<unsigned short> &
        // genesis::util::container::BitpackedVector<unsigned short>::data() const()
        // file:genesis/util/container/bitpacked_vector.hpp line:227
        cl.def(
            "data",
            ( const class std::vector<unsigned short>& (genesis::util::container::BitpackedVector<
                                                        unsigned short,
                                                        unsigned short>::*)() const ) &
                genesis::util::container::BitpackedVector<unsigned short>::data,
            "C++: genesis::util::container::BitpackedVector<unsigned short>::data() const --> "
            "const class std::vector<unsigned short> &",
            pybind11::return_value_policy::reference_internal
        );
    }
    { // genesis::util::container::BitpackedVector
      // file:genesis/util/container/bitpacked_vector_instances.hpp line:39
        pybind11::class_<
            genesis::util::container::BitpackedVector<unsigned int, unsigned int>,
            std::shared_ptr<genesis::util::container::BitpackedVector<unsigned int, unsigned int>>>
            cl( M( "genesis::util::container" ), "BitpackedVector_uint32_t", "" );
        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // int>::BitpackedVector()() file:genesis/util/container/bitpacked_vector.hpp line:90
        cl.def( pybind11::init( []() {
            return new genesis::util::container::BitpackedVector<unsigned int, unsigned int>();
        } ) );
        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // int>::BitpackedVector(unsigned long, unsigned long)(unsigned long, unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:96
        cl.def(
            pybind11::init<unsigned long, unsigned long>(),
            pybind11::arg( "size" ),
            pybind11::arg( "bit_width" )
        );

        // function-signature: genesis::util::container::BitpackedVector<unsigned
        // int>::BitpackedVector(const class genesis::util::container::BitpackedVector<uint32_t>
        // &)(const class genesis::util::container::BitpackedVector<uint32_t> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:118
        cl.def( pybind11::init(
            []( genesis::util::container::BitpackedVector<unsigned int, unsigned int> const& o ) {
                return new genesis::util::container::BitpackedVector<unsigned int, unsigned int>( o
                );
            }
        ) );
        // function-signature: class genesis::util::container::BitpackedVector<uint32_t> &
        // genesis::util::container::BitpackedVector<unsigned int>::operator=(const class
        // genesis::util::container::BitpackedVector<uint32_t> &)(const class
        // genesis::util::container::BitpackedVector<uint32_t> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:121
        cl.def(
            "assign",
            ( class genesis::util::container::BitpackedVector<uint32_t> &
              (genesis::util::container::BitpackedVector<
                  unsigned int,
                  unsigned int>::*)(const class genesis::util::container::
                                        BitpackedVector<uint32_t>&)) &
                genesis::util::container::BitpackedVector<unsigned int>::operator=,
            "C++: genesis::util::container::BitpackedVector<unsigned int>::operator=(const class "
            "genesis::util::container::BitpackedVector<uint32_t> &) --> class "
            "genesis::util::container::BitpackedVector<uint32_t> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: unsigned int genesis::util::container::BitpackedVector<unsigned
        // int>::get(unsigned long) const(unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:131
        cl.def(
            "get",
            ( unsigned int ( genesis::util::container::
                                 BitpackedVector<unsigned int, unsigned int>::* )( unsigned long )
                  const ) &
                genesis::util::container::BitpackedVector<unsigned int>::get,
            "Retrieve an element from the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<unsigned int>::get(unsigned long) const --> "
            "unsigned int",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::util::container::BitpackedVector<unsigned
        // int>::set(unsigned long, unsigned int)(unsigned long, unsigned int)
        // file:genesis/util/container/bitpacked_vector.hpp line:170
        cl.def(
            "set",
            ( void( genesis::util::container::BitpackedVector<unsigned int, unsigned int>::* )(
                unsigned long, unsigned int
            ) ) &
                genesis::util::container::BitpackedVector<unsigned int>::set,
            "Set an element in the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<unsigned int>::set(unsigned long, unsigned "
            "int) --> void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<unsigned
        // int>::size() const() file:genesis/util/container/bitpacked_vector.hpp line:217
        cl.def(
            "size",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned int, unsigned int>::* )() const ) &
                genesis::util::container::BitpackedVector<unsigned int>::size,
            "C++: genesis::util::container::BitpackedVector<unsigned int>::size() const --> "
            "unsigned long"
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<unsigned
        // int>::bit_width() const() file:genesis/util/container/bitpacked_vector.hpp line:222
        cl.def(
            "bit_width",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned int, unsigned int>::* )() const ) &
                genesis::util::container::BitpackedVector<unsigned int>::bit_width,
            "C++: genesis::util::container::BitpackedVector<unsigned int>::bit_width() const --> "
            "unsigned long"
        );
        // function-signature: const class std::vector<unsigned int> &
        // genesis::util::container::BitpackedVector<unsigned int>::data() const()
        // file:genesis/util/container/bitpacked_vector.hpp line:227
        cl.def(
            "data",
            ( const class std::vector<unsigned int>& (genesis::util::container::BitpackedVector<
                                                      unsigned int,
                                                      unsigned int>::*)() const ) &
                genesis::util::container::BitpackedVector<unsigned int>::data,
            "C++: genesis::util::container::BitpackedVector<unsigned int>::data() const --> const "
            "class std::vector<unsigned int> &",
            pybind11::return_value_policy::reference_internal
        );
    }
    { // genesis::util::container::BitpackedVector
      // file:genesis/util/container/bitpacked_vector_instances.hpp line:40
        pybind11::class_<
            genesis::util::container::BitpackedVector<unsigned long, unsigned long>,
            std::shared_ptr<
                genesis::util::container::BitpackedVector<unsigned long, unsigned long>>>
            cl( M( "genesis::util::container" ), "BitpackedVector_uint64_t", "" );
        // function-signature: genesis::util::container::BitpackedVector<>::BitpackedVector()()
        // file:genesis/util/container/bitpacked_vector.hpp line:90
        cl.def( pybind11::init( []() {
            return new genesis::util::container::BitpackedVector<unsigned long, unsigned long>();
        } ) );
        // function-signature: genesis::util::container::BitpackedVector<>::BitpackedVector(unsigned
        // long, unsigned long)(unsigned long, unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:96
        cl.def(
            pybind11::init<unsigned long, unsigned long>(),
            pybind11::arg( "size" ),
            pybind11::arg( "bit_width" )
        );

        // function-signature: genesis::util::container::BitpackedVector<>::BitpackedVector(const
        // class genesis::util::container::BitpackedVector<> &)(const class
        // genesis::util::container::BitpackedVector<> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:118
        cl.def( pybind11::init(
            []( genesis::util::container::BitpackedVector<unsigned long, unsigned long> const& o ) {
                return new genesis::util::container::BitpackedVector<unsigned long, unsigned long>(
                    o
                );
            }
        ) );
        // function-signature: class genesis::util::container::BitpackedVector<> &
        // genesis::util::container::BitpackedVector<>::operator=(const class
        // genesis::util::container::BitpackedVector<> &)(const class
        // genesis::util::container::BitpackedVector<> &)
        // file:genesis/util/container/bitpacked_vector.hpp line:121
        cl.def(
            "assign",
            ( class genesis::util::container::BitpackedVector<> &
              (genesis::util::container::BitpackedVector<
                  unsigned long,
                  unsigned long>::*)(const class genesis::util::container::BitpackedVector<>&)) &
                genesis::util::container::BitpackedVector<>::operator=,
            "C++: genesis::util::container::BitpackedVector<>::operator=(const class "
            "genesis::util::container::BitpackedVector<> &) --> class "
            "genesis::util::container::BitpackedVector<> &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "" )
        );
        // function-signature: unsigned long
        // genesis::util::container::BitpackedVector<>::get(unsigned long) const(unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:131
        cl.def(
            "get",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned long, unsigned long>::* )( unsigned long
            ) const ) &
                genesis::util::container::BitpackedVector<>::get,
            "Retrieve an element from the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<>::get(unsigned long) const --> unsigned "
            "long",
            pybind11::arg( "index" )
        );
        // function-signature: void genesis::util::container::BitpackedVector<>::set(unsigned long,
        // unsigned long)(unsigned long, unsigned long)
        // file:genesis/util/container/bitpacked_vector.hpp line:170
        cl.def(
            "set",
            ( void( genesis::util::container::BitpackedVector<unsigned long, unsigned long>::* )(
                unsigned long, unsigned long
            ) ) &
                genesis::util::container::BitpackedVector<>::set,
            "Set an element in the vector.\n\nC++: "
            "genesis::util::container::BitpackedVector<>::set(unsigned long, unsigned long) --> "
            "void",
            pybind11::arg( "index" ),
            pybind11::arg( "value" )
        );
        // function-signature: unsigned long genesis::util::container::BitpackedVector<>::size()
        // const() file:genesis/util/container/bitpacked_vector.hpp line:217
        cl.def(
            "size",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned long, unsigned long>::* )() const ) &
                genesis::util::container::BitpackedVector<>::size,
            "C++: genesis::util::container::BitpackedVector<>::size() const --> unsigned long"
        );
        // function-signature: unsigned long
        // genesis::util::container::BitpackedVector<>::bit_width() const()
        // file:genesis/util/container/bitpacked_vector.hpp line:222
        cl.def(
            "bit_width",
            ( unsigned long ( genesis::util::container::
                                  BitpackedVector<unsigned long, unsigned long>::* )() const ) &
                genesis::util::container::BitpackedVector<>::bit_width,
            "C++: genesis::util::container::BitpackedVector<>::bit_width() const --> unsigned long"
        );
        // function-signature: const class std::vector<unsigned long> &
        // genesis::util::container::BitpackedVector<>::data() const()
        // file:genesis/util/container/bitpacked_vector.hpp line:227
        cl.def(
            "data",
            ( const class std::vector<unsigned long>& (genesis::util::container::BitpackedVector<
                                                       unsigned long,
                                                       unsigned long>::*)() const ) &
                genesis::util::container::BitpackedVector<>::data,
            "C++: genesis::util::container::BitpackedVector<>::data() const --> const class "
            "std::vector<unsigned long> &",
            pybind11::return_value_policy::reference_internal
        );
    }
}
