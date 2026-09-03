/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  ExtensionContextBase.h

Abstract:   Shared device/global extension context state

Notes:      Defines ExtensionContextBase (shared per-device state and DXVK
            detection), INTCExtensionVersionHelper (version comparison), and
            ScopedDxVkExtContext (RAII helper for resolving
            ID3D11VkExtContext).

\*****************************************************************************/
#pragma once

#include "Stdafx.h"

#include "DxvkInterfaces.h"

using Microsoft::WRL::ComPtr;

struct INTCExtensionVersionHelper : public INTCExtensionVersion
{
    INTCExtensionVersionHelper() = default;
    INTCExtensionVersionHelper(uint32_t value)
    {
        HWFeatureLevel = value;
        APIVersion     = value;
        Revision       = value;
    }
    INTCExtensionVersionHelper(const INTCExtensionVersion& right)
    {
        HWFeatureLevel = right.HWFeatureLevel;
        APIVersion     = right.APIVersion;
        Revision       = right.Revision;
    }

    // Checks if other INTCExtensionVersion is supported (compatible) with this version
    bool IsSupported(const INTCExtensionVersionHelper& other)
    {
        return (this->HWFeatureLevel > other.HWFeatureLevel) || (this->HWFeatureLevel == other.HWFeatureLevel && this->APIVersion > other.APIVersion) || (this->HWFeatureLevel == other.HWFeatureLevel && this->APIVersion == other.APIVersion && this->Revision >= other.Revision);
    }
};

// INTCExtensionContext per Device to store state and centralize extension handling
struct ExtensionContextBase
{
    // Synthesized IntelDeviceInfo/driver-info values, shared by modern and legacy contexts.
    // DXVK/Vulkan has no way to query real EU count, clocks, or a driver build number, so
    // these are placeholders - just enough that games gating on a minimum driver build still
    // enable extensions, and app-side heuristics dividing by EUCount don't divide by zero.
    static constexpr const wchar_t* c_GTGenerationName        = L"DXVK-IGDEXT";
    static constexpr const wchar_t* c_DeviceDriverVersion     = L"DXVK-IGDEXT-1.0";
    static constexpr uint32_t       c_DeviceDriverBuildNumber = 8974;
    static constexpr uint32_t       c_GPUMaxFreq              = 2300;
    static constexpr uint32_t       c_GPUMinFreq              = 300;
    static constexpr uint32_t       c_GTGeneration            = 20;
    static constexpr uint32_t       c_EUCount                 = 96;
    static constexpr uint32_t       c_PackageTDP              = 75;
    static constexpr uint32_t       c_MaxFillRate             = 32;

    std::wstring               m_DeviceDriverDescription; // Device Driver description string
    ComPtr<ID3D11VkExtContext> m_pDxVkExtCtx;             // Used for detection of DXVK runtime

    // Member Functions

    ExtensionContextBase() = default;

    ExtensionContextBase(const ExtensionContextBase&)            = delete;
    ExtensionContextBase& operator=(const ExtensionContextBase&) = delete;

    virtual ~ExtensionContextBase() = default;

    ID3D11VkExtContext* GetDxVkExtContext() { return m_pDxVkExtCtx.Get(); }
    bool                DetectDxvk(ID3D11DeviceContext* pDeviceContext);

    // Helper functions

    HRESULT                    GetDeviceDriverDescription(ID3D11Device* pDevice);
    static ID3D11VkExtContext* GetDxVkExtContextForDeviceContext(ID3D11DeviceContext* pDeviceContext);

    void GetGTGenerationName(INTCDeviceInfo* pIntelDeviceInfo);

}; // struct ExtensionContextBase;

// RAII helper resolving the ID3D11VkExtContext for a given device context.
// The immediate context's interface is cached/borrowed (no AddRef), while any other
// (e.g. deferred) context is queried on demand and released automatically.
class ScopedDxVkExtContext
{
  public:
    ScopedDxVkExtContext(ID3D11VkExtContext* pImmediateCtx, ID3D11DeviceContext* pDeviceContext, ID3D11DeviceContext* pImmediateDeviceContext)
        : m_IsBorrowed(pDeviceContext == pImmediateDeviceContext),
          m_pExtContext(m_IsBorrowed ? pImmediateCtx : ExtensionContextBase::GetDxVkExtContextForDeviceContext(pDeviceContext))
    {
    }

    ~ScopedDxVkExtContext()
    {
        if (!m_IsBorrowed && m_pExtContext)
        {
            m_pExtContext->Release();
        }
    }

    ScopedDxVkExtContext(const ScopedDxVkExtContext&)            = delete;
    ScopedDxVkExtContext& operator=(const ScopedDxVkExtContext&) = delete;

    ID3D11VkExtContext* operator->() const { return m_pExtContext; }
    explicit            operator bool() const { return m_pExtContext != nullptr; }

  private:
    bool                m_IsBorrowed;
    ID3D11VkExtContext* m_pExtContext;
};
