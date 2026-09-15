cmake_minimum_required(VERSION 3.10)
project(dxvk-igdext LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SRC_DIR         "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(LEGACY_DIR      "${SRC_DIR}/legacy")
set(DLL_DIR         "${SRC_DIR}/dll")

# Vendored Intel Graphics Extensions public SDK header (igdext.h)
set(D3DEXT_SDK_PUBLIC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/d3dext-sdk/include/public")

set(SOURCES
    ${SRC_DIR}/ExtensionContextBase.cpp
    ${SRC_DIR}/D3D11ExtensionContext.cpp
    ${SRC_DIR}/D3D12ExtensionContext.cpp
    ${SRC_DIR}/WineUnixLib.cpp
    ${SRC_DIR}/IntelAppInfoUnixlib.cpp
    ${LEGACY_DIR}/LegacyD3D11ExtensionContext.cpp
    ${LEGACY_DIR}/LegacyD3D12ExtensionContext.cpp
    ${DLL_DIR}/IgdextApiDll.cpp
    ${DLL_DIR}/IgdextApiDllInt.cpp
    ${DLL_DIR}/IgdextApiDllLegacy.cpp
    ${DLL_DIR}/Igdext.def
)

add_library(igdext SHARED ${SOURCES})

target_include_directories(igdext PRIVATE
    ${SRC_DIR}
    ${LEGACY_DIR}
    ${D3DEXT_SDK_PUBLIC_DIR}
)

target_compile_definitions(igdext PRIVATE
    NOMINMAX
    WIN32_LEAN_AND_MEAN
    # Only D3D11 is unlocked in the vendored igdext.h: the D3D12 section it
    # gates behind INTC_IGDEXT_D3D12 references D3D12_STATE_OBJECT_DESC and
    # other raytracing types mingw-w64's d3d12.h doesn't define, and D3D12
    # support in this repo is stub-only anyway (see src/Stdafx.h).
    INTC_IGDEXT_D3D11
)

target_link_libraries(igdext PRIVATE dxgi ntdll)

set_target_properties(igdext PROPERTIES
    OUTPUT_NAME "igdext64"
    PREFIX ""
)

if( MINGW )
    target_link_options(igdext PRIVATE -static -static-libgcc -static-libstdc++)
endif()

# igdext64_unix.so: native Linux side of the WineUnixLib bridge, built with
# the host's own compiler, not the MinGW cross toolchain.
set(UNIX_DIR "${SRC_DIR}/unix")

set(INTELAPPINFO_UNIX_CC "cc" CACHE STRING
    "Host C compiler for igdext64_unix.so (WineUnixLib bridge's native side). Must be a native Linux compiler, not the MinGW cross compiler.")

set(IGDEXT_UNIX_SO "${CMAKE_CURRENT_BINARY_DIR}/igdext64_unix.so")
add_custom_command(
    OUTPUT ${IGDEXT_UNIX_SO}
    COMMAND ${INTELAPPINFO_UNIX_CC} -shared -fPIC -O2 -o ${IGDEXT_UNIX_SO} ${UNIX_DIR}/IntelAppInfoUnix.c
    DEPENDS ${UNIX_DIR}/IntelAppInfoUnix.c ${UNIX_DIR}/IntelAppInfoUnixProtocol.h
    COMMENT "Building igdext64_unix.so with ${INTELAPPINFO_UNIX_CC}"
)
add_custom_target(igdext_unix ALL DEPENDS ${IGDEXT_UNIX_SO})
add_dependencies(igdext igdext_unix)

include(GNUInstallDirs)
install(TARGETS igdext RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
install(FILES ${IGDEXT_UNIX_SO} DESTINATION ${CMAKE_INSTALL_BINDIR})
