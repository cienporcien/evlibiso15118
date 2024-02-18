// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d2/config.hpp>
#include <eviso15118/d2/session.hpp>
#include <eviso15118/message_d2/dc_charge_parameter_discovery.hpp>

namespace eviso15118::d2::state {

message_2::DC_ChargeParameterDiscoveryResponse
handle_request(const message_2::DC_ChargeParameterDiscoveryRequest& req, const d2::Session& session,
               const d2::SessionConfig& config);

} // namespace eviso15118::d2::state
