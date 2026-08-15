# cmake -B build -A Win32
# cmake --build build --config Release
cmake_minimum_required(VERSION 3.20)
project("brownturbo-dx" LANGUAGES C CXX VERSION 0.0.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "" FORCE)
set(POLYHOOK_STATIC_RUNTIME OFF CACHE BOOL "" FORCE)
set(FORCE_STATIC_CRT OFF CACHE BOOL "" FORCE)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(__BITSTREAM_NATIVE_END)

add_library(${PROJECT_NAME} SHARED)

add_subdirectory(libs/rakhook/lib/minhook)
add_subdirectory(libs/rakhook)
add_subdirectory(libs/samp-api)

include_directories(libs/samp-api/include/sampapi)
include_directories(libs/rakhook/lib/minhook/include)
include_directories(libs/rakhook/include)

target_sources(${PROJECT_NAME} PRIVATE
	dllmain.cpp
    Plugin.cpp
)

set_target_properties(${PROJECT_NAME}
	PROPERTIES
	OUTPUT_NAME ${PROJECT_NAME}
	SUFFIX ".asi"
)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${PROJECT_NAME} PUBLIC d3d9 minhook rakhook sampapi)

function(force_dynamic_runtime_on_all_targets dir)
    get_property(subdirs DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirs})
        force_dynamic_runtime_on_all_targets(${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)
    foreach(target ${current_targets})
        get_target_property(target_type ${target} TYPE)
        if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
            set_property(TARGET ${target} PROPERTY 
                MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL"
            )
        endif()
    endforeach()
endfunction()

force_dynamic_runtime_on_all_targets(${CMAKE_CURRENT_SOURCE_DIR})