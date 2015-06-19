/**
 * @brief Main function for testing genesis using Gtest.
 *
 * @file
 * @ingroup test
 */

#include <gtest/gtest.h>

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
