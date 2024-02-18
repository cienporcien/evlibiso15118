// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <eviso15118/d20/config.hpp>
#include <eviso15118/d20/session.hpp>
#include <eviso15118/message/service_selection.hpp>

namespace eviso15118::d20::state {

message_20::ServiceSelectionResponse handle_request(const message_20::ServiceSelectionRequest& req,
                                                    d20::Session& session);

} // namespace eviso15118::d20::state
