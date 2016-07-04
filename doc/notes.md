### Mac FsEvent Api

Can only tell you something updated in a directory. This will require the ability to compare whole directories.

Perhaps this could be optimized by sorting by mtime?

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