#ifndef GENESIS_UTILS_CORE_OPTIONS_H_
#define GENESIS_UTILS_CORE_OPTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2024 Lucas Czech

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

    Contact:
    Lucas Czech <lczech@carnegiescience.edu>
    Department of Plant Biology, Carnegie Institution For Science
    260 Panama Street, Stanford, CA 94305, USA
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/thread_pool.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

namespace genesis {
namespace utils {

// =================================================================================================
//     Options
// =================================================================================================

/**
 * @brief Simple Options class for application-wide configuration and settings.
 */
class Options
{
public:

    // -------------------------------------------------------------------------
    //     Instance
    // -------------------------------------------------------------------------

    /**
     * @brief Returns a single instance of this class.
     */
    inline static Options& get()
    {
        // Meyers-Singleton
        static Options instance;
        return instance;
    }

    // -------------------------------------------------------------------------
    //     Command Line
    // -------------------------------------------------------------------------

    /**
     * @brief Returns an array of strings containing the program's command line arguments.
     */
    inline std::vector<std::string> command_line() const
    {
        return command_line_;
    }

    /**
     * @brief Returns a string containing the program's command line arguments.
     */
    std::string command_line_string() const;

    /**
     * @brief Set arguments to the program's command line options.
     *
     * If the program is run from the command line, this method has to be used to properly
     * propagate the command line options to this options class.
     */
    void command_line( int const argc, char const* const* argv );

    // -------------------------------------------------------------------------
    //     Multi-Threading
    // -------------------------------------------------------------------------

    /**
     * @brief Initialize the global thread pool to be used for parallel computations.
     *
     * This overload uses the result of guess_number_of_threads() to get the overall number of
     * threads to use, and initializes the pool to use one fewer than that, to account for the
     * main thread also doing work. As our ThreadPool implementation uses what we call a
     * ProactiveFuture, the main thread will start processing tasks from the pool queue while
     * it is waiting for results to get ready.
     *
     * After calling this function, global_thread_pool() can be used to obtain the global thread
     * pool to enqueue work.
     */
    void init_global_thread_pool();

    /**
     * @brief Initialize the global thread pool to be used for parallel computations.
     *
     * This initializes the pool to have exactly as many threads as provided by @p num_threads here.
     * Note that the main thread will also do work, so it is recommended to keep the @p num_threads
     * at at least one fewer than the hardware concurrency (or other upper limit of threads, such as
     * OpenMP or Slum limits, that you might want to keep). Use the overload init_global_thread_pool()
     * without any arguments to do this automatically.
     *
     * If @p num_threads is `0`, the thread pool is initialized with no threads at all, meaning that
     * all enqueued work will instead be processed by the main thread once it wants to obtain the
     * results of tasks. This essentially renders the thread pool into a lazy evaluating task queue.
     *
     * After calling this function, global_thread_pool() can be used to obtain the global thread
     * pool to enqueue work.
     */
    void init_global_thread_pool( size_t num_threads );

    /**
     * @brief Return a global thread pool to be used for parallel computations.
     *
     * The thread pool has to be initialized with init_global_thread_pool() first.
     *
     * Note: In cases where we need to limit our number of spawned threads to some maximum amount,
     * we might even want to use this pool for our readers, which often use AsynchronousReader,
     * thus spawning threads to  minimize i/o wait times; but this is not recommended, as the
     * threads waiting for i/o might  then be unnecessaryliy wait in the thread pool, just for them
     * to then relatively quickly (compared to potential wait times in the thread queue) execute
     * their disk operation.
     */
    std::shared_ptr<ThreadPool> global_thread_pool() const;

    // -------------------------------------------------------------------------
    //     Random Seed & Engine
    // -------------------------------------------------------------------------

    /**
     * @brief Returns the random seed that was used to initialize the engine.
     *
     * This is the global seed that is used as a basis for all thread-local seeding.
     */
    inline unsigned long random_seed() const
    {
        return random_seed_;
    }

    /**
     * @brief Set a specific global seed for the random engine initialization.
     *
     * This sets the global seed for the thread-local random engines. Each engine
     * is initialized using this seed plus a counter based on the number of spawned threads.
     * On startup, the seed is initialized using the current system time. This value can
     * be overwritten using this method. This should hence be done prior to spawning threads,
     * for instance, before calling the init_global_thread_pool() functions.
     */
    inline void random_seed( unsigned long const seed )
    {
        random_seed_ = seed;
        // thread_local_random_engine_().seed( seed );
    }

    /**
     * @brief Returns a thread-local engine for random number generation.
     *
     * Caveat: The engines are thread-local, and hence thread-safe. However, generally, in a
     * multi-threaded setting, the order of execution in not deterministic, and hence results might
     * not be reproducible, even when using a fixed seed.
     */
    inline std::default_random_engine& random_engine()
    {
        return thread_local_random_engine_();
    }

    /**
     * @brief Return the number of seeds used to initialize thread-local random engines.
     *
     * This corresponds to the number of threads that have called random_engine().
     */
    inline size_t seed_counter() const
    {
        return seed_counter_.load();
    }

private:

    unsigned long generate_thread_seed_()
    {
        return random_seed_ + seed_counter_.fetch_add( 1, std::memory_order_relaxed );
    }

    std::default_random_engine& thread_local_random_engine_()
    {
        // Private method to access the thread-local engine
        thread_local std::default_random_engine engine( generate_thread_seed_() );
        return engine;
    }

public:

    // -------------------------------------------------------------------------
    //     File Options
    // -------------------------------------------------------------------------

    /**
     * @brief Get whether Genesis is allowed to overwrite files when outputting data.
     *
     * See allow_file_overwriting( bool ) for details.
     */
    inline bool allow_file_overwriting() const
    {
        return allow_file_overwriting_;
    }

    /**
     * @brief Set whether Genesis is allowed to overwrite files when outputting data.
     *
     * The option is `false` by default, which causes Genesis to throw an execption when it attempts
     * to write to a file that is already existing.
     *
     * By setting this option to `true`, files are silently overwritten in case they already
     * exist. This has to be activated explicitly in order to avoid losing files by accident.
     *
     * @see @link utils::file_output_stream() file_output_stream()@endlink
     */
    inline void allow_file_overwriting( bool value )
    {
        allow_file_overwriting_ = value;
    }

    // -------------------------------------------------------------------------
    //     Object Info
    // -------------------------------------------------------------------------

    /**
     * @brief Set whether an object info one-liner is printed when using the `operator <<` that
     * is defined for many classes.
     *
     * NOTE: So far, this is only implemented for the @link genesis::tree::Tree Tree@endlink class.
     * It might be extended in the future to more classes, for example if we re-activate the
     * python bindings, as this feature might be more useful when scripting. So take the word
     * "many" here with a huge grain of salt.
     *
     * In genesis, we create the functions `print_info()` and `print_gist()`,
     * and overload the `operator <<` for many classes as a convenient way to get information about
     * an object, for example for debugging purposes.
     * Using this setting, the behaviour of the `operator <<` is controlled:
     * If set to `true`, a one-liner containing basic information about the object (its type,
     * and for containers, its size) is printed.
     *
     * See also print_object_gists( int ) for an additional setting that allows to print more details
     * when using `operator <<` on an object. Both settings can also be combined. In that case,
     * first, the one-line info is printed, followed by the gist.
     */
    inline void print_object_infos( bool value )
    {
        print_obj_infos_ = value;
    }

    /**
     * @brief Get whether an object info one-liner is printed when using the `operator <<`.
     *
     * See print_object_infos( bool ) for details.
     */
    inline bool print_object_infos() const
    {
        return print_obj_infos_;
    }

    /**
     * @brief Set whether an object gist is printed when using the `operator <<` that is defined
     * for many (container) classes.
     *
     * NOTE: So far, this is only implemented for the @link genesis::tree::Tree Tree@endlink class.
     * It might be extended in the future to more classes, for example if we re-activate the
     * python bindings, as this feature might be more useful when scripting. So take the word
     * "many" here with a huge grain of salt.
     *
     * In genesis, we create the functions `print_info()` and `print_gist()`, and overload
     * the `operator <<` for many classes as a convenient way to get information about an object,
     * for example for debugging purposes.
     * Using this setting, the behaviour of the `operator <<` is controlled:
     * If set to a value `n` greather than 0, the first `n` elements that the object contains are
     * printed. If set to a negative value, all elements are printed.
     * Default is 0, that is, no gist of the object's elements is printed.
     *
     * See also print_object_infos( bool ) for an additional setting that allows to print an info
     * one-liner when using `operator <<` on an object. Both settings can also be combined.
     * In that case, first, the one-line info is printed, followed by the gist.
     */
    inline void print_object_gists( long value )
    {
        print_obj_gists_ = value;
    }

    /**
     * @brief Get whether an object gist is printed when using the `operator <<`.
     *
     * See print_object_gists( long ) for details.
     */
    inline long print_object_gists() const
    {
        return print_obj_gists_;
    }

    // -------------------------------------------------------------------------
    //     Overview
    // -------------------------------------------------------------------------

    /**
     * @brief Return a list with compile time and run time options with their values.
     */
    std::string info() const;

    /**
     * @brief Return a list of compile time options.
     */
    std::string info_compile_time() const;

    /**
     * @brief Return a list of run time options.
     */
    std::string info_run_time() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // CLI
    std::vector<std::string> command_line_;

    // Global thread pool
    std::shared_ptr<ThreadPool> thread_pool_;

    // Random engine seeding
    unsigned long random_seed_;
    std::atomic<size_t> seed_counter_{0};

    // File handling
    bool allow_file_overwriting_ = false;

    // Object dumping
    bool print_obj_infos_ = true;
    long print_obj_gists_ = 0;

    // -------------------------------------------------------------------------
    //     Hidden Class Members
    // -------------------------------------------------------------------------

private:

    // Everything private, as it is a singleton.

    /**
     * @brief Constructor, which initializes the options with reasonable defaults.
     */
    Options()
    {
        // Initialize random seed with time.
        random_seed( std::chrono::system_clock::now().time_since_epoch().count() );
    }

    ~Options() = default;

    Options( const Options& ) = delete;
    Options( Options&& )      = delete;
    Options& operator= ( const Options& ) = delete;
    Options& operator= ( Options&& )      = delete;

};

} // namespace utils
} // namespace genesis

#endif // include guard
