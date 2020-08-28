

/* mmc_cmds.c */
/**
 * @brief CSD寄存器 card specific data 卡的必要数据
 * @param nargs
 * @param argv
 * @return
 */
int do_read_extcsd(int nargs, char **argv);
int do_write_extcsd(int nargs, char **argv);

/**
 * @brief 写保护
 * @param nargs
 * @param argv
 * @return
 */
int do_writeprotect_get(int nargs, char **argv);
int do_writeprotect_set(int nargs, char **argv);
int do_disable_512B_emulation(int nargs, char **argv);
int do_write_boot_en(int nargs, char **argv);
int do_write_bkops_en(int nargs, char **argv);
int do_hwreset_en(int nargs, char **argv);
int do_hwreset_dis(int nargs, char **argv);
int do_sanitize(int nargs, char **argv);
int do_status_get(int nargs, char **argv);
int do_enh_area_set(int nargs, char **argv);
int do_vendor_cmd(int nargs, char **argv);
