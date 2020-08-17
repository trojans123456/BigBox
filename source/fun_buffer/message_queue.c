#include <string.h>

#include "message_queue.h"

void mq_init(struct message_queue *mq, void *buf, size_t bufsz)
{
    mq->mem_start = buf;
    mq->mem_end = (char *)buf + bufsz;
    mq->curr = buf;
    mq->queue_tail = mq->mem_end;
    mq->curr_sz = mq_currsz(mq);
}

struct message_item *mq_register(struct message_queue *mq, size_t nbytes)
{
    /* make queued message header */
    --(mq->queue_tail);
    mq->queue_tail->start = mq->curr;
    mq->queue_tail->size = nbytes;
    mq->queue_tail->state = MESSAGE_ITEM_UNSENT;

    /* move curr and recalculate curr_sz */
    mq->curr += nbytes;
    mq->curr_sz = mq_currsz(mq);

    return mq->queue_tail;
}

void mq_clean(struct message_queue *mq)
{
    struct message_item *new_head;

    /*收到数据时会清空并置位*/
    for(new_head = mq_get(mq, 0); new_head >= mq->queue_tail; --new_head) {
        if (new_head->state != MESSAGE_ITEM_COMPLETE) break;
    }

    /* check if everything can be removed */
    if (new_head < mq->queue_tail) {
        mq->curr = mq->mem_start;
        mq->queue_tail = mq->mem_end;
        mq->curr_sz = mq_currsz(mq);
        return;
    } else if (new_head == mq_get(mq, 0)) {
        /* do nothing */
        return;
    }

    /* move buffered data */
    size_t n = mq->curr - new_head->start;
    size_t removing = new_head->start - (uint8_t*) mq->mem_start;
    memmove(mq->mem_start, new_head->start, n);
    mq->curr = (uint8_t*)mq->mem_start + n;

    /* move queue */
    int new_tail_idx = new_head - mq->queue_tail;
    memmove(mq_get(mq, new_tail_idx), mq->queue_tail, sizeof(struct message_item) * (new_tail_idx + 1));
    mq->queue_tail = mq_get(mq, new_tail_idx);
    int i = 0;
    /* bump back start's */
    for(i = 0; i < new_tail_idx + 1; ++i) {
        mq_get(mq, i)->start -= removing;
    }

    /* get curr_sz */
    mq->curr_sz = mq_currsz(mq);
}

struct message_item *mq_find(struct message_queue *mq, find_check_ptr check, void *flag)
{
    struct message_item *curr;
    for(curr = mq_get(mq, 0); curr >= mq->queue_tail; --curr) {
        if(check)
        {
            if(check(curr,flag))
            {
                return curr;
            }
        }
    }
    return NULL;
}
