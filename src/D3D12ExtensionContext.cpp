/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D12ExtensionContext.cpp

Abstract:   D3D12 Extension Context - not supported in this repo.

Notes:      See D3D12ExtensionContext.h - this always reports zero supported
            versions and always fails to initialize, so no D3D12 extension
            context can ever be created.

\*****************************************************************************/
#include "Stdafx.h"

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
