LICENSE="CLOSED"
LIC_FILES_CHKSUM=""

INSANE_SKIP.${PN} += "already-stripped"

LIC_FILES_CHKSUM = "file://LICENSE;md5=a77c327d4d1da3707d42dde9725d4769"

#FILESEXTRAPATHS:prepend := "${THISDIR}:"
FILESPATH = "${FILE_DIRNAME}/files"

#SRC_URI += " file://files/ "

do_install() {
    install -m 0755 -d ${D}${servicedir}/dmap-het-core/
    #cp --preserve=mode,timestamps -R ${FILESPATH}/* ${D}/dmap-het-core/
    cp -R --no-dereference --preserve=mode,links -v ${THISDIR}/files/* ${D}/${servicedir}/dmap-het-core/
    #cp -r ${FILESPATH}/* ${D}${servicedir}/dmap-het-core/
    #cp -r ${WORKDIR}/xxx/yyy/zzz ${D}${sysconfdir}/xxx/yyy
    #install -m 0755 -d=/home/martin/workspace/Yocto/rpi4-64/meta-algo/recipes-algo/algo/files/* ${D}${servicedir}/dmap-het-core/
}

FILES:${PN} += " \
    ${servicedir}/dmap-het-core/* \
"
