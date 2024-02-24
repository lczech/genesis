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
 * @brief Main function for testing genesis using Gtest.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

#include <csignal>
#include <iostream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "src/common.hpp"
#include "genesis/utils/core/options.hpp"

GenesisTestEnvironment* environment;

void genesis_test_sighandler( int signum )
{
    // Without special tricks, we might not get a core dump when a SIGSEGV occurs.
    // So we overwrite the default handler to achieve that.
    // See https://stackoverflow.com/a/19308537
    LOG_ERR << "Segmentation fault (SIGSEGV)";
    std::signal( signum, SIG_DFL );
    ::kill( ::getpid(), signum );
}

/**
 * @brief Initializes and starts the test run.
 *
 * The main sets the environment for testing and then runs the tests.
 *
 * @return      Zero if all tests were successfull, non-zero otherwise.
 */
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    environment = new GenesisTestEnvironment();

    // Set a custom signal handler, see there for details.
    // Sometimes, this however does the opposite, and does not dump a core when segfauling...
    // So deactivating this for now again.
    // signal( SIGSEGV, genesis_test_sighandler );

    // Set data dir using the program path.
    std::string call = argv[0];
    std::size_t found = call.find_last_of("/\\");
    if (found != std::string::npos) {

        // Ugly hardcoded path depending on the build structe of the program.
        // For now, this is good enoguh, but maybe we should find a more portable solution.
        environment->data_dir = call.substr(0,found) + "/../../test/data/";
    }

    // If user provided one via command line, overwrite data dir.
    std::string data_dir_prefix = "--test_data_dir=";
    for (int i = 1; i < argc; ++i) {
        std::string argument = argv[i];
        if(argument.substr(0, data_dir_prefix.size()) == data_dir_prefix) {
            environment->data_dir = argument.substr(data_dir_prefix.size());
        }
    }

    // Return true iff the directory exists.
    // This is copied from the genesis library `./lib/utils/core/fs.hpp`
    // because we do not want to rely on something that we are about to test...
    auto dir_exists = [] (const std::string& dir )
    {
        struct stat info;
        if (stat (dir.c_str(), &info) != 0) {
            return false;
        }
        return static_cast<bool>( info.st_mode & S_IFDIR );
    };

    // Actually, use the data dir only if it exists.
    if (!dir_exists(environment->data_dir)) {
        environment->data_dir = "";
    }

    // We want to fail if we didn't find the test data.
    environment->fail_on_missing_data_dir = true;

    // We want to see Logging information while testing.
    genesis::utils::Logging::log_to_stdout();
    genesis::utils::Logging::max_level(genesis::utils::Logging::kDebug4);

    // Start the global thread pool, once here.
    // Needed by some of the tests.
    genesis::utils::Options::get().init_global_thread_pool( 2 );

    ::testing::AddGlobalTestEnvironment(environment);
    return RUN_ALL_TESTS();
}
