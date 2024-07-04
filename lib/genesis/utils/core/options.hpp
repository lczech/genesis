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
    Lucas Czech <lucas.czech@sund.ku.dk>
    University of Copenhagen, Globe Institute, Section for GeoGenetics
    Oster Voldgade 5-7, 1350 Copenhagen K, Denmark
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include "genesis/utils/core/info.hpp"
#include "genesis/utils/threading/thread_pool.hpp"

#include <atomic>
#include <cassert>
#include <chrono>
#include <memory>
#include <random>
#include <stdexcept>
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
    inline std::vector<std::string> const& command_line() const
    {
        return command_line_;
    }

    /**
     * @brief Returns a string containing the program's command line arguments.
     */
    std::string command_line_string() const
    {
        std::string ret = "";
        for (size_t i = 0; i < command_line_.size(); ++i) {
            std::string a = command_line_[i];
            ret += (i==0 ? "" : " ") + a;
        }
        return ret;
    }

    /**
     * @brief Set arguments to the program's command line options.
     *
     * If the program is run from the command line, this method has to be used to properly
     * propagate the command line options to this options class.
     */
    void command_line( int const argc, char const* const* argv )
    {
        // Store all arguments in the array.
        command_line_.clear();
        for (int i = 0; i < argc; i++) {
            command_line_.push_back(argv[i]);
        }
    }

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
     * This (re)sets the global seed for the thread-local random engines. The engine of the thread
     * where this function is called from, as well as any thread spawned after, is reseeded using
     * this seed plus a counter based on the number of spawned threads.
     *
     * On startup, the seed is initialized using the current system time, and that exact seed is
     * used for the main thead. The seed can be reset using this function, but this will only affect
     * the thread where the function is called from, as well as any threads spawned later. If a
     * fixed seed is needed, this function hence needs to be called prior to spawning threads,
     * and in particlar, before calling the init_global_thread_pool() functions. Otherwise,
     * this function throws an exception, in order to avoid potential bugs that would be hard
     * to track down.
     *
     * In a single threaded environment, this behaves identical to the usual way of (re)seeding
     * a random engine.
     */
    inline void random_seed( unsigned long const seed )
    {
        if( thread_pool_ ) {
            throw std::domain_error(
                "Cannot re-seed global random engine after having spawned the global thread pool, "
                "as the threads would not be aware of the re-seeding. "
                "Call Options::get().random_seed() before Options::get().init_global_thread_pool() "
                "to fix this."
            );
        }
        random_seed_ = seed;
        thread_local_random_engine_().seed( seed );
    }

    /**
     * @brief Return a thread-local engine for random number generation.
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

    inline unsigned long generate_thread_seed_()
    {
        return random_seed_ + seed_counter_.fetch_add( 1, std::memory_order_relaxed );
    }

    inline std::default_random_engine& thread_local_random_engine_()
    {
        // Private method to access the thread-local engine
        thread_local std::default_random_engine engine( generate_thread_seed_() );
        return engine;
    }

public:

    // -------------------------------------------------------------------------
    //     Global Thread Pool
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
    void init_global_thread_pool()
    {
        // Automatic guessing of the correct number of threads.
        // We then reduce it by one to account for the main thread doing work as well.
        auto const num_threads = guess_number_of_threads();
        assert( num_threads > 0 );
        if( num_threads == 0 ) {
            init_global_thread_pool( 0 );
        } else {
            init_global_thread_pool( num_threads - 1 );
        }
    }

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
    void init_global_thread_pool( size_t num_threads, size_t max_queue_size = 0 )
    {
        if( thread_pool_ ) {
            throw std::runtime_error(
                "Global thread pool has already been initialized. "
                "Cannot call Options::get().init_global_thread_pool() multiple times."
            );
        }
        thread_pool_ = std::make_shared<utils::ThreadPool>( num_threads, max_queue_size );
    }

    /**
     * @brief Return a global thread pool to be used for parallel computations.
     *
     * The thread pool has to be initialized with init_global_thread_pool() first.
     *
     * Note: In cases where we need to limit our number of spawned threads to some maximum amount,
     * we might even want to use this pool for our readers, which often use AsynchronousReader,
     * thus spawning threads to  minimize i/o wait times; but this is not recommended, as the
     * threads waiting for i/o might then be unnecessaryliy wait in the thread pool, just for them
     * to then relatively quickly (compared to potential wait times in the thread queue) execute
     * their disk operation. However, for more complex reading such as from compressed files,
     * we might want to use the global thread pool after all. See BaseInputSource::is_trivial()
     */
    std::shared_ptr<ThreadPool> global_thread_pool() const
    {
        if( ! thread_pool_ ) {
            throw std::runtime_error(
                "Global thread pool has not been initialized. "
                "Call Options::get().init_global_thread_pool() first."
            );
        }
        return thread_pool_;
    }

    /**
     * @brief Get the number of threads allocatd in the pool, plus one for the main thread.
     *
     * This is because with our ProactiveFuture implementaiton, the main thread does work as well.
     */
    size_t global_thread_pool_size() const
    {
        if( ! thread_pool_ ) {
            return 1;
        }
        return thread_pool_->size() + 1;
    }

    // -------------------------------------------------------------------------
    //     Input Reader Threads
    // -------------------------------------------------------------------------

    /**
     * @brief Decide how to use threads for input reading.
     */
    enum class InputReadingThreadPolicy
    {
        /**
         * @brief All input reading uses the global thread pool.
         *
         * This means that we do not spawn extra threads for simply I/O, which can slow down the
         * reading, as file reading then needs to wait in the global thread queue to be processed.
         * It however makes sure that we are not spawning any more threads on top of the global
         * pool, which might be relevant on some systems.
         */
        kStrict,

        /**
         * @brief Use async threads for trivial input sources, such as simple files, but use
         * the global thread pool for sources that need computation.
         *
         * This is the default, where trivial input sources such as a simple file reading get their
         * own thread, so that their I/O wait time does not block any computational threads.
         * However, input sources such as gzipped files, which need decompression first, instead
         * use the global thread pool, in order to make sure that their computation does not end
         * up in over-subscribing the threads.
         */
        kTrivialAsync,

        /**
         * @brief All reading from input sources gets their own thread.
         *
         * These threads are hence independent of the global thread pool, which can lead to
         * oversubscribing if I/O-heavy work and computational work need to be performed at the same
         * time. It can however be a performance optimization for programs that mostly do simple
         * file reading, without having to synchronize their threads via the global pool.
         */
        kAllAsync
    };

    /**
     * @brief Set the policy for the threading of input source reading.
     */
    inline void input_reading_thread_policy( InputReadingThreadPolicy policy )
    {
        input_reading_thread_policy_ = policy;
    }

    /**
     * @brief Get the policy for the threading of input source reading.
     */
    inline InputReadingThreadPolicy input_reading_thread_policy()
    {
        return input_reading_thread_policy_;
    }

    // -------------------------------------------------------------------------
    //     File Output
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
     * to write to a file that is already existing, as long as only the internal file writing
     * functions are used, such as the to_file() function to get a target for the writer classes.
     * We of course cannot control any independent file operations.
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
    //     Data Members
    // -------------------------------------------------------------------------

private:

    // CLI
    std::vector<std::string> command_line_;

    // Random engine seeding
    unsigned long random_seed_;
    std::atomic<size_t> seed_counter_{0};

    // Global thread pool
    std::shared_ptr<ThreadPool> thread_pool_;
    InputReadingThreadPolicy input_reading_thread_policy_ = InputReadingThreadPolicy::kTrivialAsync;

    // File handling
    bool allow_file_overwriting_ = false;

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
