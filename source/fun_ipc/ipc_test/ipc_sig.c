
void signal_install(int signum,void (*handler)(int),struct sigaction *old,bool add)
{
    struct sigaction s;
    struct sigaction *act = NULL;

    sigaction(signum,NULL,&s);

    if (add) {
        /* dfl 默认 */
        if (s.sa_handler == SIG_DFL) { /* Do not override existing custom signal handlers */
            memcpy(old, &s, sizeof(struct sigaction));
            s.sa_handler = handler;
            s.sa_flags = 0;
            act = &s;
        }
    }
    else if (s.sa_handler == handler) { /* Do not restore if someone modified our handler */
        act = old;
    }

    if (act != NULL)
        sigaction(signum, act, NULL);
}

static void ignore_signal(int signum, bool ignore)
{
    struct sigaction s;
    void *new_handler = NULL;

    sigaction(signum, NULL, &s);

    if (ignore) {
        if (s.sa_handler == SIG_DFL) /* Ignore only if there isn't any custom handler */
            new_handler = SIG_IGN;
    } else {
        if (s.sa_handler == SIG_IGN) /* Restore only if noone modified our SIG_IGN */
            new_handler = SIG_DFL;
    }

    if (new_handler) {
        s.sa_handler = new_handler;
        s.sa_flags = 0;
        sigaction(signum, &s, NULL);
    }
}
