// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/variant.hpp>

#include <cassert>
#include <string>

#include <eviso15118/detail/helper.hpp>
#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/appHand_Decoder.h>
#include <exi/cb/iso20_CommonMessages_Decoder.h>
#include <exi/cb/iso20_DC_Decoder.h>
#include <exi/cb/iso20_ACDP_Decoder.h>

using PayloadType = eviso15118::io::v2gtp::PayloadType;

namespace eviso15118::message_20 {

static void handle_sap(VariantAccess& va) {
    appHand_exiDocument doc;

    const auto decode_status = decode_appHand_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_appHand_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.supportedAppProtocolRes_isUsed) {
        insert_type(va, doc.supportedAppProtocolRes);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_main(VariantAccess& va) {
    iso20_exiDocument doc;

    const auto decode_status = decode_iso20_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.SessionSetupRes_isUsed) {
        insert_type(va, doc.SessionSetupRes);
     } else if (doc.AuthorizationSetupRes_isUsed) {
        insert_type(va, doc.AuthorizationSetupRes);
     } else if (doc.AuthorizationRes_isUsed) {
        insert_type(va, doc.AuthorizationRes);
     } else if (doc.ServiceDiscoveryRes_isUsed) {
        insert_type(va, doc.ServiceDiscoveryRes);
     } else if (doc.ServiceDetailRes_isUsed) {
         insert_type(va, doc.ServiceDetailRes);
     } else if (doc.ServiceSelectionRes_isUsed) {
         insert_type(va, doc.ServiceSelectionRes);
     } else if (doc.ScheduleExchangeRes_isUsed) {
         insert_type(va, doc.ScheduleExchangeRes);
     } else if (doc.PowerDeliveryRes_isUsed) {
         insert_type(va, doc.PowerDeliveryRes);
     } else if (doc.SessionStopRes_isUsed) {
         insert_type(va, doc.SessionStopRes);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_dc(VariantAccess& va) {
    iso20_dc_exiDocument doc;

    const auto decode_status = decode_iso20_dc_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_dc_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.DC_ChargeParameterDiscoveryRes_isUsed) {
        insert_type(va, doc.DC_ChargeParameterDiscoveryRes);
     } else if (doc.DC_CableCheckRes_isUsed) {
         insert_type(va, doc.DC_CableCheckRes);
     } else if (doc.DC_PreChargeRes_isUsed) {
         insert_type(va, doc.DC_PreChargeRes);
     } else if (doc.DC_ChargeLoopRes_isUsed) {
         insert_type(va, doc.DC_ChargeLoopRes);
     } else if (doc.DC_WeldingDetectionRes_isUsed) {
         insert_type(va, doc.DC_WeldingDetectionRes);
    } else {
        va.error = "chosen message type unhandled";
    }
}

static void handle_acdp(VariantAccess& va) {
    iso20_acdp_exiDocument doc;

    const auto decode_status = decode_iso20_acdp_exiDocument(&va.input_stream, &doc);

    if (decode_status != 0) {
        va.error = "decode_iso20_acdp_exiDocument failed with " + std::to_string(decode_status);
        return;
    }

    if (doc.ACDP_VehiclePositioningRes_isUsed) {
        insert_type(va, doc.ACDP_VehiclePositioningRes);
    } else if (doc.ACDP_ConnectRes_isUsed) {
        insert_type(va, doc.ACDP_ConnectRes);
    } else if (doc.ACDP_DisconnectRes_isUsed) {
        // RDB this is a little tricky here. The type of doc.ACDP_DisconnectRes is exactly the same as doc.ACDP_ConnectRes
        //     struct iso20_acdp_exiDocument {
        // union {
        //     struct iso20_acdp_ACDP_VehiclePositioningReqType ACDP_VehiclePositioningReq;
        //     struct iso20_acdp_ACDP_VehiclePositioningResType ACDP_VehiclePositioningRes;
        //     struct iso20_acdp_ACDP_ConnectReqType ACDP_ConnectReq;
        //     struct iso20_acdp_ACDP_ConnectResType ACDP_ConnectRes;
        //     struct iso20_acdp_ACDP_ConnectReqType ACDP_DisconnectReq;
        //     struct iso20_acdp_ACDP_ConnectResType ACDP_DisconnectRes;
        // This means that it is impossible to differentiate between the two based on the type (which is what this does)
        // e.g.
        // template <> void insert_type(VariantAccess& va, const struct iso20_acdp_ACDP_ConnectReqType& in) {
        //     va.insert_type<ACDP_DisconnectRequest>(in);
        // } is the same as:
        // template <> void insert_type(VariantAccess& va, const struct iso20_acdp_ACDP_ConnectReqType& in) {
        //     va.insert_type<ACDP_ConnectRequest>(in);
        // }
        // and we get a duplicate function error when trying to link. 

        // To fix this, create a new type called 
        //struct iso20_acdp_ACDP_DisconnectResType and copy everything to it and try again.
        struct iso20_acdp_ACDP_DisconnectResType DT;
        DT.EVSEElectricalChargingDeviceStatus = doc.ACDP_DisconnectRes.EVSEElectricalChargingDeviceStatus;
        DT.EVSEMechanicalChargingDeviceStatus = doc.ACDP_DisconnectRes.EVSEMechanicalChargingDeviceStatus;
        DT.EVSEProcessing = doc.ACDP_DisconnectRes.EVSEProcessing;
        DT.Header = doc.ACDP_DisconnectRes.Header;
        DT.ResponseCode = doc.ACDP_DisconnectRes.ResponseCode;
        insert_type(va, DT);
    } else {
        va.error = "chosen message type unhandled";
    }
}

Variant::Variant(io::v2gtp::PayloadType payload_type, const io::StreamInputView& buffer_view) {

    VariantAccess va{
        get_exi_input_stream(buffer_view), this->data, this->type, this->custom_deleter, this->error,
    };

    if (payload_type == PayloadType::SAP) {
        handle_sap(va);
    } else if (payload_type == PayloadType::Part20Main) {
        handle_main(va);
    } else if (payload_type == PayloadType::Part20DC) {
        handle_dc(va);
    } else if (payload_type == PayloadType::Part20ACDP) {
        handle_acdp(va);        
    } else {
        logf("Unknown type\n");
    }

    if (data) {
        // in case data was set, make sure the custom deleter and the type were set!
        assert(custom_deleter != nullptr);
        assert(type != Type::None);
    } else {
        logf("Failed due to: %s\n", error.c_str());
    }
}

Variant::~Variant() {
    if (data) {
        custom_deleter(data);
    }
}

Type Variant::get_type() const {
    return type;
}

const std::string& Variant::get_error() const {
    return error;
}

} // namespace eviso15118::message_20
