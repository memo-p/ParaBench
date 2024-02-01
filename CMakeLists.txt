cmake_minimum_required(VERSION 3.1)

project(bench_proj)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(APPLICATION_NAME "benchPara")
add_executable(${APPLICATION_NAME} code/benchPara.cpp)
target_include_directories(${APPLICATION_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/code/include)
target_link_libraries(${APPLICATION_NAME} pthread)
target_compile_options(${APPLICATION_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})
target_compile_features(${APPLICATION_NAME} PRIVATE cxx_std_17)

set(APPLICATION_NAME "benchAtom")
add_executable(${APPLICATION_NAME} code/benchAtom.cpp)
target_include_directories(${APPLICATION_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/code/include)
target_link_libraries(${APPLICATION_NAME} pthread)
target_compile_options(${APPLICATION_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})
target_compile_features(${APPLICATION_NAME} PRIVATE cxx_std_17)

set(APPLICATION_NAME "benchVector")
add_executable(${APPLICATION_NAME} code/benchVector.cpp)
target_include_directories(${APPLICATION_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/code/include)
target_link_libraries(${APPLICATION_NAME} pthread)
target_compile_options(${APPLICATION_NAME} PRIVATE ${CXX_FLAGS_REQUIRED})
target_compile_features(${APPLICATION_NAME} PRIVATE cxx_std_17)
