/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  LegacyD3D11ExtensionContext.h

Abstract:   Legacy D3D11 extension context (pre-modern-API ABI), forwarding to DXVK

Notes:      Declares LegacyD3D11ExtensionContext, which negotiates one of
            the historical D3D11_EXTENSION_FUNCS_* struct shapes and hands
            back static entry points forwarding MultiDrawIndirect/UAV
            overlap to DXVK.

\*****************************************************************************/

#pragma once

#include "LegacyExtensionVersions.h"

struct LegacyD3D11ExtensionContext final : ExtensionContextBase
{
    ComPtr<ID3D11Device>        m_pAppDevice; // Store D3D11 Device used for creating the interface
    ComPtr<ID3D11DeviceContext> m_pDeviceContext;
    D3D11_EXTENSION_FUNCS_0400 m_ExtensionFunctions{}; // This is ONLY suitable for legacy initialization

    uint32_t m_Legacy_SupportedExtVersion = 0; // Version agreed between the library and driver

    // By default we assume we are using D3D11 Intel Extensions (legacy fallback possible)
    bool m_LegacyInitialization = false;

    LegacyD3D11ExtensionContext() = default;

    LegacyD3D11ExtensionContext(const LegacyD3D11ExtensionContext&)            = delete;
    LegacyD3D11ExtensionContext& operator=(const LegacyD3D11ExtensionContext&) = delete;

    ~LegacyD3D11ExtensionContext() = default;

    HRESULT QueryCapsSupport2(LegacyIntelDeviceInfo& intelDeviceInfo);
    HRESULT GetSupportedVersions(void* pDevice, LegacyExtensionVersion* driverExtensionVersion);
    HRESULT InitExtensions(void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, LegacyExtensionInfo* pExtensionInfo, LegacyExtensionAppInfo* pExtensionAppInfo);

    void GetGTGenerationName(LegacyIntelDeviceInfo* pIntelDeviceInfo);

    BOOL ValidateMultiDrawIndirectArguments(
        UINT          drawCount,
        ID3D11Buffer* pBufferForArgs,
        UINT          alignedByteOffsetForArgs,
        UINT          byteStrideForArgs) const;

    BOOL ValidateMultiDrawIndirectCountIndirectArguments(
        ID3D11Buffer* pBufferForDrawCount,
        UINT          alignedByteOffsetForDrawCount,
        UINT          maxCount,
        ID3D11Buffer* pBufferForArgs,
        UINT          alignedByteOffsetForArgs,
        UINT          byteStrideForArgs) const;

    static HRESULT APIENTRY D3D11BeginUAVOverlap(
        ExtensionContextBase* pExtensionContext);

    static HRESULT APIENTRY D3D11EndUAVOverlap(
        ExtensionContextBase* pExtensionContext);

    static void APIENTRY D3D11MultiDrawInstancedIndirect(
        ExtensionContextBase* pExtensionContext,
        ID3D11DeviceContext*  pDeviceContext,
        UINT                  drawCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    static void APIENTRY D3D11MultiDrawIndexedInstancedIndirect(
        ExtensionContextBase* pExtensionContext,
        ID3D11DeviceContext*  pDeviceContext,
        UINT                  drawCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    static void APIENTRY D3D11MultiDrawInstancedIndirectCountIndirect(
        ExtensionContextBase* pExtensionContext,
        ID3D11DeviceContext*  pDeviceContext,
        ID3D11Buffer*         pBufferForDrawCount,
        UINT                  alignedByteOffsetForDrawCount,
        UINT                  maxCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

    static void APIENTRY D3D11MultiDrawIndexedInstancedIndirectCountIndirect(
        ExtensionContextBase* pExtensionContext,
        ID3D11DeviceContext*  pDeviceContext,
        ID3D11Buffer*         pBufferForDrawCount,
        UINT                  alignedByteOffsetForDrawCount,
        UINT                  maxCount,
        ID3D11Buffer*         pBufferForArgs,
        UINT                  alignedByteOffsetForArgs,
        UINT                  byteStrideForArgs);

private:
    // DXVK does not support driver caps query. Version 4.1.2 is the maximum for the legacy extension
    // interface and covers: MultiDrawIndirect, MultiDrawIndexedIndirect, MultiDrawIndirectCount,
    // MultiDrawIndexedIndirectCount, and UAV Overlap.
    static constexpr UINT32 c_MaxDriverSupportedExtVersion = D3D11_EXTENSION_VERSION_4_1_2;
};
