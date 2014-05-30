#ifndef __OF_IOMMU_H
#define __OF_IOMMU_H

#ifdef CONFIG_OF_IOMMU

struct iommu {
	struct list_head list;
	struct device *dev;
};

extern int of_get_dma_window(struct device_node *dn, const char *prefix,
			     int index, unsigned long *busno, dma_addr_t *addr,
			     size_t *size);

void iommu_add(struct iommu *iommu);
void iommu_del(struct iommu *iommu);

#else

static inline int of_get_dma_window(struct device_node *dn, const char *prefix,
			    int index, unsigned long *busno, dma_addr_t *addr,
			    size_t *size)
{
	return -EINVAL;
}

static inline void iommu_add(struct iommu *iommu)
{
}

static inline void iommu_del(struct iommu *iommu)
{
}

#endif	/* CONFIG_OF_IOMMU */

#endif /* __OF_IOMMU_H */
