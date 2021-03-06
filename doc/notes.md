### Mac FsEvent Api

Can only tell you something updated in a directory. This will require the ability to compare whole directories.

Perhaps this could be optimized by sorting by mtime?

### Mac Kernel

It may be possible to trace system calls for file writes. This could be useful if we wanted to backup only file diffs and not entire files.

```
sudo dtrace \
  -n 'syscall::open:entry { self->pid = pid; self->file = arg0; printf("%d %s", pid, copyinstr(arg0)); }' \
  -n 'syscall::open:return /self->file/{ printf("%d %d %s", pid, arg2, copyinstr(self->file)); self->file = 0; }' \
  -n 'syscall::close:entry { printf("%d %d", pid, arg0); }' \
  -n 'syscall::write:entry { printf("%d %d %d", pid, arg0, arg2); }' \
  -n 'syscall::pwrite:entry { printf("%d %d %d", pid, arg0, arg2); }'
```

* Header `/usr/include/dtrace.h`
* http://www.osdevcon.org/2008/files/osdevcon2008-petr.pdf
* http://stackoverflow.com/questions/28011495/writing-a-dtrace-consumer-in-c
* http://unix.stackexchange.com/questions/275175/error-on-enabled-probe-syscallopen-nocancelentry-invalid-user-access-in-ac

Procs can block dtrace. Access errors can be related to El Capitan and its System Integrity Protection (csrutil status).

The following code would disable dtrace access.

```
ptrace(PT_DENY_ATTACH, 0, 0, 0);
```

Which was more than likely implemented to protect iTunes DRM.

### Linux iNotify Api

Is not recursive. Need to add a watch for all directories. Will tell you exactly what file changed though. Uses "watch descriptors" instead of file descriptors. There is a default limit of 8192 watch descriptors. Set in `/proc/sys/fs/inotify/max_user_watches`.

### Data Model

#### Record Database

Tab delimited compressed text file. One record per row. Fields:

```
PATH TIMESTAMP SIZE CHECKSUM
```

* PATH - Relative file path from the root backup directory.
* TIMESTAMP - Time of file creation or modification, whichever is greater.
* SIZE - File size in bytes.
* CHECKSUM - Result of file bytes put though a SHA256 hash function.
