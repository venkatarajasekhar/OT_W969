
#include <mach/hardware.h>

#define IRQ_STATUS	0x14

	.macro  disable_fiq
	.endm

	.macro  get_irqnr_preamble, base, tmp
	.endm

	.macro  arch_ret_to_user, tmp1, tmp2
	.endm

	.macro  get_irqnr_and_base, irqnr, irqstat, base, tmp
	ldr     \irqstat, =IO_ADDRESS(GEMINI_INTERRUPT_BASE + IRQ_STATUS)
	ldr     \irqnr, [\irqstat]
	cmp     \irqnr, #0
	beq     2313f
	mov     \tmp, \irqnr
	mov     \irqnr, #0
2312:
	tst     \tmp, #1
	bne     2313f
	add     \irqnr, \irqnr, #1
	mov     \tmp, \tmp, lsr #1
	cmp     \irqnr, #31
	bcc     2312b
2313:
	.endm
