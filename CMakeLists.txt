cmake_minimum_required(VERSION 3.22.0)
project(crabase VERSION 0.1.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# set(CMAKE_C_EXTENSIONS OFF)
message(STATUS "c standard: ${CMAKE_C_STANDARD}")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Configuration types: ${CMAKE_CONFIGURATION_TYPES}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

foreach(cfg DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${cfg} ${CMAKE_BINARY_DIR}/lib)
endforeach()

option(CRABASE_BUILD_TESTS "Build tests" ON)
option(CRABASE_BUILD_SHARED_LIBS "Build shared library" ON)
option(CRABASE_LOG_FILE_LINE "Add __FILE__ and __LINE__ to log" ON)
option(CRABASE_RANDOM_STR_INIT_HASHCODE "Enable random string init hashcode" ON)
option(CRABASE_JSON_SAFE_INT "Force SAFE_INT in json" ON)



include(cmake/crabaseOptions.cmake)

if(CRABASE_BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()


file(GLOB_RECURSE CRABASE_SRC CONFIGURE_DEPENDS src/*.c)
file(GLOB_RECURSE CRABASE_INC CONFIGURE_DEPENDS inc/*.h)

if(CRABASE_BUILD_SHARED_LIBS)
    add_library(crabase SHARED ${CRABASE_SRC} ${CRABASE_INC})
    target_compile_definitions(crabase PRIVATE CRABASE_EXPORT)
else()
    add_library(crabase STATIC ${CRABASE_SRC} ${CRABASE_INC})
    target_compile_definitions(crabase PUBLIC CRABASE_STATIC)
endif()
add_library(crabase::crabase ALIAS crabase)

target_include_directories(crabase
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<INSTALL_INTERFACE:inc>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(crabase PUBLIC crabase_options)
if(CRABASE_LOG_FILE_LINE)
    target_compile_definitions(crabase PRIVATE CRA_LOG_FILE_LINE)
endif()
if(CRABASE_RANDOM_STR_INIT_HASHCODE)
    target_compile_definitions(crabase PRIVATE CRA_RANDOM_STR_INIT_HASHCODE)
endif()
if(CRABASE_JSON_SAFE_INT)
    target_compile_definitions(crabase PUBLIC CRA_JSON_SAFE_INT)
endif()



include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(TARGETS crabase crabase_options
    EXPORT crabaseTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    PUBLIC_HEADER DESTINATION inc
)

install(EXPORT crabaseTargets
    FILE crabaseTargets.cmake
    NAMESPACE crabase::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/crabase
)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/crabaseConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/crabaseConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/crabase
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/crabaseConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
