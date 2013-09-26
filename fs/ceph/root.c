/*
 * Copyright (C) 2012 Codethink Ltd. <mark.doffman@codethink.co.uk>
 *
 * This file is released under the GPL v2
 *
 * Allow a CephFS filesystem to be mounted as root.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/utsname.h>
#include <linux/root_dev.h>
#include <linux/in.h>
#include <net/ipconfig.h>
#include <linux/ceph/ceph_root.h>

/* linux/net/ipv4/ipconfig.c: trims ip addr off front of name, too. */
extern __be32 root_nfs_parse_addr(char *name); /*__init*/

#define MAXPATHLEN 1024

/* Parameters passed from the kernel command line */
static char ceph_root_params[256] __initdata;

/* Address of CEPH server */
static __be32 servaddr __initdata = htonl(INADDR_NONE);

/* Name of directory to mount */
static char ceph_export_path[MAXPATHLEN + 1] __initdata;

/* Text-based mount options */
static char ceph_root_options[256] __initdata;

/* server:path string passed to mount */
static char ceph_root_device[MAXPATHLEN + 1] __initdata;

/*
 * Parse out root export path and mount options from
 * passed-in string @incoming.
 *
 * Copy the export path into @exppath.
 *
 * Returns 0 on success -E2BIG if the resulting options string is too long.
 */
static int __init root_ceph_parse_options(char *incoming, char *exppath,
						const size_t exppathlen)
{
	char *p;
	int res = 0;

	/*
	 * Set the remote path
	 */
	p = strsep(&incoming, ",");
	if (*p != '\0' && strcmp(p, "default") != 0)
		strlcpy(exppath, p, exppathlen);

	/*
	 * @incoming now points to the rest of the string; if it
	 * contains something, append it to our root options buffer
	 */
	if (incoming != NULL && *incoming != '\0') {
		size_t len = strlen(ceph_root_options);
		size_t destlen = sizeof(ceph_root_options);

		if (len && ceph_root_options[len - 1] != ',')
			if (strlcat(ceph_root_options, ",", destlen) > destlen)
				res = -E2BIG;

		if (strlcat(ceph_root_options, incoming, destlen) > destlen)
			res = -E2BIG;
	}
	return res;
}

/*
 *  Parse CephFS server and directory information passed on the kernel
 *  command line.
 *
 *  cephroot=[<server-ip>:]<root-dir>[,<cephfs-options>]
 */
static int __init ceph_root_setup(char *line)
{
	ROOT_DEV = Root_CEPH;

	strlcpy(ceph_root_params, line, sizeof(ceph_root_params));

	/*
	 * Note: root_nfs_parse_addr() removes the server-ip from
	 * ceph_root_params, if it exists.
	 */
	root_server_addr = root_nfs_parse_addr(ceph_root_params);

	return 1;
}

__setup("cephroot=", ceph_root_setup);

/*
 * ceph_root_data - Return mount device and data for CEPHROOT mount.
 *
 * @root_device: OUT: Address of string containing CEPHROOT device.
 * @root_data: OUT: Address of string containing CEPHROOT mount options.
 *
 * Returns: 0 and sets @root_device and @root_data if successful.
 *          error code if unsuccessful.
 */
int __init ceph_root_data(char **root_device, char **root_data)
{
	char *tmp = NULL;
	const size_t tmplen = sizeof(ceph_export_path);
	int len;
	int ret = 0;

	servaddr = root_server_addr;
	if (servaddr == htonl(INADDR_NONE))
		return -ENOENT;

	tmp = kzalloc(tmplen, GFP_KERNEL);
	if (tmp == NULL)
		return -ENOMEM;

	if (root_server_path[0] != '\0') {
		if (root_ceph_parse_options(root_server_path, tmp, tmplen)) {
			ret = -E2BIG;
			goto out;
		}
	}

	if (ceph_root_params[0] != '\0') {
		if (root_ceph_parse_options(ceph_root_params, tmp, tmplen)) {
			ret = -E2BIG;
			goto out;
		}
	}

	/*
	 * Set up ceph_root_device. This looks like: server:/path
	 *
	 * At this point, utsname()->nodename contains our local
	 * IP address or hostname, set by ipconfig.  If "%s" exists
	 * in tmp, substitute the nodename, then shovel the whole
	 * mess into ceph_root_device.
	 */
	len = snprintf(ceph_export_path, sizeof(ceph_export_path),
				tmp, utsname()->nodename);
	if (len > (int)sizeof(ceph_export_path)) {
		ret = -E2BIG;
		goto out;
	}
	len = snprintf(ceph_root_device, sizeof(ceph_root_device),
				"%pI4:%s", &servaddr, ceph_export_path);
	if (len > (int)sizeof(ceph_root_device)) {
		ret = -E2BIG;
		goto out;
	}

	pr_debug("Root-CEPH: Root device: %s\n", ceph_root_device);
	pr_debug("Root-CEPH: Root options: %s\n", ceph_root_options);
	*root_device = ceph_root_device;
	*root_data = ceph_root_options;

out:
	kfree(tmp);
	return ret;
}
