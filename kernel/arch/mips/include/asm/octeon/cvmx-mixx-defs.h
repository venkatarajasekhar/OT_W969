

#ifndef __CVMX_MIXX_DEFS_H__
#define __CVMX_MIXX_DEFS_H__

#define CVMX_MIXX_BIST(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100078ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_CTL(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100020ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_INTENA(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100050ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_IRCNT(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100030ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_IRHWM(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100028ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_IRING1(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100010ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_IRING2(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100018ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_ISR(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100048ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_ORCNT(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100040ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_ORHWM(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100038ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_ORING1(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100000ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_ORING2(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100008ull + (((offset) & 1) * 2048))
#define CVMX_MIXX_REMCNT(offset) \
	 CVMX_ADD_IO_SEG(0x0001070000100058ull + (((offset) & 1) * 2048))

union cvmx_mixx_bist {
	uint64_t u64;
	struct cvmx_mixx_bist_s {
		uint64_t reserved_4_63:60;
		uint64_t mrqdat:1;
		uint64_t ipfdat:1;
		uint64_t irfdat:1;
		uint64_t orfdat:1;
	} s;
	struct cvmx_mixx_bist_s cn52xx;
	struct cvmx_mixx_bist_s cn52xxp1;
	struct cvmx_mixx_bist_s cn56xx;
	struct cvmx_mixx_bist_s cn56xxp1;
};

union cvmx_mixx_ctl {
	uint64_t u64;
	struct cvmx_mixx_ctl_s {
		uint64_t reserved_8_63:56;
		uint64_t crc_strip:1;
		uint64_t busy:1;
		uint64_t en:1;
		uint64_t reset:1;
		uint64_t lendian:1;
		uint64_t nbtarb:1;
		uint64_t mrq_hwm:2;
	} s;
	struct cvmx_mixx_ctl_s cn52xx;
	struct cvmx_mixx_ctl_s cn52xxp1;
	struct cvmx_mixx_ctl_s cn56xx;
	struct cvmx_mixx_ctl_s cn56xxp1;
};

union cvmx_mixx_intena {
	uint64_t u64;
	struct cvmx_mixx_intena_s {
		uint64_t reserved_7_63:57;
		uint64_t orunena:1;
		uint64_t irunena:1;
		uint64_t data_drpena:1;
		uint64_t ithena:1;
		uint64_t othena:1;
		uint64_t ivfena:1;
		uint64_t ovfena:1;
	} s;
	struct cvmx_mixx_intena_s cn52xx;
	struct cvmx_mixx_intena_s cn52xxp1;
	struct cvmx_mixx_intena_s cn56xx;
	struct cvmx_mixx_intena_s cn56xxp1;
};

union cvmx_mixx_ircnt {
	uint64_t u64;
	struct cvmx_mixx_ircnt_s {
		uint64_t reserved_20_63:44;
		uint64_t ircnt:20;
	} s;
	struct cvmx_mixx_ircnt_s cn52xx;
	struct cvmx_mixx_ircnt_s cn52xxp1;
	struct cvmx_mixx_ircnt_s cn56xx;
	struct cvmx_mixx_ircnt_s cn56xxp1;
};

union cvmx_mixx_irhwm {
	uint64_t u64;
	struct cvmx_mixx_irhwm_s {
		uint64_t reserved_40_63:24;
		uint64_t ibplwm:20;
		uint64_t irhwm:20;
	} s;
	struct cvmx_mixx_irhwm_s cn52xx;
	struct cvmx_mixx_irhwm_s cn52xxp1;
	struct cvmx_mixx_irhwm_s cn56xx;
	struct cvmx_mixx_irhwm_s cn56xxp1;
};

union cvmx_mixx_iring1 {
	uint64_t u64;
	struct cvmx_mixx_iring1_s {
		uint64_t reserved_60_63:4;
		uint64_t isize:20;
		uint64_t reserved_36_39:4;
		uint64_t ibase:33;
		uint64_t reserved_0_2:3;
	} s;
	struct cvmx_mixx_iring1_s cn52xx;
	struct cvmx_mixx_iring1_s cn52xxp1;
	struct cvmx_mixx_iring1_s cn56xx;
	struct cvmx_mixx_iring1_s cn56xxp1;
};

union cvmx_mixx_iring2 {
	uint64_t u64;
	struct cvmx_mixx_iring2_s {
		uint64_t reserved_52_63:12;
		uint64_t itlptr:20;
		uint64_t reserved_20_31:12;
		uint64_t idbell:20;
	} s;
	struct cvmx_mixx_iring2_s cn52xx;
	struct cvmx_mixx_iring2_s cn52xxp1;
	struct cvmx_mixx_iring2_s cn56xx;
	struct cvmx_mixx_iring2_s cn56xxp1;
};

union cvmx_mixx_isr {
	uint64_t u64;
	struct cvmx_mixx_isr_s {
		uint64_t reserved_7_63:57;
		uint64_t orun:1;
		uint64_t irun:1;
		uint64_t data_drp:1;
		uint64_t irthresh:1;
		uint64_t orthresh:1;
		uint64_t idblovf:1;
		uint64_t odblovf:1;
	} s;
	struct cvmx_mixx_isr_s cn52xx;
	struct cvmx_mixx_isr_s cn52xxp1;
	struct cvmx_mixx_isr_s cn56xx;
	struct cvmx_mixx_isr_s cn56xxp1;
};

union cvmx_mixx_orcnt {
	uint64_t u64;
	struct cvmx_mixx_orcnt_s {
		uint64_t reserved_20_63:44;
		uint64_t orcnt:20;
	} s;
	struct cvmx_mixx_orcnt_s cn52xx;
	struct cvmx_mixx_orcnt_s cn52xxp1;
	struct cvmx_mixx_orcnt_s cn56xx;
	struct cvmx_mixx_orcnt_s cn56xxp1;
};

union cvmx_mixx_orhwm {
	uint64_t u64;
	struct cvmx_mixx_orhwm_s {
		uint64_t reserved_20_63:44;
		uint64_t orhwm:20;
	} s;
	struct cvmx_mixx_orhwm_s cn52xx;
	struct cvmx_mixx_orhwm_s cn52xxp1;
	struct cvmx_mixx_orhwm_s cn56xx;
	struct cvmx_mixx_orhwm_s cn56xxp1;
};

union cvmx_mixx_oring1 {
	uint64_t u64;
	struct cvmx_mixx_oring1_s {
		uint64_t reserved_60_63:4;
		uint64_t osize:20;
		uint64_t reserved_36_39:4;
		uint64_t obase:33;
		uint64_t reserved_0_2:3;
	} s;
	struct cvmx_mixx_oring1_s cn52xx;
	struct cvmx_mixx_oring1_s cn52xxp1;
	struct cvmx_mixx_oring1_s cn56xx;
	struct cvmx_mixx_oring1_s cn56xxp1;
};

union cvmx_mixx_oring2 {
	uint64_t u64;
	struct cvmx_mixx_oring2_s {
		uint64_t reserved_52_63:12;
		uint64_t otlptr:20;
		uint64_t reserved_20_31:12;
		uint64_t odbell:20;
	} s;
	struct cvmx_mixx_oring2_s cn52xx;
	struct cvmx_mixx_oring2_s cn52xxp1;
	struct cvmx_mixx_oring2_s cn56xx;
	struct cvmx_mixx_oring2_s cn56xxp1;
};

union cvmx_mixx_remcnt {
	uint64_t u64;
	struct cvmx_mixx_remcnt_s {
		uint64_t reserved_52_63:12;
		uint64_t iremcnt:20;
		uint64_t reserved_20_31:12;
		uint64_t oremcnt:20;
	} s;
	struct cvmx_mixx_remcnt_s cn52xx;
	struct cvmx_mixx_remcnt_s cn52xxp1;
	struct cvmx_mixx_remcnt_s cn56xx;
	struct cvmx_mixx_remcnt_s cn56xxp1;
};

#endif
