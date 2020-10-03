#include <kern/e1000.h>
#include <kern/pmap.h>

#define E1000_REG(offset) (*(volatile uint32_t *)(reg_base + offset))

struct e1000_tx_desc tx_queue[NUM_TX_DESC] __attribute__((aligned(16)));
char tx_buffers[NUM_TX_DESC][MAX_TX_BUF_SIZE];


volatile void *reg_base; /* corresponds to BAR[0] */


int e1000_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);
	reg_base = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

	/* initialize the transmit descriptors */
	for (int i = 0; i < NUM_TX_DESC; i++) {
		tx_queue[i].addr = PADDR(tx_buffers[i]);
	}

	/* transmit initialization */

	E1000_REG(E1000_TDBAH) = 0;
	E1000_REG(E1000_TDBAL) = PADDR(tx_queue);

	E1000_REG(E1000_TDLEN) = sizeof(tx_queue);

	E1000_REG(E1000_TDH) = 0;
	E1000_REG(E1000_TDT) = 0;

	E1000_REG(E1000_TCTL) = 0 | E1000_TCTL_EN | E1000_TCTL_PSP;
	E1000_REG(E1000_TCTL) |= (E1000_TCTL_COLD & (0x40 << 12));

	E1000_REG(E1000_TIPG) = 0 | 10 | 4 << 10 | 6 << 20;

	return 0;
}
