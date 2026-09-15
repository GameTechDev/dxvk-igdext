/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IntelAppInfoUnix.c

Abstract:   Native (Unix-side) half of the WineUnixLib bridge.

Notes:      Publishes into a plain struct, no locking - a torn read of this
            diagnostic-only data is a non-issue. Exposes only
            DxvkIgdext_INTC_D3D12_GetApplicationInfo externally, not the raw
            struct. Reachable from any other native code in this process
            (e.g. the real driver) via dlopen(RTLD_NOLOAD)+dlsym.

\*****************************************************************************/
#include "IntelAppInfoUnixProtocol.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct IntelAppInfoUnixShared
{
    bool                 published;
    IntelAppInfoUnixArgs info;
} IntelAppInfoUnixShared;

static IntelAppInfoUnixShared g_IntelAppInfoShared =
{
    .published = false,
};

typedef int32_t (*unixlib_entry_t)(void* args);

static int32_t DxvkIgdext_INTC_D3D12_SetApplicationInfo(void* args)
{
    const IntelAppInfoUnixArgs* info = (const IntelAppInfoUnixArgs*)args;

    g_IntelAppInfoShared.info      = *info;
    g_IntelAppInfoShared.published = true;

    return 0;
}

const unixlib_entry_t __wine_unix_call_funcs[] =
{
    DxvkIgdext_INTC_D3D12_SetApplicationInfo,
};

__attribute__((visibility("default")))
bool DxvkIgdext_INTC_D3D12_GetApplicationInfo(IntelAppInfoUnixArgs* outArgs)
{
    *outArgs = g_IntelAppInfoShared.info;

    return g_IntelAppInfoShared.published;
}
