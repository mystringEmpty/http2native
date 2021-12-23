cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake
  CACHE STRING "Vcpkg toolchain file")
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" 
  CACHE STRING "Minimum OS X deployment version")
project(Native)

include_directories(Native Http)

aux_source_directory(Native Native_SRC)
aux_source_directory(Http Http_SRC)

add_library(http2native SHARED ${Native_SRC} ${Http_SRC})

set_property(TARGET http2native PROPERTY CXX_STANDARD 17)
set_property(TARGET http2native PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET http2native PROPERTY PREFIX "")
set_property(TARGET http2native PROPERTY SUFFIX ".dll")


if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "-fpermissive ${CMAKE_CXX_FLAGS}")
    target_link_libraries(http2native PRIVATE $<$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>:stdc++fs>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    set(CMAKE_CXX_FLAGS "-fpermissive -Wno-invalid-source-encoding ${CMAKE_CXX_FLAGS}")
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "-fpermissive -Wno-invalid-source-encoding ${CMAKE_CXX_FLAGS}")
    target_link_libraries(http2native PRIVATE $<$<VERSION_LESS:$<CXX_COMPILER_VERSION>,9.0>:c++fs>)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "/source-charset:utf-8 /execution-charset:utf-8 ${CMAKE_CXX_FLAGS} /EHsc-")
    string(REGEX MATCH static IS_STATIC ${VCPKG_TARGET_TRIPLET})
    if(IS_STATIC STREQUAL static)
        set_property(TARGET http2native PROPERTY
            MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()

include(CheckCSourceCompiles)
	
check_c_source_compiles(
[=[
int main ()
{
    return 0;
}
]=]
BSYMBOLIC_WORKS
)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported)

if(supported)
    set_property(TARGET http2native PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

find_package(OpenSSL REQUIRED)
target_link_libraries(http2native PRIVATE OpenSSL::SSL OpenSSL::Crypto)
find_package(civetweb CONFIG REQUIRED)
target_link_libraries(http2native PRIVATE civetweb::civetweb civetweb::civetweb-cpp)

set_target_properties(http2native PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(http2native PROPERTIES C_VISIBILITY_PRESET hidden)

if(NOT ((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")))
    set_target_properties(http2native PROPERTIES LINK_FLAGS_RELEASE "-s -Wl,--exclude-libs,ALL")
endif()
