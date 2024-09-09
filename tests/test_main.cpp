#include "mock_bundle.h"

#include "CppUTest/CommandLineTestRunner.h"

int main(int argc, char **argv) {
    generateTestingFiles();

    return CommandLineTestRunner::RunAllTests(argc, argv);
}
