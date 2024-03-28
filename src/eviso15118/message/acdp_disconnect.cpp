// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/acdp_disconnect.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_ACDP_Decoder.h>
#include <exi/cb/iso20_ACDP_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_acdp_ACDP_DisconnectReqType& in, ACDP_DisconnectRequest& out) {
    convert(in.Header, out.header);
    cb_convert_enum(in.EVElectricalChargingDeviceStatus,out.EVElectricalChargingDeviceStatus);
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_acdp_ACDP_DisconnectResType& in, ACDP_DisconnectResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);
    cb_convert_enum(in.EVSEProcessing, out.processing);
        
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const ACDP_DisconnectRequest& in, iso20_acdp_ACDP_ConnectReqType& out) {
    init_iso20_acdp_ACDP_ConnectReqType(&out);

    //RDB TODO Handle the various options in the request
    cb_convert_enum(in.EVElectricalChargingDeviceStatus,out.EVElectricalChargingDeviceStatus);

    convert(in.header, out.Header);

}


template <> void insert_type(VariantAccess& va, const struct iso20_acdp_ACDP_DisconnectReqType& in) {
    va.insert_type<ACDP_DisconnectRequest>(in);
}

//RBL handle the response.
//This is difficult here, since an iso20_acdp_ACDP_DisconnectResType doesn't exist since Connect and
//Disconnect share the same EXI struct.
template <> void insert_type(VariantAccess& va, const struct iso20_acdp_ACDP_DisconnectResType& in) {
    va.insert_type<ACDP_DisconnectResponse>(in);
};


template <> void convert(const ACDP_DisconnectResponse& in, struct iso20_acdp_ACDP_ConnectResType& out) {
    init_iso20_acdp_ACDP_ConnectResType(&out);
    convert(in.header, out.Header);
    cb_convert_enum(in.response_code, out.ResponseCode);
    cb_convert_enum(in.processing, out.EVSEProcessing);
    cb_convert_enum(in.processing, out.EVSEProcessing);
    cb_convert_enum(in.EVSEElectricalChargingDeviceStatus,out.EVSEElectricalChargingDeviceStatus);
    cb_convert_enum(in.EVSEMechanicalChargingDeviceStatus,out.EVSEMechanicalChargingDeviceStatus);
}

template <> int serialize_to_exi(const ACDP_DisconnectResponse& in, exi_bitstream_t& out) {
    iso20_acdp_exiDocument doc;
    init_iso20_acdp_exiDocument(&doc);

    CB_SET_USED(doc.ACDP_DisconnectRes);

    convert(in, doc.ACDP_DisconnectRes);

    return encode_iso20_acdp_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const ACDP_DisconnectRequest& in, exi_bitstream_t& out) {
    iso20_acdp_exiDocument doc;
    init_iso20_acdp_exiDocument(&doc);

    CB_SET_USED(doc.ACDP_DisconnectReq);

    convert(in, doc.ACDP_DisconnectReq);

    return encode_iso20_acdp_exiDocument(&out, &doc);
}


template <> size_t serialize(const ACDP_DisconnectResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const ACDP_DisconnectRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}


} // namespace eviso15118::message_20
