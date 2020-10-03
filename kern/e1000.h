#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H

#include <kern/pci.h>

#define E1000_VEN_ID           0x8086
#define E1000_DEV_ID_82540EM   0x100E


int e1000_attach(struct pci_func *pcif);

#endif	// JOS_KERN_E1000_H