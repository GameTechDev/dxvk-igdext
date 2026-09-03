/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  LegacyD3D12ExtensionContext.cpp

Abstract:   Legacy D3D12 extension context - stub, no D3D12 support

Notes:      GetSupportedVersions always reports the unsupported sentinel and
            InitExtensions always fails - D3D12 isn't implemented in this
            repo.

\*****************************************************************************/
#include "Stdafx.h"
#include "LegacyD3D12ExtensionContext.h"

HRESULT LegacyD3D12ExtensionContext::GetSupportedVersions([[maybe_unused]] void* pDevice, LegacyExtensionVersion* driverExtensionVersion)
{
    // No D3D12 extensions are implemented - report the "no version supported" sentinel.
    *driverExtensionVersion = {};
    return S_OK;
}

HRESULT LegacyD3D12ExtensionContext::InitExtensions([[maybe_unused]] void* pDevice, [[maybe_unused]] void** ppfnExtensionFuncs, [[maybe_unused]] UINT32 extensionFuncsSize, [[maybe_unused]] LegacyExtensionInfo* pExtensionInfo, [[maybe_unused]] LegacyExtensionAppInfo* pExtensionAppInfo)
{
    // D3D12 extension contexts cannot be created in this repo.
    return E_NOINTERFACE;
}
