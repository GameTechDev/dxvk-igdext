/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  WineUnixLib.cpp

Abstract:   See WineUnixLib.h.

\*****************************************************************************/
#include "Stdafx.h"

#include "WineUnixLib.h"

#include <winternl.h>

#include <cwchar>
#include <string>

extern "C" NTSTATUS NTAPI NtQueryVirtualMemory(
    HANDLE  processHandle,
    PVOID   baseAddress,
    ULONG   memoryInformationClass,
    PVOID   memoryInformation,
    SIZE_T  memoryInformationLength,
    PSIZE_T returnLength);

namespace
{
    constexpr ULONG MemoryWineLoadUnixLibByName = 1002;

    // "\??\unix<unixPath>" - Wine's own escape hatch for treating an NT path
    // as a literal Unix one.
    std::wstring BuildUnixEscapePath(const wchar_t* unixPath)
    {
        return std::wstring(L"\\??\\unix") + unixPath;
    }

    bool ResolveByWineDllPath(const wchar_t* soName, UINT64 (&result)[2])
    {
        wchar_t wideEnv[8192];
        DWORD   length = GetEnvironmentVariableW(L"WINEDLLPATH", wideEnv, ARRAYSIZE(wideEnv));
        if (length == 0 || length >= ARRAYSIZE(wideEnv))
        {
            return false;
        }

        wchar_t* entryStart = wideEnv;
        while (*entryStart)
        {
            wchar_t* entryEnd = wcschr(entryStart, L':');
            if (entryEnd)
            {
                *entryEnd = L'\0';
            }

            wchar_t candidate[1024];
            swprintf(candidate, ARRAYSIZE(candidate), L"%ls/igdext/x86_64-windows/%ls.so", entryStart, soName);

            std::wstring   ntPath = BuildUnixEscapePath(candidate);
            UNICODE_STRING name{};
            name.Buffer        = const_cast<wchar_t*>(ntPath.c_str());
            name.Length        = static_cast<USHORT>(ntPath.size() * sizeof(wchar_t));
            name.MaximumLength = name.Length;

            NTSTATUS status = NtQueryVirtualMemory(
                GetCurrentProcess(), &name, MemoryWineLoadUnixLibByName, result, sizeof(result), nullptr);

            if (status == 0)
            {
                return true;
            }

            if (!entryEnd)
            {
                break;
            }
            entryStart = entryEnd + 1;
        }

        return false;
    }
}

WineUnixLib LoadWineUnixLib(const wchar_t* soName)
{
    WineUnixLib lib;

    UINT64 result[2] = {};
    if (!ResolveByWineDllPath(soName, result))
    {
        return lib;
    }

    HMODULE ntdll = GetModuleHandleW(L"ntdll.dll");
    if (!ntdll)
    {
        return lib;
    }

    // __wine_unix_call_dispatcher is a data export, not a function - needs
    // one extra dereference.
    auto ppDispatcher = reinterpret_cast<WineUnixCallFn*>(GetProcAddress(ntdll, "__wine_unix_call_dispatcher"));
    if (!ppDispatcher || !*ppDispatcher)
    {
        return lib;
    }

    lib.m_FuncsHandle = result[1];
    lib.m_Dispatcher  = *ppDispatcher;
    return lib;
}

bool CallWineUnixLib(const WineUnixLib& lib, uint32_t code, void* args)
{
    if (!lib.IsLoaded())
    {
        return false;
    }
    NTSTATUS status = lib.m_Dispatcher(lib.m_FuncsHandle, code, args);
    return status == 0;
}
