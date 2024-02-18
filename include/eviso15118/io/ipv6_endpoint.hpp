// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <cstdint>

namespace eviso15118::io {

struct Ipv6EndPoint {
    uint16_t port;
    uint16_t address[8];
};

} // namespace eviso15118::io
