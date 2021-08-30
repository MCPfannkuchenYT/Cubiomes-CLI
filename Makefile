seedfinder: seedfinder.c
	gcc $^ libcubiomes.a -fwrapv -lm -o $@ -Iinclude -pthread -O3

run: seedfinder
	./$^ 0 1000 12 1 5 0 0 1

clean: seedfinder
	rm $^
