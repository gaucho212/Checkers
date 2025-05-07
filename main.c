#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#define BOARD_SIZE 8

char board[BOARD_SIZE][BOARD_SIZE];
char current_player = 'w';  // Zaczyna gracz 'w' (biały)
GtkWidget *player_label;    // Etykieta pokazująca aktualnego gracza
GtkWidget *stack;           // GtkStack do przełączania widoków
GtkWidget *grid;            // Plansza gry

bool is_white(char piece) { return piece == 'w' || piece == 'W'; }
bool is_black(char piece) { return piece == 'b' || piece == 'B'; }
bool is_queen(char piece) { return piece == 'W' || piece == 'B'; }

void initialize_board() {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            board[r][c] = ' ';
        }
    }
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if ((r + c) % 2 == 1) board[r][c] = 'b';
        }
    }
    for (int r = 5; r < 8; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if ((r + c) % 2 == 1) board[r][c] = 'w';
        }
    }
}

bool is_valid_pawn_move(int fr, int fc, int tr, int tc, char player) {
    if (tr < 0 || tr >= BOARD_SIZE || tc < 0 || tc >= BOARD_SIZE) return false;
    if (board[tr][tc] != ' ') return false;

    int dr = tr - fr;
    int dc = tc - fc;
    if (abs(dr) != abs(dc)) return false;

    int direction = (player == 'w') ? -1 : 1;
    if (abs(dr) == 1) {
        return dr == direction && board[fr][fc] == player;
    }
    if (abs(dr) == 2 && dr == 2 * direction) {
        int mr = fr + direction;
        int mc = fc + (dc > 0 ? 1 : -1);
        if (board[fr][fc] != player) return false;
        return player == 'w' ? is_black(board[mr][mc]) : is_white(board[mr][mc]);
    }
    return false;
}

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

bool has_capture(int r, int c, char player) {
    char piece = board[r][c];
    if (piece != player && piece != (player == 'w' ? 'W' : 'B')) return false;

    int dirs[4][2];
    int num_dirs = is_queen(piece) ? 4 : 2;

    if (!is_queen(piece)) {
        if (player == 'w') {
            dirs[0][0] = -1; dirs[0][1] = -1;
            dirs[1][0] = -1; dirs[1][1] = 1;
        } else {
            dirs[0][0] = 1; dirs[0][1] = -1;
            dirs[1][0] = 1; dirs[1][1] = 1;
        }
    } else {
        dirs[0][0] = 1; dirs[0][1] = 1;
        dirs[1][0] = 1; dirs[1][1] = -1;
        dirs[2][0] = -1; dirs[2][1] = 1;
        dirs[3][0] = -1; dirs[3][1] = -1;
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
        default: return NULL;
    }
}

static bool piece_selected = false;
static int sel_r = -1, sel_c = -1;

static void update_cell_image(GtkWidget *event_box, gpointer user_data);
static gboolean make_click_move(int fr, int fc, int tr, int tc);
static gboolean on_square_clicked(GtkWidget *event_box, GdkEventButton *event, gpointer user_data);
static void highlight_available_moves(int fr, int fc, char piece);
static void clear_highlights();

static gboolean make_click_move(int fr, int fc, int tr, int tc) {
    char piece = board[fr][fc];
    if ((current_player == 'w' && !is_white(piece)) || (current_player == 'b' && !is_black(piece))) {
        return FALSE;  // Nie ten gracz
    }

    char player = is_white(piece) ? 'w' : 'b';
    bool is_valid = is_queen(piece) ? 
        is_valid_queen_move(fr, fc, tr, tc, piece) : 
        is_valid_pawn_move(fr, fc, tr, tc, player);

    if (!is_valid) return FALSE;

    bool capture = abs(tr - fr) == 2 || is_queen(piece);
    bool must_capture = false;
    for (int r = 0; r < BOARD_SIZE && !must_capture; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (has_capture(r, c, player)) {
                must_capture = true;
                break;
            }
        }
    }

    if (must_capture && !capture) return FALSE;

    make_move(fr, fc, tr, tc);
    current_player = (current_player == 'w') ? 'b' : 'w';  // Zmień gracza

    char label_text[50];
    sprintf(label_text, "Ruch gracza: %s", current_player == 'w' ? "Biały" : "Czarny");
    gtk_label_set_text(GTK_LABEL(player_label), label_text);

    if (!has_legal_moves(current_player)) {
        char win_msg[50];
        sprintf(win_msg, "Gracz %s wygrywa!", current_player == 'w' ? "Czarny" : "Biały");
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", win_msg);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_stack_set_visible_child_name(GTK_STACK(stack), "menu");
    }

    return TRUE;
}

static void start_game(GtkButton *button, gpointer user_data) {
    initialize_board();  // Reset planszy
    current_player = 'w';
    gtk_label_set_text(GTK_LABEL(player_label), "Ruch gracza: Biały");
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "game");
    gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)update_cell_image, NULL);
}

static void return_to_menu(GtkButton *button, gpointer user_data) {
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "menu");
}

static void update_cell_image(GtkWidget *event_box, gpointer user_data) {
    GtkWidget *frame = gtk_bin_get_child(GTK_BIN(event_box));
    GList *children = gtk_container_get_children(GTK_CONTAINER(frame));
    for (GList *l = children; l; l = l->next) {
        if (GTK_IS_IMAGE(l->data)) gtk_widget_destroy(GTK_WIDGET(l->data));
    }
    g_list_free(children);

    int r = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(event_box), "row"));
    int c = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(event_box), "col"));
    const char *path = piece_image_path(r, c);
    if (path) {
        GtkWidget *img = gtk_image_new_from_file(path);
        gtk_container_add(GTK_CONTAINER(frame), img);
    }
    gtk_widget_show_all(frame);
}

static void highlight_available_moves(int fr, int fc, char piece) {
    char player = is_white(piece) ? 'w' : 'b';
    if (!is_queen(piece)) {
        int dirs[2][2] = {{-1, -1}, {-1, 1}};
        if (player == 'b') { dirs[0][0] = 1; dirs[1][0] = 1; }
        for (int i = 0; i < 2; i++) {
            int tr = fr + dirs[i][0];
            int tc = fc + dirs[i][1];
            if (tr >= 0 && tr < BOARD_SIZE && tc >= 0 && tc < BOARD_SIZE && board[tr][tc] == ' ' &&
                is_valid_pawn_move(fr, fc, tr, tc, player)) {
                GtkWidget *event_box = gtk_grid_get_child_at(GTK_GRID(grid), tc, tr);
                GtkStyleContext *ctx = gtk_widget_get_style_context(event_box);
                gtk_style_context_add_class(ctx, "available-move");
            }
        }
        // Sprawdzanie bicia
        int capture_dirs[4][2] = {{-2, -2}, {-2, 2}, {2, -2}, {2, 2}};
        for (int i = 0; i < 4; i++) {
            int tr = fr + capture_dirs[i][0];
            int tc = fc + capture_dirs[i][1];
            if (tr >= 0 && tr < BOARD_SIZE && tc >= 0 && tc < BOARD_SIZE && board[tr][tc] == ' ' &&
                is_valid_pawn_move(fr, fc, tr, tc, player)) {
                GtkWidget *event_box = gtk_grid_get_child_at(GTK_GRID(grid), tc, tr);
                GtkStyleContext *ctx = gtk_widget_get_style_context(event_box);
                gtk_style_context_add_class(ctx, "available-move");
            }
        }
    } else {
        int dirs[4][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}};
        for (int i = 0; i < 4; i++) {
            int rr = fr + dirs[i][0];
            int cc = fc + dirs[i][1];
            while (rr >= 0 && rr < BOARD_SIZE && cc >= 0 && cc < BOARD_SIZE) {
                if (is_valid_queen_move(fr, fc, rr, cc, piece)) {
                    GtkWidget *event_box = gtk_grid_get_child_at(GTK_GRID(grid), cc, rr);
                    GtkStyleContext *ctx = gtk_widget_get_style_context(event_box);
                    gtk_style_context_add_class(ctx, "available-move");
                }
                if (board[rr][cc] != ' ') break;
                rr += dirs[i][0];
                cc += dirs[i][1];
            }
        }
    }
}

static void clear_highlights() {
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            GtkWidget *event_box = gtk_grid_get_child_at(GTK_GRID(grid), c, r);
            GtkStyleContext *ctx = gtk_widget_get_style_context(event_box);
            gtk_style_context_remove_class(ctx, "available-move");
        }
    }
}

static gboolean on_square_clicked(GtkWidget *event_box, GdkEventButton *event, gpointer user_data) {
    GtkWidget *grid = GTK_WIDGET(user_data);
    int r = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(event_box), "row"));
    int c = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(event_box), "col"));

    if (!piece_selected) {
        char piece = board[r][c];
        if ((current_player == 'w' && is_white(piece)) || (current_player == 'b' && is_black(piece))) {
            sel_r = r;
            sel_c = c;
            piece_selected = true;
            GtkStyleContext *ctx = gtk_widget_get_style_context(event_box);
            gtk_style_context_add_class(ctx, "selected-cell");
            highlight_available_moves(r, c, piece);
        }
    } else {
        if (make_click_move(sel_r, sel_c, r, c)) {
            gtk_container_foreach(GTK_CONTAINER(grid), (GtkCallback)update_cell_image, NULL);
        }
        clear_highlights();
        GtkWidget *prev = g_object_get_data(G_OBJECT(grid), "last_selected");
        if (prev) {
            gtk_style_context_remove_class(gtk_widget_get_style_context(prev), "selected-cell");
        }
        piece_selected = false;
        sel_r = sel_c = -1;
    }

    g_object_set_data(G_OBJECT(grid), "last_selected", event_box);
    return TRUE;
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *menu_box, *game_box, *end_box, *start_button, *return_button;
    GtkCssProvider *provider;
    GdkDisplay *display = gdk_display_get_default();
    GdkScreen *screen = gdk_display_get_default_screen(display);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Warcaby");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    stack = gtk_stack_new();
    gtk_container_add(GTK_CONTAINER(window), stack);

    // Menu startowe
    menu_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    start_button = gtk_button_new_with_label("Rozpocznij grę");
    g_signal_connect(start_button, "clicked", G_CALLBACK(start_game), NULL);
    gtk_box_pack_start(GTK_BOX(menu_box), start_button, TRUE, TRUE, 0);
    gtk_stack_add_named(GTK_STACK(stack), menu_box, "menu");

    // Ekran gry
    game_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    player_label = gtk_label_new("Ruch gracza: Biały");
    gtk_box_pack_start(GTK_BOX(game_box), player_label, FALSE, FALSE, 0);

    grid = gtk_grid_new();
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_box_pack_start(GTK_BOX(game_box), grid, TRUE, TRUE, 0);

    provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        ".white-cell { background-color: beige; }\n"
        ".black-cell { background-color: sienna; }\n"
        ".selected-cell { background-color: lightblue; }\n"
        ".available-move { background-color: #F0E68C;}\n",
        -1, NULL);
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            GtkWidget *event_box = gtk_event_box_new();
            GtkWidget *frame = gtk_frame_new(NULL);
            gtk_container_add(GTK_CONTAINER(event_box), frame);
            GtkStyleContext *ctx = gtk_widget_get_style_context(event_box);
            g_object_set_data(G_OBJECT(event_box), "row", GINT_TO_POINTER(r));
            g_object_set_data(G_OBJECT(event_box), "col", GINT_TO_POINTER(c));
            g_signal_connect(event_box, "button-press-event", G_CALLBACK(on_square_clicked), grid);

            gtk_style_context_add_class(ctx, ((r + c) % 2 == 0) ? "white-cell" : "black-cell");

            gtk_grid_attach(GTK_GRID(grid), event_box, c, r, 1, 1);
        }
    }
    gtk_stack_add_named(GTK_STACK(stack), game_box, "game");

    // Ekran końcowy
    end_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *win_label = gtk_label_new("Gracz X wygrywa!");
    return_button = gtk_button_new_with_label("Powrót do menu");
    g_signal_connect(return_button, "clicked", G_CALLBACK(return_to_menu), NULL);
    gtk_box_pack_start(GTK_BOX(end_box), win_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(end_box), return_button, TRUE, TRUE, 0);
    gtk_stack_add_named(GTK_STACK(stack), end_box, "end");

    // Ustawienie menu jako domyślny widok
    gtk_stack_set_visible_child_name(GTK_STACK(stack), "menu");

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}