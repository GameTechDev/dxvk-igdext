/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  LegacyD3D12ExtensionContext.h

Abstract:   Legacy D3D12 extension context - stub, no D3D12 support

Notes:      D3D12 extensions are not implemented in this repo - GetSupportedVersions
            always reports zero versions and InitExtensions always fails.

\*****************************************************************************/

#pragma once

struct LegacyD3D12ExtensionContext final : ExtensionContextBase
{
    LegacyD3D12ExtensionContext()                                              = default;
    LegacyD3D12ExtensionContext(const LegacyD3D12ExtensionContext&)            = delete;
    LegacyD3D12ExtensionContext& operator=(const LegacyD3D12ExtensionContext&) = delete;

    HRESULT GetSupportedVersions(void* pDevice, LegacyExtensionVersion* driverExtensionVersion);
    HRESULT InitExtensions(void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, LegacyExtensionInfo* pExtensionInfo, LegacyExtensionAppInfo* pExtensionAppInfo);
};
