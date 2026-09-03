/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  Stdafx.h

Abstract:   Common include hub for the whole repo

Notes:      Central include hub: pulls in the D3D11/D3D12 headers and the
            public/legacy API headers used throughout this repo.

\*****************************************************************************/
#pragma once

#include <windows.h>
#include <cstdint>
#include <cstdio>
#include <string>
#include <wrl/client.h>

#include <d3d11.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <algorithm>
#include <memory>
#include <string_view>
#include <wchar.h>

// Intel Extensions API header files - vendored from the real public SDK
// (d3dext-sdk, see d3dext-sdk/VERSION).
//
// INTC_IGDEXT_D3D12 is deliberately NOT defined: it unlocks igdext.h's D3D12
// raytracing section, which references D3D12_STATE_OBJECT_DESC and friends -
// types mingw-w64's bundled d3d12.h doesn't have. D3D12 support in this repo
// is a stub only, so INTC_D3D12_API_CALLBACKS (only ever used as an
// unused, never-dereferenced pointer parameter) is forward-declared here
// instead of pulled in from that gated section.
#include "igdext.h"

struct INTC_D3D12_API_CALLBACKS;

// igdext_int.h is not vendored: INTCInternalExtension is only ever used as an
// opaque, never-dereferenced pointer parameter (IgdextApiDllInt.cpp's stubs),
// so a forward declaration is all that's needed.
struct INTCInternalExtension;

#include "LegacyD3D11ExtensionFuncs.h"

// Intel Extensions Infrastructure header files
#include "LegacyExtensionTypes.h"
#include "ExtensionContextBase.h" // ExtensionContextBase - internal base context class definition
#include "D3D11ExtensionContext.h"
#include "D3D12ExtensionContext.h"

// Intel Extensions API implementation header files
#include "ExtensionContext.h" // INTCExtensionContext - static library visible context class definition
