#!/bin/bash

echo "Удаление Encryption_Algorithm_RGR >>>"

DESKTOP_DIR=$(xdg-user-dir DESKTOP)
APP_NAME="Encryption_Algorithm_RGR"
INSTALL_DIR="$DESKTOP_DIR/$APP_NAME"
DESKTOP_FILE="$DESKTOP_DIR/$APP_NAME.desktop"

echo "Удаление файлов >>>"
if [ -d "$INSTALL_DIR" ]; then
    rm -rf "$INSTALL_DIR"
    echo "Удалена папка >> $INSTALL_DIR"
else
    echo "Папка $INSTALL_DIR не найдена"
fi

if [ -f "$DESKTOP_FILE" ]; then
    rm -f "$DESKTOP_FILE"
    echo "Удален ярлык >> $DESKTOP_FILE"
else
    echo "Ярлык $DESKTOP_FILE не найден"
fi

echo ""
echo "Удаление завершено!"