DESCRIPTION = "Linux kernel for ${MACHINE}"
SECTION = "kernel"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

inherit kernel machine_kernel_pr samba_change_dialect siteinfo

PACKAGES_DYNAMIC = "kernel-*"

# For packages that RDEPEND on particular kernel modules, list the ones built into
# the kernel here, so that it is known that the kernel has them built in.
KERNEL_BUILTIN_MODULES = "kernel-module-block2mtd"

KERNEL_BUILTIN_MODULES_dm8000 = "\
	kernel-module-stv0299 \
	"
KERNEL_BUILTIN_MODULES_dm800sev2 = "\
	kernel-module-stv0299 \
	"
KERNEL_BUILTIN_MODULES_dm7020hd = "\
	kernel-module-stv0299 \
	"

# By default, kernel.bbclass modifies package names to allow multiple kernels
# to be installed in parallel. We revert this change and rprovide the versioned
# package names instead, to allow only one kernel to be installed.
PKG_${KERNEL_PACKAGE_NAME}-base = "${KERNEL_PACKAGE_NAME}-base"
PKG_${KERNEL_PACKAGE_NAME}-image = "${KERNEL_PACKAGE_NAME}-image"
RPROVIDES_${KERNEL_PACKAGE_NAME}-base = "${KERNEL_PACKAGE_NAME}-${KERNEL_VERSION}"
RPROVIDES_${KERNEL_PACKAGE_NAME}-image = "${KERNEL_PACKAGE_NAME}-image-${KERNEL_VERSION} ${KERNEL_BUILTIN_MODULES}"

USB_ROOT = "/dev/sdb2"

do_install_append() {
        ${STRIP} ${D}/${KERNEL_IMAGEDEST}/${KERNEL_IMAGETYPE}-${KERNEL_VERSION}
        gzip -9 ${D}/${KERNEL_IMAGEDEST}/${KERNEL_IMAGETYPE}-${KERNEL_VERSION}
        echo "/boot/bootlogo-${MACHINE}.elf.gz filename=/boot/bootlogo-${MACHINE}.jpg" > ${D}/${KERNEL_IMAGEDEST}/autoexec.bat
        echo "/boot/${KERNEL_IMAGETYPE}-${KERNEL_VERSION}.gz ${CMDLINE}" >> ${D}/${KERNEL_IMAGEDEST}/autoexec.bat
        echo "/boot/bootlogo-${MACHINE}.elf.gz filename=/boot/bootlogo-${MACHINE}.jpg" > ${D}/${KERNEL_IMAGEDEST}/autoexec_${MACHINE}.bat
        echo "/boot/${KERNEL_IMAGETYPE}-${KERNEL_VERSION}.gz ${USB_CMDLINE}" >> ${D}/${KERNEL_IMAGEDEST}/autoexec_${MACHINE}.bat
}

# Set FILES for vmlinux package, so that kernel-image gets those files.
FILES_${KERNEL_PACKAGE_NAME}-vmlinux = "boot/vmlinux-3.2-${MACHINE}.gz"
ALLOW_EMPTY_kernel-vmlinux = "1"
FILES_${KERNEL_PACKAGE_NAME}-image += "\
	/${KERNEL_IMAGEDEST}/autoexec*.bat \
	"

pkg_preinst_${KERNEL_PACKAGE_NAME}-image() {
	if [ -z "$D" ]
	then
		if mountpoint -q /${KERNEL_IMAGEDEST}
		then
			mount -o remount,rw,compr=none /${KERNEL_IMAGEDEST}
		else
			mount -t jffs2 -o rw,compr=none mtd:boot /${KERNEL_IMAGEDEST} || mount -t jffs2 -o rw,compr=none mtd:'boot partition' /${KERNEL_IMAGEDEST}
		fi
	fi
}
pkg_prerm_${KERNEL_PACKAGE_NAME}-image() {
	if [ -z "$D" ]
	then
		if mountpoint -q /${KERNEL_IMAGEDEST}
		then
			mount -o remount,rw,compr=none /${KERNEL_IMAGEDEST}
		else
			mount -t jffs2 -o rw,compr=none mtd:boot /${KERNEL_IMAGEDEST}
		fi
	fi
}
pkg_postinst_${KERNEL_PACKAGE_NAME}-image() {
        if [ -z "$D" ] && mountpoint -q /${KERNEL_IMAGEDEST}; then
                if grep -q '\<root=/dev/mtdblock3\>' /proc/cmdline && grep -q '\<root=ubi0:rootfs\>' /boot/autoexec.bat; then
                        sed -ie 's!${CMDLINE_UBI}!${CMDLINE_JFFS2}!' /boot/autoexec.bat;
                fi
                umount /${KERNEL_IMAGEDEST};
        fi
}
pkg_postrm_${KERNEL_PACKAGE_NAME}-image () {
	if [ -z "$D" ]
	then
		umount /${KERNEL_IMAGEDEST}
	fi
}

# Do not use update-alternatives!
pkg_postinst_kernel () {
}
pkg_postrm_kernel () {
}

CMDLINE_JFFS2 = "root=/dev/mtdblock3 rootfstype=jffs2 rw ${CMDLINE_CONSOLE}"
CMDLINE_UBI = "ubi.mtd=root root=ubi0:rootfs rootfstype=ubifs rw ${CMDLINE_CONSOLE}"
CMDLINE = "${@bb.utils.contains('IMAGE_FSTYPES', 'ubifs', '${CMDLINE_UBI}', '${CMDLINE_JFFS2}', d)}"
USB_CMDLINE = "root=${USB_ROOT} rootdelay=10 rw ${CMDLINE_CONSOLE}"

LOCALVERSION = "-${MACHINE}"


do_configure_prepend() {

        echo "" > ${S}/.config

        echo "CONFIG_CMDLINE=\"${CMDLINE}\"" >> ${S}/.config

        sed -e '/CONFIG_AEABI/d' \
            -e '/CONFIG_OABI_COMPAT=/d' \
            -e '/CONFIG_CMDLINE=/d' \
            -e '/CONFIG_CPU_BIG_ENDIAN/d' \
            -e '/CONFIG_LOGO=/d' \
            -e '/CONFIG_LOGO_LINUX_CLUT224=/d' \
            -e '/CONFIG_LOCALVERSION/d' \
            -e '/CONFIG_LOCALVERSION_AUTO/d' \
	    < '${WORKDIR}/defconfig' >>'${S}/.config'

        echo 'CONFIG_LOCALVERSION="${LOCALVERSION}"' >>${S}/.config
        echo '# CONFIG_LOCALVERSION_AUTO is not set' >>${S}/.config

}
