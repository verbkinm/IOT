cmake_minimum_required(VERSION 3.16)
project(iotv_client VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Set up AUTOMOC and some sensible defaults for runtime execution
# When using Qt 6.3, you can replace the code block below with
# qt_standard_project_setup()
set(CMAKE_AUTOMOC ON)
include(GNUInstallDirs)

find_package(QT NAMES Qt5 Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Charts Concurrent Gui Multimedia Quick)

qt_add_executable(IOTV_Client_QML WIN32 MACOSX_BUNDLE
    ../lib/Base_Host/base_host.cpp ../lib/Base_Host/base_host.h
    ../lib/Base_Host/channel.cpp ../lib/Base_Host/channel.h
    ../lib/ConfigType/ConfigTypes.cpp ../lib/ConfigType/ConfigTypes.h
    ../lib/Events/actions/iotv_action.cpp ../lib/Events/actions/iotv_action.h
    ../lib/Events/actions/iotv_action_data_tx.cpp ../lib/Events/actions/iotv_action_data_tx.h
    ../lib/Events/actions/iotv_action_data_tx_ref.cpp ../lib/Events/actions/iotv_action_data_tx_ref.h
    ../lib/Events/actions/iotv_action_msg.cpp ../lib/Events/actions/iotv_action_msg.h
    ../lib/Events/events/iotv_event.cpp ../lib/Events/events/iotv_event.h
    ../lib/Events/events/iotv_event_alarm.cpp ../lib/Events/events/iotv_event_alarm.h
    ../lib/Events/events/iotv_event_connect.cpp ../lib/Events/events/iotv_event_connect.h
    ../lib/Events/events/iotv_event_data.cpp ../lib/Events/events/iotv_event_data.h
    ../lib/Events/events/iotv_event_disconnect.cpp ../lib/Events/events/iotv_event_disconnect.h
    ../lib/Events/events/iotv_event_state.cpp ../lib/Events/events/iotv_event_state.h
    ../lib/Events/events/iotv_event_timer.cpp ../lib/Events/events/iotv_event_timer.h
    ../lib/Events/iotv_event_manager.cpp ../lib/Events/iotv_event_manager.h
    ../lib/Json_Parsers/event_action_parser.cpp ../lib/Json_Parsers/event_action_parser.h
    ../lib/Log/log.cpp ../lib/Log/log.h
    ../lib/Protocols/embedded/IOTV_SH.c ../lib/Protocols/embedded/IOTV_SH.h
    ../lib/Protocols/embedded/creatorpkgs.c ../lib/Protocols/embedded/creatorpkgs.h
    ../lib/Protocols/embedded/header.c ../lib/Protocols/embedded/header.h
    ../lib/Protocols/embedded/host_broadcast.c ../lib/Protocols/embedded/host_broadcast.h
    ../lib/Protocols/embedded/identification.c ../lib/Protocols/embedded/identification.h
    ../lib/Protocols/embedded/iotv_server_embedded.c ../lib/Protocols/embedded/iotv_server_embedded.h
    ../lib/Protocols/embedded/log_data.c ../lib/Protocols/embedded/log_data.h
    ../lib/Protocols/embedded/read_write.c ../lib/Protocols/embedded/read_write.h
    ../lib/Protocols/embedded/state.c ../lib/Protocols/embedded/state.h
    ../lib/Protocols/embedded/tech.c ../lib/Protocols/embedded/tech.h
    ../lib/raw/raw.cpp ../lib/raw/raw.h
    client.cpp client.h
    device.cpp device.h
    generalaudiooutput.cpp generalaudiooutput.h
    main.cpp
    producer.cpp producer.h
    wrap_qbytearray.cpp wrap_qbytearray.h
)
target_include_directories(IOTV_Client_QML PRIVATE
    ../lib/Base_Host
    ../lib/ConfigType
    ../lib/Events
    ../lib/Json_Parsers
    ../lib/Log
    ../lib/Protocols
    ../lib/Protocols/embedded
    ../lib/raw
)

target_link_libraries(IOTV_Client_QML PRIVATE
    Qt::Charts
    Qt::Concurrent
    Qt::Core
    Qt::Gui
    Qt::Multimedia
    Qt::Quick
)

install(TARGETS IOTV_Client_QML
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Consider using qt_generate_deploy_app_script() for app deployment if
# the project can use Qt 6.3. In that case rerun qmake2cmake with
# --min-qt-version=6.3.
