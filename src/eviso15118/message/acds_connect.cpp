// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/acds_connect.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_ACDS_Decoder.h>
#include <exi/cb/iso20_ACDS_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_acds_ACDS_ConnectReqType& in, ACDS_ConnectRequest& out) {
    convert(in.Header, out.header);
    cb_convert_enum(in.EVElectricalChargingDeviceStatus,out.EVElectricalChargingDeviceStatus);
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_acds_ACDS_ConnectResType& in, ACDS_ConnectResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    cb_convert_enum(in.EVSEProcessing, out.processing);
        
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const ACDS_ConnectRequest& in, iso20_acds_ACDS_ConnectReqType& out) {
    init_iso20_acds_ACDS_ConnectReqType(&out);

    //RDB TODO Handle the various options in the request
    cb_convert_enum(in.EVElectricalChargingDeviceStatus,out.EVElectricalChargingDeviceStatus);

    convert(in.header, out.Header);

}


template <> void insert_type(VariantAccess& va, const struct iso20_acds_ACDS_ConnectReqType& in) {
    va.insert_type<ACDS_ConnectRequest>(in);
}

//RBL handle the response
template <> void insert_type(VariantAccess& va, const struct iso20_acds_ACDS_ConnectResType& in) {
    va.insert_type<ACDS_ConnectResponse>(in);
};


template <> void convert(const ACDS_ConnectResponse& in, struct iso20_acds_ACDS_ConnectResType& out) {
    init_iso20_acds_ACDS_ConnectResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.processing, out.EVSEProcessing);
    cb_convert_enum(in.EVSEElectricalChargingDeviceStatus,out.EVSEElectricalChargingDeviceStatus);
    cb_convert_enum(in.EVSEMechanicalChargingDeviceStatus,out.EVSEMechanicalChargingDeviceStatus);
}

template <> int serialize_to_exi(const ACDS_ConnectResponse& in, exi_bitstream_t& out) {
    iso20_acds_exiDocument doc;
    init_iso20_acds_exiDocument(&doc);

    CB_SET_USED(doc.ACDS_ConnectRes);

    convert(in, doc.ACDS_ConnectRes);

    return encode_iso20_acds_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const ACDS_ConnectRequest& in, exi_bitstream_t& out) {
    iso20_acds_exiDocument doc;
    init_iso20_acds_exiDocument(&doc);

    CB_SET_USED(doc.ACDS_ConnectReq);

    convert(in, doc.ACDS_ConnectReq);

    return encode_iso20_acds_exiDocument(&out, &doc);
}


template <> size_t serialize(const ACDS_ConnectResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const ACDS_ConnectRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}


} // namespace eviso15118::message_20
