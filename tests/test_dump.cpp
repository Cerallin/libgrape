#include "config.h"

#include "CppUTest/TestHarness.h"

#include "bundle.hpp"
#include "mock_bundle.h"

#include <fstream>
#include <iostream>

using namespace Grape;

Bundle<uint8_t> *bundle;

TEST_GROUP(TestGrapeBundleDump) {

    void setup() { bundle = new Bundle<uint8_t>(6, 2); };

    void teardown() { delete bundle; };
};

TEST(TestGrapeBundleDump, TestInit) { CHECK_FALSE(bundle == nullptr); }

TEST(TestGrapeBundleDump, TestDumpHeader) {
    const char filename[] = "dump.bundle";

    CHECK_EQUAL(GRAPE_OK, bundle->Add(hello_world));
    CHECK_EQUAL(GRAPE_OK, bundle->Add(ciallo_world));

    CHECK_EQUAL(GRAPE_OK, bundle->Dump(filename));
    CHECK(access(filename, F_OK) == 0);
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    std::string res(std::istreambuf_iterator<char>(stream), {});
    MEMCMP_EQUAL(&expectedHeader, res.c_str(), sizeof(GIDF_Header));
}
