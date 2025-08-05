# uClicker

**uClicker** is a lightweight, fast, and undetectable universal autoclicker for Linux.  
It works directly with raw input events, making it fully **Wayland-compatible** and independent of X11 – ideal for automation or PvP Minecraft.

---

## ⚙️ Features

- 🖱️ Raw `/dev/input/` device access  
- 🎯 Customizable hotkey  
- ⚡ Adjustable click speed (CPS)  
- 🧠 Undetectable by most anti-cheats  
- 🪟 Works under Wayland, X11, or TTY  

---

## 🛠️ Dependencies

Install `evtest` to identify your input devices:

```bash
sudo pacman -S evtest
```

---

## 🚀 Installation

### 1. Clone the repo

```bash
git clone https://github.com/m-ahiru/uClicker.git
cd uClicker
```

### 2. Identify your mouse input path

Run:

```bash
sudo evtest
```

Select your mouse from the list and note the path (e.g. `/dev/input/event7`).

---

## ⚙️ Configuration

### 🔧 Set mouse event path

Edit the source:

```bash
vim autoclicker.c
```

Replace this line:

```c
#define MOUSE_EVENT_PATH "/dev/input/event7"
```

…with your actual event path.

---

### 🔘 Set trigger hotkey

In the same file, update this line:

```c
#define TRIGGER_CODE 275  // BTN_SIDE (Mouse Button 4)
```

To find the correct trigger code:
- Run `sudo evtest`
- Press your desired mouse button
- Copy the code number from the output

---

### ⚡ Set click speed (CPS)

This line controls the click delay (in microseconds):

```c
int delay = (rand() % (62500 - 52600 + 1)) + 52600;
```

This gives a randomized delay between **52.6ms and 62.5ms**, or roughly **16–19 CPS**.  
Adjust the range to make it faster/slower.

---

## 🛠️ Compile

Use `gcc`:

```bash
gcc autoclicker.c -o autoclicker -lpthread
```

---

## ▶️ Run

```bash
sudo ./autoclicker
```

> Root is required to read from `/dev/input/`.

---

## ❗ Disclaimer

This tool is provided for educational and personal use only.  
Using autoclickers in online games may violate their terms of service.  
Use responsibly and at your own risk.
