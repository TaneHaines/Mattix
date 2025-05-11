#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <termios.h>
#include <unistd.h>

struct GameStats{
    int game_status;
    int p1_score;
    int p2_score;
    int *pegs;
    int len;
} stats;

void press_enter_to_continue() {
    struct termios oldt, newt;
    printf("Press Enter to continue...");
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    while (getchar() != '\n');

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

char* to_char(int num) {
    int length = snprintf(NULL, 0, "%d", num); 
    char* result = malloc(length + 1); 

    if (result) snprintf(result, length + 1, "%d", num); 
    return result;
}

void fisher_yates_shuffle(int *list, int len) {
    for (int i = len - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int temp = list[i];
        list[i] = list[j];
        list[j] = temp;
    }
}

int linear_search(int *list, int len, int f) {
    for(int i = 0; i <= len; ++i) {
        if (list[i] == f) return i;
    }
    return -1;
}

char* board_string(int *list, int len) {
    char *board = malloc(len * 6 + (len / 6));
    if (!board) {
        return NULL;
    }
    
    int board_index = 0;
    
    for (int i = 0; i < len; i++) {
        if (i != 0 && i % 6 == 0) {
            board[board_index++] = '\n';
        }

        char* num_str = to_char(list[i]);
        int j = 0;
        while (num_str[j] != '\0') {
            board[board_index++] = num_str[j++];
        }
        free(num_str);

        if (i != len) {
            for (int j = 0; j < 4; j++) {
                board[board_index++] = ' ';
            }
        }
    }
    board[board_index++] = '\n';
    board[board_index] = '\0';
    
    return board;
}

int* get_valid(int *list, int len, int dir, int player_pos) {
    int *available = malloc(6*sizeof(int)); 
    int position;
    int av_pos = 0;
    if (dir == 2) {
        for (int i = 0; i < 6; ++i) {
            position = player_pos-player_pos%6+i;
            if(list[position] != -99 && list[position] != 0) {
                available[av_pos] = position+1;
                ++av_pos;
            }
        }
    } else {
        for (int i = player_pos % 6; i < len; i += 6) {
            if(list[i] != -99 && list[i] != 0) {
                available[av_pos] = i+1; 
                ++av_pos;
            }
        }
    }
    return available;
}

int in_selected(int *list, int len, int selected) {
    for(int i = 0; i < len; ++i) {
        if (list[i] == selected) return 1;
    }
    return 0;
}

void game() {
    int len = 36;
    int player = 0;
    int player_pos;
    int dir = 0;
    int p1_score;
    int p2_score;
    int selected;
    p1_score = p2_score = 0;

    int *pegs = malloc(len*sizeof(int));
    int temp[] = {1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,1,2,3,8,9,9,10,-1,-1,-2,-2,-3,-3,-4,-5,-6,-7,-8,-9,-10,0};
    memcpy(pegs, temp, len*sizeof(int));
    
    srand(time(NULL));
    fisher_yates_shuffle(pegs, len);

    while (player != 1 && player != 2) {
        printf("Who is going first? (1, 2): ");
        scanf("%d", &player);

        while (getchar() != '\n');
    }
    while (dir != 1 && dir != 2) {
        printf("Which way? 1 for vertical, 2 for horizontal: ");
        scanf("%d", &dir);

        while (getchar() != '\n');
    }

    printf("Player %d will start.\n", player);

    while(1) {
        int selected = -1;
        
        char* board = board_string(pegs, len);
        printf("Player 1 Score: %d\nPlayer 2 Score: %d\nBoard:\n%s\n", p1_score, p2_score, board);
        free(board);
        
        player_pos = linear_search(pegs, len, 0);
        int *available = get_valid(pegs, len, dir, player_pos);

        int empty_count = 0;
        while (!in_selected(available, 6, selected)) {
            printf("Where next? \n");
            for (int i = 0; i < 6; ++i) {
                if(available[i]) {
                    printf("%d ", available[i]);
                } else {
                    ++empty_count;
                }
            } if (empty_count == 6) break;
            printf("\n");
            scanf("%d", &selected);

            while (getchar() != '\n');
        } --selected;
        if (empty_count == 6) break;
        printf("%d", empty_count);

        if (player == 2) {
            --player;
            p2_score += pegs[selected];
        }
        else { 
            ++player;
            p1_score += pegs[selected]; 
        } 
        
        if (dir-1) --dir; else ++dir;

        pegs[player_pos] = -99;
        pegs[selected] = 0;
        player_pos = selected;   
        
        free(available);
    }
    system("clear");
    printf("Player 1 Scored %d Points\nPlayer 2 Scored %d Points\nWinner is Player %d\n", p1_score, p2_score, (p1_score < p2_score)+1);
    stats.game_status = 1;
    stats.p1_score = p1_score;
    stats.p2_score = p2_score;
    stats.pegs = pegs; 
    stats.len = len;
}
int main() {
    int option;
    while(1) {
        system("clear");
        printf("----Mattix----\n1. Play\n2. Show stats\n3. Exit\nOption: ");
        scanf("%d", &option);
        system("clear");
        switch (option) {
            case 1:
                game();
                break;
            case 2:
                if (stats.game_status) {
                    printf("\n--- Game Stats ---\n");
                    printf("Player 1 Score: %d\n", stats.p1_score);
                    printf("Player 2 Score: %d\n", stats.p2_score);
                    printf("Pegs Array (%d values):\n", stats.len);
                    printf("%s", board_string(stats.pegs, stats.len));
                    printf("\n-------------------\n"); 
                } else {
                    printf("Use this feature after you have played a game.\n");
                }
                break;
            case 3:
                exit(0);
            default:
                break;
        }
        char c;
        while ((c = getchar()) != '\n' && c != EOF);
        press_enter_to_continue(); 

    }
}