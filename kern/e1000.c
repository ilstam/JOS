#include <kern/e1000.h>
#include <kern/pmap.h>

volatile void *reg_base; /* corresponds to BAR[0] */


// LAB 6: Your driver code here
int e1000_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);
	reg_base = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

	return 0;
}
