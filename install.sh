#!/usr/bin/env bash
#
# NekoCat installer for macOS
# Builds and installs the "kitty" desktop pet.
#
# Usage:
#   curl -fsSL https://raw.githubusercontent.com/<you>/<repo>/main/install.sh | bash
# or, after cloning the repo:
#   ./install.sh

set -euo pipefail

REPO_URL="https://github.com/Ibra2k/NekoMouse.git"   
APP_NAME="kitty"
INSTALL_DIR="$HOME/.local/share/nekocat"
BIN_DIR="/usr/local/bin"

info()  { printf "\033[1;34m==>\033[0m %s\n" "$1"; }
warn()  { printf "\033[1;33m==>\033[0m %s\n" "$1"; }
error() { printf "\033[1;31m==>\033[0m %s\n" "$1" >&2; }

# ---------------------------------------------------------------------------
# 1. Check we're on macOS
# ---------------------------------------------------------------------------
if [[ "$(uname)" != "Darwin" ]]; then
    error "This script only supports macOS. Detected: $(uname)"
    exit 1
fi

# ---------------------------------------------------------------------------
# 2. Xcode Command Line Tools (provides clang/clang++)
# ---------------------------------------------------------------------------
if ! xcode-select -p &>/dev/null; then
    info "Xcode Command Line Tools not found. Installing (a system dialog will pop up)..."
    xcode-select --install
    echo
    warn "Please complete the Command Line Tools install in the popup window,"
    warn "then re-run this script."
    exit 1
else
    info "Xcode Command Line Tools already installed."
fi

# ---------------------------------------------------------------------------
# 3. Homebrew
# ---------------------------------------------------------------------------
if ! command -v brew &>/dev/null; then
    info "Homebrew not found. Installing Homebrew..."
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

    # Apple Silicon vs Intel default prefixes
    if [[ -d /opt/homebrew/bin ]]; then
        eval "$(/opt/homebrew/bin/brew shellenv)"
    elif [[ -d /usr/local/bin ]]; then
        eval "$(/usr/local/bin/brew shellenv)"
    fi
else
    info "Homebrew already installed."
fi

# ---------------------------------------------------------------------------
# 4. Dependencies: cmake, raylib, asio, git
# ---------------------------------------------------------------------------
info "Installing build dependencies (cmake, raylib, asio, git)..."
brew install cmake raylib asio git

# ---------------------------------------------------------------------------
# 5. Clone or update the repo
# ---------------------------------------------------------------------------
if [[ -d "$INSTALL_DIR/.git" ]]; then
    info "Existing install found, updating..."
    git -C "$INSTALL_DIR" pull --ff-only
else
    info "Cloning NekoCat into $INSTALL_DIR..."
    mkdir -p "$(dirname "$INSTALL_DIR")"
    git clone "$REPO_URL" "$INSTALL_DIR"
fi

# ---------------------------------------------------------------------------
# 6. Configure and build
# ---------------------------------------------------------------------------
BUILD_DIR="$INSTALL_DIR/build"
info "Configuring build..."
cmake -S "$INSTALL_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release

info "Building (this can take a couple of minutes)..."
cmake --build "$BUILD_DIR" --config Release -j"$(sysctl -n hw.ncpu)"

# ---------------------------------------------------------------------------
# 7. Build a proper .app bundle (so double-clicking doesn't open Terminal,
#    and the app gets a real icon)
# ---------------------------------------------------------------------------
BUILT_BIN="$BUILD_DIR/$APP_NAME"
if [[ ! -f "$BUILT_BIN" ]]; then
    error "Build finished but $APP_NAME binary was not found at $BUILT_BIN."
    exit 1
fi

# Build the .icns from committed PNGs if the repo doesn't already ship
# a pre-built one. (Requires iconutil, which only exists on macOS —
# fine here since this whole script only runs on macOS.)
if [[ ! -f "$INSTALL_DIR/NekoCat.icns" && -d "$INSTALL_DIR/icon_assets" ]]; then
    info "Building NekoCat.icns from icon assets..."
    ICONSET_TMP="$BUILD_DIR/NekoCat.iconset"
    rm -rf "$ICONSET_TMP"
    mkdir -p "$ICONSET_TMP"
    cp "$INSTALL_DIR/icon_assets/icon_16.png"   "$ICONSET_TMP/icon_16x16.png"   2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_32.png"   "$ICONSET_TMP/icon_16x16@2x.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_32.png"   "$ICONSET_TMP/icon_32x32.png"   2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_64.png"   "$ICONSET_TMP/icon_32x32@2x.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_128.png"  "$ICONSET_TMP/icon_128x128.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_256.png"  "$ICONSET_TMP/icon_128x128@2x.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_256.png"  "$ICONSET_TMP/icon_256x256.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_512.png"  "$ICONSET_TMP/icon_256x256@2x.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_512.png"  "$ICONSET_TMP/icon_512x512.png" 2>/dev/null || true
    cp "$INSTALL_DIR/icon_assets/icon_1024.png" "$ICONSET_TMP/icon_512x512@2x.png" 2>/dev/null || true
    iconutil -c icns "$ICONSET_TMP" -o "$INSTALL_DIR/NekoCat.icns" 2>/dev/null || \
        warn "Could not build .icns — app will use the default icon."
fi

BUNDLE_NAME="NekoCat.app"
BUNDLE_DIR="$BUILD_DIR/$BUNDLE_NAME"
MACOS_DIR="$BUNDLE_DIR/Contents/MacOS"
RESOURCES_DIR="$BUNDLE_DIR/Contents/Resources"

info "Assembling $BUNDLE_NAME..."
rm -rf "$BUNDLE_DIR"
mkdir -p "$MACOS_DIR" "$RESOURCES_DIR"

cp "$BUILT_BIN" "$MACOS_DIR/$APP_NAME"
chmod +x "$MACOS_DIR/$APP_NAME"

if [[ -f "$INSTALL_DIR/Info.plist" ]]; then
    cp "$INSTALL_DIR/Info.plist" "$BUNDLE_DIR/Contents/Info.plist"
else
    warn "Info.plist not found in repo root — app may not launch correctly without it."
fi

if [[ -f "$INSTALL_DIR/NekoCat.icns" ]]; then
    cp "$INSTALL_DIR/NekoCat.icns" "$RESOURCES_DIR/NekoCat.icns"
else
    warn "NekoCat.icns not found — app will use the default generic icon."
fi

# Any assets your app loads at runtime (sprites, fonts, saved-color file, etc.)
# go alongside the executable so the bundle is self-contained.
if [[ -d "$INSTALL_DIR/assets" ]]; then
    # Copied next to the executable (Contents/MacOS), not Contents/Resources,
    # because the game code resolves asset paths via GetApplicationDirectory(),
    # which returns the folder the executable itself lives in.
    cp -R "$INSTALL_DIR/assets" "$MACOS_DIR/assets"
fi

# ---------------------------------------------------------------------------
# 8. Install the .app into /Applications
# ---------------------------------------------------------------------------
info "Installing NekoCat to /Applications (you may be asked for your password)..."
sudo rm -rf "/Applications/$BUNDLE_NAME"
sudo cp -R "$BUNDLE_DIR" "/Applications/$BUNDLE_NAME"

# Bundle isn't code-signed, so Gatekeeper would otherwise flag it as
# "unverified developer." Since it was just built locally from source
# rather than downloaded, it's safe to clear the quarantine flag ourselves.
sudo xattr -dr com.apple.quarantine "/Applications/$BUNDLE_NAME" 2>/dev/null || true

echo
info "Installed! Open NekoCat from Applications or Spotlight — no Terminal window will appear."
info "(If macOS still warns about an unidentified developer, right-click the app -> Open, once.)"
