/*
 * Fixup for SLAB motherboard i2c chip D+- swap.
 *
 * Copyright (c) 2012 Ian Molton <ian.molton@codethink.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/i2c.h>
#include <linux/err.h>


static const u8 datablob[] = {
0xff, 0x00, 0x00, 0x24, 0x01, 0x04, 0x02, 0x17, 0x03, 0x25, 0x04, 0x00,
0x05, 0x00, 0x06, 0x9b, 0x07, 0x20, 0x08, 0x02, 0x09, 0x00, 0x0a, 0x00,
0x0b, 0x00, 0x0c, 0x01, 0x0d, 0x32, 0x0e, 0x01, 0x0f, 0x32, 0x10, 0x32,
0x11, 0x00, 0x12, 0x00, 0x13, 0x00, 0x14, 0x00, 0x15, 0x00, 0xd0, 0x00,
0xf6, 0x00, 0xf7, 0x00, 0xf8, 0x00, 0xf9, 0x00, 0xfa, 0x00, 0xfb, 0x00,
0xfa, 0x00, 0xfb, 0x21, 0xfc, 0x43, 0xfd, 0x65, 0xfe, 0x07, 0xff, 0x01, 0, 0
};

static int
smsc2517_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	const u8 *reg = datablob;
	const u8 *data = reg+1;

	while (*reg || *data) {
		if (i2c_smbus_write_block_data(client, *reg, 1, data) < 0)
			return -ENODEV;
		reg += 2;
		data += 2;
	}

	return 0;
}

static int smsc2517_remove(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id smsc2517_ids[] = {
	{ "smsc2517-usb", 0, },
	{ /* LIST END */ }
};
MODULE_DEVICE_TABLE(i2c, smsc2517_ids);

static struct i2c_driver smsc2517_driver = {
	.driver = {
		.name	= "smsc2517-usb",
	},
	.probe		= smsc2517_probe,
	.remove		= smsc2517_remove,
	.id_table	= smsc2517_ids,
};

/*-----------------------------------------------------------------------*/

module_i2c_driver(smsc2517_driver);

MODULE_AUTHOR("Ian Molton <ian.molton@codethink.co.uk>");
MODULE_DESCRIPTION("SMSC2517-USB driver");
MODULE_LICENSE("GPL");

