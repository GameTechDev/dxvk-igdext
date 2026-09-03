/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  ExtensionContextBase.cpp

Abstract:   Shared device/global extension context state

Notes:      Implements ExtensionContextBase's driver-description lookup and
            DXVK detection via QueryInterface for ID3D11VkExtContext.

\*****************************************************************************/

#include "Stdafx.h"

#include "DxvkInterfaces.h"

// Helper functions

// Retrieve the driver description of the adapter actually backing pDevice, and cache it
HRESULT ExtensionContextBase::GetDeviceDriverDescription(ID3D11Device* pDevice)
{
    ComPtr<IDXGIDevice>  pDxgiDevice;
    ComPtr<IDXGIAdapter> pAdapter;
    DXGI_ADAPTER_DESC    adapterDesc = {0};
    HRESULT              result;

    if (FAILED(result = pDevice->QueryInterface(IID_PPV_ARGS(&pDxgiDevice))))
    {
        return result;
    }

    if (FAILED(result = pDxgiDevice->GetAdapter(&pAdapter)))
    {
        return result;
    }

    if (FAILED(result = pAdapter->GetDesc(&adapterDesc)))
    {
        return result;
    }

    m_DeviceDriverDescription = adapterDesc.Description;

    return S_OK;
};

void ExtensionContextBase::GetGTGenerationName(INTCDeviceInfo* pIntelDeviceInfo)
{
    constexpr std::wstring_view DxvkDevice = c_GTGenerationName;
    auto&                       nameBuffer = pIntelDeviceInfo->GTGenerationName;

    std::fill(std::begin(nameBuffer), std::end(nameBuffer), L'\0');
    std::copy_n(DxvkDevice.begin(), std::min(DxvkDevice.size(), std::size(nameBuffer) - 1), nameBuffer);
}

bool ExtensionContextBase::DetectDxvk(ID3D11DeviceContext* pDeviceContext)
{
    if (m_pDxVkExtCtx)
    {
        return true;
    }

    m_pDxVkExtCtx.Attach(GetDxVkExtContextForDeviceContext(pDeviceContext));

    return m_pDxVkExtCtx != nullptr;
}

ID3D11VkExtContext* ExtensionContextBase::GetDxVkExtContextForDeviceContext(ID3D11DeviceContext* pDeviceContext)
{
    if (pDeviceContext == nullptr)
    {
        return nullptr;
    }

    ID3D11VkExtContext* pVkExtCtx = nullptr;
    pDeviceContext->QueryInterface(
        __uuidof(ID3D11VkExtContext),
        reinterpret_cast<void**>(&pVkExtCtx));
    return pVkExtCtx;
}
