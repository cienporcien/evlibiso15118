// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <tuple>

#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/dc_pre_charge.hpp>

#include <eviso15118/session/feedback.hpp>

namespace eviso15118::d20::state {

std::tuple<message_20::DC_PreChargeResponse, session::feedback::DcChargeTarget>
handle_request(const message_20::DC_PreChargeRequest& req, const d20::Session& session, const float present_voltage);

} // namespace eviso15118::d20::state
