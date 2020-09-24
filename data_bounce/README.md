# Data Bounce

This folder contains a Linux-only implemention of our attack primitive **data bounce** from our paper 

[Store-to-Leak Forwarding](https://arxiv.org/pdf/1905.05725.pdf) by Michael Schwarz, Claudio Canella, Lukas Giner, Daniel Gruss

## How it works
This attack primitive exploits correct store-to-load forwarding. An attacker first writes a known value to an inaccessible (kernel) address and then loads the value from the same virtual address. The loaded value is then encoded in the cache where Flush+Reload can be used to determine whether the store-to-load forwarding logic forwarded the previous store to the load. If this is the case, the attacker learns that the used address is mapped, allowing an attacker to de-randomize the kernel.

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
  + **Solution #1:** Pin the program to one CPU core: `taskset 0x2 ./data_bounce`. Try different cores.
  + **Solution #2:** Manually set the variable `CACHE_MISS` in main.c to a threshold which allows distinguishing cache hits from misses.

## Warnings
**Warning #1**: We are providing this code as-is. You are responsible for protecting yourself, your property and data, and others from any risks caused by this code. This code may cause unexpected and undesirable behavior to occur on your machine.

**Warning #2**: This code is only for testing purposes. Do not run it on any productive systems. Do not run it on any system that might be used by another person or entity.