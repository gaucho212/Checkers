#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define BOARD_SIZE 8

// Reprezentacja planszy: ' ' - puste, 'b' - pionek czarny, 'B' - damka czarna,
// 'w' - pionek biały, 'W' - damka biała
char board[BOARD_SIZE][BOARD_SIZE];

// Funkcje pomocnicze
bool is_white(char piece) { return piece == 'w' || piece == 'W'; }
bool is_black(char piece) { return piece == 'b' || piece == 'B'; }
bool is_queen(char piece) { return piece == 'W' || piece == 'B'; }

void initialize_board() {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if ((r + c) % 2 == 0) {
                if (r < 3) board[r][c] = 'b';
                else if (r > 4) board[r][c] = 'w';
                else board[r][c] = ' ';
            } else {
                board[r][c] = ' ';
            }
        }
    }
}

// Wyświetlanie planszy
void print_board() {
    printf("\n  0 1 2 3 4 5 6 7\n");
    for (int r = 0; r < BOARD_SIZE; r++) {
        printf("%d ", r);
        for (int c = 0; c < BOARD_SIZE; c++) {
            printf("%c ", board[r][c]);
        }
        printf("\n");
    }
    printf("\n");
}

// Sprawdzenie legalności ruchu pionka
bool is_valid_pawn_move(int fr, int fc, int tr, int tc, char player) {
    if (tr < 0 || tr >= BOARD_SIZE || tc < 0 || tc >= BOARD_SIZE) return false;
    if (board[tr][tc] != ' ') return false;

    int dr = tr - fr;
    int dc = tc - fc;
    if (abs(dr) != abs(dc)) return false;

    if (player == 'w') {
        if (dr > 0) return false;  // Biały pionek porusza się w górę
        if (abs(dr) == 1) return board[fr][fc] == 'w';
        if (abs(dr) == 2) {
            int mr = (fr + tr) / 2;
            int mc = (fc + tc) / 2;
            return board[fr][fc] == 'w' && is_black(board[mr][mc]);
        }
    } else {
        if (dr < 0) return false;  // Czarny pionek porusza się w dół
        if (abs(dr) == 1) return board[fr][fc] == 'b';
        if (abs(dr) == 2) {
            int mr = (fr + tr) / 2;
            int mc = (fc + tc) / 2;
            return board[fr][fc] == 'b' && is_white(board[mr][mc]);
        }
    }
    return false;
}

// Sprawdzenie legalności ruchu damki
bool is_valid_queen_move(int fr, int fc, int tr, int tc, char player) {
    if (tr < 0 || tr >= BOARD_SIZE || tc < 0 || tc >= BOARD_SIZE) return false;
    if (board[tr][tc] != ' ') return false;

    int dr = tr - fr;
    int dc = tc - fc;
    if (abs(dr) != abs(dc)) return false;

    int sr = (dr > 0) ? 1 : -1;
    int sc = (dc > 0) ? 1 : -1;
    int r = fr + sr, c = fc + sc;
    int captured = 0;

    while (r != tr && c != tc) {
        if (board[r][c] != ' ') {
            if ((player == 'W' && is_black(board[r][c])) || (player == 'B' && is_white(board[r][c]))) {
                captured++;
                if (captured > 1) return false;
            } else {
                return false;
            }
        }
        r += sr;
        c += sc;
    }
    return true;
}

// Sprawdzenie możliwości bicia
// Sprawdzenie możliwości bicia
bool has_capture(int r, int c, char player) {
    char piece = board[r][c];
    if (piece != player && piece != (player == 'w' ? 'W' : 'B')) return false;

    int dirs[4][2];
    int num_dirs = 0;

    if (!is_queen(piece)) {
        if (player == 'w') {
            // Białe pionki biją w górę: (-1, -1) i (-1, 1)
            dirs[0][0] = -1; dirs[0][1] = -1;
            dirs[1][0] = -1; dirs[1][1] = 1;
            num_dirs = 2;
        } else {
            // Czarne pionki biją w dół: (1, -1) i (1, 1)
            dirs[0][0] = 1; dirs[0][1] = -1;
            dirs[1][0] = 1; dirs[1][1] = 1;
            num_dirs = 2;
        }
    } else {
        // Damki sprawdzają wszystkie cztery kierunki
        dirs[0][0] = 1; dirs[0][1] = 1;
        dirs[1][0] = 1; dirs[1][1] = -1;
        dirs[2][0] = -1; dirs[2][1] = 1;
        dirs[3][0] = -1; dirs[3][1] = -1;
        num_dirs = 4;
    }

    for (int i = 0; i < num_dirs; i++) {
        int tr = r + 2 * dirs[i][0];
        int tc = c + 2 * dirs[i][1];
        int mr = r + dirs[i][0];
        int mc = c + dirs[i][1];
        if (!is_queen(piece)) {
            if (tr >= 0 && tr < BOARD_SIZE && tc >= 0 && tc < BOARD_SIZE && board[tr][tc] == ' ' &&
                ((player == 'w' && is_black(board[mr][mc])) || (player == 'b' && is_white(board[mr][mc])))) {
                return true;
            }
        } else {
            // Logika dla damki pozostaje bez zmian
            int sr = dirs[i][0], sc = dirs[i][1];
            int rr = r + sr, cc = c + sc;
            bool found_opponent = false;
            while (rr >= 0 && rr < BOARD_SIZE && cc >= 0 && cc < BOARD_SIZE) {
                if (board[rr][cc] != ' ') {
                    if ((player == 'W' && is_black(board[rr][cc])) || (player == 'B' && is_white(board[rr][cc]))) {
                        if (found_opponent) break;
                        found_opponent = true;
                    } else {
                        break;
                    }
                } else if (found_opponent) {
                    return true;
                }
                rr += sr;
                cc += sc;
            }
        }
    }
    return false;
}

// Sprawdzenie legalnych ruchów
bool has_legal_moves(char player) {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if ((player == 'w' && is_white(board[r][c])) || (player == 'b' && is_black(board[r][c]))) {
                if (has_capture(r, c, player)) return true;
                if (!is_queen(board[r][c])) {
                    int dirs[2][2] = {{-1, -1}, {-1, 1}};
                    if (player == 'b') { dirs[0][0] = 1; dirs[1][0] = 1; }
                    for (int i = 0; i < 2; i++) {
                        int tr = r + dirs[i][0];
                        int tc = c + dirs[i][1];
                        if (tr >= 0 && tr < BOARD_SIZE && tc >= 0 && tc < BOARD_SIZE && board[tr][tc] == ' ') {
                            return true;
                        }
                    }
                } else {
                    int dirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
                    for (int i = 0; i < 4; i++) {
                        int rr = r + dirs[i][0];
                        int cc = c + dirs[i][1];
                        while (rr >= 0 && rr < BOARD_SIZE && cc >= 0 && cc < BOARD_SIZE && board[rr][cc] == ' ') {
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
void make_move(int fr, int fc, int tr, int tc) {
    char p = board[fr][fc];
    board[fr][fc] = ' ';
    board[tr][tc] = p;

    if (abs(tr - fr) == 2 || is_queen(p)) {
        int sr = (tr > fr) ? 1 : -1;
        int sc = (tc > fc) ? 1 : -1;
        int rr = fr + sr, cc = fc + sc;
        while (rr != tr && cc != tc) {
            if (board[rr][cc] != ' ') {
                board[rr][cc] = ' ';
                break;
            }
            rr += sr;
            cc += sc;
        }
    }

    if (p == 'w' && tr == 0) board[tr][tc] = 'W';
    if (p == 'b' && tr == 7) board[tr][tc] = 'B';
}

static const char* piece_image_path(int r, int c) {
    switch (board[r][c]) {
        case 'b': return "black.png";
        case 'B': return "black_queen.png";
        case 'w': return "white.png";
        case 'W': return "white_queen.png";
        default:  return NULL;
    }
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *grid, *cell, *img;
    GtkCssProvider *provider;
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen  *screen  = gdk_display_get_default_screen(display);

    initialize_board();

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Warcaby");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_container_add(GTK_CONTAINER(window), grid);

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        ".white-cell { background-color: beige; }\n"
        ".black-cell { background-color: sienna; }\n",
        -1, NULL);
    gtk_style_context_add_provider_for_screen(screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER);

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            cell = gtk_event_box_new();
            gtk_widget_set_name(cell, ((r + c) % 2 == 0) ? "white-cell" : "black-cell");
            const char *path = piece_image_path(r, c);
            if (path) {
                img = gtk_image_new_from_file(path);
                gtk_container_add(GTK_CONTAINER(cell), img);
            }
            gtk_grid_attach(GTK_GRID(grid), cell, c, r, 1, 1);
            gtk_widget_show(cell);
        }
    }

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    initialize_board();

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    char current_player = 'w';
    int fr, fc, tr, tc;

    while (true) {
        print_board();
        printf("Gracz %c, podaj ruch (wiersz kolumna wiersz kolumna): ", current_player);
        if (scanf("%d %d %d %d", &fr, &fc, &tr, &tc) != 4) break;

        if (board[fr][fc] != current_player && board[fr][fc] != (current_player == 'w' ? 'W' : 'B')) {
            printf("Nieprawidłowy pionek! Spróbuj ponownie.\n");
            continue;
        }

        bool capture = false;
        if (is_queen(board[fr][fc])) capture = is_valid_queen_move(fr, fc, tr, tc, board[fr][fc]);
        else capture = is_valid_pawn_move(fr, fc, tr, tc, current_player);

        bool must_capture = false;
        for (int r = 0; r < BOARD_SIZE && !must_capture; r++) {
            for (int c = 0; c < BOARD_SIZE; c++) {
                if (has_capture(r, c, current_player)) { must_capture = true; break; }
            }
        }

        if (must_capture && !capture) {
            printf("Musisz wykonać bicie! Spróbuj ponownie.\n");
            continue;
        }

        if (capture || (!must_capture && (is_valid_pawn_move(fr, fc, tr, tc, current_player) ||
            is_valid_queen_move(fr, fc, tr, tc, board[fr][fc])))) {
            make_move(fr, fc, tr, tc);
            current_player = (current_player == 'w') ? 'b' : 'w';
        } else {
            printf("Nieprawidłowy ruch! Spróbuj ponownie.\n");
            continue;
        }

        if (!has_legal_moves(current_player)) {
            print_board();
            printf("Gracz %c nie ma legalnych ruchów. Gracz %c wygrywa!\n", current_player,
                (current_player == 'w') ? 'b' : 'w');
            break;
        }
    }

    return status;
}

// Kompilacja: gcc -o warcaby main.c `pkg-config --cflags --libs gtk+-3.0`
