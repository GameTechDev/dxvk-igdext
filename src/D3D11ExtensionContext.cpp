/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D11ExtensionContext.cpp

Abstract:   Modern D3D11 extension entry points, forwarding to DXVK

Notes:      Implements D3D11ExtensionContext: version negotiation, DXVK
            detection, forwarding UAV overlap, MultiDrawIndirect, and depth
            bounds to DXVK's vendor extension interface, and CreateTexture2D
            via the plain ID3D11Device (no DXVK-specific extension involved).

\*****************************************************************************/
#include "Stdafx.h"

#include "DxvkInterfaces.h"

struct INTCExtensionVersionHelper;

// Internal Intel Device information extension
HRESULT D3D11ExtensionContext::QueryCapsSupport2(INTCDeviceInfo& deviceInfo)
{
    deviceInfo = {};
    GetGTGenerationName(&deviceInfo);

    deviceInfo.GPUMaxFreq   = c_GPUMaxFreq;
    deviceInfo.GPUMinFreq   = c_GPUMinFreq;
    deviceInfo.GTGeneration = c_GTGeneration;
    deviceInfo.EUCount      = c_EUCount;
    deviceInfo.PackageTDP   = c_PackageTDP;
    deviceInfo.MaxFillRate  = c_MaxFillRate;

    return S_OK;
}

HRESULT D3D11ExtensionContext::GetSupportedVersions(const void* pDevice, INTCExtensionVersionHelper* driverExtensionVersion)
{
    INTCExtensionVersion driverSupportedVersion = c_MaxDriverSupportedExtVersion;

    m_pAppDevice = const_cast<ID3D11Device*>(reinterpret_cast<const ID3D11Device*>(pDevice));

    // Return the obtained version numbers
    *driverExtensionVersion = driverSupportedVersion;

    return S_OK;
}

HRESULT D3D11ExtensionContext::InitExtensions(const void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, INTCExtensionInfo* pExtensionInfo, INTCExtensionAppInfo1* pExtensionAppInfo)
{
    HRESULT                    result                 = S_OK;
    INTCExtensionVersionHelper driverSupportedVersion = c_MaxDriverSupportedExtVersion;

    // Trivially reject non-initialized variables
    if (pExtensionInfo->RequestedExtensionVersion.HWFeatureLevel == 0 && pExtensionInfo->RequestedExtensionVersion.APIVersion == 0)
    {
        return E_NOINTERFACE;
    }

    // Save the Device first
    m_pAppDevice = const_cast<ID3D11Device*>(reinterpret_cast<const ID3D11Device*>(pDevice));
    m_pAppDevice->GetImmediateContext(&m_pDeviceContext);

    if (!DetectDxvk(m_pDeviceContext.Get()))
    {
        return E_NOINTERFACE;
    }

    // Check if driver supports requested extension version
    bool isExtensionVersionDriverSupported = driverSupportedVersion.IsSupported(pExtensionInfo->RequestedExtensionVersion);

    if (isExtensionVersionDriverSupported)
    {
        // Save initialized version in the Extension Context
        m_SupportedExtVersion = pExtensionInfo->RequestedExtensionVersion;
    }
    else
    {
        return E_NOINTERFACE;
    }

    if (FAILED(result = GetDeviceDriverDescription(m_pAppDevice.Get())))
    {
        return result;
    }

    // Request for additional Intel Device information and return to application
    QueryCapsSupport2(pExtensionInfo->IntelDeviceInfo);
    pExtensionInfo->pDeviceDriverDesc       = m_DeviceDriverDescription.c_str(); // Get device driver description signature
    pExtensionInfo->pDeviceDriverVersion    = c_DeviceDriverVersion;
    pExtensionInfo->DeviceDriverBuildNumber = c_DeviceDriverBuildNumber;

    return result;
}

void D3D11ExtensionContext::MultiDrawInstancedIndirect(
    ID3D11DeviceContext* pDeviceContext,
    UINT                 drawCount,
    ID3D11Buffer*        pBufferForArgs,
    UINT                 alignedByteOffsetForArgs,
    UINT                 byteStrideForArgs)
{
    if (drawCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = m_pDeviceContext.Get();
    }

    ScopedDxVkExtContext dxVkExtCtx(GetDxVkExtContext(), pDeviceContext, m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndirect(drawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

void D3D11ExtensionContext::MultiDrawIndexedInstancedIndirect(
    ID3D11DeviceContext* pDeviceContext,
    UINT                 drawCount,
    ID3D11Buffer*        pBufferForArgs,
    UINT                 alignedByteOffsetForArgs,
    UINT                 byteStrideForArgs)
{
    if (drawCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = m_pDeviceContext.Get();
    }

    ScopedDxVkExtContext dxVkExtCtx(GetDxVkExtContext(), pDeviceContext, m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndexedIndirect(drawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

void D3D11ExtensionContext::MultiDrawInstancedIndirectCountIndirect(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Buffer*        pBufferForDrawCount,
    UINT                 alignedByteOffsetForDrawCount,
    UINT                 maxCount,
    ID3D11Buffer*        pBufferForArgs,
    UINT                 alignedByteOffsetForArgs,
    UINT                 byteStrideForArgs)
{
    if (maxCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = m_pDeviceContext.Get();
    }

    ScopedDxVkExtContext dxVkExtCtx(GetDxVkExtContext(), pDeviceContext, m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndirectCount(maxCount, pBufferForDrawCount, alignedByteOffsetForDrawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

void D3D11ExtensionContext::MultiDrawIndexedInstancedIndirectCountIndirect(
    ID3D11DeviceContext* pDeviceContext,
    ID3D11Buffer*        pBufferForDrawCount,
    UINT                 alignedByteOffsetForDrawCount,
    UINT                 maxCount,
    ID3D11Buffer*        pBufferForArgs,
    UINT                 alignedByteOffsetForArgs,
    UINT                 byteStrideForArgs)
{
    if (maxCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = m_pDeviceContext.Get();
    }

    ScopedDxVkExtContext dxVkExtCtx(GetDxVkExtContext(), pDeviceContext, m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndexedIndirectCount(maxCount, pBufferForDrawCount, alignedByteOffsetForDrawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

HRESULT D3D11ExtensionContext::BeginUAVOverlap()
{
    ID3D11VkExtContext* dxVkExtCtx = GetDxVkExtContext();
    if (dxVkExtCtx == nullptr)
    {
        return E_NOINTERFACE;
    }
    dxVkExtCtx->SetBarrierControl(D3D11_VK_BARRIER_CONTROL_IGNORE_WRITE_AFTER_WRITE);
    return S_OK;
}

HRESULT D3D11ExtensionContext::EndUAVOverlap()
{
    ID3D11VkExtContext* dxVkExtCtx = GetDxVkExtContext();
    if (dxVkExtCtx == nullptr)
    {
        return E_NOINTERFACE;
    }
    dxVkExtCtx->SetBarrierControl(0);
    return S_OK;
}

void D3D11ExtensionContext::SetDepthBounds(
    BOOL  bEnable,
    FLOAT Min,
    FLOAT Max)
{
    ID3D11VkExtContext* dxVkExtCtx = GetDxVkExtContext();
    if (dxVkExtCtx == nullptr)
    {
        return;
    }
    dxVkExtCtx->SetDepthBoundsTest(bEnable, Min, Max);
}

HRESULT D3D11ExtensionContext::CreateTexture2D(
    const INTC_D3D11_TEXTURE2D_DESC* pDesc,
    const D3D11_SUBRESOURCE_DATA*    pInitialData,
    ID3D11Texture2D**                ppTexture2D)
{
    ID3D11Device* pDevice = m_pAppDevice.Get();

    HRESULT result;

    if (pDesc->pD3D11Desc->SampleDesc.Count > 1 || pDesc->pD3D11Desc->MiscFlags & D3D11_RESOURCE_MISC_TILED || pDesc->pD3D11Desc->MiscFlags & D3D11_RESOURCE_MISC_TILE_POOL)
    {
        // Multisampled/tiled resources rely on Intel-specific driver features (e.g. Tile4,
        // emulated 64-bit atomics) with no DXVK equivalent, so they aren't supported here.
        return E_NOTIMPL;
    }

    // No atomics emulation requested: modern Intel GPUs support 64-bit typed atomics
    // natively, so the plain device call is all that's needed here.
    result = pDevice->CreateTexture2D(
        pDesc->pD3D11Desc,
        pInitialData,
        ppTexture2D);

    return result;
}
