# dxvk-igdext

`igdext64.dll` for Wine/DXVK/vkd3d-proton: lets games use Intel D3D11 Graphics
Extensions (UAV overlap, MultiDrawIndirect, depth bounds) under Wine by
forwarding them to DXVK's native implementation, instead of the game silently
falling back to slower default D3D11 behavior when the extensions aren't
available. Also lets D3D12 games create an Intel extension context and
report application/engine info to the native driver.

> **Note:** Linux/Wine only for now, not for use on native Windows.

## Build

```sh
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./toolchain-mingw64.cmake
cmake --build build
```

Needs a MinGW-w64 cross toolchain. Produces `build/igdext64.dll`.

> **Note:** the CMake project also configures and builds under MSVC (drop
> `-DCMAKE_TOOLCHAIN_FILE`) if you need to build it by hand on Windows.

## Layout

```
src/           Modern (_INTC_-prefixed) extension context and public API types
├── dll/       DLL export entry points (Igdext.def + the exported functions)
└── legacy/    Legacy (pre-2019 ABI, non-INTC-prefixed) extension context and types
```

## Deploy

`igdext64.dll` needs to end up at:

```
C:\windows\system32\DriverStore\FileRepository\igd_faux.inf_1\
```

On Linux, that maps to a real path under the Wine/Proton prefix's `drive_c`:

```
$WINEPREFIX/drive_c/windows/system32/DriverStore/FileRepository/igd_faux.inf_1/
```

or, for a Steam Proton prefix specifically:

```
<Steam library>/steamapps/compatdata/<appid>/pfx/drive_c/windows/system32/DriverStore/FileRepository/igd_faux.inf_1/
```

### Handled automatically by Proton

A `proton` build places `igdext64.dll` there itself - no manual copy
step. That only works if `wine` and `dxvk` include these commits:

- **wine**: the fake DX11 Intel UMD (`igd10iumd64.dll`) and that DriverStore
  path come from `HACK/INTEL` patches -
  [fake UMD DLL](https://github.com/ValveSoftware/wine/commit/22132c7774364bac5d53ca9af5b96a6bc08675d5),
  [DriverInfPath](https://github.com/ValveSoftware/wine/commit/4c53233bc9dd083509198dfba8a3afc9bfd80d3b),
  [SetupGetInfDriverStoreLocationW](https://github.com/ValveSoftware/wine/commit/eda69af06f8cbbd6f9a5bc3a3b4e34095f957ace)
- **DXVK**: needs the
  [`[d3d11] Load Intel vendor hack DLL`](https://github.com/doitsujin/dxvk/commit/83f167d2)
  patch, which loads that fake UMD DLL
- **proton**: needs `dxvk-igdext` added as a submodule and wired into the
  build - TBD, not merged yet

## Exports

`igdext64.dll` exports two API generations, because games can be built
against either one depending on which version of Intel's SDK they linked:

- **Legacy** (`D3D11CreateDeviceExtensionContext`, etc., no `_INTC_` prefix) -
  the pre-2019 ABI (`LegacyD3D11ExtensionFuncs.h`; the D3D12 counterpart is stub-only and
  needs no header of its own), taking
  `ExtensionContextBase**`/`LegacyExtensionInfo*` instead of the modern
  `INTCExtensionContext**`/`INTCExtensionInfo*`.
- **Modern** (`_INTC_D3D11_*`/`_INTC_D3D12_*`, plus a few loader-compat
  entry points without the underscore, like `D3D11GetSupportedVersions2`)
  - the current SDK's ABI.

Both generations exist side by side so either kind of game finds what it's
looking for. D3D11 entries forward to DXVK. D3D12 entries negotiate a
version and report device info the same way D3D11 does, but have no
function table of their own beyond context creation and application info -
`SetEventMarker`, `GetCommandListHandle`, raytracing, ... are still out of
scope. Application/engine info reported through `_INTC_D3D12_SetApplicationInfo`
never touches any single `VkDevice` directly - it reaches the native driver
through an internal relay, so it's forwarded the same way regardless of
which Vulkan implementation backs the D3D12 device.

All 39 exports are also listed by name in [Igdext.def](src/dll/Igdext.def),
the module definition file the linker uses to build the export table.

| Export | Family | Behavior |
|---|---|---|
| `D3D11CreateDeviceExtensionContext` | Legacy D3D11 | fills function table forwarding to DXVK |
| `D3D11CreateDeviceExtensionContext1` | Legacy D3D11 | fills function table forwarding to DXVK |
| `D3D11DestroyDeviceExtensionContext` | Legacy D3D11 | frees context |
| `D3D11GetSupportedVersions` | Legacy D3D11 | reports hardcoded max version |
| `D3D12CreateDeviceExtensionContext` | Legacy D3D12 | stub |
| `D3D12DestroyDeviceExtensionContext` | Legacy D3D12 | stub |
| `D3D12GetSupportedVersions` | Legacy D3D12 | stub |
| `D3D11EnumInternalExtensions` | Modern D3D11 | reports zero (correct) |
| `D3D11GetSupportedVersions2` | Modern D3D11 | reports hardcoded max version |
| `_INTC_D3D11_BeginUAVOverlap` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_EndUAVOverlap` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_MultiDrawIndexedInstancedIndirect` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_MultiDrawIndexedInstancedIndirectCountIndirect` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_MultiDrawInstancedIndirect` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_MultiDrawInstancedIndirectCountIndirect` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_SetDepthBounds` | Modern D3D11 | → DXVK |
| `_INTC_D3D11_CreateTexture2D` | Modern D3D11 | → plain CreateTexture2D, no DXVK ext |
| `_INTC_D3D11_CreateDeviceExtensionContext` | Modern D3D11 | negotiates version, requires DXVK |
| `_INTC_D3D11_CreateDeviceExtensionContext1` | Modern D3D11 | negotiates version, requires DXVK |
| `_INTC_D3D11_GetSupportedVersions` | Modern D3D11 | reports hardcoded max version |
| `_INTC_D3D11_RegisterApplicationCallbacks` | Modern D3D11 | stub, inert |
| `D3D11CreateDeviceExtensionContext2` | Modern D3D11 (loader-compat) | negotiates version, requires DXVK |
| `_INTC_D3D11_INT_CreateDeviceExtensionContext` | Modern D3D11 (Internal Extensions) | stub |
| `D3D12EnumInternalExtensions` | Modern D3D12 | reports zero (correct) |
| `D3D12GetSupportedVersions2` | Modern D3D12 | reports max version |
| `_INTC_D3D12_CreateDeviceExtensionContext` | Modern D3D12 | negotiates version |
| `_INTC_D3D12_CreateDeviceExtensionContext1` | Modern D3D12 | negotiates version |
| `_INTC_D3D12_CreateDeviceExtensionContext2` | Modern D3D12 | negotiates version |
| `_INTC_D3D12_GetSupportedVersions` | Modern D3D12 | reports max version |
| `_INTC_D3D12_RegisterApplicationCallbacks` | Modern D3D12 | stub, inert |
| `_INTC_D3D12_SetApplicationInfo` | Modern D3D12 | relays app/engine info through the WineUnixLib bridge |
| `D3D12CreateDeviceExtensionContext2` | Modern D3D12 (loader-compat) | negotiates version |
| `_INTC_D3D12_INT_CreateDeviceExtensionContext` | Modern D3D12 (Internal Extensions) | stub |
| `_INTC_CreateDeviceExtensionContext` | Modern D3D11+D3D12 | D3D11 half requires DXVK - both negotiate version |
| `_INTC_CreateDeviceExtensionContext1` | Modern D3D11+D3D12 | D3D11 half requires DXVK - both negotiate version |
| `_INTC_DestroyDeviceExtensionContext` | Modern D3D11+D3D12 | frees context |
| `D3D11D3D12CreateDeviceExtensionContext2` | Modern D3D11+D3D12 (loader-compat) | D3D11 half requires DXVK - both negotiate version |
| `D3D11D3D12DestroyDeviceExtensionContext2` | Modern D3D11+D3D12 (loader-compat) | frees context |
| `_INTC_INT_CreateDeviceExtensionContext` | Modern D3D11+D3D12 (Internal Extensions) | stub |
