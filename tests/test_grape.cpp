#include "CppUTest/TestHarness.h"

#include "grape/bundle.hpp"

#include <iostream>
#include <sstream>

using namespace Grape;

Bundle<uint8_t> *bundle;

TEST_GROUP(TestGrapeBundle) {

    void setup() { bundle = new Bundle<uint8_t>(6, 2); };

    void teardown() { delete bundle; };
};

TEST(TestGrapeBundle, TestInit) { CHECK_FALSE(bundle == nullptr); }

TEST(TestGrapeBundle, TestDump) {
    int ret;
    char *buffer = (char *)calloc(0, 0x1000);
    const char expected[] = "123";

    const char hello_world[] = "ext/hello_world.txt";
    const char ciallo_world[] = "ext/ciallo_world.txt";

    bundle->Add(hello_world);
    bundle->Add(ciallo_world);

    std::ostringstream outStream;

    bundle->Dump(outStream);
    MEMCMP_EQUAL(expected, buffer, sizeof(expected));

    free(buffer);
}
