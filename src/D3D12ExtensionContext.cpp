/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D12ExtensionContext.cpp

Abstract:   D3D12 Extension Context (not supported in this repo) and
            INTC_D3D12_SetApplicationInfo (fully implemented).

Notes:      See D3D12ExtensionContext.h - this always reports zero supported
            versions and always fails to initialize, so no D3D12 extension
            context can ever be created. D3D12SetApplicationInfo needs no
            context and relays through igdext_unix.dll - see
            IgdextUnixlib.h and igdext_unix.

\*****************************************************************************/
#include "Stdafx.h"

#include "IgdextUnixlib.h"

namespace
{
    std::string WideToUtf8(const wchar_t* wide)
    {
        if (!wide || !*wide)
        {
            return {};
        }

        int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
        if (size <= 0)
        {
            return {};
        }

        std::string result(size - 1, '\0'); // size includes the null terminator, std::string adds its own
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), size, nullptr, nullptr);
        return result;
    }
}

HRESULT D3D12ExtensionContext::GetSupportedVersions(const void* pDevice, INTCExtensionVersionHelper* driverExtensionVersion)
{
    // No D3D12 extensions are implemented - report the "no version supported" sentinel.
    *driverExtensionVersion = INTCExtensionVersionHelper(0);

    return S_OK;
}

HRESULT D3D12ExtensionContext::InitExtensions(const void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, INTCExtensionInfo1* pExtensionInfo, INTCExtensionAppInfo1* pExtensionAppInfo, bool internalExtensions)
{
    // D3D12 extension contexts cannot be created in this repo.
    return E_NOINTERFACE;
}

HRESULT D3D12SetApplicationInfo(INTCExtensionAppInfo1* pExtensionAppInfo)
{
    if (!pExtensionAppInfo)
    {
        return E_INVALIDARG;
    }

    bool relayed = IgdextUnixlib_SetApplicationInfo(
        WideToUtf8(pExtensionAppInfo->pApplicationName).c_str(),
        pExtensionAppInfo->ApplicationVersion.major, pExtensionAppInfo->ApplicationVersion.minor, pExtensionAppInfo->ApplicationVersion.patch,
        WideToUtf8(pExtensionAppInfo->pEngineName).c_str(),
        pExtensionAppInfo->EngineVersion.major, pExtensionAppInfo->EngineVersion.minor, pExtensionAppInfo->EngineVersion.patch);

    return relayed ? S_OK : E_FAIL;
}
