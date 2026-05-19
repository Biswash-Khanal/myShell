cmake_minimum_required(VERSION 3.14)
project(my_project)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)


add_library(my_lib
    src/Vec.c
    src/Guards.c
)

target_include_directories(my_lib PUBLIC include)


# add_executable(my_program src/main.c)
# target_link_libraries(my_program PRIVATE my_lib)


enable_testing()

add_executable(vec_test
    test/vec_test.cpp
)

target_link_libraries(vec_test
    PRIVATE
        my_lib
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(vec_test)
