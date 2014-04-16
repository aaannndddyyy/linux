/*
 * Copyright (C) 2014 NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/io.h>
#include <linux/module.h>
#include <linux/platform_device.h>

struct latency_allowance {
	unsigned int reg;
	unsigned int shift;
	unsigned int mask;
	unsigned int def;
};

struct memory_client {
	unsigned int id;
	const char *name;

	struct latency_allowance latency;
};

static const struct memory_client memory_clients[] = {
	{
		.id = 0x01,
		.name = "display0a",
		.latency = {
			.reg = 0x2e8,
			.shift = 0,
			.mask = 0xff,
			.def = 0xc2,
		},
	}, {
		.id = 0x02,
		.name = "display0ab",
		.latency = {
			.reg = 0x2f4,
			.shift = 0,
			.mask = 0xff,
			.def = 0xc6,
		},
	}, {
		.id = 0x03,
		.name = "display0b",
		.latency = {
			.reg = 0x2e8,
			.shift = 16,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x04,
		.name = "display0bb",
		.latency = {
			.reg = 0x2f4,
			.shift = 16,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x05,
		.name = "display0c",
		.latency = {
			.reg = 0x2ec,
			.shift = 0,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x06,
		.name = "display0cb",
		.latency = {
			.reg = 0x2f8,
			.shift = 0,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x10,
		.name = "displayhc",
		.latency = {
			.reg = 0x2f0,
			.shift = 0,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x11,
		.name = "displayhcb",
		.latency = {
			.reg = 0x2fc,
			.shift = 0,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x5a,
		.name = "displayt",
		.latency = {
			.reg = 0x2f0,
			.shift = 16,
			.mask = 0xff,
			.def = 0x50,
		},
	}, {
		.id = 0x73,
		.name = "displayd",
		.latency = {
			.reg = 0x3c8,
			.shift = 0,
			.mask = 0xff,
			.def = 0x50,
		},
	},
};

struct tegra124_mc {
	void __iomem *base;
};

static const struct of_device_id tegra124_mc_of_match[] = {
	{ .compatible = "nvidia,tegra124-mc", },
	{ }
};

static int tegra124_mc_probe(struct platform_device *pdev)
{
	struct tegra124_mc *mc;
	struct resource *res;
	unsigned int i;

	mc = devm_kzalloc(&pdev->dev, sizeof(*mc), GFP_KERNEL);
	if (!mc)
		return -ENOMEM;

	platform_set_drvdata(pdev, mc);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	mc->base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(mc->base))
		return PTR_ERR(mc->base);

	for (i = 0; i < ARRAY_SIZE(memory_clients); i++) {
		const struct latency_allowance *la = &memory_clients[i].latency;
		u32 value;

		value = readl(mc->base + la->reg);
		value &= ~(la->mask << la->shift);
		value |= (la->def & la->mask) << la->shift;
		writel(value, mc->base + la->reg);
	}

	return 0;
}

static int tegra124_mc_remove(struct platform_device *pdev)
{
	return 0;
}

static struct platform_driver tegra124_mc_driver = {
	.driver = {
		.name = "tegra124-mc",
		.of_match_table = tegra124_mc_of_match,
	},
	.probe = tegra124_mc_probe,
	.remove = tegra124_mc_remove,
};
module_platform_driver(tegra124_mc_driver);

MODULE_AUTHOR("Thierry Reding <treding@nvidia.com>");
MODULE_DESCRIPTION("NVIDIA Tegra124 Memory Controller driver");
MODULE_LICENSE("GPL v2");
