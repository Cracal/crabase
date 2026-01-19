cmake_minimum_required(VERSION 3.22.0)
project(crabase VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED true)

message(STATUS "c standard: ${CMAKE_C_STANDARD}")

if(MSVC)
    # add_compile_options(/source-charset:utf-8)
    add_compile_options(/utf-8)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(
        -Wall -Wextra -pedantic -Werror

        -Wno-cast-function-type
    )
    add_compile_options(-fvisibility=hidden)
    if (MINGW)
        add_compile_options(-fexec-charset=UTF-8 -finput-charset=UTF-8)
    # else()
    #     add_compile_options(-fsanitize=undefined)
    #     add_link_options(-fsanitize=undefined)
    #     add_compile_options(-fsanitize=address)
    #     add_link_options(-fsanitize=address)
    endif()
endif()

add_library(mem_detect_config INTERFACE)
target_compile_definitions(mem_detect_config INTERFACE
    $<$<CONFIG:Debug>:CRA_MEMORY_LEAK_DETECTOR>
)

add_definitions(-DCRA_LOG_FILE_LINE)
add_definitions(-DCRA_BUILD_DLL)
add_definitions(-DCRA_RANDOM_STR_INIT_HASHCODE)
add_definitions(-DCRA_JSON_SAFE_INT)

include_directories(inc)

aux_source_directory(src/collections SRC)
aux_source_directory(src/serialize SRC)
aux_source_directory(src/threads SRC)
aux_source_directory(src SRC)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)

add_library(${PROJECT_NAME} SHARED ${SRC})
target_link_libraries(${PROJECT_NAME} PRIVATE mem_detect_config)

include(CTest)
enable_testing()

add_subdirectory(tests)
