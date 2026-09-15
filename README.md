# dxvk-igdext

`igdext64.dll` for Wine/DXVK: lets games use Intel D3D11 Graphics Extensions
(UAV overlap, MultiDrawIndirect, depth bounds) under Wine by forwarding them
to DXVK's native implementation, instead of the game silently falling back
to slower default D3D11 behavior when the extensions aren't available.

> **Note:** Linux/Wine only for now, not for use on native Windows.

## Handled automatically by Proton

Proton ships this project on its `bleeding-edge` branch as of
[`dxvk-igdext: Add submodule and copy igdext64.dll into the prefix`](https://github.com/ValveSoftware/Proton/commit/5f11660417cb19bb19e0d0bb349c43bdbf2f3fb1):
it builds `dxvk-igdext` as a submodule into
`lib/wine/igdext/x86_64-windows` and copies `igdext64.dll` into the
prefix at launch, so there is no manual step.

## Build

```sh
cmake -B build -DCMAKE_TOOLCHAIN_FILE=./toolchain-mingw64.cmake
cmake --build build
```

Needs a MinGW-w64 cross toolchain. Produces `build/igdext64.dll`.

> **Note:** the CMake project also configures and builds under MSVC (drop
> `-DCMAKE_TOOLCHAIN_FILE`) if you need to build it by hand on Windows.

## Deploy

Only needed when not using a Proton build that already does the above.
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

Copying the DLL is not enough on its own: that DriverStore path, and the
fake DX11 Intel UMD (`igd10iumd64.dll`) that makes games look for it, come
from wine's `HACK/INTEL` patches
([fake UMD DLL](https://github.com/ValveSoftware/wine/commit/22132c7774364bac5d53ca9af5b96a6bc08675d5),
[DriverInfPath](https://github.com/ValveSoftware/wine/commit/4c53233bc9dd083509198dfba8a3afc9bfd80d3b),
[SetupGetInfDriverStoreLocationW](https://github.com/ValveSoftware/wine/commit/eda69af06f8cbbd6f9a5bc3a3b4e34095f957ace))
plus DXVK's
[`[d3d11] Load Intel vendor hack DLL`](https://github.com/doitsujin/dxvk/commit/83f167d2)
patch, which loads that UMD.

Check whether the wine and DXVK you are running already carry them before
patching anything by hand: the wine side lives in Proton's own wine fork
and the DXVK side is upstream, so a reasonably recent Proton build likely
has both already.

## Layout

```
src/           Modern (_INTC_-prefixed) extension context and public API types
├── dll/       DLL export entry points (Igdext.def + the exported functions)
└── legacy/    Legacy (pre-2019 ABI, non-INTC-prefixed) extension context and types
```

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
looking for. D3D11 entries actually forward to DXVK; D3D12 entries are
inert stubs (`GetSupportedVersions` reports zero, `CreateDeviceExtensionContext`
always fails) kept only because `igdext.lib`'s loader requires the export to
exist at all.

All 38 exports are also listed by name in [Igdext.def](src/dll/Igdext.def),
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
| `D3D12GetSupportedVersions2` | Modern D3D12 | stub |
| `_INTC_D3D12_CreateDeviceExtensionContext` | Modern D3D12 | stub |
| `_INTC_D3D12_CreateDeviceExtensionContext1` | Modern D3D12 | stub |
| `_INTC_D3D12_CreateDeviceExtensionContext2` | Modern D3D12 | stub |
| `_INTC_D3D12_GetSupportedVersions` | Modern D3D12 | stub |
| `_INTC_D3D12_RegisterApplicationCallbacks` | Modern D3D12 | stub, inert |
| `D3D12CreateDeviceExtensionContext2` | Modern D3D12 (loader-compat) | stub |
| `_INTC_D3D12_INT_CreateDeviceExtensionContext` | Modern D3D12 (Internal Extensions) | stub |
| `_INTC_CreateDeviceExtensionContext` | Modern D3D11+D3D12 | D3D11 half negotiates version and requires DXVK, D3D12 half fails |
| `_INTC_CreateDeviceExtensionContext1` | Modern D3D11+D3D12 | D3D11 half negotiates version and requires DXVK, D3D12 half fails |
| `_INTC_DestroyDeviceExtensionContext` | Modern D3D11+D3D12 | frees context |
| `D3D11D3D12CreateDeviceExtensionContext2` | Modern D3D11+D3D12 (loader-compat) | D3D11 half negotiates version and requires DXVK, D3D12 half fails |
| `D3D11D3D12DestroyDeviceExtensionContext2` | Modern D3D11+D3D12 (loader-compat) | frees context |
| `_INTC_INT_CreateDeviceExtensionContext` | Modern D3D11+D3D12 (Internal Extensions) | stub |
