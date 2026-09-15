/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D12ExtensionContext.h

Abstract:   Modern D3D12 extension entry points: version negotiation and
            context creation

Notes:      Declares D3D12ExtensionContext, the class backing every
            _INTC_D3D12_* export in IgdextApiDll.cpp - mirrors
            D3D11ExtensionContext's pattern.

\*****************************************************************************/

#pragma once

#include "ExtensionVersions.h"

struct D3D12ExtensionContext final : ExtensionContextBase
{
    ComPtr<ID3D12Device> m_pAppDevice; // Store D3D12 Device used for creating the interface

    INTCExtensionVersionHelper m_SupportedExtVersion = {0}; // Version agreed between the library and driver

    D3D12ExtensionContext() = default;

    D3D12ExtensionContext(const D3D12ExtensionContext&)            = delete;
    D3D12ExtensionContext& operator=(const D3D12ExtensionContext&) = delete;

    HRESULT GetSupportedVersions(const void* pDevice, INTCExtensionVersionHelper* driverExtensionVersion);
    HRESULT InitExtensions(const void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, INTCExtensionInfo1* pExtensionInfo, INTCExtensionAppInfo1* pExtensionAppInfo, bool internalExtensions = false);

private:
    // DXVK/vkd3d-proton has no way to query real GMD ID or Xe core count -
    // placeholders, matching ExtensionContextBase's other synthesized values.
    static constexpr INTCExtensionVersion c_MaxDriverSupportedExtVersion = {EXTENSION_HW_FEATURE_LEVEL_5, EXTENSION_API_VERSION_20, EXTENSION_REVISION_0};
};
