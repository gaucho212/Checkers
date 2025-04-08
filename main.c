#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define BOARD_SIZE 8

// Reprezentacja planszy: ' ' - puste, 'b' - pionek czarny, 'B' - damka czarna, 'w' - pionek biały, 'W' - damka biała
char board[BOARD_SIZE][BOARD_SIZE];

// Funkcje pomocnicze
bool is_white(char piece) { return piece == 'w' || piece == 'W'; }
bool is_black(char piece) { return piece == 'b' || piece == 'B'; }
bool is_queen(char piece) { return piece == 'W' || piece == 'B'; }

// Inicjalizacja planszy
void initialize_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ((i + j) % 2 == 0) {  // Czarne pola
                if (i < 3) board[i][j] = 'b';         // Pionki czarne
                else if (i > 4) board[i][j] = 'w';    // Pionki białe
                else board[i][j] = ' ';               // Puste pola
            } else {
                board[i][j] = ' ';  // Białe pola (niewykorzystane)
            }
        }
    }
}

// Wyświetlanie planszy
void print_board() {
    printf("\n  0 1 2 3 4 5 6 7\n");
    for (int i = 0; i < BOARD_SIZE; i++) {
        printf("%d ", i);
        for (int j = 0; j < BOARD_SIZE; j++) {
            printf("%c ", board[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

// Sprawdzenie, czy ruch jest legalny dla pionka
bool is_valid_pawn_move(int from_x, int from_y, int to_x, int to_y, char player) {
    if (from_x < 0 || from_x >= BOARD_SIZE || from_y < 0 || from_y >= BOARD_SIZE ||
        to_x < 0 || to_x >= BOARD_SIZE || to_y < 0 || to_y >= BOARD_SIZE) {
        return false;
    }
    if (board[to_x][to_y] != ' ') return false;

    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (abs(dx) != abs(dy)) return false;

    if (player == 'w') {
        if (dx > 0) return false;  // Biały pionek porusza się w górę
        if (abs(dx) == 1) return board[from_x][from_y] == 'w';
        if (abs(dx) == 2) {
            int mid_x = (from_x + to_x) / 2;
            int mid_y = (from_y + to_y) / 2;
            return board[from_x][from_y] == 'w' && is_black(board[mid_x][mid_y]);
        }
    } else if (player == 'b') {
        if (dx < 0) return false;  // Czarny pionek porusza się w dół
        if (abs(dx) == 1) return board[from_x][from_y] == 'b';
        if (abs(dx) == 2) {
            int mid_x = (from_x + to_x) / 2;
            int mid_y = (from_y + to_y) / 2;
            return board[from_x][from_y] == 'b' && is_white(board[mid_x][mid_y]);
        }
    }
    return false;
}

// Sprawdzenie, czy ruch jest legalny dla damki
bool is_valid_queen_move(int from_x, int from_y, int to_x, int to_y, char player) {
    if (from_x < 0 || from_x >= BOARD_SIZE || from_y < 0 || from_y >= BOARD_SIZE ||
        to_x < 0 || to_x >= BOARD_SIZE || to_y < 0 || to_y >= BOARD_SIZE) {
        return false;
    }
    if (board[to_x][to_y] != ' ') return false;

    int dx = to_x - from_x;
    int dy = to_y - from_y;
    if (abs(dx) != abs(dy)) return false;

    int step_x = (dx > 0) ? 1 : -1;
    int step_y = (dy > 0) ? 1 : -1;
    int x = from_x + step_x;
    int y = from_y + step_y;
    int captured = 0;

    while (x != to_x && y != to_y) {
        if (board[x][y] != ' ') {
            if ((player == 'W' && is_black(board[x][y])) || (player == 'B' && is_white(board[x][y]))) {
                captured++;
                if (captured > 1) return false;  // Damka może bić tylko jeden pionek na raz
            } else {
                return false;  // Blokada przez własny pionek
            }
        }
        x += step_x;
        y += step_y;
    }
    return true;
}

// Sprawdzenie, czy gracz ma możliwość bicia
bool has_capture(int x, int y, char player) {
    char piece = board[x][y];
    if (piece != player && piece != (player == 'w' ? 'W' : 'B')) return false;

    if (!is_queen(piece)) {  // Dla pionka
        int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        for (int d = 0; d < 4; d++) {
            int to_x = x + 2 * directions[d][0];
            int to_y = y + 2 * directions[d][1];
            int mid_x = x + directions[d][0];
            int mid_y = y + directions[d][1];
            if (to_x >= 0 && to_x < BOARD_SIZE && to_y >= 0 && to_y < BOARD_SIZE &&
                board[to_x][to_y] == ' ' && 
                ((player == 'w' && is_black(board[mid_x][mid_y])) || (player == 'b' && is_white(board[mid_x][mid_y])))) {
                return true;
            }
        }
    } else {  // Dla damki
        int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
        for (int d = 0; d < 4; d++) {
            int step_x = directions[d][0];
            int step_y = directions[d][1];
            int tx = x + step_x;
            int ty = y + step_y;
            while (tx >= 0 && tx < BOARD_SIZE && ty >= 0 && ty < BOARD_SIZE) {
                if (board[tx][ty] != ' ') {
                    if ((player == 'W' && is_black(board[tx][ty])) || (player == 'B' && is_white(board[tx][ty]))) {
                        int next_x = tx + step_x;
                        int next_y = ty + step_y;
                        if (next_x >= 0 && next_x < BOARD_SIZE && next_y >= 0 && next_y < BOARD_SIZE && board[next_x][next_y] == ' ') {
                            return true;
                        }
                    }
                    break;
                }
                tx += step_x;
                ty += step_y;
            }
        }
    }
    return false;
}

// Sprawdzenie, czy gracz ma jakiekolwiek legalne ruchy
bool has_legal_moves(char player) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if ((player == 'w' && is_white(board[i][j])) || (player == 'b' && is_black(board[i][j]))) {
                if (has_capture(i, j, player)) return true;
                if (!is_queen(board[i][j])) {  // Ruchy zwykłe dla pionka
                    int directions[2][2] = {{-1, -1}, {-1, 1}};
                    if (player == 'b') directions[0][0] = 1, directions[1][0] = 1;
                    for (int d = 0; d < 2; d++) {
                        int to_x = i + directions[d][0];
                        int to_y = j + directions[d][1];
                        if (to_x >= 0 && to_x < BOARD_SIZE && to_y >= 0 && to_y < BOARD_SIZE && board[to_x][to_y] == ' ') {
                            return true;
                        }
                    }
                } else {  // Ruchy dla damki
                    int directions[4][2] = {{-1, -1}, {-1, 1}, {1, -1}, {1, 1}};
                    for (int d = 0; d < 4; d++) {
                        int step_x = directions[d][0];
                        int step_y = directions[d][1];
                        int tx = i + step_x;
                        int ty = j + step_y;
                        while (tx >= 0 && tx < BOARD_SIZE && ty >= 0 && ty < BOARD_SIZE && board[tx][ty] == ' ') {
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Wykonanie ruchu
void make_move(int from_x, int from_y, int to_x, int to_y) {
    char piece = board[from_x][from_y];
    board[to_x][to_y] = piece;
    board[from_x][from_y] = ' ';

    // Usunięcie zbitego pionka
    if (abs(to_x - from_x) == 2 || is_queen(piece)) {
        int dx = to_x - from_x;
        int dy = to_y - from_y;
        int step_x = (dx > 0) ? 1 : -1;
        int step_y = (dy > 0) ? 1 : -1;
        int x = from_x + step_x;
        int y = from_y + step_y;
        while (x != to_x && y != to_y) {
            if (board[x][y] != ' ') {
                board[x][y] = ' ';
                break;
            }
            x += step_x;
            y += step_y;
        }
    }

    // Promocja na damkę
    if (piece == 'w' && to_x == 0) board[to_x][to_y] = 'W';
    if (piece == 'b' && to_x == 7) board[to_x][to_y] = 'B';
}

// Główna funkcja gry
int main() {
    initialize_board();
    char current_player = 'w';
    int from_x, from_y, to_x, to_y;

    while (true) {
        print_board();
        printf("Gracz %c, podaj ruch (from_x from_y to_x to_y): ", current_player);
        scanf("%d %d %d %d", &from_x, &from_y, &to_x, &to_y);

        // Sprawdzenie, czy wybrano prawidłowy pionek
        if (board[from_x][from_y] != current_player && board[from_x][from_y] != (current_player == 'w' ? 'W' : 'B')) {
            printf("Nieprawidłowy pionek! Spróbuj ponownie.\n");
            continue;
        }

        // Sprawdzenie legalności ruchu
        bool is_capture_move = false;
        if (is_queen(board[from_x][from_y])) {
            is_capture_move = is_valid_queen_move(from_x, from_y, to_x, to_y, board[from_x][from_y]);
        } else {
            is_capture_move = is_valid_pawn_move(from_x, from_y, to_x, to_y, current_player);
        }

        // Sprawdzenie, czy gracz musi bić
        bool must_capture = false;
        for (int i = 0; i < BOARD_SIZE; i++) {
            for (int j = 0; j < BOARD_SIZE; j++) {
                if (has_capture(i, j, current_player)) {
                    must_capture = true;
                    break;
                }
            }
            if (must_capture) break;
        }

        if (must_capture && !is_capture_move) {
            printf("Musisz wykonać bicie! Spróbuj ponownie.\n");
            continue;
        }

        if (is_capture_move || (!must_capture && (is_valid_pawn_move(from_x, from_y, to_x, to_y, current_player) || is_valid_queen_move(from_x, from_y, to_x, to_y, board[from_x][from_y])))) {
            make_move(from_x, from_y, to_x, to_y);
            current_player = (current_player == 'w') ? 'b' : 'w';
        } else {
            printf("Nieprawidłowy ruch! Spróbuj ponownie.\n");
            continue;
        }

        // Sprawdzenie końca gry
        if (!has_legal_moves(current_player)) {
            print_board();
            printf("Gracz %c nie ma legalnych ruchów. Gracz %c wygrywa!\n", current_player, (current_player == 'w') ? 'b' : 'w');
            break;
        }
    }

    return 0;
}