cmake_minimum_required(VERSION 3.20)

project("chip8" CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory("libs/raylib")

add_executable(
	${PROJECT_NAME}
	"source/chip8.cpp"
	"source/VM.cpp"
)

target_include_directories(
	${PROJECT_NAME} PRIVATE
	"source"
)

target_link_libraries(
	${PROJECT_NAME} PRIVATE
	raylib
)

set_target_properties(
	${PROJECT_NAME} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}
	RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}
)
