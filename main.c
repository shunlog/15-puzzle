#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int board[16];

void print_board(){
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            printf("%d ", board[y*4+x]);
        }
    }
}

/* https://stackoverflow.com/a/6127606 */
static void shuffle(void *array, size_t n, size_t size) {
    char tmp[size];
    char *arr = array;
    size_t stride = size * sizeof(char);
    srand(time(NULL));

    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; ++i) {
            size_t rnd = (size_t) rand();
            size_t j = i + rnd / (RAND_MAX / (n - i) + 1);

            memcpy(tmp, arr + j * stride, size);
            memcpy(arr + j * stride, arr + i * stride, size);
            memcpy(arr + i * stride, tmp, size);
        }
    }
}

int main() {
    for(int y = 0; y < 16; y++){
        board[y] = y;
    }
    shuffle(board, 16, sizeof(board[0]));
    print_board();
}
