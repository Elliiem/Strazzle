#include "BaseString.h"

#include <gtest/gtest.h>

int main(int argc, char** argv) {
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);

    // Run all the tests
    return RUN_ALL_TESTS();
}