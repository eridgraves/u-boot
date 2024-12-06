/* SPDX-License-Identifier: GPL-2.0+ */
/*
 * Copyright 2024 Ezurio
 */

#ifndef __IMX8MP_NITROGEN_SMARC_H
#define __IMX8MP_NITROGEN_SMARC_H

#include <linux/sizes.h>
#include <linux/stringify.h>
#include <asm/arch/imx-regs.h>
#include "imx_env.h"

#define CFG_SYS_UBOOT_BASE	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#if defined(CONFIG_CMD_NET)
#define CFG_FEC_MXC_PHYADDR		4
#define PHY_ANEG_TIMEOUT 20000

#define CONFIG_FEC_XCV_TYPE		RGMII

#define CONFIG_FEC_MXC_KSZ_PHYADDR	7
#define FEC_QUIRK_ENET_MAC
#define GP_RGMII_PHY_RESET		IMX_GPIO_NR(1, 12)
#define GP_RGMII2_PHY_RESET		IMX_GPIO_NR(3, 16)


#endif

#ifdef CONFIG_DISTRO_DEFAULTS
#define BOOT_TARGET_DEVICES(func) \
	func(USB, usb, 0) \
	func(MMC, mmc, 1) \
	func(MMC, mmc, 2)

#include <config_distro_bootcmd.h>
#else
#define BOOTENV
#endif

#define CONFIG_SYS_FSL_USDHC_NUM	3
#define CONFIG_SYS_FSL_ESDHC_ADDR	0

#ifdef CONFIG_FSL_FSPI
#define FSL_FSPI_FLASH_SIZE		SZ_32M
#define FSL_FSPI_FLASH_NUM		1
#define FSPI0_BASE_ADDR			0x30bb0000
#define FSPI0_AMBA_BASE			0x0
#define CONFIG_FSPI_QUAD_SUPPORT

#define CONFIG_SYS_FSL_FSPI_AHB
#endif

#ifdef CONFIG_NAND_MXS
#define CONFIG_CMD_NAND_TRIMFFS

/* NAND stuff */
#define CONFIG_SYS_MAX_NAND_DEVICE     1
#define CONFIG_SYS_NAND_BASE           0x20000000
#define CONFIG_SYS_NAND_5_ADDR_CYCLE
#define CONFIG_SYS_NAND_ONFI_DETECTION
#define CONFIG_SYS_NAND_USE_FLASH_BBT
#endif /* CONFIG_NAND_MXS */

/* Link Definitions */

#define CFG_SYS_INIT_RAM_ADDR	0x40000000
#define CFG_SYS_INIT_RAM_SIZE	0x80000


/* Totally 6GB DDR */
#define CFG_SYS_SDRAM_BASE		0x40000000
#define PHYS_SDRAM			0x40000000
#define PHYS_SDRAM_SIZE			SZ_2G

#define CFG_MXC_UART_BASE		UART2_BASE_ADDR

/* USB configs */
#define CONFIG_USBD_HS
#define CONFIG_USB_GADGET_VBUS_DRAW 2

#ifdef CONFIG_DM_VIDEO
#define CONFIG_SYS_VIDEO_LOGO_MAX_SIZE (6 * 1024 * 1024)
#define CONFIG_BMP_16BPP
#define CONFIG_BMP_24BPP
#define CONFIG_BMP_32BPP
#define CONFIG_VIDEO_BMP_LOGO
#endif

#ifndef BD_CONSOLE
#if CONFIG_MXC_UART_BASE == UART1_BASE_ADDR
#define BD_CONSOLE	"ttymxc0"
#elif CONFIG_MXC_UART_BASE == UART2_BASE_ADDR
#define BD_CONSOLE	"ttymxc1"
#elif CONFIG_MXC_UART_BASE == UART3_BASE_ADDR
#define BD_CONSOLE	"ttymxc2"
#elif CONFIG_MXC_UART_BASE == UART4_BASE_ADDR
#define BD_CONSOLE	"ttymxc3"
#endif
#endif

#ifdef CONFIG_CMD_MMC
#define DISTRO_BOOT_DEV_MMC(func) func(MMC, mmc, 1) func(MMC, mmc, 0)
#else
#define DISTRO_BOOT_DEV_MMC(func)
#endif

#ifdef CONFIG_USB_STORAGE
#define DISTRO_BOOT_DEV_USB(func) func(USB, usb, 0)
#else
#define DISTRO_BOOT_DEV_USB(func)
#endif

#ifndef BOOT_TARGET_DEVICES
#define BOOT_TARGET_DEVICES(func) \
	DISTRO_BOOT_DEV_MMC(func) \
	DISTRO_BOOT_DEV_USB(func)
#endif

#include <config_distro_bootcmd.h>
#define CONFIG_CMD_FBPANEL

#define BD_RAM_BASE	0x80000000
#define BD_RAM_SCRIPT	"40020000"
#define BD_RAM_KERNEL	"40800000"
#define BD_RAM_RAMDISK	"42800000"
#define BD_RAM_FDT	"43000000"

#define BD_FUSE1		"1 3"
#define BD_FUSE1_VAL		"10002a00"	/* USDHC1 emmc */

#define BD_FUSE1_STR		"fuse1=" BD_FUSE1 "\0"
#define BD_FUSE1_VAL_STR	"fuse1_val=" BD_FUSE1_VAL "\0"
#ifdef BD_FUSE2
#define BD_FUSE2_STR		"fuse2=" BD_FUSE2 "\0"
#define BD_FUSE2_VAL_STR	"fuse2_val=" BD_FUSE2_VAL "\0"
#else
#define BD_FUSE2_STR		""
#define BD_FUSE2_VAL_STR	""
#endif

#define BD_FUSE_MAC1A		"9 1"
#define BD_FUSE_MAC1A_VAL	"00000019"
#define BD_FUSE_MAC1B		"9 0"

#define BD_FUSE_MAC1A_STR	"fuse_mac1a=" BD_FUSE_MAC1A "\0"
#define BD_FUSE_MAC1A_VAL_STR	"fuse_mac1a_val=" BD_FUSE_MAC1A_VAL "\0"
#define BD_FUSE_MAC1B_STR	"fuse_mac1b=" BD_FUSE_MAC1B "\0"

#define CONFIG_EXTRA_ENV_SETTINGS \
	"console=" BD_CONSOLE "\0" \
	"env_dev=" __stringify(CONFIG_SYS_MMC_ENV_DEV) "\0" \
	"env_part=" __stringify(CONFIG_SYS_MMC_ENV_PART) "\0" \
	"scriptaddr=" __stringify(CONFIG_SYS_LOAD_ADDR) "\0" \
	"fastboot_raw_partition_bootloader=0x0 0x1ff0 mmcpart 1\0" \
	"fastboot_raw_partition_bootloader-env=0x1ff0 0x10 mmcpart 1\0" \
	"fdt_addr=0x43000000\0" \
	"fdt_high=0xffffffffffffffff\0" \
	BD_FUSE1_STR \
	BD_FUSE1_VAL_STR \
	BD_FUSE2_STR \
	BD_FUSE2_VAL_STR \
	BD_FUSE_MAC1A_STR \
	BD_FUSE_MAC1A_VAL_STR \
	BD_FUSE_MAC1B_STR \
	"initrd_high=0xffffffffffffffff\0" \
	"mcoreboot=load ${devtype} ${devnum}:1 ${mcore_loadaddr} ${mcore_image}; " \
		"dcache flush; bootaux ${mcore_loadaddr}\0" \
	"mcore_image=mcore_fw.bin\0" \
	"mcore_loadaddr="__stringify(CONFIG_IMX_MCORE_TCM_ADDR)"\0" \
	"mcore_bootargs=clk-imx8mp.mcore_booted\0" \
	"netargs=setenv bootargs console=${console},115200 root=/dev/nfs rw " \
		"ip=dhcp nfsroot=${tftpserverip}:${nfsroot},v3,tcp\0" \
	"netboot=run netargs; " \
		"if test -z \"${fdt_file}\" -a -n \"${soc}\"; then " \
			"setenv fdt_file ${soc}-${board}${boardver}.dtb; " \
		"fi; " \
		"if test ${ip_dyn} = yes; then " \
			"setenv get_cmd dhcp; " \
		"else " \
			"setenv get_cmd tftp; " \
		"fi; " \
		"${get_cmd} ${loadaddr} ${tftpserverip}:Image; " \
		"if ${get_cmd} ${fdt_addr} ${tftpserverip}:${fdt_file}; then " \
			"booti ${loadaddr} - ${fdt_addr}; " \
		"else " \
			"echo WARN: Cannot load the DT; " \
		"fi;\0" \
	"net_upgradeu=dhcp " BD_RAM_SCRIPT " net_upgradeu.scr && source " BD_RAM_SCRIPT "\0" \
	"otg_upgradeu=run usbnetwork; tftp " BD_RAM_SCRIPT " net_upgradeu.scr && source " BD_RAM_SCRIPT "\0" \
	"uboot_defconfig=" CONFIG_DEFCONFIG "\0" \
	"upgradeu=setenv boot_scripts upgrade.scr; boot;" \
		"echo Upgrade failed!; setenv boot_scripts boot.scr\0" \
	"usbnet_devaddr=00:19:b8:00:00:02\0" \
	"usbnet_hostaddr=00:19:b8:00:00:01\0" \
	"usbnetwork=setenv ethact usb_ether; " \
		"setenv ipaddr 10.0.0.2; " \
		"setenv netmask 255.255.255.0; " \
		"setenv serverip 10.0.0.1;\0" \
	BOOTENV

#endif
