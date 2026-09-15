/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IntelAppInfoUnixProtocol.h

Abstract:   Wire format shared between the PE side and the native Linux
            side of the WineUnixLib bridge.

Notes:      Plain C, no platform headers - compiled by both the MinGW cross
            compiler and the host's native compiler.

\*****************************************************************************/
#pragma once

#include <stdint.h>

enum IntelAppInfoUnixFunc
{
    IntelAppInfoUnixFunc_SetEngineInfo = 0,
};

// Mirrors the real SDK's INTCAppInfoVersion layout.
typedef struct IntelAppInfoUnixVersion
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
    uint32_t reserved;
} IntelAppInfoUnixVersion;

typedef struct IntelAppInfoUnixArgs
{
    char                    applicationName[256];
    IntelAppInfoUnixVersion applicationVersion;
    char                    engineName[256];
    IntelAppInfoUnixVersion engineVersion;
} IntelAppInfoUnixArgs;
