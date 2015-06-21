#ifndef GENESIS_TEST_COMMON_H_
#define GENESIS_TEST_COMMON_H_

/**
 * @brief
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

/**
 * @brief Macro to be used in test bodies that depend on the data directory.
 * If it was not found on program startup, the data files cannot be used, thus
 * tests using them need to be skipped.
 */
#define NEEDS_TEST_DATA if(environment->data_dir == "") return;

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

};

/**
 * @brief Instance of the GenesisTestEnvironment, which is set in `main()`.
 */
extern GenesisTestEnvironment* environment;

#endif // include guard
