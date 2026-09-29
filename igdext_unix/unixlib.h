/*****************************************************************************\

Copyright (C) 2026 Intel Corporation

SPDX-License-Identifier: MIT

File Name:  unixlib.h

Abstract:   Interface between the PE and Unix sides of igdext_unix

Notes:      The Intel Vulkan driver mirrors set_application_info_params -
            keep its layout in sync.

\*****************************************************************************/
#pragma once

#include <stdint.h>

#include "wine/unixlib.h"

enum igdext_unix_funcs
{
    unix_set_application_info,
};

struct app_info_version
{
    uint32_t major;
    uint32_t minor;
    uint32_t patch;
};

/* Application/engine info as passed to INTC_D3D12_SetApplicationInfo, kept
 * for the native Vulkan driver to pull via
 * igdext_unix_get_application_info() (see unixlib.c). */
struct set_application_info_params
{
    char                    application_name[256];
    struct app_info_version application_version;
    char                    engine_name[256];
    struct app_info_version engine_version;
};
