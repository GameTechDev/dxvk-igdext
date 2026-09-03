/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  ExtensionContext.h

Abstract:   INTCExtensionContext - static library visible context class definition

Notes:

\*****************************************************************************/
#pragma once

struct D3D11ExtensionContext;
struct D3D12ExtensionContext;

// Intel Extensions Context class that contains implementations for D3D11 and D3D12
struct INTCExtensionContext
{
    D3D11ExtensionContext* m_pD3D11ExtensionContext = nullptr; // D3D11 internal extension context
    D3D12ExtensionContext* m_pD3D12ExtensionContext = nullptr; // D3D12 internal extension context

    INTCExtensionContext() = default;
};
