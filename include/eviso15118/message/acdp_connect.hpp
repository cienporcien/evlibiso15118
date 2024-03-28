// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include "common.hpp"

namespace eviso15118::message_20 {


struct ACDP_ConnectRequest {
    Header header;
    electricalChargingDeviceStatusType EVElectricalChargingDeviceStatus;
};



struct ACDP_ConnectResponse {

    ACDP_ConnectResponse() : processing(Processing::Ongoing){};

    Header header;
    ResponseCode response_code;
    Processing processing;
    electricalChargingDeviceStatusType EVSEElectricalChargingDeviceStatus;
    mechanicalChargingDeviceStatusType EVSEMechanicalChargingDeviceStatus;

};

} // namespace eviso15118::message_20
