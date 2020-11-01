#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>
#include <inc/error.h>

#define E1000_REG(offset) (*(volatile uint32_t *)(reg_base + offset))

struct e1000_tx_desc tx_queue[NUM_TX_DESC] __attribute__((aligned(16)));
char tx_buffers[NUM_TX_DESC][MAX_TX_BUF_SIZE];

struct e1000_rx_desc rx_queue[NUM_RX_DESC] __attribute__((aligned(16)));
char rx_buffers[NUM_RX_DESC][RX_BUF_SIZE];

volatile void *reg_base; /* corresponds to BAR[0] */


int e1000_attach(struct pci_func *pcif)
{
	pci_func_enable(pcif);
	reg_base = mmio_map_region(pcif->reg_base[0], pcif->reg_size[0]);

	/* transmit initialization */

	for (int i = 0; i < NUM_TX_DESC; i++) {
		tx_queue[i].addr = PADDR(tx_buffers[i]);
	}

	E1000_REG(E1000_TDBAH) = 0;
	E1000_REG(E1000_TDBAL) = PADDR(tx_queue);

	E1000_REG(E1000_TDLEN) = sizeof(tx_queue);

	E1000_REG(E1000_TDH) = 0;
	E1000_REG(E1000_TDT) = 0;

	E1000_REG(E1000_TCTL) = 0 | E1000_TCTL_EN | E1000_TCTL_PSP;
	E1000_REG(E1000_TCTL) |= (E1000_TCTL_COLD & (0x40 << 12));

	E1000_REG(E1000_TIPG) = 0 | 10 | 4 << 10 | 6 << 20;

	/* receive initialization */

	for (int i = 0; i < NUM_RX_DESC; i++) {
		rx_queue[i].addr = PADDR(rx_buffers[i]);
	}

	E1000_REG(E1000_RAL) = QEMU_MAC_LOW;
	E1000_REG(E1000_RAH) = QEMU_MAC_HIGH | E1000_RAH_AV;

	E1000_REG(E1000_RDBAH) = 0;
	E1000_REG(E1000_RDBAL) = PADDR(rx_queue);

	E1000_REG(E1000_RDLEN) = sizeof(rx_queue);

	E1000_REG(E1000_RDH) = 0;
	E1000_REG(E1000_RDT) = NUM_RX_DESC - 1;

	E1000_REG(E1000_RCTL) = (0 | E1000_RCTL_EN | E1000_RCTL_RDMTS_HALF |
	                         E1000_RCTL_SZ_2048 | E1000_RCTL_SECRC);

	return 0;
}

int e1000_transmit(void *data, size_t len)
{
	struct e1000_tx_desc *next = &tx_queue[E1000_REG(E1000_TDH)];

	if (len > MAX_TX_BUF_SIZE)
		return -E_FRAME_TOO_LARGE;

	if (!(next->status & E1000_TXD_STAT_DD) && next->cmd & E1000_TXD_CMD_RS) {
		/* This frame hasn't been processed by the hardware yet. */
		return -E_TX_QUEUE_FULL;
	}

	memcpy(tx_buffers[E1000_REG(E1000_TDT)], data, len);
	next->length = len;
	next->cmd |= E1000_TXD_CMD_RS | E1000_TXD_CMD_EOP;
	next->status &= ~E1000_TXD_STAT_DD;

	E1000_REG(E1000_TDT) = (E1000_REG(E1000_TDT) + 1) % NUM_TX_DESC;

	return 0;
}

int e1000_receive(void *buf, size_t len)
{
	int next_idx = (E1000_REG(E1000_RDT) + 1) % NUM_RX_DESC;
	struct e1000_rx_desc *next = &rx_queue[next_idx];

	if (!(next->status & E1000_RXD_STAT_DD))
		return -E_RX_QUEUE_EMPTY;

	if (next->length > len)
		return -E_FRAME_TOO_LARGE;

	memcpy(buf, rx_buffers[next_idx], next->length);
	next->status &= ~E1000_RXD_STAT_DD;

	E1000_REG(E1000_RDT) = next_idx;

	return next->length;
}
