#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "selector.h"

/**
*描述一个fd的数据
*/
typedef struct
{
    int in_use;
    void *data; /*用户指定数据*/
    sel_fd_handler_t handle_read;
    sel_fd_handler_t handle_write;
    sel_fd_handler_t handle_except;
}fd_control_t;

struct timer__
{
    sel_timeout_handler_t handler; /*处理函数*/
    void *user_data;/*用户数据*/
    struct timeval timeout; /*超时时间*/
    selector_t *sel_owner; /*所属监听器*/
    int in_heap;/*是否在运行*/
    struct timer__ *left,*right,*up; /*树*/
};


struct selector__
{
    fd_control_t fds[FD_SETSIZE];

    fd_set read_set;
    fd_set write_set;
    fd_set except_set;

    int maxfd;

    struct timer__ *timer_top,*timer_last;
};


/********* global ************/
static t_signal_handler user_sighup_handler = NULL;
static t_signal_handler user_sigint_handler = NULL;
static int got_sighup = 0; /* Did I get a HUP signal? */
static int got_sigint = 0; /* Did I get an INT signal? */



/**
 * @brief 分配一个定时器
 * @param sel
 * @param handler
 * @param user_data
 * @param new_timer 返回结果
 * @return
 */
int sel_alloc_timer(selector_t *sel,
                    sel_timeout_handler_t handler,
                    void *user_data,
                    sel_timer_t **new_timer)
{
    sel_timer_t *timer;

    timer = (sel_timer_t *)calloc(1,sizeof(sel_timer_t));
    if(!timer)
        return ENOMEM;

    timer->handler = handler;
    timer->user_data = user_data;
    timer->in_heap = 0;
    timer->sel_owner = sel;

    *new_timer = timer;

    return 0;
}

/**
 * @brief 释放一个定时器
 * @param timer
 * @return
 */
int sel_free_timer(sel_timer_t *timer)
{
    if(timer->in_heap)
    {
        sel_stop_timer(timer);
    }
    free(timer);

    return 0;
}

static void find_next_pos(sel_timer_t *curr,sel_timer_t ***next,sel_timer_t **parent)
{
    unsigned int upcount = 0;

    if (curr->up && (curr->up->left == curr)) {
    /* We are a left node, the next node is just my right partner. */
    *next = &(curr->up->right);
    *parent = curr->up;
    return;
    }

    /* While we are a right node, go up. */
    while (curr->up && (curr->up->right == curr)) {
    upcount++;
    curr = curr->up;
    }

    if (curr->up) {
    /* Now we are a left node, trace up then back down. */
    curr = curr->up->right;
    upcount--;
    }
    while (upcount) {
    curr = curr->left;
    upcount--;
    }
    *next = &(curr->left);
    *parent = curr;
}

static void find_prev_elem(sel_timer_t *curr,sel_timer_t **prev)
{
    unsigned int upcount = 0;

    if (curr->up && (curr->up->right == curr)) {
    /* We are a right node, the previous node is just my left partner. */
    *prev = curr->up->left;
    return;
    }

    /* While we are a left node, go up. */
    while (curr->up && (curr->up->left == curr)) {
    upcount++;
    curr = curr->up;
    }

    if (curr->up) {
    /* Now we are a right node, trace up then back down. */
    curr = curr->up->left;
    } else {
    /* We are going to the previous "row". */
    upcount--;
    }
    while (upcount) {
    curr = curr->right;
    upcount--;
    }
    *prev = curr;
}


static void
send_up(sel_timer_t *elem, sel_timer_t **top, sel_timer_t **last)
{
    sel_timer_t *tmp1, *tmp2, *parent;

    parent = elem->up;
    while (parent && (cmp_timeval(&elem->timeout, &parent->timeout) < 0)) {
    tmp1 = elem->left;
    tmp2 = elem->right;
    if (parent->left == elem) {
        elem->left = parent;
        elem->right = parent->right;
        if (elem->right)
        elem->right->up = elem;
    } else {
        elem->right = parent;
        elem->left = parent->left;
        if (elem->left)
        elem->left->up = elem;
    }
    elem->up = parent->up;

    if (parent->up) {
        if (parent->up->left == parent) {
        parent->up->left = elem;
        } else {
        parent->up->right = elem;
        }
    } else {
        *top = elem;
    }

    parent->up = elem;
    parent->left = tmp1;
    if (parent->left)
        parent->left->up = parent;
    parent->right = tmp2;
    if (parent->right)
        parent->right->up = parent;

    if (*last == elem)
        *last = parent;

    parent = elem->up;
    }
}

static void
send_down(sel_timer_t *elem, sel_timer_t **top, sel_timer_t **last)
{
    sel_timer_t *tmp1, *tmp2, *left, *right;

    left = elem->left;
    while (left) {
    right = elem->right;
    /* Choose the smaller of the two below me to swap with. */
    if ((right) && (cmp_timeval(&left->timeout, &right->timeout) > 0)) {

        if (cmp_timeval(&elem->timeout, &right->timeout) > 0) {
        /* Swap with the right element. */
        tmp1 = right->left;
        tmp2 = right->right;
        if (elem->up) {
            if (elem->up->left == elem) {
            elem->up->left = right;
            } else {
            elem->up->right = right;
            }
        } else {
            *top = right;
        }
        right->up = elem->up;
        elem->up = right;

        right->left = elem->left;
        right->right = elem;
        elem->left = tmp1;
        elem->right = tmp2;
        if (right->left)
            right->left->up = right;
        if (elem->left)
            elem->left->up = elem;
        if (elem->right)
            elem->right->up = elem;

        if (*last == right)
            *last = elem;
        } else
        goto done;
    } else {
        /* The left element is smaller, or the right doesn't exist. */
        if (cmp_timeval(&elem->timeout, &left->timeout) > 0) {
        /* Swap with the left element. */
        tmp1 = left->left;
        tmp2 = left->right;
        if (elem->up) {
            if (elem->up->left == elem) {
            elem->up->left = left;
            } else {
            elem->up->right = left;
            }
        } else {
            *top = left;
        }
        left->up = elem->up;
        elem->up = left;

        left->left = elem;
        left->right = elem->right;
        elem->left = tmp1;
        elem->right = tmp2;
        if (left->right)
            left->right->up = left;
        if (elem->left)
            elem->left->up = elem;
        if (elem->right)
            elem->right->up = elem;

        if (*last == left)
            *last = elem;
        } else
        goto done;
    }
    left = elem->left;
    }
done:
    return;
}

static int cmp_timeval(struct timeval *tv1,struct timeval *tv2)
{
    if (tv1->tv_sec < tv2->tv_sec)
    return -1;

    if (tv1->tv_sec > tv2->tv_sec)
    return 1;

    if (tv1->tv_usec < tv2->tv_usec)
    return -1;

    if (tv1->tv_usec > tv2->tv_usec)
    return 1;

    return 0;
}


static void diff_timeval(struct timeval *dest,
         struct timeval *left,
         struct timeval *right)
{
    if (   (left->tv_sec < right->tv_sec)
    || (   (left->tv_sec == right->tv_sec)
        && (left->tv_usec < right->tv_usec)))
    {
    /* If left < right, just force to zero, don't allow negative
           numbers. */
    dest->tv_sec = 0;
    dest->tv_usec = 0;
    return;
    }

    dest->tv_sec = left->tv_sec - right->tv_sec;
    dest->tv_usec = left->tv_usec - right->tv_usec;
    while (dest->tv_usec < 0) {
    dest->tv_usec += 1000000;
    dest->tv_sec--;
    }
}

static void add_to_heap(sel_timer_t **top,sel_timer_t **last,sel_timer_t *elem)
{
    sel_timer_t **next;
    sel_timer_t *parent;

    elem->left = NULL;
    elem->right = NULL;
    elem->up = NULL;

    if(*top == NULL)
    {
        *top = elem;
        *last = elem;
        goto out;
    }

    find_next_pos(*last,&next,&parent);
    *next = elem;
    elem->up = parent;
    *last = elem;

    if(cmp_timeval(&elem->timeout,&parent->timeout) < 0)
    {
        send_up(elem,top,last);
    }
out:
    return ;
}


static void
remove_from_heap(sel_timer_t **top, sel_timer_t **last, sel_timer_t *elem)
{
    sel_timer_t *to_insert;



    /* First remove the last element from the tree, if it's not what's
       being removed, we will use it for insertion into the removal
       place. */
    to_insert = *last;
    if (! to_insert->up) {
    /* This is the only element in the heap. */
    *top = NULL;
    *last = NULL;
    goto out;
    } else {
    /* Set the new last position, and remove the item we will
           insert. */
    find_prev_elem(to_insert, last);
    if (to_insert->up->left == to_insert) {
        to_insert->up->left = NULL;
    } else {
        to_insert->up->right = NULL;
    }
    }

    if (elem == to_insert) {
    /* We got lucky and removed the last element.  We are done. */
    goto out;
    }

    /* Now stick the formerly last element into the removed element's
       position. */
    if (elem->up) {
    if (elem->up->left == elem) {
        elem->up->left = to_insert;
    } else {
        elem->up->right = to_insert;
    }
    } else {
    /* The head of the tree is being replaced. */
    *top = to_insert;
    }
    to_insert->up = elem->up;
    if (elem->left)
    elem->left->up = to_insert;
    if (elem->right)
    elem->right->up = to_insert;
    to_insert->left = elem->left;
    to_insert->right = elem->right;

    if (*last == elem)
    *last = to_insert;

    elem = to_insert;

    /* Now propigate it to the right place in the tree. */
    if (elem->up && cmp_timeval(&elem->timeout, &elem->up->timeout) < 0) {
    send_up(elem, top, last);
    } else {
    send_down(elem, top, last);
    }

 out:

    return;
}

/**
 * @brief 启动一个定时器
 * @param timer
 * @param timeout
 * @return
 */
int sel_start_timer(sel_timer_t *timer,unsigned int timeout_ms)
{
    if(timer->in_heap)
        return EBUSY;

    struct timeval timeout;
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = timeout_ms % 1000 * 1000;

    timer->timeout = timeout;
    add_to_heap(&(timer->sel_owner->timer_top),&(timer->sel_owner->timer_last),timer);
    timer->in_heap = 1;

    return 0;
}

/**
 * @brief 停止一个定时器
 * @param timer
 * @return
 */
int sel_stop_timer(sel_timer_t *timer)
{
    if(!timer->in_heap)
        return ETIMEDOUT;

    remove_from_heap(&(timer->sel->timer_top),
             &(timer->sel->timer_last),
             timer);
    timer->in_heap = 0;
    return 0;
}


void sel_set_signal_handler(int sig,sel_signal_handler_t handler)
{
    if(sig == SIGHUP)
        user_sighup_handler = handler;
    else if(sig == SIGINT)
        user_sigint_handler = handler;
}

void sel_ignore_signal(int sig)
{
    signal(sig,SIG_IGN);
}

static void sighup_handler(int sig)
{
    got_sighup = 1;
}

static void sigint_handler(int sig)
{
    got_sigint = 1;
}

void setup_signals(void)
{
    struct sigaction act;
    int              err;

    act.sa_handler = sighup_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_RESTART;
    err = sigaction(SIGHUP, &act, NULL);
    if (err) {

    }

    act.sa_handler = sigint_handler;
    /* Only handle SIGINT once. */
    act.sa_flags |= SA_RESETHAND;
    err = sigaction(SIGINT, &act, NULL);
    if (err) {

    }
}

static void init_fd(fd_control_t *fd)
{
    fd->in_use = 0;
    fd->data = NULL;
    fd->handle_read = NULL;
    fd->handle_write = NULL;
    fd->handle_except = NULL;
}

/**
 * @brief 分配一个select
 * @param new_selector 注意是二级指针
 * @return
 */
int sel_alloc_selector(selector_t **new_selector)
{
    selector_t *sel;
    int i;

    sel = (selector_t *)calloc(1,sizeof(selector_t));
    if(!sel)
        return ENOMEM;

    FD_ZERO(&sel->read_set);
    FD_ZERO(&sel->write_set);
    FD_ZERO(&sel->except_set);

    for(i = 0;i < FD_SETSIZE; i++)
    {
        init_fd(&(sel->fds[i]));
    }

    sel->maxfd = 0;
    sel->timer_top = NULL;
    sel->timer_last = NULL;

    *new_selector = sel;

    return 0;
}

static void free_heap_element(sel_timer_t *elem)
{
    if(!elem)
        return ;

    free_heap_element(elem->left);
    free_heap_element(elem->right);
    free(elem);
}

/**
 * @brief 释放一个select
 * @param new_selector
 * @return
 */
int sel_free_selector(selector_t *sel)
{
    sel_timer_t *heap;
    if(!sel)
        return -1;

    heap = sel->timer_top;

    free(sel);
    free_heap_element(heap);

    return 0;
}


/**
 * @brief 设置该fd的读写异常处理函数
 * @param a_this
 * @param fd
 * @param user_data
 * @param read_handler
 * @param write_handler
 * @param except_handler
 */
void sel_set_fd_handlers(selector_t *sel,
                         int fd,
                         void *user_data,
                         sel_fd_handler_t read_handler,
                         sel_fd_handler_t write_handler,
                         sel_fd_handler_t except_handler)
{
    sel->fds[fd].in_use = 1;
    sel->fds[fd].data = user_data;
    sel->fds[fd].handle_read = read_handler;
    sel->fds[fd].handle_write = write_handler;
    sel->fds[fd].handle_except = except_handler;

    if(fd > sel->maxfd)
    {
        sel->maxfd = fd;
    }
}

/**
 * @brief 删除该fd的处理函数，同时从监听队列中移除
 * @param a_this
 * @param fd
 */
void sel_clr_fd_handlers(selector_t *sel,int fd)
{
    init_fd(&(sel->fds[fd]));
    FD_CLR(fd,&sel->read_set);
    FD_CLR(fd,&sel->write_set);
    FD_CLR(fd,&sel->except_set);

    if(fd == sel->maxfd)
    {
        while((sel->maxfd >= 0) && (! sel->fds[sel->maxfd].in_use))
        {
            sel->maxfd--;
        }
    }
}


/**
 * @brief 对fd设置是否监听和删除
 * @param a_this
 * @param fd
 * @param state
 */

void sel_set_fd_read_handler(selector_t *sel,int fd,int state)
{
    if(SEL_FD_HANDLER_ENABLED == state)
    {
        FD_SET(fd,&(sel->read_set));
    }
    else if(SEL_FD_HANDLER_DISABLED == state)
    {
        FD_CLR(fd,&(sel->read_set));
    }
}

void sel_set_fd_write_handler(selector_t *sel,int fd,int state)
{
    if(SEL_FD_HANDLER_ENABLED == state)
    {
        FD_SET(fd,&(sel->write_set));
    }
    else if(SEL_FD_HANDLER_DISABLED == state)
    {
        FD_CLR(fd,&(sel->write_set));
    }
}

void sel_set_fd_except_handler(selector_t *sel,int fd,int state)
{
    if(SEL_FD_HANDLER_ENABLED == state)
    {
        FD_SET(fd,&(sel->except_set));
    }
    else if(SEL_FD_HANDLER_DISABLED == state)
    {
        FD_CLR(fd,&(sel->except_set));
    }
}

/**
 * @brief 仅运行一次监听器，然后返回
 * @param a_this
 */
void sel_select_once(selector_t *sel)
{
    fd_set      tmp_read_set;
    fd_set      tmp_write_set;
    fd_set      tmp_except_set;
    int         i;
    int         err;
    sel_timer_t *timer;
    struct timeval timeout, *to_time;

    if (sel->timer_top) {
    struct timeval now;

    /* 先处理timer */
    gettimeofday(&now, NULL);
    timer = sel->timer_top;
    while (cmp_timeval(&now, &timer->timeout) >= 0) {
        remove_from_heap(&(sel->timer_top),
                 &(sel->timer_last),
                 timer);

        timer->in_heap = 0;
        timer->handler(sel, timer, timer->user_data);

        timer = sel->timer_top;
        gettimeofday(&now, NULL);
        if (!timer)
        goto no_timers;
    }

    /* Calculate how long to wait now. */
    diff_timeval(&timeout, &sel->timer_top->timeout, &now);
    to_time = &timeout;
    } else {
      no_timers:
    to_time = NULL;
    }
    memcpy(&tmp_read_set, &sel->read_set, sizeof(tmp_read_set));
    memcpy(&tmp_write_set, &sel->write_set, sizeof(tmp_write_set));
    memcpy(&tmp_except_set, &sel->except_set, sizeof(tmp_except_set));
    err = select(sel->maxfd+1,
         &tmp_read_set,
         &tmp_write_set,
         &tmp_except_set,
         to_time);
    if (err == 0) {
    /* A timeout occurred. */
    } else if (err < 0) {
    /* An error occurred. */
    if (errno == EINTR) {
        /* EINTR is ok, just restart the operation. */
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
    } else {
        /* An error is bad, we need to abort. */
        syslog(LOG_ERR, "select_loop() - select: %m");
        exit(1);
    }
    } else {
    /* We got some I/O. */
    for (i=0; i<=sel->maxfd; i++) {
        if (FD_ISSET(i, &tmp_read_set)) {
        if (sel->fds[i].handle_read == NULL) {
            /* Somehow we don't have a handler for this.
               Just shut it down. */
            sel_set_fd_read_handler(sel, i, SEL_FD_HANDLER_DISABLED);
        } else {
            sel->fds[i].handle_read(i, sel->fds[i].data);
        }
        }
        if (FD_ISSET(i, &tmp_write_set)) {
        if (sel->fds[i].handle_write == NULL) {
            /* Somehow we don't have a handler for this.
               Just shut it down. */
            sel_set_fd_write_handler(sel, i, SEL_FD_HANDLER_DISABLED);
        } else {
            sel->fds[i].handle_write(i, sel->fds[i].data);
        }
        }
        if (FD_ISSET(i, &tmp_except_set)) {
        if (sel->fds[i].handle_except == NULL) {
            /* Somehow we don't have a handler for this.
               Just shut it down. */
            sel_set_fd_except_handler(sel, i, SEL_FD_HANDLER_DISABLED);
        } else {
            sel->fds[i].handle_except(i, sel->fds[i].data);
        }
        }
    }
    }

    if (got_sighup) {
    got_sighup = 0;
    if (user_sighup_handler != NULL) {
        user_sighup_handler();
    }
    }
    if (got_sigint) {
    got_sigint = 0;
    if (user_sigint_handler != NULL) {
        user_sigint_handler();
    }
    }
}

/**
 * @brief 循环监听
 * @param a_this
 */
void sel_select_loop(selector_t *a_this)
{
    for(;;)
        sel_select_once(a_this);
}
