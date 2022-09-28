LICENSE="CLOSED"
LIC_FILES_CHKSUM=""

SRC_URI += " \
    file://libtensorflowlite_c.so \
    "

FILESPATH = "${FILE_DIRNAME}/files"

INSANE_SKIP:${PN} = "ldflags"
INHIBIT_PACKAGE_STRIP = "1"
INHIBIT_SYSROOT_STRIP = "1"
SOLIBS = ".so "
FILES_SOLIBSDEV = ""

do_install () {
        install -d ${D}${libdir}
        install -m 0755 ${FILESPATH}/libtensorflowlite_c.so ${D}${libdir}
        #install -m 0755  -d ${D}${libdir}/opencv4/3rdparty
        #install -m 0755 ${FILESPATH}/libcorrespondence.a ${D}${libdir}/opencv4/3rdparty
        #cp --preserve=mode,links -v ${FILESPATH}/libcorrespondence.a ${D}${libdir}/opencv4/3rdparty
}
#FILES:${PN}= " \
#            /usr/lib \
#            " 

#FILES:${PN} = "{libdir}/* {libdir}/opencv4/3rdparty/* "
#FILES:${PN} = "/usr/lib/opencv4/3rdparty/libcorrespondence.a"


