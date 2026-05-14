cmake_minimum_required(VERSION 3.16)
project(ccm LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

# Third-party header-only libraries
add_library(third_party INTERFACE)
target_include_directories(third_party INTERFACE
    ${CMAKE_SOURCE_DIR}/third_party/CLI11/include
    ${CMAKE_SOURCE_DIR}/third_party/json/include
    ${CMAKE_SOURCE_DIR}/third_party/tomlplusplus/include
)

# FTXUI (compiled library)
set(FTXUI_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(FTXUI_BUILD_DOCS OFF CACHE BOOL "" FORCE)
add_subdirectory(third_party/FTXUI)

# Main executable
add_executable(ccm
    src/main.cpp
    src/commands/list.cpp
    src/commands/current.cpp
    src/commands/use.cpp
    src/commands/add.cpp
    src/commands/remove.cpp
    src/commands/tui.cpp
    src/config/app_config.cpp
    src/config/model_registry.cpp
    src/config/claude_config.cpp
    src/providers/provider.cpp
    src/utils/fs.cpp
    src/utils/atomic_write.cpp
    src/utils/backup.cpp
    src/utils/guard.cpp
    src/utils/paths.cpp
)

target_include_directories(ccm PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ccm PRIVATE third_party ftxui::screen ftxui::dom ftxui::component)

# Tests
enable_testing()
add_executable(ccm_tests
    tests/test_main.cpp
    tests/test_app_config.cpp
    tests/test_model_registry.cpp
)
target_include_directories(ccm_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)
target_link_libraries(ccm_tests PRIVATE third_party)
add_test(NAME ccm_tests COMMAND ccm_tests)
