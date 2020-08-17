#ifndef __CAN_H_
#define __CAN_H_

#ifdef __cplusplus
extern "C" {
#endif

/*暂定普通接口，后续修改 */

int can_open(const char *name);
void can_close(int fd);

#define CAN_FILTER_STD_INIT(id) {(id),(CAN_SFF_MASK)}
#define CAN_FILTER_EXT_INIT(id) {(id),(CAN_EFF_MASK)}
#define CAN_FILTER_USR_INIT(id,mask) {(id |= CAN_EFF_FLAG),(mask &= ~CAN_ERR_FLAG)}
//过滤反转
#define CAN_FILTER_USR_INV_INIT(id,mask) {(id |= (CAN_EFF_FLAG | CAN_INV_FILTER)),(mask &= ~CAN_ERR_FLAG)}
/*
struct can_filter filter[] =
{
    CAN_FILTER_STD_INIT(0x123),
    CAN_FILTER_EXT_INIT(0x456),
    CAN_FILTER_USR_INIT(0x789,0x700)
};
*/

#ifdef __cplusplus
}
#endif

#endif // E_CAN_H_
