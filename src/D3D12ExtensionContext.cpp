/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  D3D12ExtensionContext.cpp

Abstract:   Modern D3D12 extension entry points: version negotiation, context
            creation, and application/engine info reporting to the native
            driver

Notes:      Application/engine info reaches the native driver only through
            D3D12SetApplicationInfo's WineUnixLib relay - see WineUnixLib.h.

\*****************************************************************************/
#include "Stdafx.h"

#include "IntelAppInfoUnixlib.h"

namespace
{
    std::string WideToUtf8(const wchar_t* wide)
    {
        if (!wide || !*wide)
        {
            return {};
        }

        int size = WideCharToMultiByte(CP_UTF8, 0, wide, -1, nullptr, 0, nullptr, nullptr);
        if (size <= 0)
        {
            return {};
        }

        std::string result(size - 1, '\0'); // size includes the null terminator, std::string adds its own
        WideCharToMultiByte(CP_UTF8, 0, wide, -1, result.data(), size, nullptr, nullptr);
        return result;
    }
}

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

    // App info passed directly to CreateDeviceExtensionContext is not
    // relayed yet - not supported for now.

    return result;
}

HRESULT D3D12SetApplicationInfo(INTCExtensionAppInfo1* pExtensionAppInfo)
{
    if (!pExtensionAppInfo)
    {
        return E_INVALIDARG;
    }

    IntelAppInfoUnixlib_SetEngineInfo(
        WideToUtf8(pExtensionAppInfo->pApplicationName).c_str(),
        pExtensionAppInfo->ApplicationVersion.major, pExtensionAppInfo->ApplicationVersion.minor, pExtensionAppInfo->ApplicationVersion.patch,
        WideToUtf8(pExtensionAppInfo->pEngineName).c_str(),
        pExtensionAppInfo->EngineVersion.major, pExtensionAppInfo->EngineVersion.minor, pExtensionAppInfo->EngineVersion.patch);

    return S_OK;
}
