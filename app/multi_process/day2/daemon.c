#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)daemon.c    8.1 (Berkeley) 6/4/93";
#endif /* LIBC_SCCS and not lint */

#include <errno.h>
#include <fcntl.h>
#include <paths.h>
#include <unistd.h>
#include <sys/stat.h>

#include <device-nrs.h>
#include <not-cancel.h>

int
daemon(nochdir, noclose)
    int nochdir, noclose;
{
    int fd;

    switch (__fork()) {
    case -1:
        return (-1);
    case 0:
        break;
    default:
        _exit(0);
    }

    if (__setsid() == -1) //新的seesion leader，脱离terminal组
        return (-1);

    if (!nochdir)
        (void)__chdir("/");  //避免当前路径为busy

    if (!noclose) {
        struct stat64 st;
		//IO重定向到/dev/null
        if ((fd = open_not_cancel(_PATH_DEVNULL, O_RDWR, 0)) != -1
            && (__builtin_expect (__fxstat64 (_STAT_VER, fd, &st), 0)
            == 0)) {
            if (__builtin_expect (S_ISCHR (st.st_mode), 1) != 0
#if defined DEV_NULL_MAJOR && defined DEV_NULL_MINOR
                && (st.st_rdev
                == makedev (DEV_NULL_MAJOR, DEV_NULL_MINOR))
#endif
                ) {
                (void)__dup2(fd, STDIN_FILENO);
                (void)__dup2(fd, STDOUT_FILENO);
                (void)__dup2(fd, STDERR_FILENO);
                if (fd > 2)
                    (void)__close (fd);
            } else {
                /* We must set an errno value since no
                   function call actually failed.  */
                close_not_cancel_no_status (fd);
                __set_errno (ENODEV);
                return -1;
            }
        } else {
            close_not_cancel_no_status (fd);
            return -1;
        }
    }
    return (0);
}
