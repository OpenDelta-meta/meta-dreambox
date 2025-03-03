# dreambox-arm.inc

KERNELVERSION = "3.14"

BOX_BRAND = "dreambox"

MTD_ROOTFS = "mmcblk0p2"
MTD_KERNEL = "mmcblk0p1"
KERNEL_FILE = "kernel.bin"
ROOTFS_FILE = "rootfs.tar.bz2"
MTD_BLACK  = "mmcblk0"

DISTRO_FEATURES_GFX_mipsel ?= " opengl"

PACKAGECONFIG_pn-glmark2 = "gles2"

PACKAGECONFIG_pn-cogl-1.0 = "egl-kms egl-null \
        ${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'gles2', '', d)}"

PACKAGECONFIG_pn-weston = "${@bb.utils.contains('DISTRO_FEATURES', 'opengl', 'gles', '', d)}"

DVBSKYPROVIDER = "kernel"

GLIBC_EXTRA_OECONF = "--with-tls --with-__thread"

DVBMEDIASINK_CONFIG = "--with-pcm --with-wma --with-wmv --with-dts=no --with-dtsdownmix --with-eac3 --with-vb6 --with-vb8 --with-vb9 --with-spark --with-h265"

require conf/machine/include/soc-family.inc
require conf/machine/include/tune-cortexa15.inc

MACHINE_ESSENTIAL_EXTRA_RDEPENDS += "\
	dreambox-dvb-modules-${MACHINE} \
	libpagecache \
	libpassthrough \
	flash-nrf52 \
	gstreamer1.0-plugin-dvbmediasink \
	gstreamer1.0-libav \
	kernel-module-hmac \
	kernel-module-md4 \
	kernel-module-snd-pcm-oss \
	"

DISTRO_FEATURES_remove += "directfb gtk-directfb"
    
MACHINE_FEATURES += "dreambox alsa usbhost switchoff blindscan-dvbs modem dvb-c blindscan-dvbc hdmicec recovery ci dvd usbconsole blindscan-tbs emmc mmc HDMI HDMI-IN-FHD"

PREFERRED_PROVIDER_virtual/kernel = "linux-dreambox"
PREFERRED_PROVIDER_virtual/blindscan-dvbs = "dreambox-blindscan-utils"
PREFERRED_PROVIDER_virtual/blindscan-dvbc = "dreambox-blindscan-utils"
PREFERRED_VERSION_linux-dreambox = "${KERNELVERSION}"
PREFERRED_PROVIDER_virtual/libgl ?= "mesa-gl"
PREFERRED_PROVIDER_virtual/mesa ?= "mesa-gl"

TARGET_ARCH = "arm"
DEFAULTTUNE = "cortexa15hf-neon-vfpv4"
OLDEST_KERNEL = "${KERNELVERSION}"

MACHINE_FEATURES_BACKFILL_CONSIDERED += "rtc"

DREAMBOX_IMAGE_SIZE ??= "1024"

IMAGE_FSTYPES =+ "tar.bz2"

IMAGE_CMD_tar_prepend = "\
	mkdir -p ${IMAGE_ROOTFS}/tmp; \
	rm -Rf ${IMAGE_ROOTFS}/tmp/*; \
	mkdir -p ${IMAGE_ROOTFS}${localstatedir}/lib/opkg/info; \
	mkdir -p ${DEPLOY_DIR_IMAGE}/${IMAGEDIR}; \
	cp ${DEPLOY_DIR_IMAGE}/zImage ${DEPLOY_DIR_IMAGE}/${IMAGEDIR}/${KERNEL_FILE}; \
	echo "${IMAGE_NAME}" > ${DEPLOY_DIR_IMAGE}/${IMAGEDIR}/imageversion; \
	cd ${IMAGE_ROOTFS}; \
	tar -cvf ${DEPLOY_DIR_IMAGE}/rootfs.tar -C ${IMAGE_ROOTFS} .; \
	mv ${DEPLOY_DIR_IMAGE}/rootfs.tar ${DEPLOY_DIR_IMAGE}/${IMAGEDIR}/rootfs.tar; \
	bzip2 ${DEPLOY_DIR_IMAGE}/${IMAGEDIR}/rootfs.tar; \
	rm -f ${DEPLOY_DIR_IMAGE}/*.zip; \
	cd ${DEPLOY_DIR_IMAGE}; \
	zip ${IMAGE_NAME}_web.zip ${IMAGEDIR}/*; \
	rm -f ${DEPLOY_DIR_IMAGE}/*.tar.bz2; \
	rm -Rf ${IMAGEDIR}; \
	"

STB_PLATFORM = "dm4kgen"

SOC_FAMILY = "bcm7252s"

RCNAME = "dmm2"
