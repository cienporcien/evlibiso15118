// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <eviso15118/message/service_discovery.hpp>

#include <type_traits>

#include <eviso15118/detail/variant_access.hpp>

#include <exi/cb/iso20_CommonMessages_Encoder.h>

namespace eviso15118::message_20 {

template <> void convert(const struct iso20_ServiceDiscoveryReqType& in, ServiceDiscoveryRequest& out) {
    convert(in.Header, out.header);

    if (in.SupportedServiceIDs_isUsed == true) {
        auto& temp = out.supported_service_ids.emplace();
        temp.insert(temp.end(), in.SupportedServiceIDs.ServiceID.array,
                    in.SupportedServiceIDs.ServiceID.array + in.SupportedServiceIDs.ServiceID.arrayLen);
    }
}

//RBL Add the conversion for the response from the EVCC
template <> void convert(const struct iso20_ServiceDiscoveryResType& in, ServiceDiscoveryResponse& out) {

    cb_convert_enum(in.ResponseCode, out.response_code);

    //RDB TODO Handle the various options in the response
    
    convert(in.Header, out.header);
   
}

//RDB Add conversion for the request to convert to exi
template <> void convert(const ServiceDiscoveryRequest& in, iso20_ServiceDiscoveryReqType& out) {
    init_iso20_ServiceDiscoveryReqType(&out);

    //RDB TODO Handle the various options in the request

    convert(in.header, out.Header);

}


template <> void convert(const ServiceDiscoveryResponse& in, iso20_ServiceDiscoveryResType& out) {
    init_iso20_ServiceDiscoveryResType(&out);

    cb_convert_enum(in.response_code, out.ResponseCode);
    out.ServiceRenegotiationSupported = in.service_renegotiation_supported;

    uint8_t index = 0;
    for (auto const& service : in.energy_transfer_service_list) {
        auto& out_service = out.EnergyTransferServiceList.Service.array[index++];
        cb_convert_enum(service.service_id, out_service.ServiceID);
        out_service.FreeService = service.free_service;
    }
    out.EnergyTransferServiceList.Service.arrayLen = in.energy_transfer_service_list.size();

    if (in.vas_list) {
        index = 0;
        for (auto const& service : *in.vas_list) {
            auto& out_service = out.VASList.Service.array[index++];
            cb_convert_enum(service.service_id, out_service.ServiceID);
            out_service.FreeService = service.free_service;
        }
        out.VASList.Service.arrayLen = in.vas_list.value().size();
        CB_SET_USED(out.VASList);
    }

    convert(in.header, out.Header);
}

template <> void insert_type(VariantAccess& va, const struct iso20_ServiceDiscoveryReqType& in) {
    va.insert_type<ServiceDiscoveryRequest>(in);
};

//RBL handle the response
template <> void insert_type(VariantAccess& va, const struct iso20_ServiceDiscoveryResType& in) {
    va.insert_type<ServiceDiscoveryResponse>(in);
};

template <> int serialize_to_exi(const ServiceDiscoveryResponse& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceDiscoveryRes);

    convert(in, doc.ServiceDiscoveryRes);

    return encode_iso20_exiDocument(&out, &doc);
}

//RDB output the request
template <> int serialize_to_exi(const ServiceDiscoveryRequest& in, exi_bitstream_t& out) {
    iso20_exiDocument doc;
    init_iso20_exiDocument(&doc);

    CB_SET_USED(doc.ServiceDiscoveryReq);

    convert(in, doc.ServiceDiscoveryReq);

    return encode_iso20_exiDocument(&out, &doc);
}

template <> size_t serialize(const ServiceDiscoveryResponse& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

//RDB output the request
template <> size_t serialize(const ServiceDiscoveryRequest& in, const io::StreamOutputView& out) {
    return serialize_helper(in, out);
}

} // namespace eviso15118::message_20
