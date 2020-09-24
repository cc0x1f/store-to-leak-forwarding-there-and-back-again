all: echoload

echoload: main.c cacheutils.h
	@gcc main.c -o echoload -Os -Wno-unused-result

run: echoload
	@./echoload

verify:
	@sudo cat /proc/kallsyms | grep -w startup_64

clean:
	@rm -rf echoload
