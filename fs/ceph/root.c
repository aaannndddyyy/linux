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

static int __init root_ceph_copy(char *dest, const char *src,
					const size_t destlen)
{
	if (strlcpy(dest, src, destlen) > destlen)
		return -1;
	return 0;
}

static int __init root_ceph_cat(char *dest, const char *src,
				const size_t destlen)
{
	size_t len = strlen(dest);

	if (len && dest[len - 1] != ',')
		if (strlcat(dest, ",", destlen) > destlen)
			return -1;

	if (strlcat(dest, src, destlen) > destlen)
		return -1;
	return 0;
}

/*
 * Parse out root export path and mount options from
 * passed-in string @incoming.
 *
 * Copy the export path into @exppath.
 */
static int __init root_ceph_parse_options(char *incoming, char *exppath,
						const size_t exppathlen)
{
	char *p;

	/*
	 * Set the NFS remote path
	 */
	p = strsep(&incoming, ",");
	if (*p != '\0' && strcmp(p, "default") != 0)
		if (root_ceph_copy(exppath, p, exppathlen))
			return -1;

	/*
	 * @incoming now points to the rest of the string; if it
	 * contains something, append it to our root options buffer
	 */
	if (incoming != NULL && *incoming != '\0')
		if (root_ceph_cat(ceph_root_options, incoming,
						sizeof(ceph_root_options)))
			return -1;
	return 0;
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
	 *	 ceph_root_parms, if it exists.
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
 *          -1 if unsuccessful.
 */
int __init ceph_root_data(char **root_device, char **root_data)
{
	char *tmp = NULL;
	const size_t tmplen = sizeof(ceph_export_path);
	int len;
	int retval = 0;

	servaddr = root_server_addr;
	if (servaddr == htonl(INADDR_NONE))
		return -1;

	tmp = kzalloc(tmplen, GFP_KERNEL);
	if (tmp == NULL)
		goto out_nomem;

	if (root_server_path[0] != '\0') {
		if (root_ceph_parse_options(root_server_path, tmp, tmplen))
			goto out_optionstoolong;
	}

	if (ceph_root_params[0] != '\0') {
		if (root_ceph_parse_options(ceph_root_params, tmp, tmplen))
			goto out_optionstoolong;
	}

	/*
	 * Set up ceph_root_device.  This looks like
	 *
	 *	server:/path
	 *
	 * At this point, utsname()->nodename contains our local
	 * IP address or hostname, set by ipconfig.  If "%s" exists
	 * in tmp, substitute the nodename, then shovel the whole
	 * mess into ceph_root_device.
	 */
	len = snprintf(ceph_export_path, sizeof(ceph_export_path),
				tmp, utsname()->nodename);
	if (len > (int)sizeof(ceph_export_path))
		goto out_devnametoolong;
	len = snprintf(ceph_root_device, sizeof(ceph_root_device),
				"%pI4:%s", &servaddr, ceph_export_path);
	if (len > (int)sizeof(ceph_root_device))
		goto out_devnametoolong;

	pr_debug("Root-CEPH: Root device: %s\n", ceph_root_device);
	pr_debug("Root-CEPH: Root options: %s\n", ceph_root_options);
	*root_device = ceph_root_device;
	*root_data = ceph_root_options;

	retval = 0;

out:
	kfree(tmp);
	return retval;

out_nomem:
	pr_err("Root-CEPH: could not allocate memory\n");
	goto out;

out_optionstoolong:
	pr_err("Root-CEPH: mount options string too long\n");
	goto out;

out_devnametoolong:
	pr_err("Root-CEPH: root device name too long.\n");
	goto out;
}
