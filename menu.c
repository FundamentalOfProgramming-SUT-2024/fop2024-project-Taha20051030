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
void handle_screen_game1();
void handle_screen_game2();
void draw_vertical_line(int start_row, int end_row, int col, char** place);
void draw_horizontal_line(int start_col, int end_col, int row, char** place);
void fill_rectangle_with_dots(int start_row, int end_row, int start_col, int end_col, char** place);
void handle_screen_game1();
void draw_door(int start_row, int start_col, char** place);
void draw_corridor(int start_row, int end_row, int start_col, int end_col, char** place);
void handle_character_movement(int* player_row, int* player_col, char** place);
char** create_place(int rows, int cols);
void free_place(char** place, int rows);
void display_adjacent_corridors(int row, int col, char** place);
void display_room(int start_row, int end_row, int start_col, int end_col, char** place);

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

void handle_menu()
{
    int ch;
    int highlight = 0;
    int choice = -1;

    curs_set(0);

    init_pair(1, COLOR_WHITE, COLOR_BLUE);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);//Warning
    init_pair(5, COLOR_BLUE, COLOR_BLACK);//login_success
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);//rank1
    init_pair(7, COLOR_MAGENTA, COLOR_BLACK);//rank2
    init_pair(8, COLOR_CYAN, COLOR_BLACK);//rank3


    while (1)
    {
        clear();
        int x = 10;
        int y = 5;
        char *choices[] = {"Login", "Register", "Login as Guest"};

        for (int i = 0; i < 3; i++)
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
            if (highlight < 2)
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

    if (choice == 0)
    {
        handle_login();
    }
    else if (choice == 1)
    {
        handle_regist();
    }
    else if (choice == 2)
    {
        handle_screen_game1();
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

void handle_screen_game1() {
    initscr();
    curs_set(0);
    clear();
    refresh();
    int y, x;
    getmaxyx(stdscr, y, x);
    char** place1 = create_place(y, x);

    memset(*place1, ' ', y * x * sizeof(char));

   
    draw_vertical_line(10, 13, 20, place1);
    draw_vertical_line(10, 13, 25, place1);
    draw_vertical_line(11, 16, 40, place1);
    draw_vertical_line(11, 16, 45, place1);
    draw_vertical_line(20, 24, 50, place1);
    draw_vertical_line(20, 24, 55, place1);
    draw_vertical_line(18, 21, 60, place1);
    draw_vertical_line(18, 21, 69, place1);
    draw_vertical_line(27, 33, 40, place1);
    draw_vertical_line(27, 33, 45, place1);
    draw_vertical_line(20, 26, 20, place1);
    draw_vertical_line(20, 26, 30, place1);
    
    draw_horizontal_line(21, 24, 9, place1);
    draw_horizontal_line(21, 24, 14, place1);
    draw_horizontal_line(41, 44, 10, place1);
    draw_horizontal_line(41, 44, 17, place1);
    draw_horizontal_line(51, 54, 19, place1);
    draw_horizontal_line(51, 54, 25, place1);
    draw_horizontal_line(61, 68, 17, place1);
    draw_horizontal_line(61, 68, 22, place1);
    draw_horizontal_line(41, 44, 26, place1);
    draw_horizontal_line(41, 44, 34, place1);
    draw_horizontal_line(21, 29, 19, place1);
    draw_horizontal_line(21, 29, 27, place1);
    
    fill_rectangle_with_dots(10, 13, 21, 24, place1);
    fill_rectangle_with_dots(11, 16, 41, 44, place1);
    fill_rectangle_with_dots(20, 24, 51, 54, place1);
    fill_rectangle_with_dots(18, 21, 61, 68, place1);
    fill_rectangle_with_dots(27, 33, 41, 44, place1);
    fill_rectangle_with_dots(20, 26, 21, 29, place1);

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
    
    int player_row = 11, player_col = 21;
    handle_character_movement(&player_row, &player_col, place1);
    refresh();
    getch();
    endwin();
    
    free_place(place1, y);
}

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

void draw_vertical_line(int start_row, int end_row, int col, char** place) {
    for (int i = start_row; i <= end_row; i++) {
        mvprintw(i, col, "|");
        place[i][col] = '|';
    }
}

void draw_horizontal_line(int start_col, int end_col, int row, char** place) {
    for (int i = start_col; i <= end_col; i++) {
        mvprintw(row, i, "-");
        place[row][i] = '-';
    }
}

void fill_rectangle_with_dots(int start_row, int end_row, int start_col, int end_col, char** place) {
    for (int i = start_row; i <= end_row; i++) {
        for (int j = start_col; j <= end_col; j++) {
            mvprintw(i, j, ".");
            place[i][j] = '.';
        }
    }
}

void draw_door(int start_row, int start_col, char** place) {
    mvprintw(start_row, start_col, "+");
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

void display_adjacent_corridors(int row, int col, char** place) {
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            if ((i != 0 || j != 0) && place[row + i][col + j] == '#') {
                mvprintw(row + i, col + j, "#");
            }
        }
    }
}

void handle_character_movement(int* player_row, int* player_col, char** place) {
    int ch;
    keypad(stdscr, TRUE);  

    while (1) {
        
        mvprintw(*player_row, *player_col, "X");  
        display_adjacent_corridors(*player_row, *player_col, place); 

        
        ch = getch();

        
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

        
        if (place[new_row][new_col] == '.' || place[new_row][new_col] == '+' || place[new_row][new_col] == '#') {
            
            mvprintw(*player_row, *player_col, "%c", place[*player_row][*player_col]);

            
            *player_row = new_row;
            *player_col = new_col;

            
            mvprintw(*player_row, *player_col, "X");

            
            display_corridor(*player_row, *player_col, place);
            display_room(10, 13, 20, 25, place);  
            display_room(11, 16, 40, 45, place);  
            display_room(20, 24, 50, 55, place);  
            display_room(18, 21, 60, 69, place);  
            display_room(27, 33, 40, 45, place);  
            display_room(20, 26, 20, 30, place);  
        }

        refresh();
    }
}


void display_room(int start_row, int end_row, int start_col, int end_col, char** place) {
    for (int i = start_row; i <= end_row; i++) {
        for (int j = start_col; j <= end_col; j++) {
            mvprintw(i, j, "%c", place[i][j]);
        }
    }
}

