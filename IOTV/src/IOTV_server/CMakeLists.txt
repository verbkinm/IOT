cmake_minimum_required(VERSION 3.16)
project(iotv_server VERSION 0.24 LANGUAGES C CXX)
#set(PROJECT_NAME ${PROJECT_NAME}_${PROJECT_VERSION})
#project(iotv_server_${PROJECT_VERSION})

#message("PROJECT_NAME: " ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build/release)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network SerialPort)

#-> tgbot-cpp
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)

set(Boost_USE_MULTITHREADED ON)

include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()
#<- tgbot-cpp

configure_file(config.h.in config.h)

#qt_standard_project_setup()

add_executable(${PROJECT_NAME}
    ../lib/Base_Host/base_host.cpp                      ../lib/Base_Host/base_host.h
    ../lib/Base_Host/channel.cpp                        ../lib/Base_Host/channel.h
    ../lib/ConfigType/ConfigTypes.cpp                   ../lib/ConfigType/ConfigTypes.h
    ../lib/Events/actions/iotv_action.cpp               ../lib/Events/actions/iotv_action.h
    ../lib/Events/actions/iotv_action_data_tx.cpp       ../lib/Events/actions/iotv_action_data_tx.h
    ../lib/Events/actions/iotv_action_data_tx_ref.cpp   ../lib/Events/actions/iotv_action_data_tx_ref.h
    ../lib/Events/actions/iotv_action_msg.cpp           ../lib/Events/actions/iotv_action_msg.h
    ../lib/Events/events/iotv_event.cpp                 ../lib/Events/events/iotv_event.h
    ../lib/Events/events/iotv_event_alarm.cpp           ../lib/Events/events/iotv_event_alarm.h
    ../lib/Events/events/iotv_event_connect.cpp         ../lib/Events/events/iotv_event_connect.h
    ../lib/Events/events/iotv_event_data.cpp            ../lib/Events/events/iotv_event_data.h
    ../lib/Events/events/iotv_event_disconnect.cpp      ../lib/Events/events/iotv_event_disconnect.h
    ../lib/Events/events/iotv_event_state.cpp           ../lib/Events/events/iotv_event_state.h
    ../lib/Events/events/iotv_event_timer.cpp           ../lib/Events/events/iotv_event_timer.h
    ../lib/Events/iotv_event_manager.cpp                ../lib/Events/iotv_event_manager.h
    ../lib/Json_Parsers/event_action_parser.cpp         ../lib/Json_Parsers/event_action_parser.h
    ../lib/Log/log.cpp                                  ../lib/Log/log.h
    ../lib/Protocols/embedded/IOTV_SH.c                 ../lib/Protocols/embedded/IOTV_SH.h
    ../lib/Protocols/embedded/creatorpkgs.c             ../lib/Protocols/embedded/creatorpkgs.h
    ../lib/Protocols/embedded/header.c                  ../lib/Protocols/embedded/header.h
    ../lib/Protocols/embedded/host_broadcast.c          ../lib/Protocols/embedded/host_broadcast.h
    ../lib/Protocols/embedded/identification.c          ../lib/Protocols/embedded/identification.h
    ../lib/Protocols/embedded/iotv_server_embedded.c    ../lib/Protocols/embedded/iotv_server_embedded.h
    ../lib/Protocols/embedded/log_data.c                ../lib/Protocols/embedded/log_data.h
    ../lib/Protocols/embedded/read_write.c              ../lib/Protocols/embedded/read_write.h
    ../lib/Protocols/embedded/state.c                   ../lib/Protocols/embedded/state.h
    ../lib/Protocols/embedded/tech.c                    ../lib/Protocols/embedded/tech.h
    ../lib/ThreadPool/thread_pool.cpp                   ../lib/ThreadPool/thread_pool.h
    ../lib/ThreadPool/thread_safe_queue.cpp             ../lib/ThreadPool/thread_safe_queue.h
    ../lib/raii/raii_header.cpp                         ../lib/raii/raii_header.h
    ../lib/raii/raii_iot.cpp                            ../lib/raii/raii_iot.h
    ../lib/raw/raw.cpp                                  ../lib/raw/raw.h
    ../lib/fragmentData/fragmentcollector.cpp           ../lib/fragmentData/fragmentcollector.h
    ../lib/fragmentData/fragmentcollector_identification.cpp ../lib/fragmentData/fragmentcollector_identification.h
    ../lib/fragmentData/fragmentcollector_read.cpp      ../lib/fragmentData/fragmentcollector_read.h
    IOTV_Client/iotv_client.cpp                         IOTV_Client/iotv_client.h
    IOTV_Host/iotv_host.cpp                             IOTV_Host/iotv_host.h
    IOTV_Server/iotv_server.cpp                         IOTV_Server/iotv_server.h
    IOTV_Server/maker_iotv.cpp                          IOTV_Server/maker_iotv.h
    connection_type/base_conn_type.cpp                  connection_type/base_conn_type.h
    connection_type/com_conn_type.cpp                   connection_type/com_conn_type.h
    connection_type/file_conn_type.cpp                  connection_type/file_conn_type.h
    connection_type/tcp_conn_type.cpp                   connection_type/tcp_conn_type.h
    connection_type/tcp_reverse_conn_type.cpp           connection_type/tcp_reverse_conn_type.h
    connection_type/udp_conn_type.cpp                   connection_type/udp_conn_type.h
    main.cpp
    wrapper.cpp                                         wrapper.h
    IOTV_Bot/iotv_bot.cpp                               IOTV_Bot/iotv_bot.h
    IOTV_Server/eraser_iotv.cpp                         IOTV_Server/eraser_iotv.h
    IOTV_Server/config_server.cpp                       IOTV_Server/config_server.h
    IOTV_Server/tg_bot_handler.cpp                      IOTV_Server/tg_bot_handler.h
)

target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}") #Что бы видел config.h

target_include_directories(${PROJECT_NAME} PRIVATE
    ../lib/Base_Host
    ../lib/ConfigType
    ../lib/Events
    ../lib/Json_Parsers
    ../lib/Log
    ../lib/Protocols
    ../lib/Protocols/embedded
    ../lib/ThreadPool
    ../lib/raii
    ../lib/raw
    ../lib/fragmentData
)

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt::Core
    Qt::Network
    Qt::SerialPort

    #tgbot-cpp
    /usr/local/lib/libTgBot.a
    ${CMAKE_THREAD_LIBS_INIT}
    ${OPENSSL_LIBRARIES}
    ${Boost_LIBRARIES}
    ${CURL_LIBRARIES}
    #tgbot-cpp
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

#qt_generate_deploy_app_script(
#    TARGET ${PROJECT_NAME}
#    FILENAME_VARIABLE deploy_script
#    NO_UNSUPPORTED_PLATFORM_ERROR
#)
