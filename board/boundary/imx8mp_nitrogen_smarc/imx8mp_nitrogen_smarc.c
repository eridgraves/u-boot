// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2024 Ezurio
 */

#include <efi_loader.h>
#include <env.h>
#include <errno.h>
#include <init.h>
#include <miiphy.h>
#include <netdev.h>
#include <linux/delay.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm-generic/gpio.h>
#include <asm/arch/imx8mp_pins.h>
#include <asm/arch/clock.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/gpio.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <spl.h>
#include <asm/mach-imx/dma.h>
#include <power/pmic.h>
#include <usb.h>
#include <dwc3-uboot.h>
#include <mmc.h>

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_FSEL1)
#define WDOG_PAD_CTRL	(PAD_CTL_DSE6 | PAD_CTL_ODE | PAD_CTL_PUE | PAD_CTL_PE)
#define ENET_MDIO_PAD_CTRL	(PAD_CTL_DSE1)
#define ENET_MDC_PAD_CTRL	(PAD_CTL_DSE1)

static iomux_v3_cfg_t const uart_pads[] = {
	MX8MP_PAD_UART2_RXD__UART2_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX8MP_PAD_UART2_TXD__UART2_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const wdog_pads[] = {
	MX8MP_PAD_GPIO1_IO02__WDOG1_WDOG_B | MUX_PAD_CTRL(WDOG_PAD_CTRL),
};
static iomux_v3_cfg_t const init_pads[] = {
#define GP_LVDS2_BACKLIGHT	IMX_GPIO_NR(3, 5)
	MX8MP_PAD_NAND_CLE__GPIO3_IO05 | MUX_PAD_CTRL(PAD_CTL_PE),

	MX8MP_PAD_GPIO1_IO01__GPIO1_IO01 | MUX_PAD_CTRL(PAD_CTL_PE),
	MX8MP_PAD_SAI1_RXD3__ENET1_MDIO | MUX_PAD_CTRL(ENET_MDIO_PAD_CTRL),
	MX8MP_PAD_SAI1_RXD2__ENET1_MDC | MUX_PAD_CTRL(ENET_MDC_PAD_CTRL),

#define GPIRQ_SN65DSI83		IMX_GPIO_NR(1, 6)
#define GP_LTK08_MIPI_EN	IMX_GPIO_NR(1, 6)
	MX8MP_PAD_GPIO1_IO06__GPIO1_IO06 | MUX_PAD_CTRL(PAD_CTL_PE),

#define GP_LVDS_BACKLIGHT	IMX_GPIO_NR(3, 15)
#define GPIRQ_TS_GT911		IMX_GPIO_NR(3, 15)
#define GP_TS_GT911_IRQ		IMX_GPIO_NR(3, 15)
#define GPIRQ_TS_FT5X06		IMX_GPIO_NR(3, 15)
#define GP_TS_FT5X06_WAKE	IMX_GPIO_NR(3, 15)
	MX8MP_PAD_NAND_RE_B__GPIO3_IO15 | MUX_PAD_CTRL(PAD_CTL_PE),

#define MCP23018 8
#define GP_TS_GT911_RESET	IMX_GPIO_NR(MCP23018, 7)
#define GP_ST1633_RESET		IMX_GPIO_NR(MCP23018, 7)
#define GP_TS_FT5X06_RESET	IMX_GPIO_NR(MCP23018, 7)	/* mcp23018 7 */


#define GP_TC358762_EN		IMX_GPIO_NR(4, 3)
#define GP_SC18IS602B_RESET	IMX_GPIO_NR(4, 3)
#define GP_SN65DSI83_EN		IMX_GPIO_NR(4, 3)
#define GP_MIPI_ENABLE		IMX_GPIO_NR(4, 3)
	/* enable for TPS65132 Single Inductor - Dual Output Power Supply */
#define GP_LCD133_070_ENABLE	IMX_GPIO_NR(4, 3)
	MX8MP_PAD_SAI1_RXD1__GPIO4_IO03 | MUX_PAD_CTRL(PAD_CTL_PE),
	MX8MP_PAD_GPIO1_IO05__GPIO1_IO05 | MUX_PAD_CTRL(PAD_CTL_PE),

#define GP_PWM2_MIPI		IMX_GPIO_NR(5, 4)
#define GP_LVDS_PWM		IMX_GPIO_NR(5, 4)
	MX8MP_PAD_SPDIF_RX__GPIO5_IO04 | MUX_PAD_CTRL(PAD_CTL_PE),	/* PWM2 */

#define GP_LVDS2_PWM		IMX_GPIO_NR(5, 5)
	MX8MP_PAD_SPDIF_EXT_CLK__GPIO5_IO05 | MUX_PAD_CTRL(PAD_CTL_PE),	/* PWM1 */

#define GPIRQ_RV3028		IMX_GPIO_NR(3, 2)
	MX8MP_PAD_NAND_CE1_B__GPIO3_IO02 | \
		MUX_PAD_CTRL(PAD_CTL_PE | PAD_CTL_PUE | PAD_CTL_HYS),	/* SM_GPIO12 */

	/* eqos */
	MX8MP_PAD_ENET_MDC__ENET_QOS_MDC | MUX_PAD_CTRL(PAD_CTL_ODE),
	MX8MP_PAD_ENET_MDIO__ENET_QOS_MDIO | \
		MUX_PAD_CTRL(PAD_CTL_ODE | PAD_CTL_HYS),
	/* fec */
	MX8MP_PAD_SAI1_RXD2__ENET1_MDC | MUX_PAD_CTRL(PAD_CTL_ODE),
	MX8MP_PAD_SAI1_RXD3__ENET1_MDIO | MUX_PAD_CTRL(PAD_CTL_ODE | PAD_CTL_HYS),

#define GP_USB3_1_HUB_RESET	IMX_GPIO_NR(3, 6)
	MX8MP_PAD_NAND_DATA00__GPIO3_IO06 | MUX_PAD_CTRL(PAD_CTL_PE),
	MX8MP_PAD_GPIO1_IO13__USB1_OTG_OC | \
		MUX_PAD_CTRL(PAD_CTL_PE | PAD_CTL_HYS | PAD_CTL_PUE),
};

int board_early_init_f(void)
{
	struct wdog_regs *wdog = (struct wdog_regs *)WDOG1_BASE_ADDR;

	imx_iomux_v3_setup_multiple_pads(wdog_pads, ARRAY_SIZE(wdog_pads));

	gpio_request(GP_PWM2_MIPI, "pwm2");
	gpio_direction_output(GP_PWM2_MIPI, 0);
	gpio_free(GP_PWM2_MIPI);

	gpio_request(GP_SN65DSI83_EN, "sn65en");
	gpio_direction_output(GP_SN65DSI83_EN, 0);
	gpio_free(GP_SN65DSI83_EN);

	imx_iomux_v3_setup_multiple_pads(init_pads, ARRAY_SIZE(init_pads));
	set_wdog_reset(wdog);

	imx_iomux_v3_setup_multiple_pads(uart_pads, ARRAY_SIZE(uart_pads));

	init_uart_clk(1);

	return 0;
}

#if !CONFIG_IS_ENABLED(USB_DWC3_GENERIC) && (defined(CONFIG_USB_DWC3) || defined(CONFIG_USB_XHCI_IMX8M))
int board_usb_hub_gpio_init(void)
{
	return GP_USB3_1_HUB_RESET;
}
#endif

int board_init(void)
{
	gpio_request(GPIRQ_RV3028, "rv3028_irq");
	gpio_request(GP_TS_GT911_RESET, "gt11_reset");
//	gpio_request(GP_SN65DSI83_EN, "sn65en");
//	gpio_request(GP_LTK08_MIPI_EN, "ltk08_mipi_en");
#if !CONFIG_IS_ENABLED(USB_DWC3_GENERIC)
	gpio_request(GP_USB3_1_HUB_RESET, "usb1_hub_reset");
#endif
	gpio_direction_output(GP_TS_GT911_RESET, 0);
#if !CONFIG_IS_ENABLED(USB_DWC3_GENERIC)
	gpio_direction_output(GP_USB3_1_HUB_RESET, 0);
#endif

	return 0;
}

#ifdef CONFIG_OF_BOARD_SETUP
int ft_board_setup(void *blob, struct bd_info *bd)
{
	return 0;
}
#endif

int mmc_map_to_kernel_blk(int dev_no)
{
	return dev_no;
}

#if 0
void board_env_init(void)
{
}
#endif
