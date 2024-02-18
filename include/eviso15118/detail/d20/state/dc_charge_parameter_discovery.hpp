// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d20/config.hpp>
#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/dc_charge_parameter_discovery.hpp>

namespace eviso15118::d20::state {

message_20::DC_ChargeParameterDiscoveryResponse
handle_request(const message_20::DC_ChargeParameterDiscoveryRequest& req, const d20::Session& session,
               const d20::SessionConfig& config);

} // namespace eviso15118::d20::state
