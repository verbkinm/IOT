TEMPLATE = subdirs

SUBDIRS += \
#    src/IOTV_Tests_Server \
#    src/IOTV_Client_qml \
    src/IOTV_Client \
    src/lib \
    src/IOTV_host \
    src/IOTV_host_bme280 \
    src/IOTV_server

DISTFILES += \
    IOTV_Client.supp
