#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int board[16];
enum dir {UP, DOWN, LEFT, RIGHT};
int opp[] = {DOWN, UP, RIGHT, LEFT};
typedef struct {
    int a[512];
    int top;
} stack;

stack uh = {.top = -1}; // undo history
stack rh = {.top = -1}; // redo history

void push_stack(stack *s, int n){
    s->top++;
    s->a[s->top] = n;
}

int empty(stack *s){
    return s->top < 0;
}

int pop_stack(stack *s){
    if(!empty(s)){
        int n = s->a[s->top];
        s->top--;

        return n;
    }
    puts("Stack is empty!");
    exit(1);
}

void clear_stack(stack *s){
    s->top = -1;
}

void print_board(){
    for(int y = 0; y < 4; y++){
        printf("+----+----+----+----+\n");
        for(int x = 0; x < 4; x++){
            int n = board[y*4+x];
            if (n == 0){
                printf("|    ");
            } else {
                printf("| %2d ", n);
            }
        }
        puts("|");
    }
    printf("+----+----+----+----+\n");
}

void swap_tiles(int a, int b){
    int t = board[a];
    board[a] = board[b];
    board[b] = t;
}

int find_blank(){
    int b = 0;
    while(board[b] != 0) b++;
    return b;
}

void move_tile(int d){
    int b = find_blank();
    int b2 = b;
    switch (d) {
    case UP:
        b2 += 4;
        break;
    case DOWN:
        b2 -= 4;
        break;
    case LEFT:
        b2 += 1;
        break;
    case RIGHT:
        b2 -= 1;
        break;
    default:
        exit(1);
    }
    swap_tiles(b, b2);
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

char check_move(int d){
    int b = find_blank();
    int y = b / 4;
    int x = b % 4;
    switch (d) {
    case UP:
        return y < 3;
        break;
    case DOWN:
        return y > 0;
        break;
    case LEFT:
        return x < 3;
        break;
    case RIGHT:
        return x > 0;
        break;
    default:
        exit(1);
    }

}

void player_move(int d){
    if (check_move(d)){
        move_tile(d);
        push_stack(&uh, d);
        clear_stack(&rh);
        print_board();
    } else {
        puts("Illegal move!");
    }
}

void undo(){
    if (empty(&uh)){
        puts("Undo history is empty!");
        return;
    }
    int d = pop_stack(&uh);
    push_stack(&rh, d);
    move_tile(opp[d]);
    print_board();
    return;
}

void redo(){
    if (empty(&rh)){
        puts("Redo history is empty!");
        return;
    }
    int d = pop_stack(&rh);
    push_stack(&uh, d);
    move_tile(d);
    print_board();
    return;
}

int main() {
    for(int y = 0; y < 16; y++){
        board[y] = y;
    }
    shuffle(board, 16, sizeof(board[0]));
    print_board();
}
