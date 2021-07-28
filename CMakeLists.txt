cmake_minimum_required(VERSION 3.10)
project(DittoSuite)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(schema)

include_directories(include)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_executable(dittobench
	src/dittobench.cpp
	src/close_file.cpp
	src/delete_file.cpp
	src/instruction.cpp
	src/instruction_factory.cpp
	src/instruction_set.cpp
	src/logger.cpp
	src/open_file.cpp
	src/parser.cpp
	src/resize_file.cpp
	src/shared_variables.cpp
	src/utils.cpp
	src/write_file.cpp
)

target_link_libraries(dittobench
	benchmark_proto
	${Protobuf_LIBRARIES}
	${Protobuf_PROTOC_LIBRARIES}
)
