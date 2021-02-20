cmake_minimum_required(VERSION 3.1)

project(Bezier)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -D_FORTIFY_SOURCE=2 -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector -O2)
")

set(TEST_FILE "bezier_example.cc")

set(SOURCE_FILES
        ${TEST_FILE}
        bezier.h)

add_executable(bezier ${SOURCE_FILES})

