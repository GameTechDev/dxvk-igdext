/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IgdextUnixlib.h

Abstract:   Relays application/engine name+version to igdext_unix.dll,
            a standard Wine unixlib module (see igdext_unix) that
            forwards to its own native side.

Notes:      Version components mirror the real SDK's INTCAppInfoVersion
            (igdext.h), split into scalars since this header must stay
            SDK-independent.

\*****************************************************************************/
#pragma once

#include <cstdint>

// Returns true if the info was relayed through igdext_unix.dll, false if the
// bridge isn't available (not present, missing export) or the relay call
// itself failed.
bool IgdextUnixlib_SetApplicationInfo(
    const char* applicationName,
    uint32_t    applicationVersionMajor,
    uint32_t    applicationVersionMinor,
    uint32_t    applicationVersionPatch,
    const char* engineName,
    uint32_t    engineVersionMajor,
    uint32_t    engineVersionMinor,
    uint32_t    engineVersionPatch);
