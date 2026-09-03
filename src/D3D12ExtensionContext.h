/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D12ExtensionContext.h

Abstract:   D3D12 Extension Context - not supported in this repo.

Notes:      This repo only implements the D3D11/DXVK extension path (see README.md).
            D3D12ExtensionContext exists only so the D3D12 API DLL entry points have
            something to talk to: GetSupportedVersions always reports no supported
            versions and InitExtensions always fails, so a D3D12 extension context
            can never actually be created.

\*****************************************************************************/

#pragma once

struct D3D12ExtensionContext final : ExtensionContextBase
{
    D3D12ExtensionContext()                                        = default;
    D3D12ExtensionContext(const D3D12ExtensionContext&)            = delete;
    D3D12ExtensionContext& operator=(const D3D12ExtensionContext&) = delete;

    HRESULT GetSupportedVersions(const void* pDevice, INTCExtensionVersionHelper* driverExtensionVersion);
    HRESULT InitExtensions(const void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, INTCExtensionInfo1* pExtensionInfo, INTCExtensionAppInfo1* pExtensionAppInfo, bool internalExtensions = false);
};
