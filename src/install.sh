#!/usr/bin/env bash

PREFIX="${PREFIX:-..}"
DESTDIR="${DESTDIR:-.}"

for i in *.out; do
    install -Dm755 "$i" "${DESTDIR}/${PREFIX}/${i%.*}"
done
