// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d2/session.hpp>
#include <eviso15118/message_d2/dc_welding_detection.hpp>

namespace eviso15118::d2::state {

message_2::DC_WeldingDetectionResponse handle_request(const message_2::DC_WeldingDetectionRequest& req,
                                                       const d2::Session& session, const float present_voltage);

} // namespace eviso15118::d2::state
