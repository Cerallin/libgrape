set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(include)

# CppUTest
add_subdirectory(CppUTest)
add_executable(runTests test_main.cpp)

# Link CppUTest libraries
target_link_libraries(runTests CppUTest CppUTestExt grape_static)

# Enable tests
add_test(NAME runTests COMMAND runTests)
