/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  LegacyD3D11ExtensionFuncs.h

Abstract:   Public header for Intel D3D11 Extensions Framework

Notes:      This file is intended to be included by the application to use
            Intel D3D11 Extensions Framework.

            Hand-transcribed, not vendored from the SDK: no public
            MIT-licensed header exists for the legacy (pre-2019) ABI. The
            static_assert blocks below are the drift safety net in its place.

\*****************************************************************************/

struct ExtensionContextBase;
struct LegacyExtensionInfo;
struct LegacyExtensionAppInfo;

typedef HRESULT(APIENTRY* PFNINTCDX11EXT_LEGACY_D3D11BEGINUAVOVERLAP)(
    ExtensionContextBase* pExtensionContext);

typedef HRESULT(APIENTRY* PFNINTCDX11EXT_LEGACY_D3D11ENDUAVOVERLAP)(
    ExtensionContextBase* pExtensionContext);

typedef void(APIENTRY* PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECT)(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    UINT                  drawCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs);

typedef void(APIENTRY* PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECT)(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    UINT                  drawCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs);

typedef void(APIENTRY* PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECTCOUNTINDIRECT)(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    ID3D11Buffer*         pBufferForDrawCount,
    UINT                  alignedByteOffsetForDrawCount,
    UINT                  maxCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs);

typedef void(APIENTRY* PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECTCOUNTINDIRECT)(
    ExtensionContextBase* pExtensionContext,
    ID3D11DeviceContext*  pDeviceContext,
    ID3D11Buffer*         pBufferForDrawCount,
    UINT                  alignedByteOffsetForDrawCount,
    UINT                  maxCount,
    ID3D11Buffer*         pBufferForArgs,
    UINT                  alignedByteOffsetForArgs,
    UINT                  byteStrideForArgs);

struct D3D11_EXTENSION_FUNCS_0400
{
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECT                     D3D11MultiDrawInstancedIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECT              D3D11MultiDrawIndexedInstancedIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECTCOUNTINDIRECT        D3D11MultiDrawInstancedIndirectCountIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECTCOUNTINDIRECT D3D11MultiDrawIndexedInstancedIndirectCountIndirect;
};
using D3D11ExtensionFuncs = D3D11_EXTENSION_FUNCS_0400;

// Frozen legacy shape (no public SDK source to vendor - see file header) -
// LegacyD3D11ExtensionContext::InitExtensions dispatches on this exact size.
static_assert(sizeof(D3D11_EXTENSION_FUNCS_0400) == 4 * sizeof(void*),
    "D3D11_EXTENSION_FUNCS_0400 must stay exactly 4 function pointers");

struct D3D11_EXTENSION_FUNCS_01000000
{
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECT                     D3D11MultiDrawInstancedIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECT              D3D11MultiDrawIndexedInstancedIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECTCOUNTINDIRECT        D3D11MultiDrawInstancedIndirectCountIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECTCOUNTINDIRECT D3D11MultiDrawIndexedInstancedIndirectCountIndirect;
};

struct D3D11_EXTENSION_FUNCS_01000001
{
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECT                     D3D11MultiDrawInstancedIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECT              D3D11MultiDrawIndexedInstancedIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINSTANCEDINDIRECTCOUNTINDIRECT        D3D11MultiDrawInstancedIndirectCountIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11MULTIDRAWINDEXEDINSTANCEDINDIRECTCOUNTINDIRECT D3D11MultiDrawIndexedInstancedIndirectCountIndirect;
    PFNINTCDX11EXT_LEGACY_D3D11BEGINUAVOVERLAP                                D3D11BeginUAVOverlap;
    PFNINTCDX11EXT_LEGACY_D3D11ENDUAVOVERLAP                                  D3D11EndUAVOverlap;
};

// Frozen legacy shape (no public SDK source to vendor - see file header) -
// LegacyD3D11ExtensionContext::InitExtensions dispatches on this exact size.
static_assert(sizeof(D3D11_EXTENSION_FUNCS_01000001) == 6 * sizeof(void*),
    "D3D11_EXTENSION_FUNCS_01000001 must stay exactly 6 function pointers");

using D3D11_EXTENSION_FUNCS_01000002 = D3D11_EXTENSION_FUNCS_01000001;
