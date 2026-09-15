/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IntelAppInfoUnixlib.h

Abstract:   Relays application/engine name+version to igdext64_unix.so via
            the WineUnixLib bridge.

Notes:      Version components mirror the real SDK's INTCAppInfoVersion
            (igdext.h), split into scalars since this header must stay
            SDK-independent.

\*****************************************************************************/
#pragma once

#include <cstdint>

// Best-effort; returns false if the bridge isn't available.
bool IntelAppInfoUnixlib_SetEngineInfo(
    const char* applicationName,
    uint32_t    applicationVersionMajor,
    uint32_t    applicationVersionMinor,
    uint32_t    applicationVersionPatch,
    const char* engineName,
    uint32_t    engineVersionMajor,
    uint32_t    engineVersionMinor,
    uint32_t    engineVersionPatch);
