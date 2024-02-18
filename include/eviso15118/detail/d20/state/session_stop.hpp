// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/session_stop.hpp>

namespace eviso15118::d20::state {

message_20::SessionStopResponse handle_request(const message_20::SessionStopRequest& req, const d20::Session& session);

} // namespace eviso15118::d20::state
