# Chrysalix OS

Simple cooperative operating system which can run on top of Linux using
[setcontext](https://en.wikipedia.org/wiki/Setcontext). The operating
system was written way back in 2006 and I based it on [PrexOS](http://prex.sourceforge.net/).

> NOTE: You can run it on MacOS, but it looks like setcontext will be
> removed from MacOS at somepoint

## Features

* Cooperative scheduler
* Sync Mutex/Waitgroup/Semaphores
* Event
* Memory manager
* Driver support
* Arch abstraction (I had it running on an Arm Atmel board)

## Running the OS

```
$ make clean all
$ ./build/chrysalixos 
chrysalix Initialized
chrysalix kernel starting...

Welcome to ChrysalixOS 1.0.0 (ucontext)
    compiled Aug 21 2022 12:33:32


[chrysalix] # help
tests 
helloworld 
msg 
ps top kill pdump sigtest 
peek poke reset 
free mem 
drivers fds 
version cmdtest ls help 

[chrysalix] # fds
FD# Name Data
============
0 tty0 0x0

[chrysalix] # free
Mem[0]: Total=1048576  Free=1015760

[chrysalix] # mem
Memory 0
p 0x3DA26080 - z 0
p 0x3DA26090 - z 1015760

[chrysalix] # drivers
DRV# Name Table
============
0 tty0 0x3DA25CE0

[chrysalix] # tests
test_ring...OK
test_sync...OK

```

Type `<CTRL>-C` to quit.

