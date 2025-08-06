#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "config.h"

static GtkWidget *min_cps_spin, *max_cps_spin, *trigger_entry, *event_entry;

void on_exit_cleanup() {
	system("sudo pkill -f autoclicker_backend");
}

void on_save_clicked(GtkButton *button, gpointer user_data) {
    printf("[DEBUG] Save button clicked\n");
    int min_cps = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(min_cps_spin));
    int max_cps = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(max_cps_spin));
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
}

void on_start_clicked(GtkButton *button, gpointer user_data) {
    on_save_clicked(NULL, NULL);
    system("sudo pkill -f autoclicker_backend");
    system("sudo HOME=$HOME ./autoclicker_backend &");
}

void on_stop_clicked(GtkButton *button, gpointer user_data) {
    system("sudo pkill -f autoclicker_backend");
}

void launch_ui() {
    atexit(on_exit_cleanup);
    load_config();

    GtkWidget *window, *grid, *save_btn, *start_btn, *stop_btn;
    gtk_init(NULL, NULL);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "uClicker");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    gtk_window_set_type_hint(GTK_WINDOW(window), GDK_WINDOW_TYPE_HINT_DIALOG);
    gtk_window_set_keep_above(GTK_WINDOW(window), TRUE);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    min_cps_spin = gtk_spin_button_new_with_range(1, 1000, 1);
    max_cps_spin = gtk_spin_button_new_with_range(1, 1000, 1);
    trigger_entry = gtk_entry_new();
    event_entry = gtk_entry_new();

    gtk_spin_button_set_value(GTK_SPIN_BUTTON(min_cps_spin), 1000000 / MAX_DELAY_US);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(max_cps_spin), 1000000 / MIN_DELAY_US);

    char trigger_str[10]; sprintf(trigger_str, "%d", TRIGGER_CODE);
    gtk_entry_set_text(GTK_ENTRY(trigger_entry), trigger_str);
    gtk_entry_set_text(GTK_ENTRY(event_entry), MOUSE_EVENT_PATH);

    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Min CPS:"), 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), min_cps_spin, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Max CPS:"), 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), max_cps_spin, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Trigger Code:"), 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), trigger_entry, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), gtk_label_new("Mouse Event:"), 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), event_entry, 1, 3, 1, 1);

    save_btn = gtk_button_new_with_label("Save Config");
    start_btn = gtk_button_new_with_label("Start");
    stop_btn = gtk_button_new_with_label("Stop");

    gtk_grid_attach(GTK_GRID(grid), save_btn, 0, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), start_btn, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), stop_btn, 2, 4, 1, 1);

    g_signal_connect(save_btn, "clicked", G_CALLBACK(on_save_clicked), NULL);
    g_signal_connect(start_btn, "clicked", G_CALLBACK(on_start_clicked), NULL);
    g_signal_connect(stop_btn, "clicked", G_CALLBACK(on_stop_clicked), NULL);

    gtk_widget_show_all(window);
    gtk_main();
}
