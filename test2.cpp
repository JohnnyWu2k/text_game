#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <unordered_map>
#include <sstream>
#include <curses.h>
using namespace std;

// Game variables
unordered_map<string, vector<vector<char>>> worlds;
int player_x = 1, player_y = 1;
int map_width = 15, map_height = 15;
string current_world = "0_0";
bool input_mode = false;

// Utility functions
void clear_terminal() {
    clear();
}

// Generate random maze
vector<string> generate_random_maze(int width, int height, mt19937& rng) {
    vector<string> maze(height, string(width, '#'));
    uniform_int_distribution<int> dist(1, width - 2);

    int start_x = dist(rng) | 1;
    int start_y = dist(rng) | 1;
    maze[start_y][start_x] = ' ';
    vector<pair<int, int>> walls = {{start_y, start_x}};

    while (!walls.empty()) {
        int idx = rng() % walls.size();
        auto [y, x] = walls[idx];
        walls.erase(walls.begin() + idx);

        vector<pair<int, int>> directions = {{0, 2}, {0, -2}, {2, 0}, {-2, 0}};
        shuffle(directions.begin(), directions.end(), rng);

        for (auto [dy, dx] : directions) {
            int ny = y + dy, nx = x + dx;
            if (ny > 0 && ny < height - 1 && nx > 0 && nx < width - 1 && maze[ny][nx] == '#') {
                maze[ny][nx] = ' ';
                maze[y + dy / 2][x + dx / 2] = ' ';
                walls.emplace_back(ny, nx);
            }
        }
    }

    maze[1][1] = ' ';
    return maze;
}

// Create new world
vector<vector<char>> create_new_world(int width, int height, int seed) {
    mt19937 rng(seed);
    vector<string> maze = generate_random_maze(width, height, rng);
    vector<vector<char>> map(height, vector<char>(width, '#'));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            map[y][x] = maze[y][x];
        }
    }
    return map;
}

// Display map
void display_map() {
    clear_terminal();
    auto& map = worlds[current_world];
    for (int y = 0; y < map_height; ++y) {
        for (int x = 0; x < map_width; ++x) {
            if (y == player_y && x == player_x)
                mvaddch(y, x, 'P');
            else
                mvaddch(y, x, map[y][x]);
        }
    }
    refresh();
}

// Move player
void move_player(char direction) {
    int new_y = player_y, new_x = player_x;

    if (direction == 'w') --new_y;
    else if (direction == 's') ++new_y;
    else if (direction == 'a') --new_x;
    else if (direction == 'd') ++new_x;

    auto& map = worlds[current_world];
    if (new_y >= 0 && new_y < map_height && new_x >= 0 && new_x < map_width && map[new_y][new_x] == ' ') {
        player_y = new_y;
        player_x = new_x;
    }
}

// Main game loop
void main_game_loop() {
    while (true) {
        display_map();
        if (input_mode) {
            mvprintw(map_height, 0, "Input mode: Type a command (exit to quit): ");
            char command[50];
            echo();
            getstr(command);
            noecho();

            if (string(command) == "exit") {
                input_mode = false;
            }
        } else {
            mvprintw(map_height, 0, "Move with WASD or press 't' for input mode.");
            int key = getch();
            if (key == 't') {
                input_mode = true;
            } else {
                move_player(key);
            }
        }
    }
}

// Main menu
void show_main_menu() {
    clear_terminal();
    mvprintw(0, 0, "Welcome to the Smooth Adventure Game!");
    mvprintw(1, 0, "1. Generate new");
    mvprintw(2, 0, "2. Quit");
    mvprintw(3, 0, "Use arrow keys to choose an option, press Enter to confirm.");
    refresh();

    int choice = 0;
    while (true) {
        mvprintw(1, 20, choice == 0 ? "<--" : "    ");
        mvprintw(2, 20, choice == 1 ? "<--" : "    ");
        refresh();

        int key = getch();
        if (key == KEY_UP || key == 'w') {
            choice = (choice - 1 + 2) % 2;
        } else if (key == KEY_DOWN || key == 's') {
            choice = (choice + 1) % 2;
        } else if (key == '\n') {
            break;
        }
    }

    if (choice == 0) { // Generate world
        clear_terminal();
        mvprintw(0, 0, "Enter a seed for the world generation: ");
        refresh();

        echo();
        char seed_input[20] = {0};
        getstr(seed_input);
        noecho();

        int seed = atoi(seed_input);
        worlds[current_world] = create_new_world(map_width, map_height, seed);

        clear_terminal();
        mvprintw(0, 0, "World generated successfully! Press any key to continue...");
        refresh();
        getch();
    } else {
        endwin();
        exit(0);
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    show_main_menu();
    main_game_loop();

    endwin();
    return 0;
}
