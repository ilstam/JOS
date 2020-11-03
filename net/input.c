#include "ns.h"

/* ideally this should be exported by the kernel */
#define BUF_SIZE 2048

extern union Nsipc nsipcbuf;

void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

	// LAB 6: Your code here:
	// 	- read a packet from the device driver
	//	- send it to the network server
	// Hint: When you IPC a page to the network server, it will be
	// reading from it for a while, so don't immediately receive
	// another packet in to the same physical page.

	while (1) {
		char buf[BUF_SIZE];
		int len = sys_receive_frame(buf, sizeof(buf));

		if (len == -E_RX_QUEUE_EMPTY) {
			/* let's not spin here like crazy */
			sys_yield();
			continue;
		}

		/* sys_page_alloc unmaps the page previously mapped at that
		 * address. We allocate a new page every time because the
		 * network server might not be done reading a frame before a
		 * new one arrives. If we used the same memory for nsipcbuf
		 * then its contents could be overwritten too early. */
		sys_page_alloc(0, &nsipcbuf, PTE_P | PTE_U | PTE_W);

		nsipcbuf.pkt.jp_len = len;
		memcpy(nsipcbuf.pkt.jp_data, buf, len);
		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, PTE_P | PTE_U);
	}
}
