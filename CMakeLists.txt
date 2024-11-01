cmake_minimum_required(VERSION 3.5.0)
project(nol VERSION 0.1.0 LANGUAGES C)

file(GLOB SRC_FILES "src/*.c")
add_executable(nol ${SRC_FILES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

