/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  IgdextApiDllInt.cpp

Abstract:   Graphics Extensions Internal DLL Source File

Notes:      Home of the "Internal Extensions" validation API family.

            D3D11/D3D12EnumInternalExtensions are real implementations: they
            report zero internal extensions, which is correct under DXVK and
            is also what igdext.lib's loader needs to see from these two
            eagerly-resolved exports to keep going.

            The three CreateDeviceExtensionContext stubs below exist for
            exactly the "Internal Extensions" functions igdext.lib's loader
            probes for right after the CreateDeviceExtensionContext/
            EnumInternalExtensions family, which otherwise returns nullptr
            for these three. Same reasoning as the D3D*CreateDeviceExtensionContext2
            stubs in IgdextApiDll.cpp: a loader that resolves its full
            expected export table before trusting the DLL bails out
            (FreeLibrary) on the first missing symbol. Only these three are
            stubbed - the loader never probes for anything else in this
            family.

\*****************************************************************************/

#include "Stdafx.h"

#ifdef __cplusplus
extern "C"
{
#endif

HRESULT D3D11EnumInternalExtensions(
    INTCInternalExtension** pInternalExtensions,
    UINT32*                 internalExtensionsCount)
{
    if (!internalExtensionsCount)
    {
        return E_INVALIDARG;
    }

    *internalExtensionsCount = 0;

    return S_OK;
}

HRESULT _INTC_D3D11_INT_CreateDeviceExtensionContext(...)
{
    return E_NOTIMPL;
}

HRESULT D3D12EnumInternalExtensions(
    INTCInternalExtension** pInternalExtensions,
    UINT32*                 internalExtensionsCount)
{
    if (!internalExtensionsCount)
    {
        return E_INVALIDARG;
    }

    *internalExtensionsCount = 0;

    return S_OK;
}

HRESULT _INTC_D3D12_INT_CreateDeviceExtensionContext(...)
{
    return E_NOTIMPL;
}

HRESULT _INTC_INT_CreateDeviceExtensionContext(...)
{
    return E_NOTIMPL;
}

#ifdef __cplusplus
}
#endif
