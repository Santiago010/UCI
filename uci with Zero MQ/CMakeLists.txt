cmake_minimum_required(VERSION 3.10)
project(uci)

set(CMAKE_CXX_STANDARD 11)

# Incluye los directorios de cabeceras

include_directories(/usr/include/uuid)
include_directories(/usr/include)
include_directories(/usr/include/openssl)
include_directories(/usr/include/apr-1.0)

# Define el ejecutable uci
add_executable(uci main.cpp tinyxml2.cpp)

# Enlaza las bibliotecas necesarias al ejecutable uci
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libnorm.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libprotokit.a)
target_link_libraries(uci  /usr/lib/x86_64-linux-gnu/libzmq.a
    -lgssapi_krb5
    -lnorm)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libpgm.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libsodium.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libapr-1.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libuuid.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libssl.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libgps.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libdbus-1.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libsystemd.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libcap.a)
target_link_libraries(uci /usr/lib/x86_64-linux-gnu/libcrypto.a)
