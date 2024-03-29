if (TARGET fsm)
    # FIXME (aw): need to get proper way to deal with dependencies out of edm ...
else()
    message(STATUS "Target for libfsm not found, trying sibling folder ../libfsm")
    add_subdirectory(${PROJECT_SOURCE_DIR}/../libfsm ${CMAKE_CURRENT_BINARY_DIR}/libfsm)
endif()

find_package(Threads REQUIRED)

add_library(eviso15118)
add_library(eviso15118::eviso15118 ALIAS eviso15118)
target_include_directories(eviso15118
    PUBLIC
        $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
)

target_sources(eviso15118
    PRIVATE
        misc/helper.cpp
        misc/cb_exi.cpp

        io/connection_ssl.cpp
        io/connection_plain.cpp
        io/logging.cpp
        io/poll_manager.cpp
        io/sdp_packet.cpp
        io/sdp_server.cpp
        io/socket_helper.cpp

        session/feedback.cpp
        session/iso.cpp
        session/logger.cpp

        session_d2/feedback.cpp
        session_d2/iso.cpp
        session_d2/logger.cpp

        session_d2_sap/feedback.cpp
        session_d2_sap/iso.cpp
        session_d2_sap/logger.cpp

        
        d20/fsm.cpp
        d20/context.cpp
        d20/context_helper.cpp
        d20/control_event_queue.cpp
        d20/session.cpp
        d20/config.cpp

        d20/state/supported_app_protocol.cpp
        d20/state/session_setup.cpp
        d20/state/authorization_setup.cpp
        d20/state/authorization.cpp
        d20/state/service_discovery.cpp
        d20/state/service_detail.cpp
        d20/state/service_selection.cpp
        d20/state/dc_charge_parameter_discovery.cpp
        d20/state/schedule_exchange.cpp
        d20/state/dc_cable_check.cpp
        d20/state/dc_pre_charge.cpp
        d20/state/power_delivery.cpp
        d20/state/dc_charge_loop.cpp
        d20/state/dc_welding_detection.cpp
        d20/state/session_stop.cpp

        message/variant.cpp
        message/supported_app_protocol.cpp
        message/session_setup.cpp
        message/common.cpp
        message/authorization_setup.cpp
        message/authorization.cpp
        message/service_discovery.cpp
        message/service_detail.cpp
        message/service_selection.cpp
        message/dc_charge_parameter_discovery.cpp
        message/schedule_exchange.cpp
        message/dc_cable_check.cpp
        message/dc_pre_charge.cpp
        message/power_delivery.cpp
        message/dc_charge_loop.cpp
        message/dc_welding_detection.cpp
        message/session_stop.cpp

        d2/fsm.cpp
        d2/context.cpp
        d2/context_helper.cpp
        d2/control_event_queue.cpp
        d2/session.cpp
        d2/config.cpp

        d2_sap/fsm.cpp
        d2_sap/context.cpp
        d2_sap/context_helper.cpp
        d2_sap/control_event_queue.cpp
        d2_sap/session.cpp
        d2_sap/config.cpp

        d2/state/supported_app_protocol.cpp
        d2/state/session_setup.cpp
        d2/state/authorization_setup.cpp
        d2/state/authorization.cpp
        d2/state/service_discovery.cpp
        d2/state/service_detail.cpp
        d2/state/service_selection.cpp
        d2/state/dc_charge_parameter_discovery.cpp
        d2/state/schedule_exchange.cpp
        d2/state/dc_cable_check.cpp
        d2/state/dc_pre_charge.cpp
        d2/state/power_delivery.cpp
        d2/state/dc_charge_loop.cpp
        d2/state/dc_welding_detection.cpp
        d2/state/session_stop.cpp

        d2_sap/state/supported_app_protocol.cpp

        message_d2/variant.cpp
        message_d2/supported_app_protocol.cpp
        message_d2/session_setup.cpp
        message_d2/common.cpp
        message_d2/authorization_setup.cpp
        message_d2/authorization.cpp
        message_d2/service_discovery.cpp
        message_d2/service_detail.cpp
        message_d2/service_selection.cpp
        message_d2/dc_charge_parameter_discovery.cpp
        message_d2/schedule_exchange.cpp
        message_d2/dc_cable_check.cpp
        message_d2/dc_pre_charge.cpp
        message_d2/power_delivery.cpp
        message_d2/dc_charge_loop.cpp
        message_d2/dc_welding_detection.cpp
        message_d2/session_stop.cpp

        message_d2_sap/variant.cpp
        message_d2_sap/supported_app_protocol.cpp

        tbd_controller.cpp
        tbd_controller_d2.cpp    
        tbd_controller_d2_sap.cpp
)

if (NOT ISO15118_LINK_CUSTOM_MBEDTLS)
    set(MBEDTLS_LINK_DEPENDENCIES
        mbedtls
        mbedx509
        mbedcrypto
    )
else ()
    set(MBEDTLS_LINK_DEPENDENCIES
        mbedtls_custom
        mbedx509_custom
        mbedcrypto_custom
    )
endif ()

target_link_libraries(eviso15118
    PUBLIC
        # FIXME (aw): would be nice if we could make this private!
        fsm::fsm

        ${MBEDTLS_LINK_DEPENDENCIES}


    PRIVATE
        Threads::Threads
        # RDB - get access to the existing exi libraries in libiso15118 
        cb_v2gtp
        cb_iso20

)

# FIXME (aw): do we want to have this public here?
target_compile_features(eviso15118 PUBLIC cxx_std_17)
