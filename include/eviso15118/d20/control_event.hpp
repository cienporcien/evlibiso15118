// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <variant>

namespace eviso15118::d20 {
class CableCheckFinished {
public:
    explicit CableCheckFinished(bool success_) : success(success_) {
    }

    operator bool() const {
        return success;
    }

private:
    bool success;
};

struct PresentVoltageCurrent {
    float voltage;
    float current;
};

class AuthorizationResponse {
public:
    explicit AuthorizationResponse(bool authorized_) : authorized(authorized_) {
    }

    operator bool() const {
        return authorized;
    }

private:
    bool authorized;
};

enum start_stop_charging{
    STOP_CHARGING = 0,
    START_CHARGING,
    PAUSE_CHARGING,
};

class StartStopCharging {
public:
    explicit StartStopCharging(start_stop_charging start_) : start(start_) {
    }

    operator start_stop_charging() const {
        return start;
    }

private:
    start_stop_charging start;
};

using ControlEvent = std::variant<CableCheckFinished, PresentVoltageCurrent, AuthorizationResponse, StartStopCharging>;

} // namespace eviso15118d20
