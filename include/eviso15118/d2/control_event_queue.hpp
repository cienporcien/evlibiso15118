// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <mutex>
#include <optional>
#include <queue>

#include "control_event.hpp"

namespace eviso15118::d2 {

class ControlEventQueue {
public:
    std::optional<ControlEvent> pop();
    void push(ControlEvent);

private:
    std::queue<ControlEvent> queue;
    std::mutex mutex;
};

} // namespace eviso15118::d2
