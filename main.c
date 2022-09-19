#include <stdio.h>
#include <stdlib.h>
#include <time.h>

short board[4][4];

void print_board(){
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            printf("%d ", board[y][x]);
        }
    }
}

int main() {
    srand(time(NULL));
    for(int y = 0; y < 4; y++){
        for(int x = 0; x < 4; x++){
            board[y][x] = rand();
        }
    }
    print_board();
}
