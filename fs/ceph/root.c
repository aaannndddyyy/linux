/*
 * Copyright (C) 2012 Mark Doffman <mark.doffman@codethink.co.uk>
 *
 * Allow a CephFS filesystem to be mounted as root.
 */

#include <linux/types.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/root_dev.h>

/*
 *  Parse CephFS server and directory information passed on the kernel
 *  command line.
 *
 *  cephroot=[<server-ip>:]<root-dir>[,<cephfs-options>]
 */
static int __init ceph_root_setup(char *line) 
{
	ROOT_DEV = Root_CEPH;

	return 1;
}

__setup("cephroot=", ceph_root_setup);

/* Blaghbalasdfkjs */

/*
 * ceph_root_data - Return mount device and data for CEPHROOT mount.
 *
 * @root_device: OUT: Address of string containing CEPHROOT device.
 * @root_data: OUT: Address of string containing CEPHROOT mount options.
 *
 * Returns: 0 and sets @root_device and @root_data if successful.
 *          -1 if unsuccessful.
 */
int __init ceph_root_data(char **root_device, char ** root_data)
{
	return 0;
}
