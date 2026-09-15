/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IntelAppInfoUnixlib.cpp

Abstract:   See IntelAppInfoUnixlib.h.

\*****************************************************************************/
#include "Stdafx.h"

#include "IntelAppInfoUnixlib.h"

#include "WineUnixLib.h"
#include "unix/IntelAppInfoUnixProtocol.h"

#include <cstring>

namespace
{
    WineUnixLib& GetLib()
    {
        static WineUnixLib lib = LoadWineUnixLib(L"igdext64_unix");
        return lib;
    }

    void CopyTruncated(char (&dst)[256], const char* src)
    {
        strncpy(dst, src, sizeof(dst) - 1);
        dst[sizeof(dst) - 1] = '\0';
    }
}

bool IntelAppInfoUnixlib_SetEngineInfo(
    const char* applicationName,
    uint32_t    applicationVersionMajor,
    uint32_t    applicationVersionMinor,
    uint32_t    applicationVersionPatch,
    const char* engineName,
    uint32_t    engineVersionMajor,
    uint32_t    engineVersionMinor,
    uint32_t    engineVersionPatch)
{
    const WineUnixLib& lib = GetLib();
    if (!lib.IsLoaded())
    {
        return false;
    }

    IntelAppInfoUnixArgs args{};
    CopyTruncated(args.applicationName, applicationName);
    args.applicationVersion.major = applicationVersionMajor;
    args.applicationVersion.minor = applicationVersionMinor;
    args.applicationVersion.patch = applicationVersionPatch;
    CopyTruncated(args.engineName, engineName);
    args.engineVersion.major = engineVersionMajor;
    args.engineVersion.minor = engineVersionMinor;
    args.engineVersion.patch = engineVersionPatch;

    return CallWineUnixLib(lib, IntelAppInfoUnixFunc_SetEngineInfo, &args);
}
