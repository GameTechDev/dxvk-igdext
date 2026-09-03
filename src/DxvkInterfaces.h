/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  DxvkInterfaces.h

Abstract:   DXVK COM interface definitions used for runtime detection and
            direct dispatch of extension calls to DXVK

Notes:      Duplication of d3d11_interfaces.h from DXVK
            https://github.com/doitsujin/dxvk/blob/master/src/d3d11/d3d11_interfaces.h

\*****************************************************************************/
#pragma once

#include <d3d11.h>

/**
 * \brief Barrier control flags
 */
enum D3D11_VK_BARRIER_CONTROL : uint32_t
{
    D3D11_VK_BARRIER_CONTROL_IGNORE_WRITE_AFTER_WRITE = 1 << 0,
};

/**
 * \brief Extended D3D11 context
 *
 * Provides functionality for various D3D11
 * extensions.
 */
MIDL_INTERFACE("fd0bca13-5cb6-4c3a-987e-4750de2ca791")
ID3D11VkExtContext : public IUnknown
{
    virtual void STDMETHODCALLTYPE MultiDrawIndirect(
        UINT DrawCount,
        ID3D11Buffer * pBufferForArgs,
        UINT ByteOffsetForArgs,
        UINT ByteStrideForArgs) = 0;

    virtual void STDMETHODCALLTYPE MultiDrawIndexedIndirect(
        UINT DrawCount,
        ID3D11Buffer * pBufferForArgs,
        UINT ByteOffsetForArgs,
        UINT ByteStrideForArgs) = 0;

    virtual void STDMETHODCALLTYPE MultiDrawIndirectCount(
        UINT MaxDrawCount,
        ID3D11Buffer * pBufferForCount,
        UINT ByteOffsetForCount,
        ID3D11Buffer * pBufferForArgs,
        UINT ByteOffsetForArgs,
        UINT ByteStrideForArgs) = 0;

    virtual void STDMETHODCALLTYPE MultiDrawIndexedIndirectCount(
        UINT MaxDrawCount,
        ID3D11Buffer * pBufferForCount,
        UINT ByteOffsetForCount,
        ID3D11Buffer * pBufferForArgs,
        UINT ByteOffsetForArgs,
        UINT ByteStrideForArgs) = 0;

    virtual void STDMETHODCALLTYPE SetDepthBoundsTest(
        BOOL  Enable,
        FLOAT MinDepthBounds,
        FLOAT MaxDepthBounds) = 0;

    virtual void STDMETHODCALLTYPE SetBarrierControl(
        UINT ControlFlags) = 0;
};

#ifndef _MSC_VER
// MinGW's __uuidof() emulation needs an explicit __CRT_UUID_DECL per COM type.
// MSVC derives __uuidof() directly from the MIDL_INTERFACE(...) GUID above.
__CRT_UUID_DECL(ID3D11VkExtContext, 0xfd0bca13, 0x5cb6, 0x4c3a, 0x98, 0x7e, 0x47, 0x50, 0xde, 0x2c, 0xa7, 0x91)
#endif
