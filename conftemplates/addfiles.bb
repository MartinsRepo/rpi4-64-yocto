LICENSE="CLOSED"
LIC_FILES_CHKSUM=""

SECTION = "rootfs"

SRC_URI += " \
    file://libtensorflowlite_c.so \
"

FILESPATH = "${FILE_DIRNAME}/files"

ROOTFS_POSTPROCESS_COMMAND += "modify_filesystem"


INSANE_SKIP:${PN} = "ldflags"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
SOLIBS = ".so"
FILES_SOLIBSDEV = ""

modify_filesystem() {
   rm ${IMAGE_ROOTFS}$${D}${libdir}/cmake/opencv4/OpenCVModules.cmake
}

do_install () {
        install -d ${D}${libdir}
        install -m 0755 ${FILESPATH}/libtensorflowlite_c.so ${D}${libdir}
        install -d ${D}${libdir}/cmake/opencv4
        install -m 0755 ${FILESPATH}/OpenCVModules.cmake ${D}${libdir}/cmake/opencv4
}


