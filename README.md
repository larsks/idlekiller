# idlekiller

Abort a program that gets stuck in `epoll_wait()` waiting for i/o.

I was trying to clone a large subversion repository from SourceForge using `git svn clone` (and `git svn fetch`). The fetch process would often get stuck waiting for data; it would loop calling `epoll_wait` for a long time before finely timing out.

By preloading `idlekiller.so`, it will give up waiting after 10 seconds (or `IK_IDLE_TIMEOUT` seconds).

This means I can get the fetch process to move forward like this:

```
until LD_PRELOAD=$PWD/idlekiller.so git svn fetch; do
  sleep 5
done
```

This will abort any time `git svn fetch` stops receiving data, wait five seconds, and try again.
