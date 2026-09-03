/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  LegacyExtensionTypes.h

Abstract:   Defines Extensions Framework types

Notes:      The Legacy* structs used by the pre-modern-API ABI
            (LegacyD3D11/D3D12ExtensionContext, IgdextApiDllLegacy.cpp).

            Hand-transcribed, not vendored from the SDK: no public
            MIT-licensed header exists for the legacy (pre-2019) ABI.

\*****************************************************************************/
#pragma once

struct LegacyIntelDeviceInfo
{
    uint32_t GPUMaxFreq;
    uint32_t GPUMinFreq;
    uint32_t GTGeneration;
    uint32_t EUCount;
    uint32_t PackageTDP;
    uint32_t MaxFillRate;
    char     GTGenerationName[40];
};

union LegacyExtensionVersion
{
    struct
    {
        uint32_t Revision : 16;
        uint32_t Minor : 8;
        uint32_t Major : 8;
    } Version;
    uint32_t FullVersion;
};

struct LegacyExtensionInfo
{
    const wchar_t*         pDeviceDriverDesc;
    LegacyIntelDeviceInfo  intelDeviceInfo;
    LegacyExtensionVersion requestedExtensionVersion;
    LegacyExtensionVersion returnedExtensionVersion;
};

struct LegacyExtensionAppInfo
{
    const wchar_t* pApplicationName;
    uint32_t       applicationVersion;
    const wchar_t* pEngineName;
    uint32_t       engineVersion;
};
