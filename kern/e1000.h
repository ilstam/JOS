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

#define E1000_TXD_CMD_EOP    0x00000001 /* End of Packet */
#define E1000_TXD_CMD_RS     0x00000008 /* Report Status */
#define E1000_TXD_STAT_DD    0x00000001 /* Descriptor Done */

#define MAX_TX_BUF_SIZE  1518
#define NUM_TX_DESC        64 /* Number of transmit descriptors -
                                 JOS's tests require no more than 64 */

/* QEMU's default MAC; corresponds to 52:54:00:12:34:56 */
#define QEMU_MAC_LOW   0x12005452
#define QEMU_MAC_HIGH  0x00005634

#define E1000_RAL      0x05400  /* Receive Address Low - RW */
#define E1000_RAH      0x05404  /* Receive Address High - RW */
#define E1000_RAH_AV   0x80000000 /* Receive descriptor valid */

#define E1000_RDBAL    0x02800  /* RX Descriptor Base Address Low - RW */
#define E1000_RDBAH    0x02804  /* RX Descriptor Base Address High - RW */
#define E1000_RDLEN    0x02808  /* RX Descriptor Length - RW */

#define E1000_RDH      0x02810  /* RX Descriptor Head - RW */
#define E1000_RDT      0x02818  /* RX Descriptor Tail - RW */

#define E1000_RCTL     0x00100  /* RX Control - RW */

#define E1000_RCTL_EN             0x00000002    /* enable */
#define E1000_RCTL_RDMTS_HALF     0x00000000    /* rx desc min threshold size */
#define E1000_RCTL_SZ_2048        0x00000000    /* rx buffer size 2048 */
#define E1000_RCTL_SECRC          0x04000000    /* Strip Ethernet CRC */

#define E1000_RXD_STAT_DD         0x01    /* Descriptor Done */

#define RX_BUF_SIZE      2048
#define NUM_RX_DESC       128 /* Number of receive descriptors -
                                 JOS's tests require at least 128 */

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


/* Receive Descriptor */
struct e1000_rx_desc {
    uint64_t addr;       /* Address of the descriptor's data buffer */
    uint16_t length;     /* Length of data DMAed into data buffer */
    uint16_t csum;       /* Packet checksum */
    uint8_t status;      /* Descriptor status */
    uint8_t errors;      /* Descriptor Errors */
    uint16_t special;
} __attribute__((packed));


int e1000_attach(struct pci_func *pcif);
int e1000_transmit(void *data, size_t len);
int e1000_receive(void *buf, size_t len);

#endif	// JOS_KERN_E1000_H
