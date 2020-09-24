all: data_bounce

data_bounce: main.c cacheutils.h
	@gcc main.c -o data_bounce -Os -Wno-unused-result

run: data_bounce
	@./data_bounce

verify:
	@sudo cat /proc/kallsyms | grep -w startup_64

clean:
	@rm -rf data_bounce
