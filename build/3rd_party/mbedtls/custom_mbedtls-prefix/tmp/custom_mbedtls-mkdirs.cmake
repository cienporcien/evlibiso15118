# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/src/custom_mbedtls"
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-build"
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix"
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/tmp"
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/src/custom_mbedtls-stamp"
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/src"
  "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/src/custom_mbedtls-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/src/custom_mbedtls-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/opbrid/checkout/everest_main/libiso15118/build/3rd_party/mbedtls/custom_mbedtls-prefix/src/custom_mbedtls-stamp${cfgdir}") # cfgdir has leading slash
endif()
