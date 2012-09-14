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
#include <linux/gpio.h>
#include <linux/platform_device.h>
#include <linux/mv643xx_eth.h>

#include <linux/mtd/partitions.h>
#include <linux/ata_platform.h>
#include <plat/mvsdio.h>

#include <asm/mach/arch.h>
#include <asm/mach-types.h>
#include <asm/system_misc.h>

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

static void __init csb1724_init(void)
{
	/*
	 * Basic setup. Needs to be called early.
	 */
	kirkwood_init();
	kirkwood_mpp_conf(csb1724_mpp_config);

	orion_gpio_set_valid(28, 1);
	orion_gpio_set_valid(29, 1);

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
		kirkwood_pcie_init(KW_PCIE1 | KW_PCIE0, KW_PCIE0 | KW_PCIE1);

	return 0;
}

subsys_initcall(csb1724_pci_init);

static int reset_gpio = -1;

static void csb1724_do_restart(char mode, const char *cmd)
{
	printk(KERN_INFO "Resetting board\n");

	gpio_direction_output(reset_gpio, 0);
}

static int __init csb1724_reset_setup(char *line)
{
	if (kstrtoint(line, 0, &reset_gpio))
		printk(KERN_ERR "%s: unknown value '%s'\n", __func__, line);
	return 1;
}

__setup("csb1724_reset=", csb1724_reset_setup);

static int __init csb1724_reset_init(void)
{
	int ret;

	if (reset_gpio >= 0) {
		printk(KERN_INFO "%s: using gpio %d\n", __func__, reset_gpio);

		ret = gpio_request(reset_gpio, "board reset");
		if (ret != 0) {
			printk(KERN_ERR "Failed to get gpio\n");
			return ret;
		}

		arm_pm_restart = csb1724_do_restart;
	}

	return 0;
}

device_initcall(csb1724_reset_init);


MACHINE_START(CSB1724, "Cogent CSB1724")
	.atag_offset	= 0x100,
	.init_machine	= csb1724_init,
	.map_io		= kirkwood_map_io,
	.init_early	= kirkwood_init_early,
	.init_irq	= kirkwood_init_irq,
	.timer		= &kirkwood_timer,
	.restart	= kirkwood_restart,
MACHINE_END
