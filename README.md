# uClicker

**uClicker** is a fast, undetectable, and minimal universal Linux autoclicker with a clean GTK-based GUI.  
It interacts directly with `/dev/input/` for raw device access, making it fully **Wayland-compatible**, and perfect for use cases like **Minecraft PvP** or **automation**.

![Screenshot](assets/ss.png)

---

## ⚙️ Features

- 🖱️ **Raw input** device access via `/dev/input/eventX`
- 🔑 Customizable **trigger key**
- ⚡ Adjustable **CPS range**
- 🧠 Undetectable by most anti-cheats
- 🧩 Works under **Wayland** and **X11**
- 💾 Config is saved to `~/.config/uClicker/uClicker.conf`

---

## 📦 Dependencies

You’ll need:

- `gtk3` (for the GUI)
- `gcc` & `make` (for building)
- `pkg-config` (for detecting GTK and other libs during build)
- `evtest` (optional, for identifying input devices)
- `sudo` **or** udev rules that give your user access to `/dev/input/*` and `/dev/uinput` (recommended for running without root)
- *(Optional)* `libevdev` development package if you implement automatic device discovery later

Note: On minimal installations you may also need a complete build toolchain package such as `base-devel` (Arch) or `build-essential` (Debian/Ubuntu).

### Install on Arch Linux
sudo pacman -S --needed base-devel pkgconf gtk3 evtest

### Install on Debian/Ubuntu/Mint
sudo apt update
sudo apt install build-essential pkg-config libgtk-3-dev evtest

### Install on Fedora
sudo dnf install @development-tools pkgconf-pkg-config gtk3-devel evtest

---

## 🚫 Run Without sudo (recommended)

By default, only root can read `/dev/input/event*` and write to `/dev/uinput`.  
To run uClicker as a normal user:

1. Add your user to the `input` group  
   sudo usermod -aG input $USER  
   Log out and back in afterwards.

2. Create udev rules  
   sudo sh -c 'cat >/etc/udev/rules.d/99-uinput.rules << "EOF"
KERNEL=="uinput", MODE="0660", GROUP="input"
KERNEL=="event*", SUBSYSTEM=="input", MODE="0640", GROUP="input"
EOF'  
   sudo udevadm control --reload  
   sudo udevadm trigger

3. Load `uinput` (and persist across reboots)  
   sudo modprobe uinput  
   echo uinput | sudo tee /etc/modules-load.d/uinput.conf >/dev/null

4. Verify devices  
   ls -l /dev/uinput /dev/input/event*  
   # Group should be 'input', permissions should allow group rw for uinput and r for event*

Now you can run the backend without sudo.

---

## 🚀 Installation

1) Clone the repo  
git clone https://github.com/m-ahiru/uClicker.git  
cd uClicker

2) Build the app  
make  
This compiles both the backend (`autoclicker_backend`) and the GUI frontend (`uClicker`).  
If you see warnings here, continue with the next steps.

---

## ▶️ Usage

Step 1: Find your mouse event path  
sudo evtest  
Select your mouse and note the event path (e.g. `/dev/input/event7`).  
(If you set up udev as above, you can also run `evtest` without sudo depending on your distro’s policy.)

Step 2: Start the GUI  
./uClicker

The GUI lets you:  
- Set your CPS range (Min/Max)  
- Enter your trigger key code  
- Set the mouse event path  
- Save your config  
- Start or stop the backend clicker  

When you press your chosen trigger key (e.g. `F6`), the backend will start clicking in your desired CPS range.

---

## 🔧 Backend Details

- Backend binary: `autoclicker_backend`
- Accesses `/dev/input/event*` and `/dev/uinput`
- If you did the udev setup, you can run it without sudo. Otherwise, you’ll need sudo for the backend.

The GUI will:  
- Save your settings  
- Stop any running backend instance  
- Launch a fresh backend with updated config

---

## 💾 Config File

Path:  
~/.config/uClicker/uClicker.conf

Example:  
min_delay_us=55555  
max_delay_us=62500  
trigger_code=275  
mouse_event_path=/dev/input/event7

---

## 🛑 Stop Everything

Click “Stop” in the UI to kill the backend.  
Exiting the GUI also automatically stops it.

---

## 🧠 Important Notes

- The app currently expects to be run from the project root (`./uClicker`) so the assets load correctly.
- With the udev setup above, you avoid password prompts and can run entirely as a normal user.
- Wayland/X11 doesn’t matter for click injection since uClicker uses kernel-level input via uinput.

---

## ❗ Disclaimer

This tool is provided for educational and personal use only.  
Using autoclickers in online games may violate terms of service.  
Use responsibly and at your own risk.
