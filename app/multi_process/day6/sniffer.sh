mv /tmp/UNIX.domain /tmp/UNIX.domain.original
socat -t100 -x -v UNIX-LISTEN:/tmp/UNIX.domain,mode=777,reuseaddr,fork UNIX-CONNECT:/tmp/UNIX.domain.original
