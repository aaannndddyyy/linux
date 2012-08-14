/* arch/arm/mach-kirkwood/csb1724-setup.c
 *
 * Copyright 2012 Codethink Limited.
 *	Ben Dooks <ben.dooks@codethink.co.uk>
 *
 * Cogent CSB1724 SoM setup.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/mv643xx_eth.h>
#include <linux/i2c.h>

#include <linux/mtd/partitions.h>
#include <linux/ata_platform.h>
#include <plat/mvsdio.h>

#include <asm/mach/arch.h>
#include <asm/mach-types.h>

#include "common.h"
#include "mpp.h"

static struct mtd_partition csb1724_nand_parts[] = {
	{
		.name		= "u-boot",
		.offset		= 0,
		.size		= SZ_1M,
	}, {
		.name		= "uImage",
		.offset		= MTDPART_OFS_NXTBLK,
		.size		= SZ_4M - SZ_1M,
	}, {
		.name		= "rootfs",
		.offset		= MTDPART_OFS_NXTBLK,
		.size		= MTDPART_SIZ_FULL
	},
};

static struct mv643xx_eth_platform_data csb1724_ge00_data = {
	.phy_addr	= MV643XX_ETH_PHY_ADDR(0),
};

static struct mv643xx_eth_platform_data csb1724_ge01_data = {
	.phy_addr	= MV643XX_ETH_PHY_ADDR(1),
};

static struct mv_sata_platform_data csb1724_sata_data = {
	.n_ports	= 2,
};

static struct mvsdio_platform_data csb1724_mvsdio_data = {
	.gpio_card_detect	= 38,
};

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

static struct i2c_board_info i2c_board_info_csb1724[] = {
	{
		I2C_BOARD_INFO("smsc2517-usb", 0x2c),
	},
};

static void __init csb1724_init(void)
{
	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_init();
	kirkwood_mpp_conf(csb1724_mpp_config);

	i2c_register_board_info(0, ARRAY_AND_SIZE(i2c_board_info_csb1724));
	kirkwood_i2c_init();
	kirkwood_i2c1_init();
	kirkwood_ehci_init();
	kirkwood_uart0_init();
	kirkwood_ge00_init(&csb1724_ge00_data);
	kirkwood_ge01_init(&csb1724_ge01_data);
	kirkwood_sata_init(&csb1724_sata_data);
	kirkwood_sdio_init(&csb1724_mvsdio_data);

	kirkwood_nand_init(ARRAY_AND_SIZE(csb1724_nand_parts), 25);
}

static int __init csb1724_pci_init(void)
{
	if (machine_is_csb1724())
		kirkwood_pcie_init(KW_PCIE1 | KW_PCIE0);

	return 0;
}

subsys_initcall(csb1724_pci_init);

MACHINE_START(CSB1724, "Cogent CSB1724")
	.atag_offset	= 0x100,
	.init_machine	= csb1724_init,
	.map_io		= kirkwood_map_io,
	.init_early	= kirkwood_init_early,
	.init_irq	= kirkwood_init_irq,
	.timer		= &kirkwood_timer,
	.restart	= kirkwood_restart,
MACHINE_END
