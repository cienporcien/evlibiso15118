// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "../fsm.hpp"
#include <eviso15118/message/acdp_disconnect.hpp>

namespace eviso15118::d20::state {

struct ACDP_Disconnect : public FsmSimpleState {
    using FsmSimpleState::FsmSimpleState;

    void enter() final;

    HandleEventReturnType handle_event(AllocatorType&, FsmEvent) final;

    message_20::ACDP_DisconnectRequest setup_request(const d20::Session &session);

private:
    bool acdp_disconnect_initiated{false};
    bool acdp_disconnect_done{false};
};

} // namespace eviso15118::d20::stat