/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IgdextUnixlib.cpp

Abstract:   See IgdextUnixlib.h.

Notes:      igdext_unix.dll is a normal PE dll (built against the Wine
            toolchain, see igdext_unix) - loading it is a plain
            LoadLibraryW+GetProcAddress. Its own unixlib.c handles the real
            Unix-side bridge internally.

\*****************************************************************************/
#include "Stdafx.h"

#include "IgdextUnixlib.h"

namespace
{
    using PFN_SetApplicationInfo = BOOL(WINAPI*)(
        const char* applicationName, UINT32 appMajor, UINT32 appMinor, UINT32 appPatch,
        const char* engineName, UINT32 engMajor, UINT32 engMinor, UINT32 engPatch);

    PFN_SetApplicationInfo LoadSetApplicationInfo()
    {
        HMODULE hModule = LoadLibraryW(L"igdext_unix.dll");
        if (!hModule)
        {
            return nullptr;
        }

        return reinterpret_cast<PFN_SetApplicationInfo>(GetProcAddress(hModule, "IgdextUnix_SetApplicationInfo"));
    }

    PFN_SetApplicationInfo GetSetApplicationInfo()
    {
        static PFN_SetApplicationInfo pfnSetApplicationInfo = LoadSetApplicationInfo();
        return pfnSetApplicationInfo;
    }
}

bool IgdextUnixlib_SetApplicationInfo(
    const char* applicationName,
    uint32_t    applicationVersionMajor,
    uint32_t    applicationVersionMinor,
    uint32_t    applicationVersionPatch,
    const char* engineName,
    uint32_t    engineVersionMajor,
    uint32_t    engineVersionMinor,
    uint32_t    engineVersionPatch)
{
    PFN_SetApplicationInfo pfnSetApplicationInfo = GetSetApplicationInfo();
    if (!pfnSetApplicationInfo)
    {
        return false;
    }

    return pfnSetApplicationInfo(
        applicationName, applicationVersionMajor, applicationVersionMinor, applicationVersionPatch,
        engineName, engineVersionMajor, engineVersionMinor, engineVersionPatch) != FALSE;
}
