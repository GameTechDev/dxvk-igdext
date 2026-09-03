/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IgdextApiDllLegacy.cpp

Abstract:   Legacy (non-INTC-prefixed) API DLL exports

Notes:      The legacy counterpart to IgdextApiDll.cpp: dispatches
            D3D11/D3D12{Get,Create,Destroy}* into
            LegacyD3D11/D3D12ExtensionContext.

\*****************************************************************************/
#include "Stdafx.h"

#include "LegacyD3D11ExtensionContext.h"
#include "LegacyD3D12ExtensionContext.h"

#ifdef __cplusplus
extern "C"
{
#endif

HRESULT D3D11GetSupportedVersions(
    ID3D11Device* pDevice,
    UINT32*       supportedExtVersionsCount,
    UINT32*       pSupportedExtVersions)
{
    HRESULT result = S_OK;

    if (!pDevice)
    {
        return E_INVALIDARG;
    }

    // DXVK always reports the same fixed max driver version, so filtering
    // Legacy_SupportedExtVersions against it is a no-op - just expose the table as-is.
    constexpr UINT32 NumSupportedVersions = sizeof(Legacy_SupportedExtVersions) / sizeof(UINT32);

    // Report back number of supported versions
    if (*supportedExtVersionsCount == 0)
    {
        LegacyD3D11ExtensionContext* pExtensionContext      = new LegacyD3D11ExtensionContext(); // Create temporary ExtensionContext
        LegacyExtensionVersion       driverExtensionVersion = {0};

        // Request driver for a maximum supported version on a current platform
        if (FAILED(result = pExtensionContext->GetSupportedVersions(pDevice, &driverExtensionVersion)))
        {
            delete pExtensionContext;

            return result;
        }

        delete pExtensionContext;

        *supportedExtVersionsCount = NumSupportedVersions;

        return S_OK;
    }

    // Return a list of supported extension versions
    if (*supportedExtVersionsCount == NumSupportedVersions && pSupportedExtVersions)
    {
        memcpy(pSupportedExtVersions, Legacy_SupportedExtVersions, sizeof(Legacy_SupportedExtVersions));

        return S_OK;
    }

    // No appropriate size buffer available in pSupportedExtVersions
    return E_OUTOFMEMORY;
}

HRESULT D3D11CreateDeviceExtensionContext(
    ID3D11Device*           pDevice,
    ExtensionContextBase**  ppExtensionContext,
    D3D11ExtensionFuncs**   ppfnExtensionFuncs,
    LegacyExtensionInfo*    pExtensionInfo,
    LegacyExtensionAppInfo* pExtensionAppInfo)
{
    HRESULT result = S_OK;

    if (!pDevice || !ppExtensionContext || !ppfnExtensionFuncs || !pExtensionInfo)
    {
        result = E_INVALIDARG;
    }
    else
    {
        LegacyD3D11ExtensionContext* pExtensionContext = new LegacyD3D11ExtensionContext();

        *ppExtensionContext = pExtensionContext;

        if (!pExtensionContext)
        {
            result = E_OUTOFMEMORY;
        }
        else
        {
            result = pExtensionContext->InitExtensions(pDevice, reinterpret_cast<void**>(ppfnExtensionFuncs), 0, pExtensionInfo, pExtensionAppInfo);
            if (FAILED(result))
            {
                delete pExtensionContext;
                *ppExtensionContext = nullptr;
            }
        }
    }
    return result;
}

HRESULT D3D11CreateDeviceExtensionContext1(
    ID3D11Device*           pDevice,
    ExtensionContextBase**  ppExtensionContext,
    void**                  ppfnExtensionFuncs,
    UINT32                  extensionFuncsSize,
    LegacyExtensionInfo*    pExtensionInfo,
    LegacyExtensionAppInfo* pExtensionAppInfo)
{
    HRESULT result = S_OK;

    if (!pDevice || !ppExtensionContext || !ppfnExtensionFuncs || extensionFuncsSize == 0 || !pExtensionInfo)
    {
        result = E_INVALIDARG;
    }
    else
    {
        LegacyD3D11ExtensionContext* pExtensionContext = new LegacyD3D11ExtensionContext();

        *ppExtensionContext = pExtensionContext;

        if (!pExtensionContext)
        {
            result = E_OUTOFMEMORY;
        }
        else
        {
            result = pExtensionContext->InitExtensions(pDevice, ppfnExtensionFuncs, extensionFuncsSize, pExtensionInfo, pExtensionAppInfo);
            if (FAILED(result))
            {
                delete pExtensionContext;
                *ppExtensionContext = nullptr;
            }
        }
    }
    return result;
}

HRESULT D3D11DestroyDeviceExtensionContext(
    ExtensionContextBase** ppExtensionContext)
{
    HRESULT result = S_OK;

    if (!ppExtensionContext)
    {
        result = E_INVALIDARG;
    }
    else
    {
        LegacyD3D11ExtensionContext* pExtensionContext = reinterpret_cast<LegacyD3D11ExtensionContext*>(*ppExtensionContext);

        delete pExtensionContext;
        *ppExtensionContext = nullptr;
    }

    return result;
}

// D3D12 extensions are not implemented in this repo - always report zero supported versions
// and never successfully create a context.
HRESULT D3D12GetSupportedVersions(
    ID3D12Device* pDevice,
    UINT32*       supportedExtVersionsCount,
    UINT32*       pSupportedExtVersions)
{
    HRESULT result = S_OK;

    if (!pDevice || !supportedExtVersionsCount)
    {
        return E_INVALIDARG;
    }

    LegacyD3D12ExtensionContext* pExtensionContext      = new LegacyD3D12ExtensionContext(); // Create temporary ExtensionContext
    LegacyExtensionVersion       driverExtensionVersion = {0};

    // Request driver for a maximum supported version on a current platform - the context
    // always reports the "no version supported" sentinel, so zero versions are ever returned.
    result = pExtensionContext->GetSupportedVersions(pDevice, &driverExtensionVersion);

    delete pExtensionContext;

    if (FAILED(result))
    {
        return result;
    }

    *supportedExtVersionsCount = 0;

    return S_OK;
}

HRESULT D3D12CreateDeviceExtensionContext(
    ID3D12Device*           pDevice,
    ExtensionContextBase**  ppExtensionContext,
    void**                  ppfnExtensionFuncs,
    UINT32                  extensionFuncsSize,
    LegacyExtensionInfo*    pExtensionInfo,
    LegacyExtensionAppInfo* pExtensionAppInfo)
{
    HRESULT result = S_OK;

    if (!pDevice || !ppExtensionContext || !ppfnExtensionFuncs || extensionFuncsSize == 0 || !pExtensionInfo)
    {
        result = E_INVALIDARG;
    }
    else
    {
        LegacyD3D12ExtensionContext* pExtensionContext = new LegacyD3D12ExtensionContext();

        *ppExtensionContext = pExtensionContext;

        if (!pExtensionContext)
        {
            result = E_OUTOFMEMORY;
        }
        else
        {
            result = pExtensionContext->InitExtensions(pDevice, ppfnExtensionFuncs, extensionFuncsSize, pExtensionInfo, pExtensionAppInfo);
            if (FAILED(result))
            {
                delete pExtensionContext;
                *ppExtensionContext = nullptr;
            }
        }
    }
    return result;
}

HRESULT D3D12DestroyDeviceExtensionContext(
    ExtensionContextBase** ppExtensionContext)
{
    HRESULT result = S_OK;

    if (!ppExtensionContext)
    {
        result = E_INVALIDARG;
    }
    else
    {
        LegacyD3D12ExtensionContext* pExtensionContext = reinterpret_cast<LegacyD3D12ExtensionContext*>(*ppExtensionContext);

        delete pExtensionContext;
        *ppExtensionContext = nullptr;
    }

    return result;
}

#ifdef __cplusplus
} // extern "C"
#endif
