#!/usr/bin/env bash
set -euo pipefail

# ─── 1. CONFIGURATION ─────────────────────────────────────────────────────────
APP_NAME="Conquest"

# Your repo root
PROJECT_DIR="$HOME/projects/rogueLike"

# SDL2 core
SDL2_VERSION="2.32.6"
SDL2_ARCHIVE="SDL2-devel-${SDL2_VERSION}-mingw.tar.gz"
SDL2_URL="https://libsdl.org/release/${SDL2_ARCHIVE}"

# SDL2_ttf
SDL2_TTF_VERSION="2.20.2"
SDL2_TTF_ARCHIVE="SDL2_ttf-devel-${SDL2_TTF_VERSION}-mingw.tar.gz"
SDL2_TTF_URL="https://libsdl.org/projects/SDL_ttf/release/${SDL2_TTF_ARCHIVE}"

# SDL2_image (for PNG, JPG, etc)
SDL2_IMAGE_VERSION="2.6.2"
SDL2_IMAGE_ARCHIVE="SDL2_image-devel-${SDL2_IMAGE_VERSION}-mingw.tar.gz"
SDL2_IMAGE_URL="https://libsdl.org/projects/SDL_image/release/${SDL2_IMAGE_ARCHIVE}"

# Conquest install tree on WSL
APP_ROOT="$HOME/${APP_NAME}"
DOWNLOAD_DIR="$APP_ROOT/downloads"
SDK_DIR="$APP_ROOT/sdk"
SRC_DIR="$APP_ROOT/src"
BIN_DIR="$APP_ROOT/bin"
RES_DIR="$APP_ROOT/resources"

# Windows target
WIN_USER="$(powershell.exe -NoProfile -Command '$Env:USERNAME' | tr -d '\r\n')"
WIN_APP_DIR="/mnt/c/Users/$WIN_USER/${APP_NAME}"

# ─── 2. MAKE DIRECTORIES ────────────────────────────────────────────────────────
mkdir -p \
  "$DOWNLOAD_DIR" \
  "$SDK_DIR" \
  "$SRC_DIR" \
  "$BIN_DIR" \
  "$RES_DIR" \
  "$WIN_APP_DIR"

# ─── 3. SYNC GAME SOURCE ────────────────────────────────────────────────────────
rsync -av --delete "$PROJECT_DIR/src/" "$SRC_DIR/"

# ─── 4. SDL2 DOWNLOAD & EXTRACT ────────────────────────────────────────────────
if [ ! -f "$DOWNLOAD_DIR/$SDL2_ARCHIVE" ]; then
  echo "Downloading SDL2 ${SDL2_VERSION}…"
  wget -q --show-progress -P "$DOWNLOAD_DIR" "$SDL2_URL"
fi
if [ ! -d "$SDK_DIR/SDL2-${SDL2_VERSION}/x86_64-w64-mingw32" ]; then
  echo "Extracting SDL2…"
  tar -xf "$DOWNLOAD_DIR/$SDL2_ARCHIVE" -C "$SDK_DIR"
fi

# ─── 5. SDL2_ttf DOWNLOAD & EXTRACT ────────────────────────────────────────────
if [ ! -f "$DOWNLOAD_DIR/$SDL2_TTF_ARCHIVE" ]; then
  echo "Downloading SDL2_ttf ${SDL2_TTF_VERSION}…"
  wget -q --show-progress -P "$DOWNLOAD_DIR" "$SDL2_TTF_URL"
fi
if [ ! -d "$SDK_DIR/SDL2_ttf-${SDL2_TTF_VERSION}/x86_64-w64-mingw32" ]; then
  echo "Extracting SDL2_ttf…"
  tar -xf "$DOWNLOAD_DIR/$SDL2_TTF_ARCHIVE" -C "$SDK_DIR"
fi

# ─── 6. SDL2_image DOWNLOAD & EXTRACT ─────────────────────────────────────────
if [ ! -f "$DOWNLOAD_DIR/$SDL2_IMAGE_ARCHIVE" ]; then
  echo "Downloading SDL2_image ${SDL2_IMAGE_VERSION}…"
  wget -q --show-progress -P "$DOWNLOAD_DIR" "$SDL2_IMAGE_URL"
fi
if [ ! -d "$SDK_DIR/SDL2_image-${SDL2_IMAGE_VERSION}/x86_64-w64-mingw32" ]; then
  echo "Extracting SDL2_image…"
  tar -xf "$DOWNLOAD_DIR/$SDL2_IMAGE_ARCHIVE" -C "$SDK_DIR"
fi

# ─── 7. EXPORT SDK PATHS ───────────────────────────────────────────────────────
export SDL2_WIN="$SDK_DIR/SDL2-${SDL2_VERSION}/x86_64-w64-mingw32"
export SDL2_TTF_WIN="$SDK_DIR/SDL2_ttf-${SDL2_TTF_VERSION}/x86_64-w64-mingw32"
export SDL2_IMG_WIN="$SDK_DIR/SDL2_image-${SDL2_IMAGE_VERSION}/x86_64-w64-mingw32"

# ─── 8. BUILD WINDOWS EXECUTABLE ───────────────────────────────────────────────
readarray -d '' src_files < <(
  find "$SRC_DIR" -type f -name '*.c' -print0
)

gcc_flags=(
  "${src_files[@]}"
  -o "$BIN_DIR/${APP_NAME}.exe"
  # includes
  -I"$SDL2_WIN/include"
  -I"$SDL2_WIN/include/SDL2"
  -I"$SDL2_TTF_WIN/include"
  -I"$SDL2_TTF_WIN/include/SDL2"
  -I"$SDL2_IMG_WIN/include"
  -I"$SDL2_IMG_WIN/include/SDL2"
  # libs
  -L"$SDL2_WIN/lib"
  -L"$SDL2_TTF_WIN/lib"
  -L"$SDL2_IMG_WIN/lib"
  -lmingw32
  -lSDL2main
  -lSDL2
  -lSDL2_ttf
  -lSDL2_image
  -mconsole
)

x86_64-w64-mingw32-gcc "${gcc_flags[@]}"

# ─── 9. DEPLOY TO WINDOWS ──────────────────────────────────────────────────────
echo "Copying runtime + exe → $WIN_APP_DIR"
sudo rm -rf "$WIN_APP_DIR"/*

# Copy every DLL from SDL2, SDL2_ttf, SDL2_image bins
cp "$SDL2_WIN"/bin/*.dll        "$WIN_APP_DIR/"
cp "$SDL2_TTF_WIN"/bin/*.dll    "$WIN_APP_DIR/"
cp "$SDL2_IMG_WIN"/bin/*.dll    "$WIN_APP_DIR/"

# Copy the exe
cp -f "$BIN_DIR/${APP_NAME}.exe" "$WIN_APP_DIR/"

# assets?
if [ -d "$SRC_DIR/resources" ]; then
  rsync -av --delete \
    "$SRC_DIR/resources/" \
    "$WIN_APP_DIR/resources/"
fi

echo "✅ Done! Launch on Windows via:"
echo "   C:\\Users\\$WIN_USER\\${APP_NAME}\\${APP_NAME}.exe"
[ -d "$WIN_APP_DIR/resources" ] && echo "   …resources under C:\\Users\\$WIN_USER\\${APP_NAME}\\resources"
