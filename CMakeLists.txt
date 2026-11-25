
cmake_minimum_required(VERSION 3.16)

project(Spaghetti_Graveyard_Zero_2027 LANGUAGES C)

# Use C17
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Engine library
add_library(bbEngine STATIC
        engine/common/bbTerminal.c
        engine/storage/deque/bbSegmentedDeque.c
        engine/storage/segmented/bbSegmentedPool2.c
        engine/storage/segmented/bbSegmentedList.c
        engine/core/bbCore.c
        engine/core/bbInstruction.c
        engine/core/bbInstruction_integrity.c
        engine/core/bbInstruction_internal.c
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

add_executable(Segmented_List
        tests/Test_SegmentedList.c
)

target_compile_definitions(Segmented_List PRIVATE
        _POSIX_C_SOURCE=200809L
)

target_link_libraries(Segmented_List PRIVATE
        bbEngine
)

target_compile_options(Segmented_List PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
)

target_link_options(Segmented_List PRIVATE
        -fsanitize=address,undefined
)


add_executable(Segmented_Deque
        tests/Test_SegmentedDeque.c
)

target_compile_definitions(Segmented_Deque PRIVATE
        _POSIX_C_SOURCE=200809L
)

target_link_libraries(Segmented_Deque PRIVATE
        bbEngine
)

target_compile_options(Segmented_Deque PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
)

target_link_options(Segmented_Deque PRIVATE
        -fsanitize=address,undefined
)


add_executable(Set_Time
        tests/Test_setTime.c
)

target_compile_definitions(Set_Time PRIVATE
        _POSIX_C_SOURCE=200809L
)

target_link_libraries(Set_Time PRIVATE
        bbEngine
)

target_compile_options(Set_Time PRIVATE
        -fsanitize=address,undefined
        -fno-omit-frame-pointer
)

target_link_options(Set_Time PRIVATE
        -fsanitize=address,undefined
)