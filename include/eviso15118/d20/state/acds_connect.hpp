// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../fsm.hpp"
#include <eviso15118/message/acds_connect.hpp>

namespace eviso15118::d20::state {

struct ACDS_Connect : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;

    message_20::ACDS_ConnectRequest setup_request(const d20::Session &session);

private:
    bool acds_connect_initiated{false};
    bool acds_connect_done{false};
};

} // namespace eviso15118::d20::stat