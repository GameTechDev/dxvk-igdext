/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D11ExtensionContext.h

Abstract:   Modern D3D11 extension entry points, forwarding to DXVK

Notes:      Declares D3D11ExtensionContext, the class backing every
            _INTC_D3D11_* export in IgdextApiDll.cpp.

\*****************************************************************************/

#pragma once

#include "ExtensionVersions.h"

struct D3D11ExtensionContext final : ExtensionContextBase
{
    ComPtr<ID3D11Device>        m_pAppDevice; // Store D3D11 Device used for creating the interface
    ComPtr<ID3D11DeviceContext> m_pDeviceContext;

    INTCExtensionVersionHelper m_SupportedExtVersion = {0}; // Version agreed between the library and driver

    D3D11ExtensionContext() = default;

    D3D11ExtensionContext(const D3D11ExtensionContext&)            = delete;
    D3D11ExtensionContext& operator=(const D3D11ExtensionContext&) = delete;

    ~D3D11ExtensionContext() = default;

    HRESULT QueryCapsSupport2(INTCDeviceInfo& deviceInfo);
    HRESULT GetSupportedVersions(const void* pDevice, INTCExtensionVersionHelper* driverExtensionVersion);
    HRESULT InitExtensions(const void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, INTCExtensionInfo* pExtensionInfo, INTCExtensionAppInfo1* pExtensionAppInfo);

    void MultiDrawInstancedIndirect(
        ID3D11DeviceContext*  pDeviceContext,
        UINT                  drawCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    void MultiDrawIndexedInstancedIndirect(
        ID3D11DeviceContext*  pDeviceContext,
        UINT                  drawCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    void MultiDrawInstancedIndirectCountIndirect(
        ID3D11DeviceContext*  pDeviceContext,
        ID3D11Buffer*         pBufferForDrawCount,
        UINT                  alignedByteOffsetForDrawCount,
        UINT                  maxCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    void MultiDrawIndexedInstancedIndirectCountIndirect(
        ID3D11DeviceContext*  pDeviceContext,
        ID3D11Buffer*         pBufferForDrawCount,
        UINT                  alignedByteOffsetForDrawCount,
        UINT                  maxCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    HRESULT BeginUAVOverlap();

    HRESULT EndUAVOverlap();

    void SetDepthBounds(
        BOOL  bEnable,
        FLOAT Min,
        FLOAT Max);

    HRESULT CreateTexture2D(
        const INTC_D3D11_TEXTURE2D_DESC* pDesc,
        const D3D11_SUBRESOURCE_DATA*    pInitialData,
        ID3D11Texture2D**                ppTexture2D);

private:
    // DXVK does not support driver caps query. HW Feature Level 5 / API Version 20 covers all currently supported extensions.
    static constexpr INTCExtensionVersion c_MaxDriverSupportedExtVersion = {EXTENSION_HW_FEATURE_LEVEL_5, EXTENSION_API_VERSION_20, EXTENSION_REVISION_0};
};
