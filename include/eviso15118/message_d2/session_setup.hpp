// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

#include "common.hpp"

namespace eviso15118::message_2 {

struct SessionSetupRequest {
    Header header;
    std::string evccid;
};

struct SessionSetupResponse {
    Header header;
    ResponseCode response_code;
    std::string evseid;
};

} // namespace eviso15118::message_2
