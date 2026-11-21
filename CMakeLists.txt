
cmake_minimum_required(VERSION 3.16)

project(Spaghetti_Graveyard_Zero_2027 LANGUAGES C)

# Use C17
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Engine library
add_library(bbEngine STATIC
        engine/storage/segmented/bbSegmentedPool.c
        engine/common/bbTerminal.c
)

# Allow includes relative to the repository root
target_include_directories(bbEngine PUBLIC
        ${PROJECT_SOURCE_DIR}
)

target_compile_options(bbEngine PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
)
target_compile_definitions(bbEngine PRIVATE
        _POSIX_C_SOURCE=200809L
)

add_executable(Hello_World
        tests/Test_HelloWorld.c
)

target_link_libraries(Hello_World PRIVATE
        bbEngine
)


add_executable(Segmented_Pool
        tests/Test_SegmentedPool.c
)

target_compile_definitions(Segmented_Pool PRIVATE
        _POSIX_C_SOURCE=200809L
)

target_link_libraries(Segmented_Pool PRIVATE
        bbEngine
)

target_compile_options(Segmented_Pool PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
)

target_link_options(Segmented_Pool PRIVATE
        -fsanitize=address,undefined
)