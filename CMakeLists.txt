cmake_minimum_required(VERSION 3.0)
project(spatioTemporalDecisions)

include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake-modules)

find_package(OpenMP)
if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include(GetGitRevisionDescription)

git_describe(GIT_DESCRIBE_RESULT)
add_definitions(-DGIT_DESCRIBE=${GIT_DESCRIBE_RESULT})
add_definitions(-DPROJECT_ROOT_DIR=${CMAKE_SOURCE_DIR})


add_subdirectory(src/main)
add_subdirectory(libs/googletest)
add_subdirectory(src/test)
