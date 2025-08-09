#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

int MIN_DELAY_US = 47619;         
int MAX_DELAY_US = 66666;        
int TRIGGER_CODE = 275;               
char MOUSE_EVENT_PATH[256] = "/dev/input/event7";

static const char *user_home(void) {
    const char *home = getenv("HOME");
    return (home && *home) ? home : ".";
}

const char *get_config_path(void) {
    static char path[512];
    snprintf(path, sizeof(path), "%s/.config/uClicker/uClicker.conf", user_home());
    return path;
}

void ensure_config_dir_exists(void) {
    char base[512], dir[512];
    snprintf(base, sizeof(base), "%s/.config", user_home());
    snprintf(dir,  sizeof(dir),  "%s/.config/uClicker", user_home());

    if (mkdir(base, 0700) != 0 && errno != EEXIST) {
        perror("[ERROR] mkdir ~/.config");
    }
    errno = 0;
    if (mkdir(dir, 0700) != 0 && errno != EEXIST) {
        perror("[ERROR] mkdir ~/.config/uClicker");
    }
}

static void write_defaults(const char *path) {
    FILE *f = fopen(path, "w");
    if (!f) { perror("[ERROR] fopen(defaults)"); return; }

    fprintf(f, "min_delay_us=%d\n", MIN_DELAY_US);
    fprintf(f, "max_delay_us=%d\n", MAX_DELAY_US);
    fprintf(f, "trigger_code=%d\n", TRIGGER_CODE);
    fprintf(f, "mouse_event_path=%s\n", MOUSE_EVENT_PATH);

    fflush(f);
    int fd = fileno(f);
    if (fd != -1) fsync(fd);
    if (fclose(f) != 0) perror("[ERROR] fclose(defaults)");

    chmod(path, 0600);
    printf("[DEBUG] Default config written to: %s\n", path);
}

void save_config(void) {
    ensure_config_dir_exists();
    const char *path = get_config_path();
    printf("[DEBUG] Writing config to: %s\n", path);

    FILE *file = fopen(path, "w");
    if (!file) { perror("[ERROR] fopen(save)"); return; }

    fprintf(file, "min_delay_us=%d\n", MIN_DELAY_US);
    fprintf(file, "max_delay_us=%d\n", MAX_DELAY_US);
    fprintf(file, "trigger_code=%d\n", TRIGGER_CODE);
    fprintf(file, "mouse_event_path=%s\n", MOUSE_EVENT_PATH);

    fflush(file);
    int fd = fileno(file);
    if (fd != -1) fsync(fd);
    if (fclose(file) != 0) { perror("[ERROR] fclose(save)"); return; }

    chmod(path, 0600);
    printf("[DEBUG] Config saved successfully\n");
}

void load_config(void) {
    ensure_config_dir_exists();
    const char *path = get_config_path();

    FILE *file = fopen(path, "r");
    if (!file) {
        printf("[INFO] No config file found at %s — creating defaults...\n", path);
        write_defaults(path);
        return;
    }

    if (fseek(file, 0, SEEK_END) == 0) {
        long size = ftell(file);
        if (size <= 0) {
            fclose(file);
            printf("[WARN] Config file empty — writing defaults...\n");
            write_defaults(path);
            return;
        }
        rewind(file);
    } else {
        fclose(file);
        printf("[WARN] Cannot seek config — writing defaults...\n");
        write_defaults(path);
        return;
    }

    if (fscanf(file, "min_delay_us=%d\n", &MIN_DELAY_US) != 1) {/* keep default */}
    if (fscanf(file, "max_delay_us=%d\n", &MAX_DELAY_US) != 1) {/* keep default */}
    if (fscanf(file, "trigger_code=%d\n", &TRIGGER_CODE) != 1) {/* keep default */}
    if (fscanf(file, "mouse_event_path=%255s\n", MOUSE_EVENT_PATH) != 1) {
        strncpy(MOUSE_EVENT_PATH, "/dev/input/event7", sizeof(MOUSE_EVENT_PATH));
        MOUSE_EVENT_PATH[sizeof(MOUSE_EVENT_PATH)-1] = '\0';
    }

    fclose(file);
    printf("[DEBUG] Config loaded from: %s\n", path);
}

