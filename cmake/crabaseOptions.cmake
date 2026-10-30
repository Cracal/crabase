add_library(crabase_options INTERFACE)

target_compile_options(crabase_options INTERFACE
    # GCC/Clang
    $<$<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>:
        -Wall -Werror -Wextra -pedantic

        -Wno-cast-function-type
        -fvisibility=hidden
    >
)

target_compile_options(crabase_options INTERFACE
    # MSVC
    $<$<C_COMPILER_ID:MSVC>:
        /W4 /WX /utf-8 /std:c11
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
