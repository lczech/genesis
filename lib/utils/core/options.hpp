#ifndef GENESIS_UTILS_CORE_OPTIONS_H_
#define GENESIS_UTILS_CORE_OPTIONS_H_

/*
    Genesis - A toolkit for working with phylogenetic data.
    Copyright (C) 2014-2016 Lucas Czech

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

// =============================================================================
//     Options
// =============================================================================

/**
 * @brief Simple Options class for application-wide configuration and settings.
 */
class Options
{
public:

    // -----------------------------------------------------
    //     Instance
    // -----------------------------------------------------

    /**
     * @brief Returns a single instance of this class.
     */
    inline static Options& get() {
            // Meyers-Singleton
            static Options instance;
            return instance;
    }

    // -----------------------------------------------------
    //     Setters for Properties
    // -----------------------------------------------------

    void set_command_line (const int argc, const char* argv[]);
    void set_number_of_threads (const unsigned int number);
    void set_random_seed (const unsigned seed);

    // -----------------------------------------------------
    //     Getters for Properties
    // -----------------------------------------------------

    /**
     * @brief Returns an array of strings containing the program's command line arguments.
     */
    inline std::vector<std::string> command_line () const
    {
        return command_line_;
    }

    /**
     * @brief Returns the number of threads.
     */
    inline unsigned int number_of_threads() const
    {
        return number_of_threads_;
    }

    /**
     * @brief Returns the random seed that was used to initialize the engine.
     */
    inline unsigned random_seed() const
    {
        return random_seed_;
    }

    /**
     * @brief Returns the default engine for random number generation.
     *
     * Caveat: This is not intended for the use in more than one thread. As the order of execution in
     * threads is not deterministic, results would not be reproducible, even when using a fixed seed.
     * Furthermore, it might be a speed bottleneck to add a mutex to this method.
     *
     * If in the furture there is need for multi-threaded random engines, they needed to be outfitted
     * with separate seeds each (otherwise they would all produce the same results). Thus, for now we
     * simply assume single-threaded use.
     */
    inline std::default_random_engine& random_engine()
    {
        return random_engine_;
    }

    std::string command_line_string() const;
    std::string dump() const;

    // -----------------------------------------------------
    //     Data Members
    // -----------------------------------------------------

private:

    std::vector<std::string>   command_line_;
    unsigned int               number_of_threads_;
    unsigned                   random_seed_;
    std::default_random_engine random_engine_;

    // -----------------------------------------------------
    //     Hidden Class Members
    // -----------------------------------------------------

private:

    // Everything private, as it is a singleton.
    Options();
    Options( const Options& ) = delete;
    Options( Options&& )      = delete;
    Options& operator= ( const Options& ) = delete;
    Options& operator= ( Options&& )      = delete;
};

} // namespace utils
} // namespace genesis

#endif // include guard
