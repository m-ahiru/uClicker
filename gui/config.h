#ifndef CONFIG_H
#define CONFIG_H

extern int MIN_DELAY_US;
extern int MAX_DELAY_US;
extern int TRIGGER_CODE;
extern char MOUSE_EVENT_PATH[256];

const char *get_config_path(void);
void load_config();
void save_config();
void ensure_config_dir_exists();

#endif
