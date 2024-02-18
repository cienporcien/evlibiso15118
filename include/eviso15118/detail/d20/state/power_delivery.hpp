// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/power_delivery.hpp>

namespace eviso15118::d20::state {

message_20::PowerDeliveryResponse handle_request(const message_20::PowerDeliveryRequest& req,
                                                 const d20::Session& session);

} // namespace eviso15118::d20::state
