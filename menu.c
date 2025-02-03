#include <stdio.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

typedef struct
{
    char username[50];
    char password[50];
    char email[50];
    int score;
    int gold;
    int games_played;
    time_t first_game_time;
    
} User;
User current_user;

User person[100];
int countperson = 0;

typedef struct {
    char** place;
    int rows;
    int cols;
} Map;

Map maps[10];
int current_map = 0;

typedef struct {
    int row;
    int col;
} Corridor;

Corridor corridors[1000];
int corridor_count = 0;

Corridor corridors2[1000];
int corridor2_count = 0;

void getUsername(char username[]);
void getpass(char password[]);
void getemail(char email[]);
int checkpass(char password[]);
int checkemail(char email[]);
void handle_menu();
void handle_regist();
void handle_login();
void handle_before_game();
int compare(const void *a, const void *b);//Rank
void handle_scoreboard();
void load_users();
void handle_setting();
//Map
void handle_screen_game1(int *player_row, int *player_col);
void handle_screen_game2(int *player_row, int *player_col);
void handle_character_movement_game2(int* player_row, int* player_col, char** place);
void draw_vertical_line(int start_row, int end_row, int col, char** place, int display);
void draw_horizontal_line(int start_col, int end_col, int row, char** place, int display);
void fill_rectangle_with_dots(int start_row, int end_row, int start_col, int end_col, char** place, int display);
void draw_door(int start_row, int start_col, char** place);
void draw_corridor(int start_row, int end_row, int start_col, int end_col, char** place);
void handle_character_movement_game1(int* player_row, int* player_col, char** place);
void handle_character_movement_game2(int* player_row, int* player_col, char** place);
char** create_place(int rows, int cols);
void free_place(char** place, int rows);
void display_adjacent_corridors(int row, int col, char** place);
void add_stairs(int row, int col, char** place, int display);
void add_Obstacle(int row, int col, char** place, int display);
void display_message_for_duration(int row, int col, const char* message, int duration_ms);
void display_corridors(Corridor* corridors, int count, char** place);
void add_gold(int row, int col, char** place, int display, int* flag, int index);
void display_score();
void add_black_gold();
void add_food(int row, int col, char** place, int display, int* flag, int index);
void display_status();
void game_over();
void victory();

int main()
{
    load_users();
    initscr();
    start_color();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    handle_menu();

    endwin();
    return 0;
}

void handle_menu() {
    int ch;
    int highlight = 0;
    int choice = -1;

    curs_set(0);

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK); // Warning
    init_pair(5, COLOR_BLUE, COLOR_BLACK); // login_success
    init_pair(6, COLOR_YELLOW, COLOR_BLACK); // rank1
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK); // rank2
    init_pair(8, COLOR_CYAN, COLOR_BLACK); // rank3
    init_color(10, 1000, 467, 718);
    init_pair(9, 10, COLOR_BLACK);

    while (1) {
        clear();
        int x = 10;
        int y = 5;
        char *choices[] = {"Login", "Register", "Login as Guest"};

        for (int i = 0; i < 3; i++) {
            if (highlight == i) {
                attron(COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
            }

            mvprintw(y + i, x, "%s", choices[i]);

            if (highlight == i) {
                attroff(COLOR_PAIR(1));
            } else {
                attroff(COLOR_PAIR(2));
            }
        }

        ch = getch();

        switch (ch) {
        case KEY_UP:
            if (highlight > 0) {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < 2) {
                highlight++;
            }
            break;
        case 10:
            choice = highlight;
            break;
        }

        if (choice != -1) {
            break;
        }
    }

    clear();
    curs_set(1);

    int player_row = 11, player_col = 21;

    if (choice == 0) {
        handle_login();
    } else if (choice == 1) {
        handle_regist();
    } else if (choice == 2) {
        handle_screen_game1(&player_row, &player_col);
    }

    refresh();
    getch();
}


void getUsername(char username[])
{
    attron(COLOR_PAIR(3));
    mvprintw(10, 10, "Please enter your name: ");
    refresh();
    attroff(COLOR_PAIR(3));
    mvscanw(10, 35, "%s", username);
}

void getpass(char password[])
{
    int valid = 0;
    while (!valid)
    {
        attron(COLOR_PAIR(3));
        mvprintw(12, 10, "Please enter your password: ");
        refresh();
        attroff(COLOR_PAIR(3));
        mvscanw(12, 38, "%s", password);
        valid = checkpass(password);
        if (!valid)
        {
            attron(COLOR_PAIR(4));
            mvprintw(12, 38, "The pass is invalid!");
            refresh();
            napms(1000);
            attroff(COLOR_PAIR(4));
            mvprintw(12, 38, "                    ");
            move(12, 38);
        }
    }
}

void getemail(char email[])
{
    int valid = 0;
    while (!valid)
    {
        attron(COLOR_PAIR(3));
        mvprintw(14, 10, "Please enter your email: ");
        refresh();
        attroff(COLOR_PAIR(3));
        mvscanw(14, 35, "%s", email);
        valid = checkemail(email);
        if (!valid)
        {
            attron(COLOR_PAIR(4));
            mvprintw(14, 35, "The email is invalid!");
            refresh();
            napms(1000);
            attroff(COLOR_PAIR(4));
            mvprintw(14, 35, "                     ");
            move(14, 35);
        }
    }
}

int checkpass(char password[])
{
    int n = strlen(password);
    int capital = 0;
    int lowercase = 0;
    int digit = 0;
    for (int i = 0; i < n; i++)
    {
        if (password[i] >= 65 && password[i] <= 90)
            capital++;
        else if (password[i] >= 97 && password[i] <= 122)
            lowercase++;
        else if (password[i] >= 48 && password[i] <= 57)
            digit++;
    }
    return (capital > 0 && lowercase > 0 && digit > 0 && n >= 7);
}

int checkemail(char email[])
{
    int n = strlen(email);
    int one = -1;
    int two = -1;

    for (int i = 0; i < n; i++)
    {
        if (email[i] == ' ')
        {
            return 0;
        }
    }

    for (int i = 0; i < n; i++)
    {
        if (email[i] == '@')
        {
            if (one == -1)
            {
                one = i;
            }
            else
            {
                return 0;
            }
        }
        else if (email[i] == '.')
        {
            two = i;
        }
    }

    if (one != -1 && two != -1 && two > one + 1 && (n - two) == 4)
    {
        return 1;
    }

    return 0;
}

void handle_regist()
{
    echo();
    int is_duplicate = 1;
    char new_username[50];

    while (is_duplicate)
    {
        getUsername(new_username);

        is_duplicate = 0;
        FILE *file = fopen("users.txt", "r");
        if (file != NULL) {
            char file_username[50];
            char file_password[50];
            char file_email[50];

            while (fscanf(file, "%s %s %s", file_username, file_password, file_email) != EOF) {
                if (strcmp(file_username, new_username) == 0) {
                    is_duplicate = 1;
                    break;
                }
            }

            fclose(file);
        }

        if (is_duplicate) {
            mvprintw(16, 10, "Username already exists! Please try again.");
            refresh();
            napms(2000);
            clear();
        }
    }

    strcpy(person[countperson].username, new_username);
    getpass(person[countperson].password);
    getemail(person[countperson].email);

    FILE *file = fopen("users.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s %s %s\n", person[countperson].username, person[countperson].password, person[countperson].email);
        fclose(file);
    }

    countperson++;

    noecho();
    attron(COLOR_PAIR(5));
    mvprintw(12, 38, "Registration successful!");
    refresh();
    napms(2000);
    attroff(COLOR_PAIR(5));
    clear();
    refresh();
    handle_login();
}

void handle_login()
{
    char username[50];
    char password[50];
    int found = 0;

    while (!found)
    {
        echo();
        getUsername(username);
        getpass(password);
        noecho();

        FILE *file = fopen("users.txt", "r");
        if (file != NULL) {
            char file_username[50];
            char file_password[50];
            char file_email[50];

            while (fscanf(file, "%s %s %s", file_username, file_password, file_email) != EOF) {
                if (strcmp(file_username, username) == 0 && strcmp(file_password, password) == 0) {
                    found = 1;
                    break;
                }
            }

            fclose(file);
        }

        if (!found) {
            attron(COLOR_PAIR(4));
            mvprintw(16, 10, "Username or password is invalid! Please try again.");
            refresh();
            napms(1000);
            attroff(COLOR_PAIR(4));
            clear();
        }
    }

    if (found) {
        attron(COLOR_PAIR(5));
        mvprintw(12, 38, "Login successful!");
        refresh();
        napms(1000);
        attroff(COLOR_PAIR(5));
        handle_before_game();
    }

    refresh();
    getch();
}

void handle_before_game()
{
    int ch;
    int highlight = 0;
    int choice = -1;

    curs_set(0);

    char *choices[] = {"New Game", "Resume Game", "Settings", "Scoreboard"};

    while (1)
    {
        clear();
        int x = 10;
        int y = 5;

        for (int i = 0; i < 4; i++)
        {
            if (highlight == i)
            {
                attron(COLOR_PAIR(1));
            }
            else
            {
                attron(COLOR_PAIR(2));
            }

            mvprintw(y + i, x, "%s", choices[i]);

            if (highlight == i)
            {
                attroff(COLOR_PAIR(1));
            }
            else
            {
                attroff(COLOR_PAIR(2));
            }
        }

        ch = getch();

        switch (ch)
        {
        case KEY_UP:
            if (highlight > 0)
            {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < 3)
            {
                highlight++;
            }
            break;
        case 10:
            choice = highlight;
            break;
        }

        if (choice != -1)
        {
            break;
        }
    }

    clear();
    curs_set(1);

    switch (choice)
    {
    case 0:
        mvprintw(5, 10, "New Game selected");
        break;
    case 1:
        mvprintw(5, 10, "Resume Game selected");
        break;
    case 2:
        handle_setting();
        break;
    case 3:
        handle_scoreboard(person, countperson);
        break;
    }

    refresh();
    getch();
}

int compare(const void *a, const void *b) {
    User *userA = (User *)a;
    User *userB = (User *)b;
    return userB->score - userA->score;
}

void load_users() {
    FILE *file = fopen("users.txt", "r");
    if (file != NULL) {
        while (fscanf(file, "%s %s %s", person[countperson].username, person[countperson].password, person[countperson].email) != EOF) {
            person[countperson].score = 0;
            person[countperson].gold = 0;
            person[countperson].games_played = 0;
            person[countperson].first_game_time = time(NULL);
            countperson++;
        }
        fclose(file);
    }
}

void handle_scoreboard(User users[], int n) {
    qsort(users, n, sizeof(User), compare);
    int page = 0;
    int max_per_page = 4;
    int total_pages = (n + max_per_page - 1) / max_per_page;
    int ch;

    while (1) {
        time_t now = time(NULL);
        int row = 3, col = 1;
        clear();
        mvprintw(0, 0, "q: Quit  n: Next Page  p: Previous Page");
        mvprintw(1, 0, "Scoreboard (Page %d of %d):", page + 1, total_pages);

        int start = page * max_per_page;
        int end = start + max_per_page > n ? n : start + max_per_page;

        for (int i = start; i < end; i++) {
            if (i == 0) {
                attron(COLOR_PAIR(6));
                attron(A_BOLD);
                mvprintw(row++, col, "Rank 1: GOAT %s", users[i].username);
                attroff(COLOR_PAIR(6));
                attroff(A_BOLD);
            } else if (i == 1) {
                attron(COLOR_PAIR(7));
                attron(A_BOLD);
                mvprintw(row++, col, "Rank 2: Legend %s", users[i].username);
                attroff(COLOR_PAIR(7));
                attroff(A_BOLD);
            } else if (i == 2) {
                attron(COLOR_PAIR(8));
                attron(A_BOLD);
                mvprintw(row++, col, "Rank 3: Champion %s", users[i].username);
                attroff(COLOR_PAIR(8));
                attroff(A_BOLD);
            } else {
                mvprintw(row++, col, "Rank %d: %s", i + 1, users[i].username);
            }

            mvprintw(row++, col, "Score: %d", users[i].score);
            mvprintw(row++, col, "Gold: %d", users[i].gold);
            mvprintw(row++, col, "Games Played: %d", users[i].games_played);

            double days_since_first_game = difftime(now, users[i].first_game_time) / (60 * 60 * 24);
            mvprintw(row++, col, "Days since first game: %.0f", days_since_first_game);

            if (strcmp(users[i].username, current_user.username) == 0) {
                attron(A_BOLD | A_BLINK);
                mvprintw(row - 7, col, "Rank %d: %s", i + 1, users[i].username);
                attroff(A_BOLD | A_BLINK);
            }

            row++;
        }

        mvprintw(row++, col, "q: Quit  n: Next Page  p: Previous Page");

        refresh();
        ch = getch();

        switch (ch) {
            case 'n':
                if (page < total_pages - 1) {
                    page++;
                }
                break;
            case 'p':
                if (page > 0) {
                    page--;
                }
                break;
            case 'q':
                return;
        }
    }
}

void handle_setting() {
    int ch;
    int highlight = 0;
    int choice = -1;

    curs_set(0);

    char *settings[] = {"Game level", "Player color"};

    while (1) {
        clear();
        int x = 10;
        int y = 5;

        for (int i = 0; i < 2; i++) {
            if (highlight == i) {
                attron(COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
            }

            mvprintw(y + i, x, "%s", settings[i]);

            if (highlight == i) {
                attroff(COLOR_PAIR(1));
            } else {
                attroff(COLOR_PAIR(2));
            }
        }

        ch = getch();

        switch (ch) {
        case KEY_UP:
            if (highlight > 0) {
                highlight--;
            }
            break;
        case KEY_DOWN:
            if (highlight < 1) {
                highlight++;
            }
            break;
        case 10:
            choice = highlight;
            break;
        }

        if (choice != -1) {
            break;
        }
    }

    clear();
    curs_set(1);

    switch (choice) {
    case 0:
        mvprintw(5, 10, "Game level selected");
        break;
    case 1:
        mvprintw(5, 10, "Player color selected");
        break;
    }

    refresh();
    getch();
}

/*------------------------------------------------------------------*/

int room1_displayed = 1;
int room2_displayed = 0;
int room3_displayed = 0;
int room4_displayed = 0;
int room5_displayed = 0;
int room6_displayed = 0;

int room11_displayed = 0;
int room22_displayed = 0;
int room33_displayed = 0;
int room44_displayed = 1;
int room55_displayed = 0;
int room66_displayed = 0;

int gold_displayed[6] = {0, 0, 0, 0, 0, 0}; 
int black_gold_displayed[6] = {0, 0, 0, 0, 0, 0};

int gold_displayed2[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; 
int black_gold_displayed2[6] = {0, 0, 0, 0, 0, 0};

int food_displayed[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int food_displayed2[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

int player_score = 0;

int player_food = 0;
int player_health = 100;

int player_hunger = 0;
int hunger_max = 10;
int hunger_max_moves = 10;
int hunger_moves_counter = 0;
int health_moves_counter = 0;


char** create_place(int rows, int cols) {
    char** place = (char**)malloc(rows * sizeof(char*));
    for (int i = 0; i < rows; i++) {
        place[i] = (char*)malloc(cols * sizeof(char));
    }
    return place;
}

void free_place(char** place, int rows) {
    for (int i = 0; i < rows; i++) {
        free(place[i]);
    }
    free(place);
}

void draw_vertical_line(int start_row, int end_row, int col, char** place, int display) {
    if (display) {
        for (int i = start_row; i <= end_row; i++) {
            attron(COLOR_PAIR(7));
            mvprintw(i, col, "|");
            attroff(COLOR_PAIR(7));
            place[i][col] = '|';
        }
    }
}

void draw_horizontal_line(int start_col, int end_col, int row, char** place, int display) {
    if (display) {
        for (int i = start_col; i <= end_col; i++) {
            attron(COLOR_PAIR(7));
            mvprintw(row, i, "-");
            attroff(COLOR_PAIR(7));
            place[row][i] = '-';
        }
    }
}

void fill_rectangle_with_dots(int start_row, int end_row, int start_col, int end_col, char** place, int display) {
    if (display) {
        for (int i = start_row; i <= end_row; i++) {
            for (int j = start_col; j <= end_col; j++) {
                attron(COLOR_PAIR(3));
                mvprintw(i, j, ".");
                attroff(COLOR_PAIR(3));
                place[i][j] = '.';
            }
        }
    }
}

void draw_door(int start_row, int start_col, char** place) {
    attron(COLOR_PAIR(8));
    mvprintw(start_row, start_col, "+");
    attroff(COLOR_PAIR(8));
    place[start_row][start_col] = '+';
}

void draw_corridor(int start_row, int end_row, int start_col, int end_col, char** place) {
    if (start_row == end_row) {
        for (int i = start_col; i <= end_col; i++) {
            place[start_row][i] = '#';
        }
    } else if (start_col == end_col) {
        for (int i = start_row; i <= end_row; i++) {
            place[i][start_col] = '#';
        }
    }
}

void display_corridor(int row, int col, char** place) {
    if (place[row][col] == '#') {
        mvprintw(row, col, "#");
    }
}

void display_corridors(Corridor* corridors, int count, char** place) {
    for (int i = 0; i < count; i++) {
        int row = corridors[i].row;
        int col = corridors[i].col;
        mvprintw(row, col, "#");
        place[row][col] = '#';
    }
}


void display_adjacent_corridors(int row, int col, char** place) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if ((i != 0 || j != 0) && place[row + i][col + j] == '#') {
                mvprintw(row + i, col + j, "#");
            }
        }
    }
}

void add_stairs(int row, int col, char** place, int display) {
    if (display) {
        mvprintw(row, col, "<");
        place[row][col] = '<';
    }
}

void add_Obstacle(int row, int col, char** place, int display) {
    if (display) {
        attron(COLOR_PAIR(4));
        mvprintw(row, col, "O");
        attroff(COLOR_PAIR(4));
        place[row][col] = 'O';
    }
}

int message_displayed1 = 0;
int message_displayed2 = 0;
int message_displayed3 = 0;
int message_displayed4 = 0;
int message_displayed5 = 0;

int message_displayed11 = 0;
int message_displayed22 = 0;
int message_displayed33 = 0;
int message_displayed44 = 0;
int message_displayed55 = 0;

int trap_activated = 0;
int trap_activated2 = 0; 

void handle_character_movement_game1(int* player_row, int* player_col, char** place) {
    int ch;
    keypad(stdscr, TRUE);
    

    while (1) {
        mvprintw(*player_row, *player_col, "X");
        display_adjacent_corridors(*player_row, *player_col, place);

        
        if ((*player_row == 12 && *player_col == 40 && !message_displayed1) || (*player_row == 21 && *player_col == 50 && !message_displayed2) ||
            (*player_row == 29 && *player_col == 45 && !message_displayed3) || (*player_row == 27 && *player_col == 28 && !message_displayed4) ||
            (*player_row == 22 && *player_col == 65 && !message_displayed5)) {
            mvprintw(0, 20, "New Room!");
            refresh();
            napms(2000);  
            mvprintw(0, 20, "         ");  
            refresh();
            if (*player_row == 12 && *player_col == 40) {
                message_displayed1 = 1;
                room2_displayed = 1;
                handle_screen_game1(player_row, player_col);
            }
            if (*player_row == 21 && *player_col == 50) {
                message_displayed2 = 1;
                room3_displayed = 1;
                handle_screen_game1(player_row, player_col);
            }
            if (*player_row == 29 && *player_col == 45) {
                message_displayed3 = 1;
                room5_displayed = 1;
                handle_screen_game1(player_row, player_col);
            }
            if (*player_row == 27 && *player_col == 28) {
                message_displayed4 = 1;
                room6_displayed = 1;
                handle_screen_game1(player_row, player_col);
            }
            if (*player_row == 22 && *player_col == 65) {
                message_displayed5 = 1;
                room4_displayed = 1;
                handle_screen_game1(player_row, player_col);
            }

            
            for (int i = 0; i < corridor_count; i++) {
                mvprintw(corridors[i].row, corridors[i].col, "#");
            }
        }

        
        if (*player_row == 10 && *player_col == 24 && !trap_activated) {
            mvprintw(0, 20, "You hit a trap!");
            refresh();
            napms(2000);  
            mvprintw(0, 20, "               ");  
            refresh();
            place[10][24] = '^';  
            trap_activated = 1;  
            player_health -= 10;
            display_status();
        }

        ch = getch();

        if (ch == 'e' && player_food > 0) {
        player_food -= 1;
        player_health += 15;
        player_hunger = 0;
        hunger_moves_counter = 0;
        display_status();
        }

        int new_row = *player_row;
        int new_col = *player_col;

        switch (ch) {
            case KEY_UP:
            case '8':
                new_row--;
                break;
            case KEY_DOWN:
            case '2':
                new_row++;
                break;
            case KEY_LEFT:
            case '4':
                new_col--;
                break;
            case KEY_RIGHT:
            case '6':
                new_col++;
                break;
            case '7':
                new_row--;
                new_col--;
                break;
            case '9':
                new_row--;
                new_col++;
                break;
            case '1':
                new_row++;
                new_col--;
                break;
            case '3':
                new_row++;
                new_col++;
                break;
        }

        hunger_moves_counter++;
        health_moves_counter++;

        if (hunger_moves_counter >= hunger_max_moves) {
            hunger_moves_counter = 0;
            if (player_hunger < hunger_max) {
                player_hunger++;
                display_status();
            } else if (player_hunger == hunger_max) {
                player_health -= 5;
                display_status();
            }
        }

        if (player_hunger == 0 && health_moves_counter >= 5)
        {
            player_health += 5;
            health_moves_counter = 0;
            display_status();
        }

        if (player_health <= 0) {
            game_over();
        }

        if (place[new_row][new_col] == '.' || place[new_row][new_col] == '+' || place[new_row][new_col] == '#' || 
            place[new_row][new_col] == '<' || place[new_row][new_col] == '0' || place[new_row][new_col] == '$' || 
            place[new_row][new_col] == '^' || place[new_row][new_col] == '@') 
        {
            mvprintw(*player_row, *player_col, "%c", place[*player_row][*player_col]);

            *player_row = new_row;
            *player_col = new_col;

            
            if (place[*player_row][*player_col] == '#') {
                corridors[corridor_count].row = *player_row;
                corridors[corridor_count].col = *player_col;
                corridor_count++;
            }

            if (place[*player_row][*player_col] == '0') {
                place[*player_row][*player_col] = '.'; 
                player_score += 10;
                display_score();
                mvprintw(0, 20, "Gold Collected!");  
                refresh();
                napms(2000);  
                mvprintw(0, 20, "               ");  
                refresh();
            }

            if (place[*player_row][*player_col] == '$') {
                place[*player_row][*player_col] = '.'; 
                player_score += 40;
                display_score();
                mvprintw(0, 20, "Black Gold Collected!");  
                refresh();
                napms(2000);  
                mvprintw(0, 20, "                     ");  
                refresh();
            }

            if (place[new_row][new_col] == '<') {
                clear();
                *player_row = 22;
                *player_col = 52;
                handle_screen_game2(player_row, player_col);
            }

            if (place[*player_row][*player_col] == '@') {
            if (player_food < 5)
            {
                place[*player_row][*player_col] = '.';
                player_food += 1;
                display_status();
                mvprintw(0, 20, "Food Collected!");
            }
            else
            {
                mvprintw(0, 20, "You already have 5 food items, can't collect more!");
            }
            refresh();
            napms(2000);
            mvprintw(0, 20, "                                              ");
            refresh();
            }

            mvprintw(*player_row, *player_col, "X");
            display_corridor(*player_row, *player_col, place);
        }

        refresh();
    }
}


void handle_character_movement_game2(int* player_row, int* player_col, char** place) {
    int ch;
    keypad(stdscr, TRUE);

    while (1) {
        mvprintw(*player_row, *player_col, "X");
        display_adjacent_corridors(*player_row, *player_col, place);

        
        if ((*player_row == 36 && *player_col == 36 && !message_displayed11) || (*player_row == 26 && *player_col == 46 && !message_displayed22) ||
            (*player_row == 24 && *player_col == 23 && !message_displayed33) || (*player_row == 18 && *player_col == 72 && !message_displayed44) ||
            (*player_row == 23 && *player_col == 80 && !message_displayed55)) {
            mvprintw(0, 20, "New Room!");
            refresh();
            napms(2000);  
            mvprintw(0, 20, "         ");  
            refresh();
            mvprintw(*player_row, *player_col, "X");  
            if (*player_row == 36 && *player_col == 36) {
                message_displayed11 = 1;
                room22_displayed = 1;
                handle_screen_game2(player_row, player_col);
            }
            if (*player_row == 26 && *player_col == 46) {
                message_displayed22 = 1;
                room33_displayed = 1;
                handle_screen_game2(player_row, player_col);
            }
            if (*player_row == 24 && *player_col == 23) {
                message_displayed33 = 1;
                room11_displayed = 1;
                handle_screen_game2(player_row, player_col);
            }
            
            
            if (*player_row == 18 && *player_col == 72) {
                message_displayed44 = 1;
                room55_displayed = 1;
                handle_screen_game2(player_row, player_col);
            }
            if (*player_row == 23 && *player_col == 80) {
                message_displayed55 = 1;
                room66_displayed = 1;
                handle_screen_game2(player_row, player_col);
            }

            
            for (int i = 0; i < corridor_count; i++) {
                mvprintw(corridors[i].row, corridors[i].col, "#");
            }
        }

        if (*player_row == 23 && *player_col == 23) {
                mvprintw(0, 20, "You entered the treasure room!");
                refresh();
                napms(2000);
                victory();
            }

        if (*player_row == 24 && *player_col == 52 && !trap_activated2) {
            mvprintw(0, 20, "You hit a trap!");
            refresh();
            napms(2000);  
            mvprintw(0, 20, "               ");  
            refresh();
            place[24][52] = '^'; 
            trap_activated2 = 1;  
            player_health -= 10;
            display_status();
        }

        ch = getch();

        if (ch == 'e' && player_food > 0) {
        player_food -= 1;
        player_health += 15;
        player_hunger = 0;
        hunger_moves_counter = 0;
        display_status();
        }

        int new_row = *player_row;
        int new_col = *player_col;

        switch (ch) {
            case KEY_UP:
            case '8':
                new_row--;
                break;
            case KEY_DOWN:
            case '2':
                new_row++;
                break;
            case KEY_LEFT:
            case '4':
                new_col--;
                break;
            case KEY_RIGHT:
            case '6':
                new_col++;
                break;
            case '7':
                new_row--;
                new_col--;
                break;
            case '9':
                new_row--;
                new_col++;
                break;
            case '1':
                new_row++;
                new_col--;
                break;
            case '3':
                new_row++;
                new_col++;
                break;
        }

        hunger_moves_counter++;
        health_moves_counter++;

        if (hunger_moves_counter >= hunger_max_moves) {
            hunger_moves_counter = 0;
            if (player_hunger < hunger_max) {
                player_hunger++;
                display_status();
            } else if (player_hunger == hunger_max) {
                player_health -= 5;
                display_status();
            }
        }

        if (player_hunger == 0 && health_moves_counter >= 5)
        {
            player_health += 5;
            health_moves_counter = 0;
            display_status();
        }

        if (player_health <= 0) {
            game_over();
        }

        if (place[new_row][new_col] == '.' || place[new_row][new_col] == '+' || place[new_row][new_col] == '#' ||
            place[new_row][new_col] == '<' || place[new_row][new_col] == '$' || place[new_row][new_col] == '0' ||
            place[new_row][new_col] == '^' || place[new_row][new_col] == '@')
            {
            mvprintw(*player_row, *player_col, "%c", place[*player_row][*player_col]);

            *player_row = new_row;
            *player_col = new_col;

            
            if (place[*player_row][*player_col] == '#') {
            corridors2[corridor2_count].row = *player_row;
            corridors2[corridor2_count].col = *player_col;
            corridor2_count++;
            }

            if (place[*player_row][*player_col] == '0') {
            place[*player_row][*player_col] = '.'; 
            player_score += 10;
            display_score();
            mvprintw(0, 20, "Gold Collected!");  
            refresh();
            napms(2000);  
            mvprintw(0, 20, "               ");  
            refresh();
            }

            if (place[*player_row][*player_col] == '$') {
            place[*player_row][*player_col] = '.'; 
            player_score += 40;
            display_score();
            mvprintw(0, 20, "Black Gold Collected!");  
            refresh();
            napms(2000);  
            mvprintw(0, 20, "                     ");  
            refresh();
            }

            if (place[*player_row][*player_col] == '@') {
            if (player_food < 5)
            {
                place[*player_row][*player_col] = '.';
                player_food += 1;
                display_status();
                mvprintw(0, 20, "Food Collected!");
            }
            else
            {
                mvprintw(0, 20, "You already have 5 food items, can't collect more!");
            }
            refresh();
            napms(2000);
            mvprintw(0, 20, "                                              ");
            refresh();
            }

            mvprintw(*player_row, *player_col, "X");
            display_corridor(*player_row, *player_col, place);
        }

        refresh();
    }
}

void handle_screen_game1(int *player_row, int *player_col) {
    initscr();
    curs_set(0);
    clear();
    refresh();
    int y, x;
    getmaxyx(stdscr, y, x);
    char** place1 = create_place(y, x);

    memset(*place1, ' ', y * x * sizeof(char));
    mvprintw(0, 0, "Situation of game :");

    display_score();
    display_status();
    display_corridors(corridors, corridor_count, place1);
    
    draw_vertical_line(10, 13, 20, place1, room1_displayed);
    draw_vertical_line(10, 13, 25, place1, room1_displayed);
    draw_vertical_line(11, 16, 40, place1, room2_displayed);
    draw_vertical_line(11, 16, 45, place1, room2_displayed);
    draw_vertical_line(20, 24, 50, place1, room3_displayed);
    draw_vertical_line(20, 24, 55, place1, room3_displayed);
    draw_vertical_line(18, 21, 60, place1, room4_displayed);
    draw_vertical_line(18, 21, 69, place1, room4_displayed);
    draw_vertical_line(27, 33, 40, place1, room5_displayed);
    draw_vertical_line(27, 33, 45, place1, room5_displayed);
    draw_vertical_line(20, 26, 20, place1, room6_displayed);
    draw_vertical_line(20, 26, 30, place1, room6_displayed);
    
    draw_horizontal_line(21, 24, 9, place1, room1_displayed);
    draw_horizontal_line(21, 24, 14, place1, room1_displayed);
    draw_horizontal_line(41, 44, 10, place1, room2_displayed);
    draw_horizontal_line(41, 44, 17, place1, room2_displayed);
    draw_horizontal_line(51, 54, 19, place1, room3_displayed);
    draw_horizontal_line(51, 54, 25, place1, room3_displayed);
    draw_horizontal_line(61, 68, 17, place1, room4_displayed);
    draw_horizontal_line(61, 68, 22, place1, room4_displayed);
    draw_horizontal_line(41, 44, 26, place1, room5_displayed);
    draw_horizontal_line(41, 44, 34, place1, room5_displayed);
    draw_horizontal_line(21, 29, 19, place1, room6_displayed);
    draw_horizontal_line(21, 29, 27, place1, room6_displayed);
    
    fill_rectangle_with_dots(10, 13, 21, 24, place1, room1_displayed);
    fill_rectangle_with_dots(11, 16, 41, 44, place1, room2_displayed);
    fill_rectangle_with_dots(20, 24, 51, 54, place1, room3_displayed);
    fill_rectangle_with_dots(18, 21, 61, 68, place1, room4_displayed);
    fill_rectangle_with_dots(27, 33, 41, 44, place1, room5_displayed);
    fill_rectangle_with_dots(20, 26, 21, 29, place1, room6_displayed);

    add_Obstacle(12,22,place1, room1_displayed);
    add_Obstacle(13,43,place1, room2_displayed);
    add_Obstacle(15,42,place1, room2_displayed);
    add_Obstacle(21,52,place1, room3_displayed);
    add_Obstacle(24,53,place1, room3_displayed);
    add_Obstacle(28,41,place1, room5_displayed);
    add_Obstacle(30,43,place1, room5_displayed);
    add_Obstacle(31,42,place1, room5_displayed);
    add_Obstacle(20,62,place1, room4_displayed);
    add_Obstacle(24,22,place1, room6_displayed);
    add_Obstacle(21,26,place1, room6_displayed);

    add_stairs(22,53,place1, room3_displayed);

    draw_door(12, 25, place1);
    draw_door(12, 40, place1);
    draw_door(14, 45, place1);
    draw_door(21, 50, place1);
    draw_door(23, 55, place1);
    draw_door(22, 65, place1);
    draw_door(25, 52, place1);
    draw_door(29, 45, place1);
    draw_door(29, 40, place1);
    draw_door(27, 28, place1);

    draw_corridor(12, 12, 26, 39, place1); 
    draw_corridor(14, 14, 46, 47, place1); 
    draw_corridor(14, 21, 47, 47, place1);
    draw_corridor(21, 21, 47, 49, place1);
    draw_corridor(23, 23, 56, 65, place1);
    draw_corridor(26, 29, 52, 52, place1);
    draw_corridor(29, 29, 46, 52, place1);
    draw_corridor(29, 29, 28, 39, place1);
    draw_corridor(28, 29, 28, 28, place1);

    
    add_gold(13, 22, place1, room1_displayed, gold_displayed, 0);
    add_gold(15, 43, place1, room2_displayed, gold_displayed, 1);
    add_gold(22, 54, place1, room3_displayed, gold_displayed, 2);
    add_gold(19, 62, place1, room4_displayed, gold_displayed, 3);
    add_gold(20, 64, place1, room4_displayed, gold_displayed, 4);
    add_gold(29, 42, place1, room5_displayed, gold_displayed, 5);

    add_black_gold(24, 28, place1, room6_displayed, black_gold_displayed, 0);
    
    add_food(11, 24, place1, room1_displayed, food_displayed, 0);
    add_food(11, 42, place1, room2_displayed, food_displayed, 1);
    add_food(23, 51, place1, room3_displayed, food_displayed, 2);
    add_food(18, 67, place1, room4_displayed, food_displayed, 3);
    add_food(20, 64, place1, room4_displayed, food_displayed, 4);
    add_food(32, 43, place1, room5_displayed, food_displayed, 5);
    add_food(23, 24, place1, room6_displayed, food_displayed, 6);
    add_food(20, 29, place1, room6_displayed, food_displayed, 7);

    handle_character_movement_game1(player_row, player_col, place1);
    refresh();
    getch();
    endwin();
    
    free_place(place1, y);
}

int welcome_displayed2 = 0;

void handle_screen_game2(int *player_row, int *player_col) {
    initscr();
    curs_set(0);
    clear();
    refresh();
    int y, x;
    getmaxyx(stdscr, y, x);
    char** place2 = create_place(y, x);

    memset(*place2, ' ', y * x * sizeof(char));

    mvprintw(0, 0, "Situation of game :");

    display_score();
    display_status();

    display_corridors(corridors2, corridor2_count, place2);
    
    draw_vertical_line(15, 23, 20, place2, room11_displayed);
    draw_vertical_line(15, 23, 25, place2, room11_displayed);
    draw_vertical_line(32, 35, 37, place2, room22_displayed);
    draw_vertical_line(32, 35, 30, place2, room22_displayed);
    draw_vertical_line(27, 31, 45, place2, room33_displayed);
    draw_vertical_line(27, 31, 52, place2, room33_displayed);
    draw_vertical_line(20, 24, 50, place2, room44_displayed);
    draw_vertical_line(20, 24, 55, place2, room44_displayed);
    draw_vertical_line(13, 17, 66, place2, room55_displayed);
    draw_vertical_line(13, 17, 74, place2, room55_displayed);
    draw_vertical_line(24, 29, 79, place2, room66_displayed);
    draw_vertical_line(24, 29, 88, place2, room66_displayed);

    
    draw_horizontal_line(21, 24, 14, place2, room11_displayed);
    draw_horizontal_line(21, 24, 24, place2, room11_displayed);
    draw_horizontal_line(31, 36, 31, place2, room22_displayed);
    draw_horizontal_line(31, 36, 36, place2, room22_displayed);
    draw_horizontal_line(46, 51, 26, place2, room33_displayed);
    draw_horizontal_line(46, 51, 32, place2, room33_displayed);
    draw_horizontal_line(51, 54, 19, place2, room44_displayed);
    draw_horizontal_line(51, 54, 25, place2, room44_displayed);
    draw_horizontal_line(67, 73, 12, place2, room55_displayed);
    draw_horizontal_line(67, 73, 18, place2, room55_displayed);
    draw_horizontal_line(80, 87, 23, place2, room66_displayed);
    draw_horizontal_line(80, 87, 30, place2, room66_displayed);

    
    fill_rectangle_with_dots(15, 23, 21, 24, place2, room11_displayed);
    fill_rectangle_with_dots(32, 35, 31, 36, place2, room22_displayed);
    fill_rectangle_with_dots(27, 31, 46, 51, place2, room33_displayed);
    fill_rectangle_with_dots(20, 24, 51, 54, place2, room44_displayed);
    fill_rectangle_with_dots(13, 17, 67, 73, place2, room55_displayed);
    fill_rectangle_with_dots(24, 29, 80, 87, place2, room66_displayed);

    
    draw_door(24, 23, place2);
    draw_door(31, 32, place2);
    draw_door(36, 36, place2);
    draw_door(32, 47, place2);
    draw_door(26, 46, place2);
    draw_door(20, 50, place2);
    draw_door(23, 55, place2);
    draw_door(18, 72, place2);
    draw_door(14, 74, place2);
    draw_door(23, 80, place2);

    
    draw_corridor(25, 28, 23, 23, place2);
    draw_corridor(28, 28, 23, 32, place2); 
    draw_corridor(28, 30, 32, 32, place2); 
    draw_corridor(37, 38, 36, 36, place2); 
    draw_corridor(38, 38, 36, 47, place2);
    draw_corridor(33, 38, 47, 47, place2); 
    draw_corridor(22, 25, 46, 46, place2);
    draw_corridor(22, 22, 44, 46, place2);
    draw_corridor(20, 22, 44, 44, place2);
    draw_corridor(20, 20, 44, 49, place2);
    draw_corridor(14, 14, 75, 80, place2);  
    draw_corridor(14, 22, 80, 80, place2);
    draw_corridor(23, 23, 56, 72, place2);
    draw_corridor(19, 22, 72, 72, place2);

    
    add_Obstacle(18,23,place2, room11_displayed);
    add_Obstacle(21,22,place2, room11_displayed);
    add_Obstacle(33,32,place2, room22_displayed);
    add_Obstacle(27,47,place2, room33_displayed);
    add_Obstacle(21,52,place2, room44_displayed);
    add_Obstacle(23,53,place2, room44_displayed);
    add_Obstacle(14,69,place2, room55_displayed);
    add_Obstacle(16,72,place2, room55_displayed);
    add_Obstacle(25,86,place2, room66_displayed);

    add_stairs(22,53,place2, room44_displayed);

    add_gold(20, 54, place2, room44_displayed, gold_displayed2, 0);
    add_gold(28, 50, place2, room33_displayed, gold_displayed2, 1);
    add_gold(34, 35, place2, room22_displayed, gold_displayed2, 2);
    add_gold(15, 21, place2, room11_displayed, gold_displayed2, 3);
    add_gold(13, 70, place2, room55_displayed, gold_displayed2, 4);
    add_gold(15, 22, place2, room11_displayed, gold_displayed2, 5);
    add_gold(15, 23, place2, room11_displayed, gold_displayed2, 6);
    add_gold(15, 24, place2, room11_displayed, gold_displayed2, 7);
    add_gold(16, 21, place2, room11_displayed, gold_displayed2, 8);
    add_gold(16, 22, place2, room11_displayed, gold_displayed2, 9);
    add_gold(16, 23, place2, room11_displayed, gold_displayed2, 10);
    add_gold(16, 24, place2, room11_displayed, gold_displayed2, 11);
    add_gold(17, 21, place2, room11_displayed, gold_displayed2, 12);
    add_gold(17, 22, place2, room11_displayed, gold_displayed2, 13);
    add_gold(17, 23, place2, room11_displayed, gold_displayed2, 14);
    add_gold(17, 24, place2, room11_displayed, gold_displayed2, 15);
    add_black_gold(26, 84, place2, room66_displayed, black_gold_displayed2, 0);
    add_black_gold(18, 21, place2, room11_displayed, black_gold_displayed2, 1);
    add_black_gold(18, 22, place2, room11_displayed, black_gold_displayed2, 2);
    add_black_gold(18, 24, place2, room11_displayed, black_gold_displayed2, 3);

    

    add_food(23, 51, place2, room44_displayed, food_displayed2, 0);
    add_food(21, 54, place2, room44_displayed, food_displayed2, 1);
    add_food(30, 47, place2, room33_displayed, food_displayed2, 2);
    add_food(29, 51, place2, room33_displayed, food_displayed2, 3);
    add_food(25, 82, place2, room66_displayed, food_displayed2, 4);
    add_food(27, 82, place2, room66_displayed, food_displayed2, 5);
    add_food(16, 69, place2, room55_displayed, food_displayed2, 6);
    add_food(13, 72, place2, room55_displayed, food_displayed2, 7);

    
    if(!welcome_displayed2){
    mvprintw(0, 20, "Welcome to the new floor!");
    refresh();
    napms(2000); 
    mvprintw(0, 20, "                         ");
    refresh();
    welcome_displayed2 = 1;
    }

    handle_character_movement_game2(player_row, player_col, place2);

    refresh();

    getch();
    endwin();
    
    free_place(place2, y);
}

void add_gold(int row, int col, char** place, int display, int* flag, int index) {
    if (display && !flag[index]) {
        attron(COLOR_PAIR(6));
        mvprintw(row, col, "0");
        attroff(COLOR_PAIR(6));
        place[row][col] = '0';
        flag[index] = 1;
    }
}

void display_score() {
    mvprintw(0, COLS - 30, "Player Score: %d", player_score); 
}

void add_black_gold(int row, int col, char** place, int display, int* flag, int index) {
    if (display && !flag[index]) {
        attron(COLOR_PAIR(8));
        mvprintw(row, col, "$");
        attroff(COLOR_PAIR(8));
        place[row][col] = '$';
        flag[index] = 1;
    }
}

void add_food(int row, int col, char** place, int display, int* flag, int index) {
    if (display && !flag[index]) {
        attron(COLOR_PAIR(9));
        mvprintw(row, col, "@");
        attroff(COLOR_PAIR(9));
        place[row][col] = '@';
        flag[index] = 1;
    }
}

void display_status() {
    
    mvprintw(0, COLS - 30, "                     ");
    mvprintw(1, COLS - 30, "                     ");
    mvprintw(2, COLS - 30, "                     ");
    mvprintw(3, COLS - 30, "                     ");
    mvprintw(4, COLS - 30, "                     ");

    mvprintw(0, COLS - 30, "Player Score: %d", player_score);
    mvprintw(1, COLS - 30, "Player Health: %d", player_health);
    mvprintw(2, COLS - 30, "Food: %d", player_food);
    mvprintw(3, COLS - 30, "Hunger: %d", player_hunger);
    mvprintw(4, COLS - 30, "Hunger Bar: ");
    
    for (int i = 0; i < hunger_max; i++) {
        mvprintw(4, COLS - 18 + i, " ");
    }
    
    for (int i = 0; i < player_hunger; i++) {
        mvprintw(4, COLS - 18 + i, "|");
    }
    
    refresh();
}

void game_over() {
    clear();
    attron(A_BOLD);
    mvprintw(LINES / 2 - 2, COLS / 2 - 16, "You finished the game with a score of %d", player_score);
    refresh();
    napms(3000);
    mvprintw(LINES / 2 - 1, COLS / 2 - 8, "Your health is over");
    refresh();
    napms(3000);
    mvprintw(LINES / 2, COLS / 2 - 7, "You lost the game");
    refresh();
    napms(3000);
    endwin();
    exit(0);
}

void victory() {
    clear();
    attron(A_BOLD);
    mvprintw(LINES / 2 - 10, COLS / 2 - 16, "Congratulations, you have won the game!");
    refresh();
    napms(2000);
    mvprintw(LINES / 2 - 7, COLS / 2 - 7, "Your score is %d", player_score);
    refresh();
    napms(3000);
    mvprintw(LINES / 2 - 4, COLS / 2 - 5, "Well done!");
    refresh();
    napms(3000);
    endwin();
    exit(0);
}


