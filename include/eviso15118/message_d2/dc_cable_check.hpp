// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "common.hpp"

namespace eviso15118::message_2 {

struct DC_CableCheckRequest {
    Header header;
};

struct DC_CableCheckResponse {

    DC_CableCheckResponse() : processing(Processing::Ongoing){};

    Header header;
    ResponseCode response_code;

    Processing processing;
};

} // namespace eviso15118::message_2
