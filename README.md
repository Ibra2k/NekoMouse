# Neko Cat

## Controls

- CTRL + Q = Kill the cat process
- CTRL + C = Open color palette to change cat color
- CTRL + S = Save cat color
- CTRL + ↑/↓ = Increase and Decrease speed respectively
- LEFT ALT + S = Sit/Stand cat
- CTRL + O = Connect Online

## macOS Setup

First, run the setup script to install dependencies:

```bash
cd
git clone https://github.com/Ibra2k/Neko-Mouse.git
cd Neko-Mouse

# -- Run for first time users --
chmod +x setup_kitty_macos.sh
./setup_kitty_macos.sh
# -- Run for first time users --

mkdir build && cd build
cmake ..
make
./kitty
```

## Linux Setup

Install dependencies:
```bash
sudo apt install git cmake libraylib-dev libx11-dev
```

Then build as above.
