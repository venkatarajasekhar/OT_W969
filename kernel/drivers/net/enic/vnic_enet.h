

#ifndef _VNIC_ENIC_H_
#define _VNIC_ENIC_H_

/* Hardware intr coalesce timer is in units of 1.5us */
#define INTR_COALESCE_USEC_TO_HW(usec) ((usec) * 2/3)
#define INTR_COALESCE_HW_TO_USEC(usec) ((usec) * 3/2)

/* Device-specific region: enet configuration */
struct vnic_enet_config {
	u32 flags;
	u32 wq_desc_count;
	u32 rq_desc_count;
	u16 mtu;
	u16 intr_timer;
	u8 intr_timer_type;
	u8 intr_mode;
	char devname[16];
	u32 intr_timer_usec;
};

#define VENETF_TSO		0x1	/* TSO enabled */
#define VENETF_LRO		0x2	/* LRO enabled */
#define VENETF_RXCSUM		0x4	/* RX csum enabled */
#define VENETF_TXCSUM		0x8	/* TX csum enabled */
#define VENETF_RSS		0x10	/* RSS enabled */
#define VENETF_RSSHASH_IPV4	0x20	/* Hash on IPv4 fields */
#define VENETF_RSSHASH_TCPIPV4	0x40	/* Hash on TCP + IPv4 fields */
#define VENETF_RSSHASH_IPV6	0x80	/* Hash on IPv6 fields */
#define VENETF_RSSHASH_TCPIPV6	0x100	/* Hash on TCP + IPv6 fields */
#define VENETF_RSSHASH_IPV6_EX	0x200	/* Hash on IPv6 extended fields */
#define VENETF_RSSHASH_TCPIPV6_EX 0x400	/* Hash on TCP + IPv6 ext. fields */

#endif /* _VNIC_ENIC_H_ */