#ifndef __MESSAGE_QUEUE_H
#define __MESSAGE_QUEUE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

enum message_item_state
{
    MESSAGE_ITEM_UNSENT, /* not send */
    MESSAGE_ITEM_AWAITING_ACK, /* wait ack */
    MESSAGE_ITEM_COMPLETE /*complete */
};

/**
 * @brief 消息队列中的条目
 */
struct message_item
{
    uint8_t *start;/*消息起始地址*/
    size_t size; /*消息内容大小*/
    enum message_item_state state;
};

/**
 * @brief 消息队列
 */
struct message_queue
{
    void *mem_start; /*队列内存起始地址*/
    void *mem_end; /*队列内存结束地址*/
    uint8_t *curr; /*指向缓冲区中可以将字节打包的位置*/
    size_t curr_sz; /*可以写入的字节数*/
    struct message_item *queue_tail;
};

/**
 * @brief 初始化一个message 队列
 * @ingroup details
 *
 * @param[out] mq The message queue to initialize.
 * @param[in] buf The buffer for this message queue.
 * @param[in] bufsz The number of bytes in the buffer.
 *
 * @relates mqtt_message_queue
 */
void mq_init(struct message_queue *mq, void *buf, size_t bufsz);

/**
 * @brief Clear as many messages from the front of the queue as possible.
 * @ingroup details
 *
 * @note Calls to this function are the \em only way to remove messages from the queue.
 *
 * @param mq The message queue.
 *
 *
 */
void mq_clean(struct message_queue *mq);

/**
 * @brief Register a message that was just added to the buffer.
 * @ingroup details
 *
 * @note This function should be called immediately following a call to a packer function
 *       that returned a positive value. The positive value (number of bytes packed) should
 *       be passed to this function.
 *
 * @param mq The message queue.
 * @param[in] nbytes The number of bytes that were just packed.
 *
 * @note This function will step mqtt_message_queue::curr and update mqtt_message_queue::curr_sz.
 * @relates mqtt_message_queue
 *
 * @returns The newly added struct mqtt_queued_message.
 */
struct message_item* mq_register(struct message_queue *mq, size_t nbytes);


/**
 * @brief Find a message in the message queue.
 * @ingroup details
 *
 * @param mq The message queue.
 * @param[in] control_type The control type of the message you want to find.
 * @param[in] packet_id The packet ID of the message you want to find. Set to \c NULL if you
 *            don't want to specify a packet ID.
 *
 * @relates mqtt_message_queue
 * @returns The found message. \c NULL if the message was not found.
 */

typedef bool (*find_check_ptr)(struct message_item *item,void *flag);
struct message_item* mq_find(struct message_queue *mq, find_check_ptr check,void *flag);

/**
 * @brief Returns the mqtt_queued_message at \p index.
 * @ingroup details
 *
 * @param mq_ptr A pointer to the message queue.
 * @param index The index of the message.
 *
 * @returns The mqtt_queued_message at \p index.
 */
#define mq_get(mq_ptr, index) (((struct message_item*) ((mq_ptr)->mem_end)) - 1 - index)

/**
 * @brief Returns the number of messages in the message queue, \p mq_ptr.
 * @ingroup details
 */
#define mq_length(mq_ptr) (((struct message_item*) ((mq_ptr)->mem_end)) - (mq_ptr)->queue_tail)

/**
 * @brief Used internally to recalculate the \c curr_sz.
 * @ingroup details
 */
#define mq_currsz(mq_ptr) (mq_ptr->curr >= (uint8_t*) ((mq_ptr)->queue_tail - 1)) ? 0 : ((uint8_t*) ((mq_ptr)->queue_tail - 1)) - (mq_ptr)->curr


#define mq_try_pack(mq,tmp,item,pack_call)                          \
    tmp = pack_call;                                                \
    if (tmp < 0) {                                                  \
        return tmp;                                                 \
    } else if (tmp == 0) {                                          \
        mq_clean(&mq);                                              \
        tmp = pack_call;                                            \
        if (tmp < 0) {                                              \
            return tmp;                                             \
        } else if(tmp == 0) {                                       \
            return -1;                                              \
        }                                                           \
    }                                                               \
    item = mq_register(&mq, tmp);




#ifdef __cplusplus
}
#endif

#endif

