add_library(crabase_options INTERFACE)

if(CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(CRABASE_MSVC_OPTIONS /W4 /WX /utf-8 /std:c11)
else()
    set(CRABASE_GNU_OPTIONS -Wall -Werror -Wextra -pedantic -fvisibility=hidden)
endif()
target_compile_options(crabase_options INTERFACE
    ${CRABASE_MSVC_OPTIONS}
    ${CRABASE_GNU_OPTIONS}
)

target_compile_definitions(crabase_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:
        _POSIX_C_SOURCE=200809L
        _GNU_SOURCE
    >
)

target_link_libraries(crabase_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>,$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:
        m
    >
)

option(CRABASE_ENABLE_RANDOM_STRING_INIT_HASHCODE "Enable random string init hashcode" ON)
if(CRABASE_ENABLE_RANDOM_STRING_INIT_HASHCODE)
    target_compile_definitions(crabase_options INTERFACE CRA_RANDOM_STR_INIT_HASHCODE)
endif()
