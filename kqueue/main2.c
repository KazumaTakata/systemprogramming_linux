/* This program monitors a specified file using kevent. When it sees
 * the file change it will run a specified command. It goes to some
 * limited effort to debounce, so it should hopefully only call the
 * command at a quiescent point.
 */

// @export head
#include <sys/types.h>
#include <sys/event.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <assert.h>
// @end

/* Global ID of the Kqueue */
// @export globals

// @export kqueue-declare
int kq;
// @end
// @export command-declare
char *command;
// @export timespec-declare
struct timespec debounce_timeout;
// @end

/* Loop that receives notifications of events, and runs
 * the specified program when the event fires.
 */
// @export kq_loop
void kq_loop(void)
{
    struct kevent ke;
    int r;

    while (1)
    {
        // @export kevent-receive
        r = kevent(kq,
                   /* register list */ NULL, 0,
                   /* event list */ &ke, 1,
                   /* timeout */ NULL);
        if (r == -1)
        {
            err(1, "kevent");
        }
        assert(ke.filter == EVFILT_VNODE && ke.fflags & NOTE_WRITE);
        // @end
        // @export debounce
        /* debounce */
        do
        {
            r = kevent(kq,
                       /* register list */ NULL, 0,
                       /* event list */ &ke, 1,
                       /* timeout */ &debounce_timeout);
            if (r == -1)
            {
                err(1, "kevent");
            }
        } while (r != 0);
        // @end
        // @export command-run
        system(command);
        // @end
    }
}

// @export main
int main(int argc, char **argv)
{
    int r, fd;
    // @export kevent-declare-main
    struct kevent ke;
    // @end

    if (argc != 3)
    {
        errx(1, "Usage: %s filename command", argv[0]);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        err(1, "An occurred error opening %s", argv[1]);
    }

    // @export command-init
    command = argv[2];
    // @end

    // @export timespec-init
    /* Set debounce timeout to 0.5 seconds */
    debounce_timeout.tv_sec = 0;
    debounce_timeout.tv_nsec = 500000000;
    // @end

    /* Create the kernel queue */
    // @export kqueue-create
    kq = kqueue();
    if (kq == -1)
    {
        err(1, "kq!");
    }
    // @end

    /* Add an event */
    // @export kevent-register
    EV_SET(&ke,
           /* the file we are monitoring */ fd,
           /* we monitor vnode changes */ EVFILT_VNODE,
           /* when the file is written add an event, and then clear the
              condition so it doesn't re- fire */
           EV_ADD | EV_CLEAR,
           /* just care about writes to the file */ NOTE_WRITE,
           /* don't care about value */ 0, NULL);
    r = kevent(kq, /* register list */ &ke, 1, /* event list */ NULL, 0, /* timeout */ NULL);

    if (r == -1)
    {
        err(1, "kevent failed");
    }
    // @end

    /* Go to the processing loop */
    kq_loop();
    return 0;
}