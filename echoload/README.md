# EchoLoad

This folder contains a Linux-only implemention of our attack primitive **EchoLoad** from our paper 

[KASLR: Break It, Fix It, Repeat](http://cc0x1f.net/publications/kaslr.pdf) by Claudio Canella, Michael Schwarz, Martin Haubenwallner, Martin Schwarzl and Daniel Gruss

## How it works
This attack primitive exploits that the hardware mitigations for Meltdown zero out the result of a load when it fails a permission check. An attacker simply needs to load data from an inaccessible (kernel) address and encode the returned value in the cache. As the load fails the permission check, the returned value will be zero which can be retrieved from the cache using Flush+Reload. If a checked address is not backed by physical memory, the load will instead stall and no value will be returned. This difference in behavior can be exploited to de-randomize the kernel.

## Usage
The Makefile provides all that is needed to run the executable by simply executing
```
make run
```

This will build the executable and automatically run it.
To verify whether the found addresses actually correspond to the position of the kernel you can execute the following command
```
sudo cat /proc/kallsyms | grep -w startup_64
```

For simplicity you can also simply execute 
```
make verify
```
which will execute the command.

## Troubleshooting
* No output or every address is mapped
  + **Solution #1:** Pin the program to one CPU core: `taskset 0x2 ./echoload`. Try different cores.
  + **Solution #2:** Manually set the variable `CACHE_MISS` in main.c to a threshold which allows distinguishing cache hits from misses.

* Program crashes with an illegal instruction
  + **Solution #1** By default, our implementation uses Intel TSX which is not supported on all CPUs. On those CPUs an illegal instruction exception will be thrown. To solve this you need to switch to speculation or segfault handling in main.c

## Warnings
**Warning #1**: We are providing this code as-is. You are responsible for protecting yourself, your property and data, and others from any risks caused by this code. This code may cause unexpected and undesirable behavior to occur on your machine.

**Warning #2**: This code is only for testing purposes. Do not run it on any productive systems. Do not run it on any system that might be used by another person or entity.