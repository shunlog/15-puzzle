main: main.c
	gcc -g -Wall -Wextra -pedantic -std=c11 -O0 -Wno-unused-result -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op -Wshift-overflow=2 -Wduplicated-cond -Wcast-qual -Wcast-align -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -fsanitize=address -fsanitize=undefined -fno-sanitize-recover -fstack-protector main.c -lm -o main

run: main
	./main < in.txt
