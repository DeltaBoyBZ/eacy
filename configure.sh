#!/bin/sh

GDB_INIT_FILE="${HOME}/.config/gdb/gdbinit"

[ -f "$GDB_INIT_FILE" ] || GDB_INIT_FILE="${HOME}/.gdbinit"

touch $GDB_INIT_FILE
SOURCE_COMMAND="source /usr/local/share/eacy/eacy.py"

grep "$SOURCE_COMMAND" "$GDB_INIT_FILE" \
    || echo "$SOURCE_COMMAND" >> "$GDB_INIT_FILE"

[ -d "${HOME}/.vim" ] || exit

[ -d "${HOME}/.vim/plugged/eacy" ] && exit

mkdir -p "${HOME}/.vim/plugin"
cp "plugin/eacy.vim" "${HOME}/.vim/plugin/eacy.vim"

