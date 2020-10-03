#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

#define E1000_VEN_ID           0x8086
#define E1000_DEV_ID_82540EM   0x100E

#define E1000_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818  /* TX Descripotr Tail - RW */
#define E1000_TCTL     0x00400  /* TX Control - RW */
#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */

#define E1000_TCTL_EN     0x00000002    /* enable tx */
#define E1000_TCTL_PSP    0x00000008    /* pad short packets */
#define E1000_TCTL_COLD   0x003ff000    /* collision distance */


#define MAX_TX_BUF_SIZE  1518
#define NUM_TX_DESC        64 /* Number of transmit descriptors -
                                 JOS's tests require no more than 64*/

/* Legacy Transmit Descriptor */
struct e1000_tx_desc {
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	uint8_t cmd;
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));

int e1000_attach(struct pci_func *pcif);

#endif	// JOS_KERN_E1000_H
