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
    $<$<C_COMPILER_ID:MSVC>:/W4 /WX /utf-8>
)

target_compile_options(crabase_options INTERFACE
    $<$<AND:$<PLATFORM_ID:Linux>, $<OR:$<C_COMPILER_ID:GNU>,$<C_COMPILER_ID:Clang>>>:
        -lm
    >
)

option(ENABLE_MEMORY_LEAK_DETECTOR "Enable memory leak detector" ON)
if(ENABLE_MEMORY_LEAK_DETECTOR)
    target_compile_definitions(crabase_options INTERFACE
        $<$<CONFIG:Debug>:CRABASE_DEBUG CRABASE_MEMORY_LEAK_DETECTOR>
    )
endif()
