if(NOT VCPKG_TARGET_IS_LINUX AND NOT VCPKG_TARGET_IS_OSX)
    message(FATAL_ERROR "libcargs currently only supports Linux and macOS platforms.")
endif()

vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO lucocozz/cargs
    REF v${VERSION}
    SHA512 74bb7eebb1f328d3b27c51c2cec74f87b0a11fd7c801bb3d2d4b56b8ed7448461043f8f01b68f69cee67c2c73217ebb8b641a5e76a632052910ddb13b750045f
    HEAD_REF main
)

vcpkg_find_acquire_program(MESON)
message(STATUS "Using Meson: ${MESON}")

vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
    regex WITH_REGEX
)

set(OPTIONS -Dtests=false -Dexamples=false -Dbenchmarks=false)
if(NOT "regex" IN_LIST FEATURES)
    list(APPEND OPTIONS -Ddisable_regex=true)
endif()

vcpkg_configure_meson(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS ${OPTIONS}
)

vcpkg_install_meson()
vcpkg_fixup_pkgconfig()

file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}" RENAME copyright)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/share")
