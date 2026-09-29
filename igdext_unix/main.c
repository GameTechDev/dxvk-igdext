/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  main.c

Abstract:   PE side of igdext_unix: publishes Intel D3D12 extension
            application/engine info for the native Vulkan driver to pick up

Notes:      Forwards IgdextUnix_SetApplicationInfo to unixlib.c via
            WINE_UNIX_CALL.

\*****************************************************************************/

#include <stdarg.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winternl.h"
#include "wine/debug.h"

#include "unixlib.h"

WINE_DEFAULT_DEBUG_CHANNEL(igdext_unix);

static INIT_ONCE unix_init_once = INIT_ONCE_STATIC_INIT;
static BOOL unix_lib_initialized;

static BOOL WINAPI init_unix_lib_once( INIT_ONCE *once, void *param, void **context )
{
    unix_lib_initialized = !__wine_init_unix_call();
    return TRUE;
}

static BOOL init_unix_lib(void)
{
    InitOnceExecuteOnce( &unix_init_once, init_unix_lib_once, NULL, NULL );
    return unix_lib_initialized;
}

static void copy_truncated( char *dst, size_t dst_size, const char *src )
{
    size_t i = 0;

    if (!src) src = "";
    for (; i + 1 < dst_size && src[i]; ++i) dst[i] = src[i];
    dst[i] = 0;
}

BOOL WINAPI IgdextUnix_SetApplicationInfo( const char *application_name,
        UINT32 application_version_major, UINT32 application_version_minor, UINT32 application_version_patch,
        const char *engine_name,
        UINT32 engine_version_major, UINT32 engine_version_minor, UINT32 engine_version_patch )
{
    struct set_application_info_params params = {0};

    TRACE( "app '%s' engine '%s'.\n", application_name, engine_name );

    if (!init_unix_lib())
        return FALSE;

    copy_truncated( params.application_name, sizeof(params.application_name), application_name );
    params.application_version.major = application_version_major;
    params.application_version.minor = application_version_minor;
    params.application_version.patch = application_version_patch;
    copy_truncated( params.engine_name, sizeof(params.engine_name), engine_name );
    params.engine_version.major = engine_version_major;
    params.engine_version.minor = engine_version_minor;
    params.engine_version.patch = engine_version_patch;

    return !WINE_UNIX_CALL( unix_set_application_info, &params );
}

BOOL WINAPI DllMain( HINSTANCE instance, DWORD reason, void *reserved )
{
    switch (reason)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls( instance );
            break;
    }

    return TRUE;
}
