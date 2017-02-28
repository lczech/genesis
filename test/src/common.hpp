#ifndef GENESIS_TEST_COMMON_H_
#define GENESIS_TEST_COMMON_H_

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
 * @ingroup test
 */

// Disable warnings for GCC, also works in Clang, see http://stackoverflow.com/a/13492589/4184258
// Furthermore, while we are at it, also disable the warning produced by the pragma
// for disabling warnings in MSVC...
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"

// Disable warnings for MSVC, see http://stackoverflow.com/a/2106590/4184258
#pragma warning (push, 0)

// Include the GTest header, which produces ugly warnings with our compile flags.
#include <gtest/gtest.h>

// Re-enable warnings
#pragma warning (pop)
#pragma GCC diagnostic pop

#include "genesis/utils/core/logging.hpp"

/**
 * @brief Macro to be used in test bodies that depend on the data directory.
 * If it was not found on program startup, the data files cannot be used, thus
 * tests using them need to be skipped.
 */
#define NEEDS_TEST_DATA                                    \
    if(environment->data_dir == "") {                      \
        if (environment->fail_on_missing_data_dir) {       \
            FAIL() << "No test data directory found.";     \
        }                                                  \
        return;                                            \
    };

/**
 * @Brief Environment class for testing genesis.
 */
class GenesisTestEnvironment : public ::testing::Environment {
public:

    // -------------------------------------------------------------------------
    //     Gtest Functions
    // -------------------------------------------------------------------------

    virtual ~GenesisTestEnvironment() {}

    // Override this to define how to set up the environment.
    virtual void SetUp() {}

    // Override this to define how to tear down the environment.
    virtual void TearDown() {}

    // -------------------------------------------------------------------------
    //     Global Settings
    // -------------------------------------------------------------------------

    std::string data_dir;

    bool fail_on_missing_data_dir;
};

/**
 * @brief Instance of the GenesisTestEnvironment, which is set in `main()`.
 */
extern GenesisTestEnvironment* environment;

#endif // include guard
