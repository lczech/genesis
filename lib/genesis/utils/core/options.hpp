#ifndef GENESIS_UTILS_CORE_OPTIONS_H_
#define GENESIS_UTILS_CORE_OPTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2017 Lucas Czech

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
    Lucas Czech <lucas.czech@h-its.org>
    Exelixis Lab, Heidelberg Institute for Theoretical Studies
    Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

/**
 * @brief
 *
 * @file
 * @ingroup utils
 */

#include <random>
#include <string>
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
    inline static Options& get() {
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
    //     Number of Threads
    // -------------------------------------------------------------------------

    /**
     * @brief Returns the number of threads.
     */
    inline unsigned int number_of_threads() const
    {
        return number_of_threads_;
    }

    /**
     * @brief Overwrite the system given number of threads.
     *
     * On startup, the value is initialized with the actual number of cores available in the system
     * using std::thread::hardware_concurrency(). This method overwrites this value.
     */
    void number_of_threads (const unsigned int number);

    /**
     * @brief Return whether the binary was compiled using Pthreads.
     */
    bool using_pthreads() const;

    /**
     * @brief Return whether the binary was compiled using OpenMP.
     */
    bool using_openmp() const;

    // -------------------------------------------------------------------------
    //     Random Seed & Engine
    // -------------------------------------------------------------------------

    /**
     * @brief Returns the random seed that was used to initialize the engine.
     */
    inline unsigned random_seed() const
    {
        return random_seed_;
    }

    /**
     * @brief Set a specific seed for the random engine.
     *
     * On startup, the random engine is initialized using the current system time. This value can
     * be overwritten using this method.
     */
    void random_seed (const unsigned seed);

    /**
     * @brief Returns the default engine for random number generation.
     *
     * Caveat: This is not intended for the use in more than one thread. As the order of execution in
     * threads is not deterministic, results would not be reproducible, even when using a fixed seed.
     * Furthermore, it might be a speed bottleneck to add a mutex to this method.
     *
     * If in the future there is need for multi-threaded random engines, they needed to be outfitted
     * with separate seeds each (otherwise they would all produce the same results). Thus, for now we
     * simply assume single-threaded use.
     */
    inline std::default_random_engine& random_engine()
    {
        return random_engine_;
    }

    // -------------------------------------------------------------------------
    //     Misc Options
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
    //     Run Time Environment
    // -------------------------------------------------------------------------

    /**
     * @brief Return true iff the standard input stream is a terminal, and false if not, i.e., if
     * it is a file or a pipe.
     */
    bool stdin_is_terminal() const;

    /**
     * @brief Return true iff the standard output stream is a terminal, and false if not, i.e., if
     * it is a file or a pipe.
     */
    bool stdout_is_terminal() const;

    /**
     * @brief Return true iff the standard error stream is a terminal, and false if not, i.e., if
     * it is a file or a pipe.
     */
    bool stderr_is_terminal() const;

    // -------------------------------------------------------------------------
    //     Compile Time Environment
    // -------------------------------------------------------------------------

    /**
     * @brief Return whether the system uses little endian memory.
     */
    static bool is_little_endian();

    /**
     * @brief Return whether the system uses big endian memory.
     */
    static bool is_big_endian();

    /**
     * @brief Return the platform under which genesis was compiled.
     *
     * This can be either "Win32", "Linux", "Apple", "Unix" or "Unknown".
     */
    static std::string platform();

    /**
     * @brief Return the compiler family (name) that was used to compile genesis.
     *
     * See compiler_version() to get the version of the compiler.
     */
    static std::string compiler_family();

    /**
     * @brief Return the compiler version that was used to compile genesis.
     *
     * See compiler_family() to get the corresponding compiler name.
     */
    static std::string compiler_version();

    /**
     * @brief Return the CPP version that was used to compile genesis.
     */
    static std::string cpp_version();

    // -------------------------------------------------------------------------
    //     Build Type
    // -------------------------------------------------------------------------

    #if defined( DEBUG ) && defined( NDEBUG )
        static_assert( false, "Cannot compile with both DEBUG and NDEBUG flags set." );
    #endif

    #if ! defined( DEBUG ) && ! defined( NDEBUG )
        static_assert( false, "Cannot compile with neiher DEBUG nor NDEBUG flag set." );
    #endif

    /**
     * @brief Return whether the binary was compiled with build type `DEBUG`.
     */
    static bool is_debug();

    /**
     * @brief Return whether the binary was compiled with build type `RELEASE`.
     */
    static bool is_release();

    /**
     * @brief Return the build type that was used to compile the binary, i.e., "debug" or "release".
     */
    static std::string build_type();

    // -------------------------------------------------------------------------
    //     Dump & Overview
    // -------------------------------------------------------------------------

    /**
     * @brief Return a list with compile time and run time options with their values.
     */
    std::string info() const;

    // -------------------------------------------------------------------------
    //     Data Members
    // -------------------------------------------------------------------------

private:

    std::vector<std::string>   command_line_;
    unsigned int               number_of_threads_;

    unsigned                   random_seed_;
    std::default_random_engine random_engine_;

    bool                       allow_file_overwriting_ = false;

    // -------------------------------------------------------------------------
    //     Hidden Class Members
    // -------------------------------------------------------------------------

private:

    // Everything private, as it is a singleton.

    /**
     * @brief Constructor, which initializes the options with reasonable defaults.
     */
    Options();

    ~Options() = default;

    Options( const Options& ) = delete;
    Options( Options&& )      = delete;
    Options& operator= ( const Options& ) = delete;
    Options& operator= ( Options&& )      = delete;

};

} // namespace utils
} // namespace genesis

#endif // include guard
