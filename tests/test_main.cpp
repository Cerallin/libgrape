#include "CppUTest/CommandLineTestRunner.h"

#include "test_grape.cpp"

int main(int argc, char **argv) {
    return CommandLineTestRunner::RunAllTests(argc, argv);
}
