// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <vector>

#include <eviso15118/d2/config.hpp>
#include <eviso15118/d2/session.hpp>
#include <eviso15118/message_d2/common.hpp>
#include <eviso15118/message_d2/service_discovery.hpp>

namespace eviso15118::d2::state {

message_2::ServiceDiscoveryResponse handle_request(const message_2::ServiceDiscoveryRequest& req,
                                                    d2::Session& session,
                                                    const std::vector<message_2::ServiceCategory>& energy_services,
                                                    const std::vector<message_2::ServiceCategory>& vas_services);

} // namespace eviso15118::d2::state
