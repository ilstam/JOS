#include "ns.h"

extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver

	while (1) {
		int request = ipc_recv(NULL, &nsipcbuf, NULL);

		if (request != NSREQ_OUTPUT)
			continue;

		/* in case of any error simply drop the packet */
		sys_transmit_frame(nsipcbuf.pkt.jp_data, nsipcbuf.pkt.jp_len);
	}
}
