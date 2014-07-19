/* Generic I/O port emulation, based on MN10300 code
 *
 * Copyright (C) 2007 Red Hat, Inc. All Rights Reserved.
 * Written by David Howells (dhowells@redhat.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public Licence
 * as published by the Free Software Foundation; either version
 * 2 of the Licence, or (at your option) any later version.
 */
#ifndef __ASM_GENERIC_IO_H
#define __ASM_GENERIC_IO_H

#include <asm/page.h> /* I/O is all done through memory accesses */
#include <linux/types.h>

#ifdef CONFIG_GENERIC_IOMAP
#include <asm-generic/iomap.h>
#endif

#include <asm-generic/pci_iomap.h>

#ifndef mmiowb
#define mmiowb() do {} while (0)
#endif

/*
 * raw_{read,write}{b,w,l,q} access memory in native endian.
 *
 * On some architectures the memory mapped IO stuff needs to be accessed
 * differently. On the simple architectures, we just read/write the
 * memory location directly.
 */
#ifndef __raw_readb
#define __raw_readb __raw_readb
static inline u8 __raw_readb(const volatile void __iomem *addr)
{
	return *(const volatile u8 __force *) addr;
}
#endif

#ifndef __raw_readw
#define __raw_readw __raw_readw
static inline u16 __raw_readw(const volatile void __iomem *addr)
{
	return *(const volatile u16 __force *) addr;
}
#endif

#ifndef __raw_readl
#define __raw_readl __raw_readl
static inline u32 __raw_readl(const volatile void __iomem *addr)
{
	return *(const volatile u32 __force *) addr;
}
#endif

#ifdef CONFIG_64BIT
#ifndef __raw_readq
#define __raw_readq __raw_readq
static inline u64 __raw_readq(const volatile void __iomem *addr)
{
	return *(const volatile u64 __force *) addr;
}
#endif
#endif /* CONFIG_64BIT */


#ifndef __raw_writeb
#define __raw_writeb __raw_writeb
static inline void __raw_writeb(u8 b, volatile void __iomem *addr)
{
	*(volatile u8 __force *) addr = b;
}
#endif

#ifndef __raw_writew
#define __raw_writew __raw_writew
static inline void __raw_writew(u16 b, volatile void __iomem *addr)
{
	*(volatile u16 __force *) addr = b;
}
#endif

#ifndef __raw_writel
#define __raw_writel __raw_writel
static inline void __raw_writel(u32 b, volatile void __iomem *addr)
{
	*(volatile u32 __force *) addr = b;
}
#endif

#ifdef CONFIG_64BIT
#ifndef __raw_writeq
#define __raw_writeq __raw_writeq
static inline void __raw_writeq(u64 b, volatile void __iomem *addr)
{
	*(volatile u64 __force *) addr = b;
}
#endif
#endif /* CONFIG_64BIT */


/*
 * {read,write}{b,w,l,q} access little endian memory
 * and return result in native endian
 */
#ifndef readb
#define readb __raw_readb
#endif

#ifndef readw
#define readw readw
static inline u16 readw(const volatile void __iomem *addr)
{
	return __le16_to_cpu(__raw_readw(addr));
}
#endif

#ifndef readl
#define readl readl
static inline u32 readl(const volatile void __iomem *addr)
{
	return __le32_to_cpu(__raw_readl(addr));
}
#endif

#ifdef CONFIG_64BIT
#ifndef readq
#define readq readq
static inline u64 readq(const volatile void __iomem *addr)
{
	return __le64_to_cpu(__raw_readq(addr));
}
#endif
#endif /* CONFIG_64BIT */


#ifndef writeb
#define writeb __raw_writeb
#endif

#ifndef writew
#define writew(b,addr) __raw_writew(__cpu_to_le16(b),addr)
#endif

#ifndef writel
#define writel(b,addr) __raw_writel(__cpu_to_le32(b),addr)
#endif

#ifdef CONFIG_64BIT
#ifndef writeq
#define writeq(b, addr) __raw_writeq(__cpu_to_le64(b), addr)
#endif
#endif /* CONFIG_64BIT */


/*
 * {read,write}s{b,w,l.q}b access native memory in chunks specified by count
 */
#ifndef readsb
#define readsb readsb
static inline void readsb(const void __iomem *addr, void *buffer, int count)
{
	if (count) {
		u8 *buf = buffer;
		do {
			u8 x = __raw_readb(addr);
			*buf++ = x;
		} while (--count);
	}
}
#endif

#ifndef readsw
#define readsw readsw
static inline void readsw(const void __iomem *addr, void *buffer, int count)
{
	if (count) {
		u16 *buf = buffer;
		do {
			u16 x = __raw_readw(addr);
			*buf++ = x;
		} while (--count);
	}
}
#endif

#ifndef readsl
#define readsl readsl
static inline void readsl(const void __iomem *addr, void *buffer, int count)
{
	if (count) {
		u32 *buf = buffer;
		do {
			u32 x = __raw_readl(addr);
			*buf++ = x;
		} while (--count);
	}
}
#endif

#ifdef CONFIG_64BIT
#ifndef readsq
#define readsq readsq
static inline void readsq(const void __iomem *addr, void *buffer, int count)
{
	if (count) {
		u64 *buf = buffer;
		do {
			u64 x = __raw_readq(addr);
			*buf++ = x;
		} while (--count);
	}
}
#endif
#endif /* CONFIG_64BIT */


#ifndef writesb
#define writesb writesb
static inline void writesb(void __iomem *addr, const void *buffer, int count)
{
	if (count) {
		const u8 *buf = buffer;
		do {
			__raw_writeb(*buf++, addr);
		} while (--count);
	}
}
#endif

#ifndef writesw
#define writesw writesw
static inline void writesw(void __iomem *addr, const void *buffer, int count)
{
	if (count) {
		const u16 *buf = buffer;
		do {
			__raw_writew(*buf++, addr);
		} while (--count);
	}
}
#endif

#ifndef writesl
#define writesl writesl
static inline void writesl(void __iomem *addr, const void *buffer, int count)
{
	if (count) {
		const u32 *buf = buffer;
		do {
			__raw_writel(*buf++, addr);
		} while (--count);
	}
}
#endif

#ifdef CONFIG_64BIT
#ifndef writesq
#define writesq writesq
static inline void writesq(void __iomem *addr, const void *buffer, int count)
{
	if (count) {
		const u64 *buf = buffer;
		do {
			__raw_writeq(*buf++, addr);
		} while (--count);
	}
}
#endif
#endif /* CONFIG_64BIT */

#ifndef PCI_IOBASE
#define PCI_IOBASE ((void __iomem *) 0)
#endif

#ifndef IO_SPACE_LIMIT
#define IO_SPACE_LIMIT 0xffff
#endif

/*****************************************************************************/
/*
 * traditional input/output functions
 */

#ifndef inb
#define inb inb
static inline u8 inb(unsigned long addr)
{
	return readb(PCI_IOBASE + addr);
}
#endif

#ifndef inw
#define inw inw
static inline u16 inw(unsigned long addr)
{
	return readw(PCI_IOBASE + addr);
}
#endif

#ifndef inl
#define inl inl
static inline u32 inl(unsigned long addr)
{
	return readl(PCI_IOBASE + addr);
}
#endif

#ifndef outb
#define outb outb
static inline void outb(u8 b, unsigned long addr)
{
	writeb(b, PCI_IOBASE + addr);
}
#endif

#ifndef outw
#define outw outw
static inline void outw(u16 b, unsigned long addr)
{
	writew(b, PCI_IOBASE + addr);
}
#endif

#ifndef outl
#define outl outl
static inline void outl(u32 b, unsigned long addr)
{
	writel(b, PCI_IOBASE + addr);
}
#endif

#define inb_p(addr)	inb(addr)
#define inw_p(addr)	inw(addr)
#define inl_p(addr)	inl(addr)
#define outb_p(x, addr)	outb((x), (addr))
#define outw_p(x, addr)	outw((x), (addr))
#define outl_p(x, addr)	outl((x), (addr))

#ifndef insb
#define insb(addr, buffer, count) readsb(PCI_IOBASE + addr, buffer, count)
#endif

#ifndef insw
#define insw(addr, buffer, count) readsw(PCI_IOBASE + addr, buffer, count)
#endif

#ifndef insl
#define insl(addr, buffer, count) readsl(PCI_IOBASE + addr, buffer, count)
#endif

#ifndef outsb
#define outsb(addr, buffer, count) writesb(PCI_IOBASE + addr, buffer, count)
#endif

#ifndef outsw
#define outsw(addr, buffer, count) writesw(PCI_IOBASE + addr, buffer, count)
#endif

#ifndef outsl
#define outsl(addr, buffer, count) writesl(PCI_IOBASE + addr, buffer, count)
#endif

#define insb_p(port,to,len)	insb(port,to,len)
#define insw_p(port,to,len)	insw(port,to,len)
#define insl_p(port,to,len)	insl(port,to,len)

#define outsb_p(port,from,len)	outsb(port,from,len)
#define outsw_p(port,from,len)	outsw(port,from,len)
#define outsl_p(port,from,len)	outsl(port,from,len)

#ifndef CONFIG_GENERIC_IOMAP
#define ioread8(addr)		readb(addr)
#define ioread16(addr)		readw(addr)
#define ioread32(addr)		readl(addr)

#define iowrite8(v, addr)	writeb((v), (addr))
#define iowrite16(v, addr)	writew((v), (addr))
#define iowrite32(v, addr)	writel((v), (addr))

#ifndef ioread16be
#define ioread16be(addr)	__be16_to_cpu(__raw_readw(addr))
#endif

#ifndef ioread32be
#define ioread32be(addr)	__be32_to_cpu(__raw_readl(addr))
#endif

#ifndef iowrite16be
#define iowrite16be(v, addr)	__raw_writew(__cpu_to_be16(v), addr)
#endif

#ifndef iowrite32be
#define iowrite32be(v, addr)	__raw_writel(__cpu_to_be32(v), addr)
#endif

#define ioread8_rep(p, dst, count) readsb(p, dst, count)
#define ioread16_rep(p, dst, count) readsw(p, dst, count)
#define ioread32_rep(p, dst, count) readsl(p, dst, count)

#define iowrite8_rep(p, src, count) writesb(p, src, count)
#define iowrite16_rep(p, src, count) writesw(p, src, count)
#define iowrite32_rep(p, src, count) writesl(p, src, count)
#endif /* CONFIG_GENERIC_IOMAP */

#ifdef __KERNEL__

#include <linux/vmalloc.h>
#define __io_virt(x) ((void __force *) (x))

#ifndef CONFIG_GENERIC_IOMAP
struct pci_dev;
extern void __iomem *pci_iomap(struct pci_dev *dev, int bar, unsigned long max);

#ifndef pci_iounmap
#define pci_iounmap pci_iounmap
static inline void pci_iounmap(struct pci_dev *dev, void __iomem *p)
{
}
#endif
#endif /* CONFIG_GENERIC_IOMAP */

/*
 * Change virtual addresses to physical addresses and vv.
 * These are pretty trivial
 */
#ifndef virt_to_phys
#define virt_to_phys virt_to_phys
static inline unsigned long virt_to_phys(volatile void *address)
{
	return __pa((unsigned long)address);
}
#endif

#ifndef phys_to_virt
#define phys_to_virt phys_to_virt
static inline void *phys_to_virt(unsigned long address)
{
	return __va(address);
}
#endif

/*
 * Change "struct page" to physical address.
 *
 * This implementation is for the no-MMU case only... if you have an MMU
 * you'll need to provide your own definitions.
 */
#ifndef CONFIG_MMU
static inline void __iomem *ioremap(phys_addr_t offset, unsigned long size)
{
	return (void __iomem*) (unsigned long)offset;
}

#define __ioremap(offset, size, flags)	ioremap(offset, size)

#ifndef ioremap_nocache
#define ioremap_nocache ioremap
#endif

#ifndef ioremap_wc
#define ioremap_wc ioremap_nocache
#endif

static inline void iounmap(void __iomem *addr)
{
}
#endif /* CONFIG_MMU */

#ifdef CONFIG_HAS_IOPORT_MAP
#ifndef CONFIG_GENERIC_IOMAP
#ifndef ioport_map
#define ioport_map ioport_map
static inline void __iomem *ioport_map(unsigned long port, unsigned int nr)
{
	return PCI_IOBASE + (port & IO_SPACE_LIMIT);
}
#endif

#ifndef ioport_unmap
#define ioport_unmap ioport_unmap
static inline void ioport_unmap(void __iomem *p)
{
}
#endif
#else /* CONFIG_GENERIC_IOMAP */
extern void __iomem *ioport_map(unsigned long port, unsigned int nr);
extern void ioport_unmap(void __iomem *p);
#endif /* CONFIG_GENERIC_IOMAP */
#endif /* CONFIG_HAS_IOPORT_MAP */

#ifndef xlate_dev_kmem_ptr
#define xlate_dev_kmem_ptr(p)	p
#endif
#ifndef xlate_dev_mem_ptr
#define xlate_dev_mem_ptr(p)	__va(p)
#endif

#ifdef CONFIG_VIRT_TO_BUS
#ifndef virt_to_bus
static inline unsigned long virt_to_bus(volatile void *address)
{
	return ((unsigned long) address);
}

static inline void *bus_to_virt(unsigned long address)
{
	return (void *) address;
}
#endif
#endif

#ifndef memset_io
#define memset_io(a, b, c)	memset(__io_virt(a), (b), (c))
#endif

#ifndef memcpy_fromio
#define memcpy_fromio(a, b, c)	memcpy((a), __io_virt(b), (c))
#endif
#ifndef memcpy_toio
#define memcpy_toio(a, b, c)	memcpy(__io_virt(a), (b), (c))
#endif

#endif /* __KERNEL__ */

#endif /* __ASM_GENERIC_IO_H */
