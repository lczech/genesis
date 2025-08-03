#include <genesis/util/container/mru_cache.hpp>
#include <genesis/util/container/mru_cache_instances.hpp>
#include <memory>
#include <pybind11/functional.h>
#include <pybind11/pytypes.h>
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

void bind_genesis_util_container_mru_cache_instances(
    std::function< pybind11::module&( std::string const& namespace_ ) >& M
)
{
    { // genesis::util::container::MruCache file:genesis/util/container/mru_cache_instances.hpp
      // line:38
        pybind11::class_<
            genesis::util::container::MruCache<pybind11::object, pybind11::object>,
            std::shared_ptr<genesis::util::container::MruCache<pybind11::object, pybind11::object>>>
            cl( M( "genesis::util::container" ), "MruCache", "" );
        // function-signature: genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::MruCache()() file:genesis/util/container/mru_cache.hpp line:128
        cl.def( pybind11::init( []() {
            return new genesis::util::container::MruCache<pybind11::object, pybind11::object>();
        } ) );
        // function-signature: genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::MruCache(unsigned long)(unsigned long)
        // file:genesis/util/container/mru_cache.hpp line:136
        cl.def( pybind11::init<unsigned long>(), pybind11::arg( "capacity" ) );

        cl.def_readwrite(
            "load_function",
            &genesis::util::container::MruCache<pybind11::object, pybind11::object>::load_function
        );
        cl.def_readwrite(
            "release_function",
            &genesis::util::container::MruCache<pybind11::object, pybind11::object>::
                release_function
        );
        // function-signature: unsigned long genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::size() const() file:genesis/util/container/mru_cache.hpp line:213
        cl.def(
            "size",
            ( unsigned long ( genesis::util::container::
                                  MruCache<pybind11::object, pybind11::object>::* )() const ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::size,
            "Get the current count of elements being loaded in the cache.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, pybind11::object>::size() const "
            "--> unsigned long"
        );
        // function-signature: unsigned long genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::capacity() const() file:genesis/util/container/mru_cache.hpp line:224
        cl.def(
            "capacity",
            ( unsigned long ( genesis::util::container::
                                  MruCache<pybind11::object, pybind11::object>::* )() const ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::capacity,
            "Return the currently set capacity of the cache, i.e., its maximal size.\n\n A "
            "capacity of 0 means limitless, that is, no elements are ever removed.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, pybind11::object>::capacity() "
            "const --> unsigned long"
        );
        // function-signature: bool genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::empty() const() file:genesis/util/container/mru_cache.hpp line:233
        cl.def(
            "empty",
            ( bool( genesis::util::container::MruCache<pybind11::object, pybind11::object>::* )()
                  const ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::empty,
            "Return whether the cache is currently empty, that is, has no elements loaded.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, pybind11::object>::empty() const "
            "--> bool"
        );
        // function-signature: unsigned long genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::cache_hits() const() file:genesis/util/container/mru_cache.hpp
        // line:244
        cl.def(
            "cache_hits",
            ( unsigned long ( genesis::util::container::
                                  MruCache<pybind11::object, pybind11::object>::* )() const ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::cache_hits,
            "Number of cache hits.\n\n Returns the number of times that an element could be "
            "retrieved without calling the load\n function.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, pybind11::object>::cache_hits() "
            "const --> unsigned long"
        );
        // function-signature: unsigned long genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::cache_misses() const() file:genesis/util/container/mru_cache.hpp
        // line:256
        cl.def(
            "cache_misses",
            ( unsigned long ( genesis::util::container::
                                  MruCache<pybind11::object, pybind11::object>::* )() const ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::
                    cache_misses,
            "Number of cache misses.\n\n This includes the (probably rare) cases where a "
            "multithreaded access led to the same element\n being loaded multiple times. That is, "
            "this counter returns the number of times that\n the load function was called.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, "
            "pybind11::object>::cache_misses() const --> unsigned long"
        );
        // function-signature: class pybind11::object &
        // genesis::util::container::MruCache<pybind11::object, pybind11::object>::fetch(const class
        // pybind11::object &)(const class pybind11::object &)
        // file:genesis/util/container/mru_cache.hpp line:287
        cl.def(
            "fetch",
            ( class pybind11::object & (genesis::util::container::MruCache<
                                           pybind11::object,
                                           pybind11::object>::*)(const class pybind11::object&)) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::fetch,
            "Get an element.\n\n This is the main function of the class. It gets an element given "
            "its  either by\n retrieving it from the cache, or loading it into the cache first, if "
            "needed.\n\n If loading an element leads to the capacity of the cache begin "
            "exceeded,\n the least recently used element is removed. The removal is done after "
            "loading the new\n element. This means that the memory usage can be one more element "
            "than the capacity() allows.\n This is done to make sure that an exception thrown when "
            "loading the new element does not lead\n to the cache being altered.\n\n Thread "
            "safety: Not thread safe, because it does not use a guard, and because it returns a\n "
            "reference, which can become dangling if other threads fetch new elements, leading to "
            "the\n referenced one being removed. For multi-threaded use, see fetch_copy().\n\n "
            "Caveat: Even in single-thread use, a variable storing a reference obtained from "
            "fetch()\n can become dangling, if more new elements are fetched or touched than the "
            "capacity allows.\n Thus, the variable needs to go out of scope before this happens. "
            "For example, a loop\n over keys, fetching an element in the beginning of the loop "
            "body and keeping the reference\n only within the loop body without calling fetch() "
            "again, is fine.\n\nC++: genesis::util::container::MruCache<pybind11::object, "
            "pybind11::object>::fetch(const class pybind11::object &) --> class pybind11::object &",
            pybind11::return_value_policy::reference_internal,
            pybind11::arg( "key" )
        );
        // function-signature: class pybind11::object
        // genesis::util::container::MruCache<pybind11::object, pybind11::object>::fetch_copy(const
        // class pybind11::object &)(const class pybind11::object &)
        // file:genesis/util/container/mru_cache.hpp line:353
        cl.def(
            "fetch_copy",
            ( class pybind11::object( genesis::util::container::MruCache<
                                      pybind11::object,
                                      pybind11::object>::* )( const class pybind11::object& ) ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::fetch_copy,
            "Get an element by copy.\n\n This works exactly the same as fetch(), but is thread "
            "safe and returns a copy.\n See fetch() for details.\n\n Because the loading is not "
            "part of the mutex that makes this function thread safe,\n it is possible to parallely "
            "load elements in different threads.\n However, when two threads need to load an "
            "element at the same time, the loading\n may happen twice. Then, only the first thread "
            "that finishes loading stores the element\n in the cache, while the other one is "
            "discarded. This is done in order to allow\n parallel loading without the hassle of "
            "per-element locks.\n\n If the cache is used in a multi-threaded environment and holds "
            "large elements,\n making actual copies might be too expensive. In that case, a neat "
            "trick is to store\n shared pointers to the elements instead:\n\n ~~~cpp\n // Path to "
            "some data.\n std::string dir = \"/path/to/data\";\n\n // Create a cache from file "
            "names to shared pointers of file contents.\n MruCache<std::string, "
            "std::shared_ptr<std::string>> cache{ 5 };\n\n // Load elements from file.\n "
            "cache.load_function = [ &dir ]( std::string const& file ){\n     return "
            "std::make_shared<std::string>( file_read( dir + \"/\" + file ));\n };\n\n // Fetch an "
            "element, that is, load a file into the cache.\n // Store it by copy, which just "
            "copies the shared pointer.\n auto content = cache.fetch_copy( \"large_file.txt\" );\n "
            "~~~\n\n As the control block of `std::shared_ptr` is thread safe, these shared "
            "pointer copies can\n stay alive in a thread that still needs the element, even if the "
            "element was removed from\n the cache by other threads in the meantime.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, "
            "pybind11::object>::fetch_copy(const class pybind11::object &) --> class "
            "pybind11::object",
            pybind11::arg( "key" )
        );
        // function-signature: bool genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::contains(const class pybind11::object &)(const class pybind11::object
        // &) file:genesis/util/container/mru_cache.hpp line:416
        cl.def(
            "contains",
            ( bool( genesis::util::container::MruCache<
                    pybind11::object,
                    pybind11::object>::* )( const class pybind11::object& ) ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::contains,
            "Return whether an element is currently in the cache.\n\n Thread safety: Safe. But the "
            "element might be removed by other threads soon.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, "
            "pybind11::object>::contains(const class pybind11::object &) --> bool",
            pybind11::arg( "key" )
        );
        // function-signature: void genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::touch(const class pybind11::object &)(const class pybind11::object &)
        // file:genesis/util/container/mru_cache.hpp line:432
        cl.def(
            "touch",
            ( void( genesis::util::container::MruCache<
                    pybind11::object,
                    pybind11::object>::* )( const class pybind11::object& ) ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::touch,
            "Bring an element to the front, and load it if needed.\n\n The function behaves just "
            "like fetch_copy(), but without returning the element.\n Useful to pre-load the "
            "cache.\n\n Be aware however that having touched an element in multi threaded used "
            "does not guarantee\n that it stays in the cache for long. Other threads might have "
            "fetched other elements,\n leading to the removal of the touched one. In that case, it "
            "has to be loaded again when\n fetched later.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, pybind11::object>::touch(const "
            "class pybind11::object &) --> void",
            pybind11::arg( "key" )
        );
        // function-signature: void genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::capacity(unsigned long)(unsigned long)
        // file:genesis/util/container/mru_cache.hpp line:446
        cl.def(
            "capacity",
            ( void( genesis::util::container::MruCache<pybind11::object, pybind11::object>::* )(
                unsigned long
            ) ) &
                genesis::util::container::MruCache<pybind11::object, pybind11::object>::capacity,
            "Set the capacity of the cache, i.e., its maximal size.\n\n Setting the capacity to 0 "
            "means limitless, that is, no elements are ever removed.\n\nC++: "
            "genesis::util::container::MruCache<pybind11::object, "
            "pybind11::object>::capacity(unsigned long) --> void",
            pybind11::arg( "value" )
        );
        // function-signature: void genesis::util::container::MruCache<pybind11::object,
        // pybind11::object>::clear()() file:genesis/util/container/mru_cache.hpp line:461
        cl.def(
            "clear",
            ( void( genesis::util::container::MruCache<pybind11::object, pybind11::object>::* )()
            ) & genesis::util::container::MruCache<pybind11::object, pybind11::object>::clear,
            "Clear the cache.\n\nC++: genesis::util::container::MruCache<pybind11::object, "
            "pybind11::object>::clear() --> void"
        );
    }
}
