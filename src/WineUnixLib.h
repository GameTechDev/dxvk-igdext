/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  WineUnixLib.h

Abstract:   Loads a native (.so) Wine "unixlib" module and dispatches calls
            into it.

Notes:      Depends on a small ntdll backport (branch unixlib-load-by-name
            of this project's wine fork) adding MemoryWineLoadUnixLibByName
            to NtQueryVirtualMemory. Resolves WINEDLLPATH itself and looks
            for "<entry>/igdext/x86_64-windows/<soName>.so" - the same
            layout CMakeLists.txt installs igdext64.dll under.

            Fails cleanly (IsLoaded() == false) on real Windows or a Wine
            build without that backport.

\*****************************************************************************/
#pragma once

#include <cstdint>

using WineUnixCallFn = int32_t(__stdcall*)(uint64_t funcsHandle, uint32_t code, void* args);

struct WineUnixLib
{
    uint64_t       m_FuncsHandle = 0;
    WineUnixCallFn m_Dispatcher  = nullptr;

    bool IsLoaded() const
    {
        return m_FuncsHandle != 0 && m_Dispatcher != nullptr;
    }
};

WineUnixLib LoadWineUnixLib(const wchar_t* soName);

bool CallWineUnixLib(const WineUnixLib& lib, uint32_t code, void* args);
