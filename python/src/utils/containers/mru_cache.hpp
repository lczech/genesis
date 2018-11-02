/**
 * @brief
 *
 * @file
 * @ingroup python
 */

#include <src/common.hpp>

#include "genesis/genesis.hpp"

template <typename Key, typename T>
void PythonExportClass_::genesis::utils::MruCache(std::string name)
{

    // -------------------------------------------------------------------
    //     Class MruCache
    // -------------------------------------------------------------------

    using namespace ::genesis::utils;

    using MruCacheType = MruCache<typename Key, typename T>;

    pybind11::class_< MruCacheType, std::shared_ptr<MruCacheType> > ( scope, name.c_str() )
        .def(
            pybind11::init<  >(),
            get_docstring("::genesis::utils::MruCache::MruCache ()")
        )
        .def(
            pybind11::init< size_t >(),
            pybind11::arg("capacity"),
            get_docstring("::genesis::utils::MruCache::MruCache (size_t capacity)")
        )
        .def(
            pybind11::init< MruCache const & >(),
            pybind11::arg("arg")
        )

        // Public Member Functions

        .def(
            "capacity",
            ( size_type ( MruCacheType::* )(  ) const )( &MruCacheType::capacity ),
            get_docstring("size_type ::genesis::utils::MruCache::capacity () const")
        )
        .def(
            "capacity",
            ( void ( MruCacheType::* )( size_t ))( &MruCacheType::capacity ),
            pybind11::arg("value"),
            get_docstring("void ::genesis::utils::MruCache::capacity (size_t value)")
        )
        .def(
            "cbegin",
            ( const_iterator ( MruCacheType::* )(  ))( &MruCacheType::cbegin ),
            get_docstring("const_iterator ::genesis::utils::MruCache::cbegin ()")
        )
        .def(
            "cend",
            ( const_iterator ( MruCacheType::* )(  ))( &MruCacheType::cend ),
            get_docstring("const_iterator ::genesis::utils::MruCache::cend ()")
        )
        .def(
            "clear",
            ( void ( MruCacheType::* )(  ))( &MruCacheType::clear ),
            get_docstring("void ::genesis::utils::MruCache::clear ()")
        )
        .def(
            "contains",
            ( bool ( MruCacheType::* )( key_type const & ))( &MruCacheType::contains ),
            pybind11::arg("key"),
            get_docstring("bool ::genesis::utils::MruCache::contains (key_type const & key)")
        )
        .def(
            "empty",
            ( bool ( MruCacheType::* )(  ) const )( &MruCacheType::empty ),
            get_docstring("bool ::genesis::utils::MruCache::empty () const")
        )
        .def(
            "fetch",
            ( mapped_type & ( MruCacheType::* )( key_type const & ))( &MruCacheType::fetch ),
            pybind11::arg("key"),
            get_docstring("mapped_type & ::genesis::utils::MruCache::fetch (key_type const & key)")
        )
        .def(
            "fetch_copy",
            ( mapped_type ( MruCacheType::* )( key_type const & ))( &MruCacheType::fetch_copy ),
            pybind11::arg("key"),
            get_docstring("mapped_type ::genesis::utils::MruCache::fetch_copy (key_type const & key)")
        )
        .def(
            "size",
            ( size_type ( MruCacheType::* )(  ) const )( &MruCacheType::size ),
            get_docstring("size_type ::genesis::utils::MruCache::size () const")
        )
        .def(
            "touch",
            ( void ( MruCacheType::* )( key_type const & ))( &MruCacheType::touch ),
            pybind11::arg("key"),
            get_docstring("void ::genesis::utils::MruCache::touch (key_type const & key)")
        )

        // Iterators

        .def(
            "__iter__",
            []( ::genesis::utils::MruCache& obj ){
                return pybind11::make_iterator( obj.begin(), obj.end() );
            },
            py::keep_alive<0, 1>()
        )
    ;
}
