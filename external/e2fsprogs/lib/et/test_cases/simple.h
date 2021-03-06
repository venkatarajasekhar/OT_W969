
#include <et/com_err.h>

#define KRB_MK_AP_TKFIL                          (39525376L)
#define KRB_MK_AP_NOTKT                          (39525377L)
#define KRB_MK_AP_TGTEXP                         (39525378L)
#define KRB_RD_AP_UNDEC                          (39525379L)
#define KRB_RD_AP_EXP                            (39525380L)
#define KRB_RD_AP_REPEAT                         (39525381L)
#define KRB_RD_AP_NOT_US                         (39525382L)
#define KRB_RD_AP_INCON                          (39525383L)
#define KRB_RD_AP_TIME                           (39525384L)
#define KRB_RD_AP_BADD                           (39525385L)
#define KRB_RD_AP_VERSION                        (39525386L)
#define KRB_RD_AP_MSG_TYPE                       (39525387L)
#define KRB_RD_AP_MODIFIED                       (39525388L)
#define KRB_RD_AP_ORDER                          (39525389L)
#define KRB_RD_AP_UNAUTHOR                       (39525390L)
#define KRB_GT_PW_NULL                           (39525391L)
#define KRB_GT_PW_BADPW                          (39525392L)
#define KRB_GT_PW_PROT                           (39525393L)
#define KRB_GT_PW_KDCERR                         (39525394L)
#define KRB_GT_PW_NULLTKT                        (39525395L)
#define KRB_SKDC_RETRY                           (39525396L)
#define KRB_SKDC_CANT                            (39525397L)
extern const struct error_table et_krb_error_table;
extern void initialize_krb_error_table(void);

/* For compatibility with Heimdal */
extern void initialize_krb_error_table_r(struct et_list **list);

#define ERROR_TABLE_BASE_krb (39525376L)

/* for compatibility with older versions... */
#define init_krb_err_tbl initialize_krb_error_table
#define krb_err_base ERROR_TABLE_BASE_krb
