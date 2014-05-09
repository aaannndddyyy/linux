/*
 * Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "priv.h"

#include <subdev/fb.h>

#include <linux/mm.h>
#include <linux/types.h>
#include <linux/dma-contiguous.h>

static void
gk20a_ram_put(struct nouveau_fb *pfb, struct nouveau_mem **pmem)
{
	struct device *dev = nv_device_base(nv_device(pfb));
	struct nouveau_mem *mem = *pmem;
	int i;

	*pmem = NULL;
	if (unlikely(mem == NULL))
		return;

	for (i = 0; i < mem->size; i++) {
		struct page *page;

		if (mem->pages[i] == 0)
			break;

		page = pfn_to_page(mem->pages[i] >> PAGE_SHIFT);
		dma_release_from_contiguous(dev, page, 1);
	}

	kfree(mem->pages);
	kfree(mem);
}

static int
gk20a_ram_get(struct nouveau_fb *pfb, u64 size, u32 align, u32 ncmin,
	     u32 memtype, struct nouveau_mem **pmem)
{
	struct device *dev = nv_device_base(nv_device(pfb));
	struct nouveau_mem *mem;
	int type = memtype & 0xff;
	dma_addr_t dma_addr;
	int npages;
	int order;
	int i;

	nv_debug(pfb, "%s: size: %llx align: %x, ncmin: %x\n", __func__, size,
		 align, ncmin);

	npages = size >> PAGE_SHIFT;
	if (npages == 0)
		npages = 1;

	if (align == 0)
		align = PAGE_SIZE;
	align >>= PAGE_SHIFT;

	/* round alignment to the next power of 2, if needed */
	order = fls(align);
	if ((align & (align - 1)) == 0)
		order--;

	ncmin >>= PAGE_SHIFT;
	/*
	 * allocate pages by chunks of "align" size, otherwise we may leave
	 * holes in the contiguous memory area.
	 */
	if (ncmin == 0)
		ncmin = npages;
	else if (align > ncmin)
		ncmin = align;

	mem = kzalloc(sizeof(*mem), GFP_KERNEL);
	if (!mem)
		return -ENOMEM;

	mem->size = npages;
	mem->memtype = type;

	mem->pages = kzalloc(sizeof(dma_addr_t) * npages, GFP_KERNEL);
	if (!mem) {
		kfree(mem);
		return -ENOMEM;
	}

	while (npages) {
		struct page *pages;
		int pos = 0;

		/* don't overflow in case size is not a multiple of ncmin */
		if (ncmin > npages)
			ncmin = npages;

		pages = dma_alloc_from_contiguous(dev, ncmin, order);
		if (!pages) {
			gk20a_ram_put(pfb, &mem);
			return -ENOMEM;
		}

		dma_addr = (dma_addr_t)(page_to_pfn(pages) << PAGE_SHIFT);

		nv_debug(pfb, "  alloc count: %x, order: %x, addr: %pad\n", ncmin,
			 order, &dma_addr);

		for (i = 0; i < ncmin; i++)
			mem->pages[pos + i] = dma_addr + (PAGE_SIZE * i);

		pos += ncmin;
		npages -= ncmin;
	}

	mem->offset = (u64)mem->pages[0];

	*pmem = mem;

	return 0;
}

static int
gk20a_ram_ctor(struct nouveau_object *parent, struct nouveau_object *engine,
	      struct nouveau_oclass *oclass, void *data, u32 datasize,
	      struct nouveau_object **pobject)
{
	struct nouveau_ram *ram;
	int ret;

	ret = nouveau_ram_create(parent, engine, oclass, &ram);
	*pobject = nv_object(ram);
	if (ret)
		return ret;
	ram->type = NV_MEM_TYPE_STOLEN;
	ram->size = get_num_physpages() << PAGE_SHIFT;

	ram->get = gk20a_ram_get;
	ram->put = gk20a_ram_put;

	return 0;
}

struct nouveau_oclass
gk20a_ram_oclass = {
	.ofuncs = &(struct nouveau_ofuncs) {
		.ctor = gk20a_ram_ctor,
		.dtor = _nouveau_ram_dtor,
		.init = _nouveau_ram_init,
		.fini = _nouveau_ram_fini,
	},
};
