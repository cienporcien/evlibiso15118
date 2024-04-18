// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/acds_disconnect.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_ACDS_Decoder.h>
#include <exi/cb/iso20_ACDS_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_acds_ACDS_DisconnectReqType& in, ACDS_DisconnectRequest& out) {
    convert(in.Header, out.header);
    cb_convert_enum(in.EVElectricalChargingDeviceStatus,out.EVElectricalChargingDeviceStatus);
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_acds_ACDS_DisconnectResType& in, ACDS_DisconnectResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    cb_convert_enum(in.EVSEProcessing, out.processing);
        
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const ACDS_DisconnectRequest& in, iso20_acds_ACDS_ConnectReqType& out) {
    init_iso20_acds_ACDS_ConnectReqType(&out);

    //RDB TODO Handle the various options in the request
    cb_convert_enum(in.EVElectricalChargingDeviceStatus,out.EVElectricalChargingDeviceStatus);

    convert(in.header, out.Header);

}


template <> void insert_type(VariantAccess& va, const struct iso20_acds_ACDS_DisconnectReqType& in) {
    va.insert_type<ACDS_DisconnectRequest>(in);
}

//RBL handle the response.
//This is difficult here, since an iso20_acds_ACDS_DisconnectResType doesn't exist since Connect and
//Disconnect share the same EXI struct.
template <> void insert_type(VariantAccess& va, const struct iso20_acds_ACDS_DisconnectResType& in) {
    va.insert_type<ACDS_DisconnectResponse>(in);
};


template <> void convert(const ACDS_DisconnectResponse& in, struct iso20_acds_ACDS_ConnectResType& out) {
    init_iso20_acds_ACDS_ConnectResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.processing, out.EVSEProcessing);
    cb_convert_enum(in.processing, out.EVSEProcessing);
    cb_convert_enum(in.EVSEElectricalChargingDeviceStatus,out.EVSEElectricalChargingDeviceStatus);
    cb_convert_enum(in.EVSEMechanicalChargingDeviceStatus,out.EVSEMechanicalChargingDeviceStatus);
}

template <> int serialize_to_exi(const ACDS_DisconnectResponse& in, exi_bitstream_t& out) {
    iso20_acds_exiDocument doc;
    init_iso20_acds_exiDocument(&doc);

    CB_SET_USED(doc.ACDS_DisconnectRes);

    convert(in, doc.ACDS_DisconnectRes);

    return encode_iso20_acds_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const ACDS_DisconnectRequest& in, exi_bitstream_t& out) {
    iso20_acds_exiDocument doc;
    init_iso20_acds_exiDocument(&doc);

    CB_SET_USED(doc.ACDS_DisconnectReq);

    convert(in, doc.ACDS_DisconnectReq);

    return encode_iso20_acds_exiDocument(&out, &doc);
}


template <> size_t serialize(const ACDS_DisconnectResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const ACDS_DisconnectRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}


} // namespace eviso15118::message_20
