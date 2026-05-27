packages:=

boost_packages = boost

libevent_packages = libevent

# Alias-specific: Tor is statically bundled because Alias runs as a Tor
# hidden service. xz (liblzma) is a transitive dependency of Tor.
tor_packages = tor xz

protobuf_native_packages = native_protobuf
protobuf_packages = protobuf

qrencode_linux_packages = qrencode
qrencode_darwin_packages = qrencode
qrencode_mingw32_packages = qrencode

qt_linux_packages:=qt expat libxcb xcb_proto libXau xproto freetype fontconfig libxkbcommon libxcb_util libxcb_util_render libxcb_util_keysyms libxcb_util_image libxcb_util_wm
qt_darwin_packages=qt
qt_mingw32_packages=qt

bdb_packages=bdb
sqlite_packages=sqlite

zmq_packages=sodium zeromq

upnp_packages=miniupnpc
natpmp_packages=libnatpmp
usb_packages=libusb hidapi protobuf
usb_linux_packages=eudev
usb_native_packages=native_protobuf

multiprocess_packages = libmultiprocess capnp
multiprocess_native_packages = native_libmultiprocess native_capnp

usdt_linux_packages=systemtap
