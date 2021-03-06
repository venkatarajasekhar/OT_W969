
#ifndef __ASM_SH_UACCESS_64_H
#define __ASM_SH_UACCESS_64_H


#define __get_user_size(x,ptr,size,retval)			\
do {								\
	retval = 0;						\
	switch (size) {						\
	case 1:							\
		retval = __get_user_asm_b((void *)&x,		\
					  (long)ptr);		\
		break;						\
	case 2:							\
		retval = __get_user_asm_w((void *)&x,		\
					  (long)ptr);		\
		break;						\
	case 4:							\
		retval = __get_user_asm_l((void *)&x,		\
					  (long)ptr);		\
		break;						\
	case 8:							\
		retval = __get_user_asm_q((void *)&x,		\
					  (long)ptr);		\
		break;						\
	default:						\
		__get_user_unknown();				\
		break;						\
	}							\
} while (0)

extern long __get_user_asm_b(void *, long);
extern long __get_user_asm_w(void *, long);
extern long __get_user_asm_l(void *, long);
extern long __get_user_asm_q(void *, long);
extern void __get_user_unknown(void);

#define __put_user_size(x,ptr,size,retval)			\
do {								\
	retval = 0;						\
	switch (size) {						\
	case 1:							\
		retval = __put_user_asm_b((void *)&x,		\
					  (long)ptr);		\
		break;						\
	case 2:							\
		retval = __put_user_asm_w((void *)&x,		\
					  (long)ptr);		\
		break;						\
	case 4:							\
		retval = __put_user_asm_l((void *)&x,		\
					  (long)ptr);		\
		break;						\
	case 8:							\
		retval = __put_user_asm_q((void *)&x,		\
					  (long)ptr);		\
		break;						\
	default:						\
		__put_user_unknown();				\
	}							\
} while (0)

extern long __put_user_asm_b(void *, long);
extern long __put_user_asm_w(void *, long);
extern long __put_user_asm_l(void *, long);
extern long __put_user_asm_q(void *, long);
extern void __put_user_unknown(void);

extern long __strnlen_user(const char *__s, long __n);
extern int __strncpy_from_user(unsigned long __dest,
	       unsigned long __user __src, int __count);

#endif /* __ASM_SH_UACCESS_64_H */
