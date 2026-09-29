/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  unixlib.c

Abstract:   Unix side of igdext_unix

Notes:      Keeps the published info and exports
            igdext_unix_get_application_info() for the native Vulkan
            driver to pull.

\*****************************************************************************/

#if 0
#pragma makedep unix
#endif

#include "config.h"

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winternl.h"

#include "wine/debug.h"

#include "unixlib.h"

WINE_DEFAULT_DEBUG_CHANNEL(igdext_unix);

static struct
{
    BOOL                     published;
    struct set_application_info_params info;
} app_info;

static NTSTATUS set_application_info( void *args )
{
    const struct set_application_info_params *params = args;

    TRACE( "app '%s' (v%u.%u.%u) engine '%s' (v%u.%u.%u).\n",
           params->application_name, params->application_version.major,
           params->application_version.minor, params->application_version.patch,
           params->engine_name, params->engine_version.major,
           params->engine_version.minor, params->engine_version.patch );

    app_info.info = *params;
    app_info.published = TRUE;
    return STATUS_SUCCESS;
}

const unixlib_entry_t __wine_unix_call_funcs[] =
{
    set_application_info,
};

/* Exported for any other native code in this process (e.g. the real Intel
 * driver) to pick up via dlopen(RTLD_NOLOAD)+dlsym - independent of the
 * WINE_UNIX_CALL mechanism above. */
__attribute__((visibility("default")))
BOOL igdext_unix_get_application_info( struct set_application_info_params *out )
{
    *out = app_info.info;
    return app_info.published;
}
