

#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/cpufreq.h>
#include <linux/io.h>

#include <mach/map.h>
#include <mach/regs-mem.h>
#include <mach/regs-clock.h>

#include <plat/cpu-freq-core.h>

void s3c2410_cpufreq_setrefresh(struct s3c_cpufreq_config *cfg)
{
	struct s3c_cpufreq_board *board = cfg->board;
	unsigned long refresh;
	unsigned long refval;

	/* Reduce both the refresh time (in ns) and the frequency (in MHz)
	 * down to ensure that we do not overflow 32 bit numbers.
	 *
	 * This should work for HCLK up to 133MHz and refresh period up
	 * to 30usec.
	 */

	refresh = (cfg->freq.hclk / 100) * (board->refresh / 10);
	refresh = DIV_ROUND_UP(refresh, (1000 * 1000)); /* apply scale  */
	refresh = (1 << 11) + 1 - refresh;

	s3c_freq_dbg("%s: refresh value %lu\n", __func__, refresh);

	refval = __raw_readl(S3C2410_REFRESH);
	refval &= ~((1 << 12) - 1);
	refval |= refresh;
	__raw_writel(refval, S3C2410_REFRESH);
}

void s3c2410_set_fvco(struct s3c_cpufreq_config *cfg)
{
	__raw_writel(cfg->pll.index, S3C2410_MPLLCON);
}
