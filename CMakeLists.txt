cmake_minimum_required(VERSION 3.10)
project(PmrDoublyLinkedList)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Все исходные файлы
set(SOURCE_FILES
    src/MemoryResource.cpp
)

# Основное приложение - называется main
add_executable(main main.cpp ${SOURCE_FILES})
target_include_directories(main PUBLIC include)

# Тесты с Google Test (автоматическая загрузка)
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Тесты - называются tests
add_executable(tests tests/tests.cpp ${SOURCE_FILES})
target_include_directories(tests PUBLIC include)
target_link_libraries(tests gtest gtest_main)

# Для запуска тестов через ctest
enable_testing()
add_test(NAME PmrDoublyLinkedListTests COMMAND tests)

# Опционально: добавить предупреждения компилятора
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
    target_compile_options(main PRIVATE -Wall -Wextra)
    target_compile_options(tests PRIVATE -Wall -Wextra)
endif()