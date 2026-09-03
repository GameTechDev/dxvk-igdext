/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  LegacyD3D11ExtensionContext.cpp

Abstract:   Legacy D3D11 extension context (pre-modern-API ABI), forwarding to DXVK

Notes:      Implements LegacyD3D11ExtensionContext: version negotiation
            across the historical D3D11_EXTENSION_FUNCS_* struct shapes, and
            forwarding MultiDrawIndirect/UAV overlap to DXVK.

\*****************************************************************************/
#include "Stdafx.h"
#include "LegacyD3D11ExtensionContext.h"

#include "DxvkInterfaces.h"

void LegacyD3D11ExtensionContext::GetGTGenerationName(LegacyIntelDeviceInfo* pIntelDeviceInfo)
{
    const std::wstring_view DxvkDevice = c_GTGenerationName;
    auto&                   nameBuffer = pIntelDeviceInfo->GTGenerationName;

    std::fill(std::begin(nameBuffer), std::end(nameBuffer), '\0');
    std::copy_n(DxvkDevice.begin(), std::min(DxvkDevice.size(), std::size(nameBuffer) - 1), nameBuffer);
}

// Internal Intel Device information extension
HRESULT LegacyD3D11ExtensionContext::QueryCapsSupport2(LegacyIntelDeviceInfo& intelDeviceInfo)
{
    intelDeviceInfo = {};
    GetGTGenerationName(&intelDeviceInfo);

    intelDeviceInfo.GPUMaxFreq   = c_GPUMaxFreq;
    intelDeviceInfo.GPUMinFreq   = c_GPUMinFreq;
    intelDeviceInfo.GTGeneration = c_GTGeneration;
    intelDeviceInfo.EUCount      = c_EUCount;
    intelDeviceInfo.PackageTDP   = c_PackageTDP;
    intelDeviceInfo.MaxFillRate  = c_MaxFillRate;

    return S_OK;
}

HRESULT LegacyD3D11ExtensionContext::GetSupportedVersions(void* pDevice, LegacyExtensionVersion* driverExtensionVersion)
{
    UINT32 driverSupportedVersion = 0;

    m_pAppDevice = reinterpret_cast<ID3D11Device*>(pDevice);

    driverSupportedVersion = c_MaxDriverSupportedExtVersion;

    driverExtensionVersion->FullVersion = driverSupportedVersion;

    return S_OK;
}

HRESULT LegacyD3D11ExtensionContext::InitExtensions(void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, LegacyExtensionInfo* pExtensionInfo, LegacyExtensionAppInfo* pExtensionAppInfo)
{
    HRESULT result = S_OK;

    m_LegacyInitialization = extensionFuncsSize == 0 ? true : false;

    if (ppfnExtensionFuncs == nullptr)
    {
        return E_NOINTERFACE;
    }

    const UINT32 requestedVersion = pExtensionInfo->requestedExtensionVersion.FullVersion;

    // Legacy initialization only supports version 4.0, but can be returned for all base revisions
    const bool isLegacyVersionSupported =
        requestedVersion == D3D11_EXTENSION_VERSION_4_0 || requestedVersion == D3D11_EXTENSION_VERSION_5_0 || requestedVersion == D3D11_EXTENSION_VERSION_6_0 || requestedVersion == D3D11_EXTENSION_VERSION_7_0;

    // Non-legacy initialization can request any version in the supported list
    const bool isRequestedVersionSupported = m_LegacyInitialization
        ? isLegacyVersionSupported
        : std::find(std::begin(Legacy_SupportedExtVersions), std::end(Legacy_SupportedExtVersions), requestedVersion) != std::end(Legacy_SupportedExtVersions);

    if (!isRequestedVersionSupported)
    {
        return E_NOINTERFACE;
    }

    // Save the Device first
    m_pAppDevice = reinterpret_cast<ID3D11Device*>(pDevice);
    m_pAppDevice->GetImmediateContext(&m_pDeviceContext);

    if (!DetectDxvk(m_pDeviceContext.Get()))
    {
        return E_NOINTERFACE;
    }

    // The requested version was already confirmed above to be one of the versions this
    // reimplementation supports, so it is always compatible with c_MaxDriverSupportedExtVersion.
    m_Legacy_SupportedExtVersion                         = !m_LegacyInitialization ? pExtensionInfo->requestedExtensionVersion.FullVersion : D3D11_EXTENSION_VERSION_4_0; // Only 4.0 supported for legacy initialized
    pExtensionInfo->returnedExtensionVersion.FullVersion = m_Legacy_SupportedExtVersion;

    // If we agreed on a supported version, return version specific API entry points.
    // All D3D11 legacy versions ever only produced 2 distinct struct shapes (basic
    // MultiDraw-only vs extended MultiDraw+UAVOverlap), and those 2 shapes have distinct
    // sizes - so extensionFuncsSize alone (largest shape first, then the older/smaller one)
    // is sufficient to pick the right one, no need to enumerate every version number here.
    if (SUCCEEDED(result))
    {
        if (m_LegacyInitialization)
        {
            // D3D11CreateDeviceExtensionContext (extensionFuncsSize == 0) only ever hands back the base struct.
            memset(&m_ExtensionFunctions, 0, sizeof(D3D11_EXTENSION_FUNCS_0400));

            m_ExtensionFunctions.D3D11MultiDrawIndexedInstancedIndirect              = LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirect;
            m_ExtensionFunctions.D3D11MultiDrawIndexedInstancedIndirectCountIndirect = LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirectCountIndirect;
            m_ExtensionFunctions.D3D11MultiDrawInstancedIndirect                     = LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirect;
            m_ExtensionFunctions.D3D11MultiDrawInstancedIndirectCountIndirect        = LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirectCountIndirect;

            *ppfnExtensionFuncs = &m_ExtensionFunctions;
        }
        else if (extensionFuncsSize == sizeof(D3D11_EXTENSION_FUNCS_01000001))
        {
            D3D11_EXTENSION_FUNCS_01000001* extensionFunctions = reinterpret_cast<D3D11_EXTENSION_FUNCS_01000001*>(*ppfnExtensionFuncs);
            memset(extensionFunctions, 0, sizeof(D3D11_EXTENSION_FUNCS_01000001));

            extensionFunctions->D3D11MultiDrawIndexedInstancedIndirect              = LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirect;
            extensionFunctions->D3D11MultiDrawIndexedInstancedIndirectCountIndirect = LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirectCountIndirect;
            extensionFunctions->D3D11MultiDrawInstancedIndirect                     = LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirect;
            extensionFunctions->D3D11MultiDrawInstancedIndirectCountIndirect        = LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirectCountIndirect;
            extensionFunctions->D3D11BeginUAVOverlap                                = LegacyD3D11ExtensionContext::D3D11BeginUAVOverlap;
            extensionFunctions->D3D11EndUAVOverlap                                  = LegacyD3D11ExtensionContext::D3D11EndUAVOverlap;
        }
        else if (extensionFuncsSize == sizeof(D3D11_EXTENSION_FUNCS_0400))
        {
            D3D11_EXTENSION_FUNCS_0400* extensionFunctions = reinterpret_cast<D3D11_EXTENSION_FUNCS_0400*>(*ppfnExtensionFuncs);
            memset(extensionFunctions, 0, sizeof(D3D11_EXTENSION_FUNCS_0400));

            extensionFunctions->D3D11MultiDrawIndexedInstancedIndirect              = LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirect;
            extensionFunctions->D3D11MultiDrawIndexedInstancedIndirectCountIndirect = LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirectCountIndirect;
            extensionFunctions->D3D11MultiDrawInstancedIndirect                     = LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirect;
            extensionFunctions->D3D11MultiDrawInstancedIndirectCountIndirect        = LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirectCountIndirect;
        }
        else
        {
            return E_POINTER; // ppfnExtensionFuncs pointer passed has a wrong size/version
        }

        if (FAILED(result = GetDeviceDriverDescription(m_pAppDevice.Get())))
        {
            return result;
        }

        // Request for additional Intel Device information and return to application
        QueryCapsSupport2(pExtensionInfo->intelDeviceInfo);
        pExtensionInfo->returnedExtensionVersion.FullVersion = m_Legacy_SupportedExtVersion;      // Store final version of returned API
        pExtensionInfo->pDeviceDriverDesc                    = m_DeviceDriverDescription.c_str(); // Get device driver description signature
    }

    return result;
}

BOOL LegacyD3D11ExtensionContext::ValidateMultiDrawIndirectArguments(
    UINT          drawCount,
    ID3D11Buffer* pBufferForArgs,
    UINT          alignedByteOffsetForArgs,
    UINT          byteStrideForArgs) const
{
    if (!pBufferForArgs)
    {
        return FALSE;
    }

    D3D11_BUFFER_DESC bufferForArgsDesc = {};

    pBufferForArgs->GetDesc(&bufferForArgsDesc);

    if (static_cast<UINT64>(drawCount) * static_cast<UINT64>(byteStrideForArgs) + static_cast<UINT64>(alignedByteOffsetForArgs) > static_cast<UINT64>(bufferForArgsDesc.ByteWidth))
    {
        return FALSE;
    }

    return TRUE;
}

BOOL LegacyD3D11ExtensionContext::ValidateMultiDrawIndirectCountIndirectArguments(
    ID3D11Buffer* pBufferForDrawCount,
    UINT          alignedByteOffsetForDrawCount,
    UINT          maxCount,
    ID3D11Buffer* pBufferForArgs,
    UINT          alignedByteOffsetForArgs,
    UINT          byteStrideForArgs) const
{
    if (!pBufferForDrawCount)
    {
        return FALSE;
    }

    if (!pBufferForArgs)
    {
        return FALSE;
    }

    D3D11_BUFFER_DESC bufferForDrawCountDesc = {};

    pBufferForDrawCount->GetDesc(&bufferForDrawCountDesc);

    if (static_cast<UINT64>(alignedByteOffsetForDrawCount) + sizeof(UINT) > static_cast<UINT64>(bufferForDrawCountDesc.ByteWidth))
    {
        return FALSE;
    }

    D3D11_BUFFER_DESC bufferForArgsDesc = {};

    pBufferForArgs->GetDesc(&bufferForArgsDesc);

    if (static_cast<UINT64>(maxCount) * static_cast<UINT64>(byteStrideForArgs) + static_cast<UINT64>(alignedByteOffsetForArgs) > static_cast<UINT64>(bufferForArgsDesc.ByteWidth))
    {
        return FALSE;
    }

    return TRUE;
}

HRESULT APIENTRY LegacyD3D11ExtensionContext::D3D11BeginUAVOverlap(
    ExtensionContextBase* pExtensionContext)
{
    LegacyD3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(pExtensionContext);

    ID3D11VkExtContext* dxVkExtCtx = pD3D11ExtensionContext->GetDxVkExtContext();
    if (dxVkExtCtx == nullptr)
    {
        return E_NOINTERFACE;
    }
    dxVkExtCtx->SetBarrierControl(D3D11_VK_BARRIER_CONTROL_IGNORE_WRITE_AFTER_WRITE);
    return S_OK;
}

HRESULT APIENTRY LegacyD3D11ExtensionContext::D3D11EndUAVOverlap(
    ExtensionContextBase* pExtensionContext)
{
    LegacyD3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(pExtensionContext);

    ID3D11VkExtContext* dxVkExtCtx = pD3D11ExtensionContext->GetDxVkExtContext();
    if (dxVkExtCtx == nullptr)
    {
        return E_NOINTERFACE;
    }
    dxVkExtCtx->SetBarrierControl(0);
    return S_OK;
}

void APIENTRY LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirect(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    UINT                  drawCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    LegacyD3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(pExtensionContext);

    if (drawCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = pD3D11ExtensionContext->m_pDeviceContext.Get();
    }

    if (!pD3D11ExtensionContext->ValidateMultiDrawIndirectArguments(
            drawCount,
            pBufferForArgs,
            alignedByteOffsetForArgs,
            byteStrideForArgs))
    {
        return;
    }

    ScopedDxVkExtContext dxVkExtCtx(pD3D11ExtensionContext->GetDxVkExtContext(), pDeviceContext, pD3D11ExtensionContext->m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndirect(drawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

void APIENTRY LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirect(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    UINT                  drawCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    LegacyD3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(pExtensionContext);

    if (drawCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = pD3D11ExtensionContext->m_pDeviceContext.Get();
    }

    if (!pD3D11ExtensionContext->ValidateMultiDrawIndirectArguments(
            drawCount,
            pBufferForArgs,
            alignedByteOffsetForArgs,
            byteStrideForArgs))
    {
        return;
    }

    ScopedDxVkExtContext dxVkExtCtx(pD3D11ExtensionContext->GetDxVkExtContext(), pDeviceContext, pD3D11ExtensionContext->m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndexedIndirect(drawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

void APIENTRY LegacyD3D11ExtensionContext::D3D11MultiDrawInstancedIndirectCountIndirect(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    ID3D11Buffer*         pBufferForDrawCount,
    UINT                  alignedByteOffsetForDrawCount,
    UINT                  maxCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    LegacyD3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(pExtensionContext);

    if (maxCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = pD3D11ExtensionContext->m_pDeviceContext.Get();
    }

    if (!pD3D11ExtensionContext->ValidateMultiDrawIndirectCountIndirectArguments(
            pBufferForDrawCount,
            alignedByteOffsetForDrawCount,
            maxCount,
            pBufferForArgs,
            alignedByteOffsetForArgs,
            byteStrideForArgs))
    {
        return;
    }

    ScopedDxVkExtContext dxVkExtCtx(pD3D11ExtensionContext->GetDxVkExtContext(), pDeviceContext, pD3D11ExtensionContext->m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndirectCount(maxCount, pBufferForDrawCount, alignedByteOffsetForDrawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}

void APIENTRY LegacyD3D11ExtensionContext::D3D11MultiDrawIndexedInstancedIndirectCountIndirect(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    ID3D11Buffer*         pBufferForDrawCount,
    UINT                  alignedByteOffsetForDrawCount,
    UINT                  maxCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    LegacyD3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(pExtensionContext);

    if (maxCount == 0)
    {
        return;
    }

    // If pDeviceContext passed is nullptr, ImmediateContext is used instead
    if (pDeviceContext == nullptr)
    {
        pDeviceContext = pD3D11ExtensionContext->m_pDeviceContext.Get();
    }

    if (!pD3D11ExtensionContext->ValidateMultiDrawIndirectCountIndirectArguments(
            pBufferForDrawCount,
            alignedByteOffsetForDrawCount,
            maxCount,
            pBufferForArgs,
            alignedByteOffsetForArgs,
            byteStrideForArgs))
    {
        return;
    }

    ScopedDxVkExtContext dxVkExtCtx(pD3D11ExtensionContext->GetDxVkExtContext(), pDeviceContext, pD3D11ExtensionContext->m_pDeviceContext.Get());
    if (!dxVkExtCtx)
    {
        return;
    }
    dxVkExtCtx->MultiDrawIndexedIndirectCount(maxCount, pBufferForDrawCount, alignedByteOffsetForDrawCount, pBufferForArgs, alignedByteOffsetForArgs, byteStrideForArgs);
}
