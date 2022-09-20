#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

char save_path[256];

// https://stackoverflow.com/a/27125283
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

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

void pretty_print_stack(stack *s){
    for (int i = 0; i <= s->top; i++){
        printf("-> %d ", s->a[i]);
    }
}

void print_board(){
    for(int y = 0; y < 4; y++){
        printf("%s+----+----+----+----+\n%s", KCYN, KNRM);
        for(int x = 0; x < 4; x++){
            int i = y*4+x;
            int n = board[i];
            if (n == 0){
                printf("%s|%s    ", KCYN, KNRM);
            } else {
                printf("%s|%s", KCYN, KNRM);
                if (n == i+1){
                    printf(" %s%2d%s ", KGRN, n, KNRM);
                } else {
                    printf(" %2d ", n);
                }
            }
        }
        printf("%s|%s\n", KCYN, KNRM);
    }
    printf("%s+----+----+----+----+\n%s", KCYN, KNRM);
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

void generate_board(){
    for(int y = 0; y < 16; y++){
        board[y] = y;
    }
    shuffle(board, 16, sizeof(board[0]));
}

void winning_board(){
    for(int y = 0; y < 14; y++){
        board[y] = y+1;
    }
    board[14] = 0;
    board[15] = 15;
}

int all(int *a, int n){
    for(int i = 0; i < n; i++){
        if (!a[i]) return 0;
    }
    return 1;
}

int swaps(){
    // computes number of swaps needed to
    // obtain current permutation from
    // sorted list
    int checked[16] = {0};
    int n = 0;
    while(!all(checked, 15)){
        int i = 0;
        while(checked[i]){
            i++;
        }
        int cl = 0;
        while (!checked[i]){
            checked[i] = 1;
            cl++;
            i = board[i];
        }
        n += cl - 1;
    }
    return n;
}

int taxicab_of_blank(){
    int b = find_blank();
    int y = b / 4;
    int x = b % 4;
    int d = 3 - x + 3 - y;
    return d;
}

int is_valid(){
    int s = swaps();
    // swap the 0 15 places to the right
    s += 15;
    int td = taxicab_of_blank();
    return !((s + td) % 2);
}

void generate_valid_board(){
    int n = 0;
    while(1){
        generate_board();
        n++;
        if(is_valid()){
            printf("Generated valid board in %d tries.\n", n);
            return;
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

int solved(){
    for (int i = 0; i < 15; i++){
        if (board[i] != i+1){
            return 0;
        }
    }
    if (board[15] != 0) return 0;
    return 1;
}

void player_move(int d){
    if (check_move(d)){
        move_tile(d);
        push_stack(&uh, d);
        clear_stack(&rh);
        print_board();
        if (solved()){
            int moves = uh.top + 1;
            printf("%sFinally! You solved the board in %d moves.%s\
                Go ahead, try again! Don't forget\
                to save your game if it's worth it.",
                   KGRN, moves, KNRM);
        }
    } else {
        printf("%sIllegal move!%s\n", KRED, KNRM);
    }
}

void undo(){
    if (empty(&uh)){
        printf("%sUndo history is empty!%s\n", KRED, KNRM);
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
        printf("%sRedo history is empty!%s\n", KRED, KNRM);
        return;
    }
    int d = pop_stack(&rh);
    push_stack(&uh, d);
    move_tile(d);
    print_board();
    return;
}

void unknown_command(){
    puts("Unknown command!");
}

void print_menu_help(){
    puts("Menu commands:");
    puts("s\tStart new game");
    puts("q\tQuit game");
}

void print_game_help(){
    puts("Game commands:");
    puts("u\tMove up");
    puts("d\tMove down");
    puts("l\tMove left");
    puts("r\tMove right");
    /* puts("a\tLet algorithm solve it"); */
    puts("s\tSave game to file");
    puts("L\tLoad game from file");
    puts("U\tUndo");
    puts("R\tRedo");
    puts("n\tNew game");
    puts("q\tQuit game");
    puts("p\tPrint game board");
    puts("P\tPrint undo/redo history");
    puts("h\tPrint this help menu");
}

void welcome(){
    puts(" _ ____    ____                _\n/ | ___|  |  _ \\ _   _ _______| | ___\n| |___ \\  | |_) | | | |_  /_  / |/ _ \\\n| |___) | |  __/| |_| |/ / / /| |  __/\n|_|____/  |_|    \\__,_/___/___|_|\\___| ");
    puts("Welcome to my implementation of 15 puzzle. \nType \"h\" for the help menu, or \"s\" if you want to figure it out yourself.");
}

void quit_game(){
    puts("See ya!");
    exit(0);
}

void print_undo_redo(){
    printf("Undo history: ");
    pretty_print_stack(&uh);
    puts("");
    printf("Redo history: ");
    pretty_print_stack(&rh);
    puts("");
}

void fprint_board(FILE* fp){
    for (int i = 0; i < 16; i++){
        fprintf(fp, "%d ", board[i]);
    }
}

void fprint_stack(FILE *fp, stack *s){
    for (int i = 0; i <= s->top; i++){
        fprintf(fp, "%d ", s->a[i]);
    }
}

void fprint_hist(FILE* fp){
    fprintf(fp, "%d %d\n", uh.top + 1, rh.top + 1);
    fprint_stack(fp, &uh);
    fputs("\n", fp);
    fprint_stack(fp, &rh);
}

void fload_board(FILE* fp){
    for (int i = 0; i < 16; i++){
        fscanf(fp, "%d", &board[i]);
    }
}

void fload_stack(FILE *fp, stack *s, int n){
    for (int i = 0; i < n; i++){
        fscanf(fp, "%d ", &s->a[i]);
    }
}

void fload_hist(FILE* fp){
    fscanf(fp, "%d %d\n", &uh.top, &rh.top);
    uh.top--;
    rh.top--;
    fload_stack(fp, &uh, uh.top+1);
    fputs("\n", fp);
    fload_stack(fp, &rh, rh.top+1);
}

void get_save_path(){
    const char *homedir = getpwuid(getuid())->pw_dir;
    const char *fname = "15_puzzle_save.txt";
    sprintf(save_path, "%s/%s", homedir, fname);
}

void save(){
    get_save_path();
    FILE* fp = fopen(save_path, "w");
    if(!fp) {
        printf("%sCan't create save file!\n%s", KRED, KNRM);
        return;
    }
    fprint_board(fp);
    fputs("\n", fp);
    fprint_hist(fp);
    fputs("\n", fp);
    fclose(fp);
    printf("%sGame saved to %s\n%s", KGRN, save_path, KNRM);
}

void load(){
    get_save_path();
    FILE* fp = fopen(save_path, "r");
    if(!fp) {
        printf("%sCan't open save file!\n%s", KRED, KNRM);
        return;
    }
    fload_board(fp);
    fload_hist(fp);
    printf("%sSave loaded from %s\n%s", KGRN, save_path, KNRM);
    print_board();
}

void menu_loop();

void game_loop(){
    clear_stack(&uh);
    clear_stack(&rh);
    generate_valid_board();
    puts("Try to solve the puzzle in as few moves as possible!");
    print_board();

    while(1){
        printf(">>> ");
        char c[256];
        if (scanf(" %255[^\n]s", c) != 1){
            unknown_command();
            continue;
        };
        if (strcmp(c,"u") == 0){
            player_move(UP);
        }
        else if (strcmp(c,"d") == 0){
            player_move(DOWN);
        }
        else if (strcmp(c,"l") == 0){
            player_move(LEFT);
        }
        else if (strcmp(c,"r") == 0){
            player_move(RIGHT);
        } else if (strcmp(c,"p") == 0){
            print_board();
        } else if (strcmp(c,"P") == 0){
            print_undo_redo();
        } else if (strcmp(c,"h") == 0){
            print_game_help();
        } else if (strcmp(c,"U") == 0){
            undo();
        } else if (strcmp(c,"R") == 0){
            redo();
        } else if (strcmp(c,"S") == 0){
            save();
        } else if (strcmp(c,"L") == 0){
            load();
        } else if (strcmp(c,"m") == 0){
            menu_loop();
        } else if (strcmp(c,"n") == 0){
            game_loop();
        } else if (strcmp(c,"q") == 0){
            quit_game();
        }
        else {
            unknown_command();
        }
    }
}

void menu_loop(){
    welcome();
    while(1){
        printf(">>> ");
        char c[256];
        if (scanf(" %255[^\n]s", c) != 1){
            unknown_command();
            continue;
        };
        if (strcmp(c,"h") == 0){
            print_menu_help();
        }
        else if (strcmp(c,"s") == 0){
            game_loop();
        }
        else if (strcmp(c,"q") == 0){
            quit_game();
        } else {
            unknown_command();
        }
    }
}

int main() {
    srand(time(NULL));
    menu_loop();
}
