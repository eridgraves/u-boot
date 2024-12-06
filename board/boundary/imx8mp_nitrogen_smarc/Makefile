#
# Copyright 2023 Laird Connect
#
# SPDX-License-Identifier:      GPL-2.0+
#

obj-y += imx8mp_nitrogen_smarc.o

ifdef CONFIG_SPL_BUILD
obj-y += spl.o
obj-$(CONFIG_IMX8M_LPDDR4) += lpddr4_timing_2gr0.o
endif
