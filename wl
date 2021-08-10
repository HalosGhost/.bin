#!/usr/bin/env sh

WALL_PATH="${WALL_PATH:-${XDG_DATA_HOME:-$HOME}/img/bkg}"
WALL="${WALL:-$HOME/.xbkg}"

IMAGES=$(find "$WALL_PATH" -type f)
NEW=$(echo "$IMAGES" | shuf | head -1)

if [ "$1" != '-n' ]; then
    ln -sf "$NEW" "$WALL"
fi

yawa -F "$WALL"
