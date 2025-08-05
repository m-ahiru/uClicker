#include <fcntl.h>
#include <linux/uinput.h>
#include <linux/input.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MOUSE_EVENT_PATH "/dev/input/event7"
#define TRIGGER_CODE 275 // BTN_SIDE

volatile int clicking = 0;

void emit_click(int fd) {
    struct input_event ev = {0};

    // Press
    ev.type = EV_KEY; ev.code = BTN_LEFT; ev.value = 1;
    write(fd, &ev, sizeof(ev));
    ev.type = EV_SYN; ev.code = SYN_REPORT; ev.value = 0;
    write(fd, &ev, sizeof(ev));

    usleep(10000); // gedrückt halten

    // Release
    ev.type = EV_KEY; ev.code = BTN_LEFT; ev.value = 0;
    write(fd, &ev, sizeof(ev));
    ev.type = EV_SYN; ev.code = SYN_REPORT; ev.value = 0;
    write(fd, &ev, sizeof(ev));
}

int setup_uinput() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("uinput open");
        exit(1);
    }

    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);

    struct uinput_user_dev uidev = {0};
    snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, "autoclicker");
    uidev.id.bustype = BUS_USB;
    uidev.id.vendor  = 0x1234;
    uidev.id.product = 0x5678;
    uidev.id.version = 1;

    write(fd, &uidev, sizeof(uidev));
    ioctl(fd, UI_DEV_CREATE);
    return fd;
}

void* click_thread(void* arg) {
    int uinput_fd = *(int*)arg;
    srand(time(NULL)); // Initialisiere Zufall

    while (1) {
        if (clicking) {
            emit_click(uinput_fd);

            // Zufallswartezeit: 52.6ms – 62.5ms → 52600 – 62500 µs
            int delay = (rand() % (62500 - 52600 + 1)) + 52600;
            usleep(delay);
        } else {
            usleep(10000);
        }
    }
    return NULL;
}

int main() {
    int uinput_fd = setup_uinput();
    int mouse_fd = open(MOUSE_EVENT_PATH, O_RDONLY);
    if (mouse_fd < 0) {
        perror("open mouse device");
        return 1;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, click_thread, &uinput_fd);

    struct input_event ev;
    printf("AC running...\n");

    while (1) {
        if (read(mouse_fd, &ev, sizeof(ev)) == sizeof(ev)) {
            if (ev.type == EV_KEY && ev.code == TRIGGER_CODE) {
                clicking = ev.value;
                if (clicking)
                    printf("⏺️  Start\n");
                else
                    printf("⏹️  Stop\n");
            }
        }
    }

    return 0;
}

