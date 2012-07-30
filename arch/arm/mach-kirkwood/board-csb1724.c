/*
 * Copyright 2012 (C), Ian Molton <ian.molton@codethink.co.uk>
 *
 * arch/arm/mach-kirkwood/board-csb1724.c
 *
 * Cogent csb1724 Board Init for drivers not converted to
 * flattened device tree yet.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include "common.h"
#include "mpp.h"

static unsigned int csb1724_mpp_config[] __initdata = {
	MPP0_NF_IO2,
	MPP1_NF_IO3,
	MPP2_NF_IO4,
	MPP3_NF_IO5,
	MPP4_NF_IO6,
	MPP5_NF_IO7,
	MPP8_TW0_SDA,
	MPP9_TW0_SCK,
	MPP12_SD_CLK,
	MPP13_SD_CMD,
	MPP14_SD_D0,
	MPP15_SD_D1,
	MPP16_SD_D2,
	MPP17_SD_D3,
	MPP18_NF_IO0,
	MPP19_NF_IO1,
	MPP20_GE1_TXD0,
	MPP21_GE1_TXD1,
	MPP22_GE1_TXD2,
	MPP23_GE1_TXD3,
	MPP24_GE1_RXD0,
	MPP25_GE1_RXD1,
	MPP26_GE1_RXD2,
	MPP27_GE1_RXD3,
	MPP30_GE1_RXCTL,
	MPP31_GE1_RXCLK,
	MPP32_GE1_TCLKOUT,
	MPP33_GE1_TXCTL,
	MPP34_SATA1_ACTn,
	MPP35_SATA0_ACTn,
	MPP36_TW1_SDA,
	MPP37_TW1_SCK,
};

void __init csb1724_init(void)
{
	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_mpp_conf(csb1724_mpp_config);
	kirkwood_ge00_init(NULL);
	kirkwood_ge01_init(NULL);
}
