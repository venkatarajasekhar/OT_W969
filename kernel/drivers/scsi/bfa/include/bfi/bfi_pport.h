
#ifndef __BFI_PPORT_H__
#define __BFI_PPORT_H__

#include <bfi/bfi.h>
#include <defs/bfa_defs_pport.h>

#pragma pack(1)

enum bfi_fcport_h2i {
	BFI_FCPORT_H2I_ENABLE_REQ		= (1),
	BFI_FCPORT_H2I_DISABLE_REQ		= (2),
	BFI_FCPORT_H2I_SET_SVC_PARAMS_REQ	= (3),
	BFI_FCPORT_H2I_STATS_GET_REQ		= (4),
	BFI_FCPORT_H2I_STATS_CLEAR_REQ		= (5),
};

enum bfi_fcport_i2h {
	BFI_FCPORT_I2H_ENABLE_RSP		= BFA_I2HM(1),
	BFI_FCPORT_I2H_DISABLE_RSP		= BFA_I2HM(2),
	BFI_FCPORT_I2H_SET_SVC_PARAMS_RSP	= BFA_I2HM(3),
	BFI_FCPORT_I2H_STATS_GET_RSP		= BFA_I2HM(4),
	BFI_FCPORT_I2H_STATS_CLEAR_RSP		= BFA_I2HM(5),
	BFI_FCPORT_I2H_EVENT			= BFA_I2HM(6),
};

struct bfi_fcport_req_s {
	struct bfi_mhdr_s  mh;		/*  msg header			    */
	u32	   msgtag;	/*  msgtag for reply		    */
};

struct bfi_fcport_rsp_s {
	struct bfi_mhdr_s  mh;		/*  common msg header		    */
	u8		   status;	/*  port enable status		    */
	u8		   rsvd[3];
	u32	   msgtag;	/*  msgtag for reply		    */
};

struct bfi_fcport_enable_req_s {
	struct bfi_mhdr_s  mh;		/*  msg header			    */
	u32	   rsvd1;
	wwn_t		   nwwn;	/*  node wwn of physical port	    */
	wwn_t		   pwwn;	/*  port wwn of physical port	    */
	struct bfa_pport_cfg_s port_cfg; /*  port configuration	    */
	union bfi_addr_u   stats_dma_addr; /*  DMA address for stats	    */
	u32	   msgtag;	/*  msgtag for reply		    */
	u32	   rsvd2;
};

struct bfi_fcport_set_svc_params_req_s {
	struct bfi_mhdr_s  mh;		/*  msg header */
	u16	   tx_bbcredit;	/*  Tx credits */
	u16	   rsvd;
};

struct bfi_fcport_event_s {
	struct bfi_mhdr_s 	mh;	/*  common msg header */
	struct bfa_pport_link_s	link_state;
};

union bfi_fcport_h2i_msg_u {
	struct bfi_mhdr_s			*mhdr;
	struct bfi_fcport_enable_req_s		*penable;
	struct bfi_fcport_req_s			*pdisable;
	struct bfi_fcport_set_svc_params_req_s	*psetsvcparams;
	struct bfi_fcport_req_s			*pstatsget;
	struct bfi_fcport_req_s			*pstatsclear;
};

union bfi_fcport_i2h_msg_u {
	struct bfi_msg_s			*msg;
	struct bfi_fcport_rsp_s			*penable_rsp;
	struct bfi_fcport_rsp_s			*pdisable_rsp;
	struct bfi_fcport_rsp_s			*psetsvcparams_rsp;
	struct bfi_fcport_rsp_s			*pstatsget_rsp;
	struct bfi_fcport_rsp_s			*pstatsclear_rsp;
	struct bfi_fcport_event_s		*event;
};

#pragma pack()

#endif /* __BFI_PPORT_H__ */
