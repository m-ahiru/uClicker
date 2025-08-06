#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int MIN_DELAY_US = 83333;
int MAX_DELAY_US = 55555;
int TRIGGER_CODE = 275;
char MOUSE_EVENT_PATH[256] = "/dev/input/event7";

const char *get_config_path() {
    static char path[512];
    const char *home = getenv("HOME");
    if (!home) home = ".";
    snprintf(path, sizeof(path), "%s/.config/uClicker/uClicker.conf", home);
    return path;
}

void ensure_config_dir_exists() {
    const char *home = getenv("HOME");
    if (!home) return;
    char dir[512];
    snprintf(dir, sizeof(dir), "%s/.config/uClicker", home);
}

void save_config() {
    ensure_config_dir_exists();
    const char *path = get_config_path();
    printf("[DEBUG] Writing config to: %s\n", path);
    FILE *file = fopen(path, "w");
    if (!file) {
        perror("[ERROR] fopen");
        return;
    }
    fprintf(file, "min_delay_us=%d\n", MIN_DELAY_US);
    fprintf(file, "max_delay_us=%d\n", MAX_DELAY_US);
    fprintf(file, "trigger_code=%d\n", TRIGGER_CODE);
    fprintf(file, "mouse_event_path=%s\n", MOUSE_EVENT_PATH);
    fclose(file);
    printf("[DEBUG] Config saved successfully\n");
}

void load_config() {
    const char *path = get_config_path();
    FILE *file = fopen(path, "r");
    if (!file) {
        printf("[INFO] No config file found at %s\n", path);
	system("mkdir ~/.config/uClicker");
	system("touch ~/.config/uClicker/uClicker.conf");
        return;
    }
    fscanf(file, "min_delay_us=%d\n", &MIN_DELAY_US);
    fscanf(file, "max_delay_us=%d\n", &MAX_DELAY_US);
    fscanf(file, "trigger_code=%d\n", &TRIGGER_CODE);
    fscanf(file, "mouse_event_path=%s\n", MOUSE_EVENT_PATH);
    fclose(file);
    printf("[DEBUG] Config loaded from: %s\n", path);
}
