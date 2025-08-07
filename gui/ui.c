#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"

static GtkWidget *min_cps_slider, *max_cps_slider, *trigger_entry, *event_entry;
static GtkWidget *status_label;

static void set_status(const char *status, const char *color) {
    char markup[128];
    snprintf(markup, sizeof(markup), "<span foreground=\"%s\">●</span> %s", color, status);
    gtk_label_set_markup(GTK_LABEL(status_label), markup);
}

static void on_exit_cleanup(void) {
    system("sudo pkill -f autoclicker_backend");
}

static void on_save_clicked(GtkButton *button, gpointer user_data) {
    int min_cps = (int)gtk_range_get_value(GTK_RANGE(min_cps_slider));
    int max_cps = (int)gtk_range_get_value(GTK_RANGE(max_cps_slider));
    if (min_cps > max_cps) {
        printf("[ERROR] min_cps > max_cps\n");
        return;
    }

    MIN_DELAY_US = 1000000 / max_cps;
    MAX_DELAY_US = 1000000 / min_cps;

    TRIGGER_CODE = atoi(gtk_entry_get_text(GTK_ENTRY(trigger_entry)));
    strncpy(MOUSE_EVENT_PATH, gtk_entry_get_text(GTK_ENTRY(event_entry)), sizeof(MOUSE_EVENT_PATH));

    system("sudo pkill -f autoclicker_backend");
    save_config();

    set_status("Autoclicker Stopped", "red");
}

static void on_start_clicked(GtkButton *button, gpointer user_data) {
    on_save_clicked(NULL, NULL);

    system("sudo pkill -f autoclicker_backend");
    system("sudo HOME=$HOME ./autoclicker_backend &");

    set_status("Autoclicker Running", "green");
}

static void on_stop_clicked(GtkButton *button, gpointer user_data) {
    system("sudo pkill -f autoclicker_backend");
    set_status("Autoclicker Stopped", "red");
}

void launch_ui(void) {
    atexit(on_exit_cleanup);
    load_config();

    GtkWidget *window, *grid, *save_btn, *start_btn, *stop_btn;
    gtk_init(NULL, NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "uClicker");
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *header_bar = gtk_header_bar_new();
    gtk_header_bar_set_title(GTK_HEADER_BAR(header_bar), "uClicker");
    gtk_header_bar_set_show_close_button(GTK_HEADER_BAR(header_bar), TRUE);
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(
        provider,
        "* { background-color: #ffffff; color: black; font-size: 12pt; }"
        "scale slider { background-color: #007BFF; border-radius: 4px; }"
        "scale trough { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 4px; min-height: 8px; }"
        "button { margin: 4px; }"
        "label, entry, scale { margin: 2px; }"
        "headerbar { background-color: #f7f7f7; border-bottom: 1px solid #ddd; }"
        "headerbar title { color: #333333; font-weight: bold; }",
        -1, NULL
    );
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
                                              GTK_STYLE_PROVIDER(provider),
                                              GTK_STYLE_PROVIDER_PRIORITY_USER
    );

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 4);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 4);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 6);
    gtk_container_add(GTK_CONTAINER(window), grid);

    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file_at_scale("assets/logo.png", 120, 40, TRUE, NULL);
    GtkWidget *logo = gtk_image_new_from_pixbuf(pixbuf);
    gtk_grid_attach(GTK_GRID(grid), logo, 0, 0, 3, 1);

    GtkAdjustment *min_adj = gtk_adjustment_new(1, 1, 40, 1, 5, 0);
    GtkAdjustment *max_adj = gtk_adjustment_new(1, 1, 40, 1, 5, 0);
    min_cps_slider = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, min_adj);
    max_cps_slider = gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, max_adj);

    gtk_scale_set_draw_value(GTK_SCALE(min_cps_slider), TRUE);
    gtk_scale_set_draw_value(GTK_SCALE(max_cps_slider), TRUE);
    gtk_scale_set_value_pos(GTK_SCALE(min_cps_slider), GTK_POS_RIGHT);
    gtk_scale_set_value_pos(GTK_SCALE(max_cps_slider), GTK_POS_RIGHT);
    gtk_scale_set_digits(GTK_SCALE(min_cps_slider), 0);
    gtk_scale_set_digits(GTK_SCALE(max_cps_slider), 0);

    trigger_entry = gtk_entry_new();
    event_entry   = gtk_entry_new();

    gtk_range_set_value(GTK_RANGE(min_cps_slider), 1000000 / MAX_DELAY_US);
    gtk_range_set_value(GTK_RANGE(max_cps_slider), 1000000 / MIN_DELAY_US);
    char trigger_str[16]; snprintf(trigger_str, sizeof(trigger_str), "%d", TRIGGER_CODE);
    gtk_entry_set_text(GTK_ENTRY(trigger_entry), trigger_str);
    gtk_entry_set_text(GTK_ENTRY(event_entry), MOUSE_EVENT_PATH);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Min CPS:"),       0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), min_cps_slider,                  1, 1, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Max CPS:"),       0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), max_cps_slider,                  1, 2, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Trigger Code:"),  0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), trigger_entry,                   1, 3, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Mouse Event:"),   0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), event_entry,                     1, 4, 2, 1);

    save_btn  = gtk_button_new_with_label("Save Config");
    start_btn = gtk_button_new_with_label("Start");
    stop_btn  = gtk_button_new_with_label("Stop");
    gtk_grid_attach(GTK_GRID(grid), save_btn,  0, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), start_btn, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stop_btn,  2, 5, 1, 1);

    status_label = gtk_label_new(NULL);
    gtk_label_set_xalign(GTK_LABEL(status_label), 0.0);
    set_status("Autoclicker Stopped", "red"); // Initialer Status
    gtk_grid_attach(GTK_GRID(grid), status_label, 0, 6, 3, 1);

    GtkWidget *footer = gtk_label_new(NULL);
    gtk_label_set_markup(
        GTK_LABEL(footer),
                         "Made by <a href=\"https://www.youtube.com/@mahiru404l\">"
                         "<span foreground=\"#007BFF\"><b>Mahiru</b></span></a>"
    );
    gtk_label_set_xalign(GTK_LABEL(footer), 0.5);
    gtk_label_set_selectable(GTK_LABEL(footer), TRUE);
    gtk_grid_attach(GTK_GRID(grid), footer, 0, 7, 3, 1);

    g_signal_connect(save_btn,  "clicked", G_CALLBACK(on_save_clicked),  NULL);
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), NULL);
    g_signal_connect(stop_btn,  "clicked", G_CALLBACK(on_stop_clicked),  NULL);

    gtk_widget_show_all(window);
    gtk_window_resize(GTK_WINDOW(window), 320, 270);

    gtk_main();
}

#ifdef UCLICKER_STANDALONE_UI
int main(void) {
    launch_ui();
    return 0;
}
#endif
