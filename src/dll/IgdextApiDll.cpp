/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IgdextApiDll.cpp

Abstract:   Graphics Extensions SDK API Source File

Notes:      Home of the modern (non-legacy) D3D11/D3D12 API DLL exports.

\*****************************************************************************/

#include "Stdafx.h"

#ifdef __cplusplus
extern "C"
{
#endif

HRESULT _INTC_D3D11_GetSupportedVersions(
    const ID3D11Device*   pDevice,
    INTCExtensionVersion* pSupportedExtVersions,
    uint32_t*             pSupportedExtVersionsCount)
{
    HRESULT result = S_OK;

    if (!pDevice)
    {
        return E_INVALIDARG;
    }

    // The historical SupportedExtVersions[] table entries are all backward-compatible with
    // the fixed driver version queried below, so it's served directly - fixed, compile-time
    // data, safe to read concurrently across devices/calls with no shared mutable state.
    constexpr UINT32 NumSupportedVersions = sizeof(SupportedExtVersions) / sizeof(INTCExtensionVersion);

    // Report back number of supported versions
    if (pSupportedExtVersions == nullptr && pSupportedExtVersionsCount != nullptr)
    {
        D3D11ExtensionContext*     pExtensionContext      = new D3D11ExtensionContext(); // Create temporary INTCExtensionContext
        INTCExtensionVersionHelper driverExtensionVersion = {0};

        // Request driver for a maximum supported version on a current platform
        if (FAILED(result = pExtensionContext->GetSupportedVersions(pDevice, &driverExtensionVersion)))
        {
            delete pExtensionContext;

            return result;
        }

        delete pExtensionContext;

        *pSupportedExtVersionsCount = NumSupportedVersions;

        return S_OK;
    }
    // Return a list of supported extension versions
    else if (pSupportedExtVersions != nullptr && *pSupportedExtVersionsCount == NumSupportedVersions)
    {
        // Return all supported interface versions
        for (UINT32 i = 0; i < NumSupportedVersions; i++)
        {
            pSupportedExtVersions[i] = SupportedExtVersions[i];
        }

        return S_OK;
    }

    // No appropriate size buffer available in pSupportedExtVersions
    return E_OUTOFMEMORY;
}

HRESULT D3D11GetSupportedVersions2(
    const ID3D11Device*   pDevice,
    INTCExtensionVersion* pSupportedExtVersions,
    uint32_t*             pSupportedExtVersionsCount)
{
    HRESULT result = S_OK;

    if (!pDevice)
    {
        return E_INVALIDARG;
    }

    // The historical SupportedExtVersions[] table entries are all backward-compatible with
    // the fixed driver version queried below, so it's served directly - fixed, compile-time
    // data, safe to read concurrently across devices/calls with no shared mutable state.
    constexpr UINT32 NumSupportedVersions = sizeof(SupportedExtVersions) / sizeof(INTCExtensionVersion);

    // Report back number of supported versions
    if (pSupportedExtVersions == nullptr && pSupportedExtVersionsCount != nullptr)
    {
        D3D11ExtensionContext*     pExtensionContext      = new D3D11ExtensionContext(); // Create temporary INTCExtensionContext
        INTCExtensionVersionHelper driverExtensionVersion = {0};

        // Request driver for a maximum supported version on a current platform
        if (FAILED(result = pExtensionContext->GetSupportedVersions(pDevice, &driverExtensionVersion)))
        {
            delete pExtensionContext;

            return result;
        }

        delete pExtensionContext;

        *pSupportedExtVersionsCount = NumSupportedVersions;

        return S_OK;
    }
    // Return a list of supported extension versions
    else if (pSupportedExtVersions != nullptr && *pSupportedExtVersionsCount == NumSupportedVersions)
    {
        // Return all supported interface versions
        for (UINT32 i = 0; i < NumSupportedVersions; i++)
        {
            pSupportedExtVersions[i] = SupportedExtVersions[i];
        }

        return S_OK;
    }

    // No appropriate size buffer available in pSupportedExtVersions
    return E_OUTOFMEMORY;
}

HRESULT _INTC_D3D12_GetSupportedVersions(
    const ID3D12Device*   pDevice,
    INTCExtensionVersion* pSupportedExtVersions,
    uint32_t*             pSupportedExtVersionsCount)
{
    HRESULT result = S_OK;

    if (!pDevice || !pSupportedExtVersionsCount)
    {
        return E_INVALIDARG;
    }

    D3D12ExtensionContext*     pExtensionContext      = new D3D12ExtensionContext(); // Create temporary INTCExtensionContext
    INTCExtensionVersionHelper driverExtensionVersion = {0};

    // D3D12 extensions are not implemented in this repo - the context always reports
    // the "no version supported" sentinel, so zero supported versions are ever returned.
    result = pExtensionContext->GetSupportedVersions(pDevice, &driverExtensionVersion);

    delete pExtensionContext;

    if (FAILED(result))
    {
        return result;
    }

    *pSupportedExtVersionsCount = 0;

    return S_OK;
}

HRESULT D3D12GetSupportedVersions2(
    const ID3D12Device*   pDevice,
    INTCExtensionVersion* pSupportedExtVersions,
    uint32_t*             pSupportedExtVersionsCount)
{
    if (!pDevice || !pSupportedExtVersionsCount)
    {
        return E_INVALIDARG;
    }

    // D3D12 extensions are not implemented in this repo - always report zero
    // supported versions, matching _INTC_D3D12_GetSupportedVersions above.
    *pSupportedExtVersionsCount = 0;

    return S_OK;
}

HRESULT _INTC_D3D11_CreateDeviceExtensionContext(
    const ID3D11Device*    pDevice,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo*  pExtensionAppInfo)
{
    // Initialize latest version...
    INTCExtensionAppInfo1 extensionAppInfo;
    memset(&extensionAppInfo, 0, sizeof(INTCExtensionAppInfo1));

    if (pExtensionAppInfo)
    {
        extensionAppInfo.pApplicationName   = pExtensionAppInfo->pApplicationName;
        extensionAppInfo.ApplicationVersion = {0};
        extensionAppInfo.pEngineName        = pExtensionAppInfo->pEngineName;
        extensionAppInfo.EngineVersion      = {0};
    }

    HRESULT result = S_OK;

    // Validate input arguments
    if (!pDevice ||            // D3D11 Device must be present
        !ppExtensionContext || // Address that receives Extension Context pointer
        !pExtensionInfo)       // ExtensionInfo structure must be present
    {
        return E_INVALIDARG;
    }

    INTCExtensionContext* pExtensionContext     = *ppExtensionContext;
    const bool            contextWasPreexisting = pExtensionContext != nullptr;

    // Create Graphics Extensions context object INTCExtensionContext if it does not exist
    // It might exist if it was previously created and/or if it was created on a different API device
    if (pExtensionContext == nullptr)
    {
        pExtensionContext = new INTCExtensionContext();
        if (pExtensionContext == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    // Create D3D11 Graphics Extensions Context object, a part of the common INTCExtensionContext
    if (pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        pExtensionContext->m_pD3D11ExtensionContext = new D3D11ExtensionContext();
        if (pExtensionContext->m_pD3D11ExtensionContext == nullptr)
        {
            // Only torn down if we allocated it in this call - a caller-owned, preexisting
            // context (e.g. one already carrying a live D3D12 half) must survive
            if (!contextWasPreexisting)
            {
                delete pExtensionContext;
            }

            return E_OUTOFMEMORY;
        }
    }
    else
    {
        // We already seem to have a context
        return S_OK;
    }

    // Create all the necessary infrastructure and initialize D3D11 Graphics Extensions Context
    if (FAILED(result = pExtensionContext->m_pD3D11ExtensionContext->InitExtensions(pDevice, nullptr, 0, pExtensionInfo, &extensionAppInfo)))
    {
        // Only tear down what this call allocated: the D3D11 half always, and the whole
        // context only if we created it here (a caller-owned preexisting context, e.g. one
        // already carrying a live D3D12 half, must survive a failed D3D11 half)
        delete pExtensionContext->m_pD3D11ExtensionContext;
        pExtensionContext->m_pD3D11ExtensionContext = nullptr;

        if (!contextWasPreexisting)
        {
            delete pExtensionContext;
            *ppExtensionContext = nullptr;
        }

        return result;
    }

    // Return pointer to the newly created Graphics Extensions Context
    *ppExtensionContext = pExtensionContext;

    return result;
}

HRESULT _INTC_D3D11_CreateDeviceExtensionContext1(
    const ID3D11Device*    pDevice,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo1* pExtensionAppInfo)
{
    HRESULT result = S_OK;

    // Validate input arguments
    if (!pDevice ||            // D3D11 Device must be present
        !ppExtensionContext || // Address that receives Extension Context pointer
        !pExtensionInfo)       // ExtensionInfo structure must be present
    {
        return E_INVALIDARG;
    }

    INTCExtensionContext* pExtensionContext     = *ppExtensionContext;
    const bool            contextWasPreexisting = pExtensionContext != nullptr;

    // Create Graphics Extensions context object INTCExtensionContext if it does not exist
    // It might exist if it was previously created and/or if it was created on a different API device
    if (pExtensionContext == nullptr)
    {
        pExtensionContext = new INTCExtensionContext();
        if (pExtensionContext == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    // Create D3D11 Graphics Extensions Context object, a part of the common INTCExtensionContext
    if (pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        pExtensionContext->m_pD3D11ExtensionContext = new D3D11ExtensionContext();
        if (pExtensionContext->m_pD3D11ExtensionContext == nullptr)
        {
            // Only torn down if we allocated it in this call - a caller-owned, preexisting
            // context (e.g. one already carrying a live D3D12 half) must survive
            if (!contextWasPreexisting)
            {
                delete pExtensionContext;
            }

            return E_OUTOFMEMORY;
        }
    }
    else
    {
        // We already seem to have a context
        return S_OK;
    }

    // Create all the necessary infrastructure and initialize D3D11 Graphics Extensions Context
    if (FAILED(result = pExtensionContext->m_pD3D11ExtensionContext->InitExtensions(pDevice, nullptr, 0, pExtensionInfo, pExtensionAppInfo)))
    {
        // Only tear down what this call allocated: the D3D11 half always, and the whole
        // context only if we created it here (a caller-owned preexisting context, e.g. one
        // already carrying a live D3D12 half, must survive a failed D3D11 half)
        delete pExtensionContext->m_pD3D11ExtensionContext;
        pExtensionContext->m_pD3D11ExtensionContext = nullptr;

        if (!contextWasPreexisting)
        {
            delete pExtensionContext;
            *ppExtensionContext = nullptr;
        }

        return result;
    }

    // Return pointer to the newly created Graphics Extensions Context
    *ppExtensionContext = pExtensionContext;

    return result;
}

HRESULT _INTC_D3D11_RegisterApplicationCallbacks(
    const INTC_D3D11_API_CALLBACKS* pCallbacks)
{
    return E_NOTIMPL;
}

HRESULT _INTC_D3D12_CreateDeviceExtensionContext(
    const ID3D12Device*    pDevice,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo*  pExtensionAppInfo)
{
    if (!pDevice || !ppExtensionContext || !pExtensionInfo)
    {
        return E_INVALIDARG;
    }

    // D3D12ExtensionContext::InitExtensions expects the extended INTCExtensionInfo1 layout;
    // only RequestedExtensionVersion is meaningful since the stub never writes device info back.
    INTCExtensionInfo1 extensionInfo1;
    memset(&extensionInfo1, 0, sizeof(INTCExtensionInfo1));
    extensionInfo1.RequestedExtensionVersion = pExtensionInfo->RequestedExtensionVersion;

    INTCExtensionAppInfo1 extensionAppInfo;
    memset(&extensionAppInfo, 0, sizeof(INTCExtensionAppInfo1));

    if (pExtensionAppInfo)
    {
        extensionAppInfo.pApplicationName = pExtensionAppInfo->pApplicationName;
        extensionAppInfo.pEngineName      = pExtensionAppInfo->pEngineName;
    }

    HRESULT result = S_OK;

    INTCExtensionContext* pExtensionContext     = *ppExtensionContext;
    const bool            contextWasPreexisting = pExtensionContext != nullptr;

    if (pExtensionContext == nullptr)
    {
        pExtensionContext = new INTCExtensionContext();
        if (pExtensionContext == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    if (pExtensionContext->m_pD3D12ExtensionContext == nullptr)
    {
        pExtensionContext->m_pD3D12ExtensionContext = new D3D12ExtensionContext();
        if (pExtensionContext->m_pD3D12ExtensionContext == nullptr)
        {
            // Only torn down if we allocated it in this call - a caller-owned, preexisting
            // context (e.g. one already carrying a live D3D11 half) must survive
            if (!contextWasPreexisting)
            {
                delete pExtensionContext;
            }

            return E_OUTOFMEMORY;
        }
    }
    else
    {
        // We already seem to have a context
        return S_OK;
    }

    if (FAILED(result = pExtensionContext->m_pD3D12ExtensionContext->InitExtensions(pDevice, nullptr, 0, &extensionInfo1, &extensionAppInfo)))
    {
        // Only tear down what this call allocated: the D3D12 half always, and the whole
        // context only if we created it here (a caller-owned preexisting context, e.g. one
        // already carrying a live D3D11 half, must survive a failed D3D12 half)
        delete pExtensionContext->m_pD3D12ExtensionContext;
        pExtensionContext->m_pD3D12ExtensionContext = nullptr;

        if (!contextWasPreexisting)
        {
            delete pExtensionContext;
            *ppExtensionContext = nullptr;
        }

        return result;
    }

    *ppExtensionContext = pExtensionContext;

    return result;
}

HRESULT _INTC_D3D12_CreateDeviceExtensionContext1(
    const ID3D12Device*    pDevice,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo1* pExtensionAppInfo)
{
    if (!pDevice || !ppExtensionContext || !pExtensionInfo)
    {
        return E_INVALIDARG;
    }

    // D3D12ExtensionContext::InitExtensions expects the extended INTCExtensionInfo1 layout;
    // only RequestedExtensionVersion is meaningful since the stub never writes device info back.
    INTCExtensionInfo1 extensionInfo1;
    memset(&extensionInfo1, 0, sizeof(INTCExtensionInfo1));
    extensionInfo1.RequestedExtensionVersion = pExtensionInfo->RequestedExtensionVersion;

    HRESULT result = S_OK;

    INTCExtensionContext* pExtensionContext     = *ppExtensionContext;
    const bool            contextWasPreexisting = pExtensionContext != nullptr;

    if (pExtensionContext == nullptr)
    {
        pExtensionContext = new INTCExtensionContext();
        if (pExtensionContext == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    if (pExtensionContext->m_pD3D12ExtensionContext == nullptr)
    {
        pExtensionContext->m_pD3D12ExtensionContext = new D3D12ExtensionContext();
        if (pExtensionContext->m_pD3D12ExtensionContext == nullptr)
        {
            // Only torn down if we allocated it in this call - a caller-owned, preexisting
            // context (e.g. one already carrying a live D3D11 half) must survive
            if (!contextWasPreexisting)
            {
                delete pExtensionContext;
            }

            return E_OUTOFMEMORY;
        }
    }
    else
    {
        // We already seem to have a context
        return S_OK;
    }

    if (FAILED(result = pExtensionContext->m_pD3D12ExtensionContext->InitExtensions(pDevice, nullptr, 0, &extensionInfo1, pExtensionAppInfo)))
    {
        // Only tear down what this call allocated: the D3D12 half always, and the whole
        // context only if we created it here (a caller-owned preexisting context, e.g. one
        // already carrying a live D3D11 half, must survive a failed D3D12 half)
        delete pExtensionContext->m_pD3D12ExtensionContext;
        pExtensionContext->m_pD3D12ExtensionContext = nullptr;

        if (!contextWasPreexisting)
        {
            delete pExtensionContext;
            *ppExtensionContext = nullptr;
        }

        return result;
    }

    *ppExtensionContext = pExtensionContext;

    return result;
}

HRESULT _INTC_D3D12_CreateDeviceExtensionContext2(
    const ID3D12Device*    pDevice,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo1*    pExtensionInfo,
    INTCExtensionAppInfo1* pExtensionAppInfo)
{
    if (!pDevice || !ppExtensionContext || !pExtensionInfo)
    {
        return E_INVALIDARG;
    }

    HRESULT result = S_OK;

    INTCExtensionContext* pExtensionContext     = *ppExtensionContext;
    const bool            contextWasPreexisting = pExtensionContext != nullptr;

    if (pExtensionContext == nullptr)
    {
        pExtensionContext = new INTCExtensionContext();
        if (pExtensionContext == nullptr)
        {
            return E_OUTOFMEMORY;
        }
    }

    if (pExtensionContext->m_pD3D12ExtensionContext == nullptr)
    {
        pExtensionContext->m_pD3D12ExtensionContext = new D3D12ExtensionContext();
        if (pExtensionContext->m_pD3D12ExtensionContext == nullptr)
        {
            // Only torn down if we allocated it in this call - a caller-owned, preexisting
            // context (e.g. one already carrying a live D3D11 half) must survive
            if (!contextWasPreexisting)
            {
                delete pExtensionContext;
            }

            return E_OUTOFMEMORY;
        }
    }
    else
    {
        // We already seem to have a context
        return S_OK;
    }

    if (FAILED(result = pExtensionContext->m_pD3D12ExtensionContext->InitExtensions(pDevice, nullptr, 0, pExtensionInfo, pExtensionAppInfo)))
    {
        // Only tear down what this call allocated: the D3D12 half always, and the whole
        // context only if we created it here (a caller-owned preexisting context, e.g. one
        // already carrying a live D3D11 half, must survive a failed D3D12 half)
        delete pExtensionContext->m_pD3D12ExtensionContext;
        pExtensionContext->m_pD3D12ExtensionContext = nullptr;

        if (!contextWasPreexisting)
        {
            delete pExtensionContext;
            *ppExtensionContext = nullptr;
        }

        return result;
    }

    *ppExtensionContext = pExtensionContext;

    return result;
}

HRESULT _INTC_D3D12_RegisterApplicationCallbacks(
    const INTC_D3D12_API_CALLBACKS* pCallbacks)
{
    return E_NOTIMPL;
}

HRESULT _INTC_CreateDeviceExtensionContext(
    const ID3D11Device*    pD3D11Device,
    const ID3D12Device*    pD3D12Device,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo*  pExtensionAppInfo)
{
    HRESULT hr        = E_FAIL;
    void*   pExtFuncs = nullptr;
    if (pD3D11Device)
    {
        if (FAILED(hr = _INTC_D3D11_CreateDeviceExtensionContext(pD3D11Device, ppExtensionContext, pExtensionInfo, pExtensionAppInfo)))
        {
            return hr;
        }
    }
    if (pD3D12Device)
    {
        if (FAILED(hr = _INTC_D3D12_CreateDeviceExtensionContext(pD3D12Device, ppExtensionContext, pExtensionInfo, pExtensionAppInfo)))
        {
            return hr;
        }
    }
    return S_OK;
}

HRESULT _INTC_CreateDeviceExtensionContext1(
    ID3D11Device*          pD3D11Device,
    ID3D12Device*          pD3D12Device,
    INTCExtensionContext** ppExtensionContext,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo1* pExtensionAppInfo)
{
    HRESULT hr = E_FAIL;

    if (pD3D11Device)
    {
        if (FAILED(hr = _INTC_D3D11_CreateDeviceExtensionContext1(pD3D11Device, ppExtensionContext, pExtensionInfo, pExtensionAppInfo)))
        {
            return hr;
        }
    }

    if (pD3D12Device)
    {
        if (FAILED(hr = _INTC_D3D12_CreateDeviceExtensionContext1(pD3D12Device, ppExtensionContext, pExtensionInfo, pExtensionAppInfo)))
        {
            return hr;
        }
    }

    return S_OK;
}

HRESULT _INTC_DestroyDeviceExtensionContext(
    INTCExtensionContext** ppExtensionContext)
{
    HRESULT result = S_OK;

    if (!ppExtensionContext)
    {
        return E_INVALIDARG;
    }
    else if (!(*ppExtensionContext))
    {
        // Nothing to delete
        return S_OK;
    }

    // Delete D3D11 Internal Context
    if ((*ppExtensionContext)->m_pD3D11ExtensionContext)
    {
        D3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<D3D11ExtensionContext*>((*ppExtensionContext)->m_pD3D11ExtensionContext);

        delete pD3D11ExtensionContext;
        pD3D11ExtensionContext = nullptr;
    }

    // Delete D3D12 Internal Context
    if ((*ppExtensionContext)->m_pD3D12ExtensionContext)
    {
        D3D12ExtensionContext* pD3D12ExtensionContext = reinterpret_cast<D3D12ExtensionContext*>((*ppExtensionContext)->m_pD3D12ExtensionContext);

        delete pD3D12ExtensionContext;
        pD3D12ExtensionContext = nullptr;
    }

    // Delete whole Extension Context object
    delete *ppExtensionContext;
    *ppExtensionContext = nullptr;

    return S_OK;
}

HRESULT D3D11D3D12DestroyDeviceExtensionContext2(
    INTCExtensionContext** ppExtensionContext)
{
    HRESULT result = S_OK;

    if (!ppExtensionContext)
    {
        return E_INVALIDARG;
    }
    else if (!(*ppExtensionContext))
    {
        // Nothing to delete
        return S_OK;
    }

    // Delete D3D11 Internal Context
    if ((*ppExtensionContext)->m_pD3D11ExtensionContext)
    {
        D3D11ExtensionContext* pD3D11ExtensionContext = reinterpret_cast<D3D11ExtensionContext*>((*ppExtensionContext)->m_pD3D11ExtensionContext);

        delete pD3D11ExtensionContext;
        pD3D11ExtensionContext = nullptr;
    }

    // Delete D3D12 Internal Context
    if ((*ppExtensionContext)->m_pD3D12ExtensionContext)
    {
        D3D12ExtensionContext* pD3D12ExtensionContext = reinterpret_cast<D3D12ExtensionContext*>((*ppExtensionContext)->m_pD3D12ExtensionContext);

        delete pD3D12ExtensionContext;
        pD3D12ExtensionContext = nullptr;
    }

    // Delete whole Extension Context object
    delete *ppExtensionContext;
    *ppExtensionContext = nullptr;

    return S_OK;
}

static INTCExtensionAppInfo1 ConvertAppInfoToAppInfo1(const INTCExtensionAppInfo* pExtensionAppInfo)
{
    INTCExtensionAppInfo1 appInfo1;
    memset(&appInfo1, 0, sizeof(appInfo1));

    if (pExtensionAppInfo)
    {
        appInfo1.pApplicationName         = pExtensionAppInfo->pApplicationName;
        appInfo1.ApplicationVersion.major = pExtensionAppInfo->ApplicationVersion;
        appInfo1.pEngineName              = pExtensionAppInfo->pEngineName;
        appInfo1.EngineVersion.major      = pExtensionAppInfo->EngineVersion;
    }

    return appInfo1;
}

HRESULT D3D11CreateDeviceExtensionContext2(
    ID3D11Device*          pDevice,
    INTCExtensionContext** ppExtensionContext,
    void**                 ppfnExtensionFuncs,
    UINT32                 extensionFuncsSize,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo*  pExtensionAppInfo)
{
    INTCExtensionAppInfo1 appInfo1 = ConvertAppInfoToAppInfo1(pExtensionAppInfo);

    return _INTC_D3D11_CreateDeviceExtensionContext1(pDevice, ppExtensionContext, pExtensionInfo, &appInfo1);
}

HRESULT D3D12CreateDeviceExtensionContext2(
    ID3D12Device*          pDevice,
    INTCExtensionContext** ppExtensionContext,
    void**                 ppfnExtensionFuncs,
    UINT32                 extensionFuncsSize,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo*  pExtensionAppInfo)
{
    INTCExtensionAppInfo1 appInfo1 = ConvertAppInfoToAppInfo1(pExtensionAppInfo);

    return _INTC_D3D12_CreateDeviceExtensionContext1(pDevice, ppExtensionContext, pExtensionInfo, &appInfo1);
}

HRESULT D3D11D3D12CreateDeviceExtensionContext2(
    ID3D11Device*          pD3D11Device,
    ID3D12Device*          pD3D12Device,
    INTCExtensionContext** ppExtensionContext,
    void**                 ppfnD3D11ExtensionFuncs,
    void**                 ppfnD3D12ExtensionFuncs,
    UINT32                 D3D11ExtensionFuncsSize,
    UINT32                 D3D12ExtensionFuncsSize,
    INTCExtensionInfo*     pExtensionInfo,
    INTCExtensionAppInfo*  pExtensionAppInfo)
{
    HRESULT hr = E_FAIL;

    if (pD3D11Device)
    {
        if (FAILED(hr = D3D11CreateDeviceExtensionContext2(pD3D11Device, ppExtensionContext, ppfnD3D11ExtensionFuncs, D3D11ExtensionFuncsSize, pExtensionInfo, pExtensionAppInfo)))
        {
            return hr;
        }
    }

    if (pD3D12Device)
    {
        if (FAILED(hr = D3D12CreateDeviceExtensionContext2(pD3D12Device, ppExtensionContext, ppfnD3D12ExtensionFuncs, D3D12ExtensionFuncsSize, pExtensionInfo, pExtensionAppInfo)))
        {
            return hr;
        }
    }

    return S_OK;
}

HRESULT _INTC_D3D11_BeginUAVOverlap(
    INTCExtensionContext* pExtensionContext)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return E_NOINTERFACE;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->BeginUAVOverlap();
}

HRESULT _INTC_D3D11_EndUAVOverlap(
    INTCExtensionContext* pExtensionContext)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return E_NOINTERFACE;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->EndUAVOverlap();
}

void _INTC_D3D11_MultiDrawInstancedIndirect(
    INTCExtensionContext* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    UINT                  drawCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->MultiDrawInstancedIndirect(
        pDeviceContext,
        drawCount,
        pBufferForArgs,
        alignedByteOffsetForArgs,
        byteStrideForArgs);
}

void _INTC_D3D11_MultiDrawIndexedInstancedIndirect(
    INTCExtensionContext* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    UINT                  drawCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->MultiDrawIndexedInstancedIndirect(
        pDeviceContext,
        drawCount,
        pBufferForArgs,
        alignedByteOffsetForArgs,
        byteStrideForArgs);
}

void _INTC_D3D11_MultiDrawInstancedIndirectCountIndirect(
    INTCExtensionContext* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    ID3D11Buffer*         pBufferForDrawCount,
    UINT                  alignedByteOffsetForDrawCount,
    UINT                  maxCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->MultiDrawInstancedIndirectCountIndirect(
        pDeviceContext,
        pBufferForDrawCount,
        alignedByteOffsetForDrawCount,
        maxCount,
        pBufferForArgs,
        alignedByteOffsetForArgs,
        byteStrideForArgs);
}

void _INTC_D3D11_MultiDrawIndexedInstancedIndirectCountIndirect(
    INTCExtensionContext* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    ID3D11Buffer*         pBufferForDrawCount,
    UINT                  alignedByteOffsetForDrawCount,
    UINT                  maxCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->MultiDrawIndexedInstancedIndirectCountIndirect(
        pDeviceContext,
        pBufferForDrawCount,
        alignedByteOffsetForDrawCount,
        maxCount,
        pBufferForArgs,
        alignedByteOffsetForArgs,
        byteStrideForArgs);
}

void _INTC_D3D11_SetDepthBounds(
    INTCExtensionContext* pExtensionContext,
    BOOL                  bEnable,
    FLOAT                 Min,
    FLOAT                 Max)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->SetDepthBounds(
        bEnable,
        Min,
        Max);
}

HRESULT _INTC_D3D11_CreateTexture2D(
    INTCExtensionContext*            pExtensionContext,
    const INTC_D3D11_TEXTURE2D_DESC* pDesc,
    const D3D11_SUBRESOURCE_DATA*    pInitialData,
    ID3D11Texture2D**                ppTexture2D)
{
    // Validate Graphics Extensions context
    if (pExtensionContext == nullptr || pExtensionContext->m_pD3D11ExtensionContext == nullptr)
    {
        return E_NOINTERFACE;
    }

    return pExtensionContext->m_pD3D11ExtensionContext->CreateTexture2D(
        pDesc,
        pInitialData,
        ppTexture2D);
}

#ifdef __cplusplus
}
#endif
