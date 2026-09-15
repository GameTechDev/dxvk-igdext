/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D12ExtensionContext.cpp

Abstract:   Modern D3D12 extension entry points: version negotiation and
            context creation

Notes:      Mirrors D3D11ExtensionContext.cpp's pattern: negotiate a
            version, then report device info via ExtensionContextBase's
            D3D12 (LUID-based) GetDeviceDriverDescription overload.

\*****************************************************************************/
#include "Stdafx.h"

HRESULT D3D12ExtensionContext::GetSupportedVersions(const void* pDevice, INTCExtensionVersionHelper* driverExtensionVersion)
{
    INTCExtensionVersion driverSupportedVersion = c_MaxDriverSupportedExtVersion;

    m_pAppDevice = const_cast<ID3D12Device*>(reinterpret_cast<const ID3D12Device*>(pDevice));

    *driverExtensionVersion = driverSupportedVersion;

    return S_OK;
}

HRESULT D3D12ExtensionContext::InitExtensions(const void* pDevice, void** ppfnExtensionFuncs, UINT32 extensionFuncsSize, INTCExtensionInfo1* pExtensionInfo, INTCExtensionAppInfo1* pExtensionAppInfo, bool internalExtensions)
{
    HRESULT                    result                 = S_OK;
    INTCExtensionVersionHelper driverSupportedVersion = c_MaxDriverSupportedExtVersion;

    // Trivially reject non-initialized variables
    if (pExtensionInfo->RequestedExtensionVersion.HWFeatureLevel == 0 && pExtensionInfo->RequestedExtensionVersion.APIVersion == 0)
    {
        return E_NOINTERFACE;
    }

    // Save the Device first
    m_pAppDevice = const_cast<ID3D12Device*>(reinterpret_cast<const ID3D12Device*>(pDevice));

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
    GetGTGenerationName(&pExtensionInfo->IntelDeviceInfo);
    pExtensionInfo->IntelDeviceInfo.GPUMaxFreq   = c_GPUMaxFreq;
    pExtensionInfo->IntelDeviceInfo.GPUMinFreq   = c_GPUMinFreq;
    pExtensionInfo->IntelDeviceInfo.GTGeneration = c_GTGeneration;
    pExtensionInfo->IntelDeviceInfo.EUCount      = c_EUCount;
    pExtensionInfo->IntelDeviceInfo.PackageTDP   = c_PackageTDP;
    pExtensionInfo->IntelDeviceInfo.MaxFillRate  = c_MaxFillRate;
    pExtensionInfo->IntelDeviceInfo.GMDID        = c_GMDID;
    pExtensionInfo->IntelDeviceInfo.XeCoresCount = c_XeCoresCount;

    pExtensionInfo->pDeviceDriverDesc       = m_DeviceDriverDescription.c_str(); // Get device driver description signature
    pExtensionInfo->pDeviceDriverVersion    = c_DeviceDriverVersion;
    pExtensionInfo->DeviceDriverBuildNumber = c_DeviceDriverBuildNumber;

    return result;
}
