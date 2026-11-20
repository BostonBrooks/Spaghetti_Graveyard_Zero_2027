
cmake_minimum_required(VERSION 3.16)

project(Spaghetti_Graveyard_Zero_2027 LANGUAGES C)

# Use C17
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Engine library
add_library(bbEngine STATIC
        engine/storage/segmented/bbSegmentedPool.c
)

# Allow includes relative to the repository root
target_include_directories(bbEngine PUBLIC
        ${PROJECT_SOURCE_DIR}
)


add_executable(Hello_World
        tests/Test_HelloWorld.c
)

target_link_libraries(Hello_World PRIVATE
        bbEngine
)