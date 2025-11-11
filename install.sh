#!/bin/bash

set -e

echo "Установка Encryption_Algorithm_RGR >>>"

DESKTOP_DIR=$(xdg-user-dir DESKTOP)
APP_NAME="Encryption_Algorithm_RGR"
INSTALL_DIR="$DESKTOP_DIR/$APP_NAME"
DESKTOP_FILE="$DESKTOP_DIR/$APP_NAME.desktop"
LIBS_DIR="$INSTALL_DIR/libs"

REQUIRED_FILES=("src/main.cpp" "src/cipher_manager.cpp" "src/encrypt_decrypt_keygen.cpp" "ciphers/atbash.cpp" "ciphers/hill.cpp" "ciphers/permutations.cpp" "utils/file_utils.cpp")
for file in "${REQUIRED_FILES[@]}"; do
    if [ ! -f "$file" ]; then
        echo "Ошибка >> файл '$file' не найден."
        exit 1
    fi
done

if ! command -v g++ &> /dev/null; then
    echo "Ошибка >> компилятор g++ не найден."
    exit 1
fi

rm -rf "$INSTALL_DIR"
rm -f "$DESKTOP_FILE"

mkdir -p "$INSTALL_DIR"
mkdir -p "$LIBS_DIR"

echo "Компиляция библиотек >>>"
echo "1) Атбаш"
g++ -shared -fPIC -o "$LIBS_DIR/atbash.so" ciphers/atbash.cpp

echo "2) Шифр Хилла"
g++ -shared -fPIC -o "$LIBS_DIR/hill.so" ciphers/hill.cpp

echo "3) Шифр фиксированной перестановки"
g++ -shared -fPIC -o "$LIBS_DIR/permutations.so" ciphers/permutations.cpp

echo "Компиляция программы >>>"
g++ -o "$INSTALL_DIR/Encryption_Algorithm_RGR" src/main.cpp src/cipher_manager.cpp src/encrypt_decrypt_keygen.cpp utils/file_utils.cpp

cat > "$INSTALL_DIR/README.txt" <<'EOF'
Encryption_Algorithm_RGR - программа для шифрования и дешифрования текстов 
и файлов с использованием трех алгоритмов:

1. Атбаш
    - простая замена символов;
    - не требует ключа;

2. Шифр Хилла
   - матричное шифрование;
   - ключ: 4 числа через запятую (например: 1,3,5,7);
   - определитель матрицы должен быть нечетным.

3. Шифр фиксированной перестановки
   - юлочная перестановка символов;
   - ключ: перестановка чисел 1-4 (например: 3,2,4,1);
   - размер блока: 4 символа.

Запуск программы происходит через ярлык на рабочем столе.
EOF

echo "Создание ярлыка на рабочем столе >>>"
cat > "$DESKTOP_FILE" <<EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=Encryption_Algorithm_RGR
Comment=Программа для шифрования и дешифрования
Exec=$INSTALL_DIR/Encryption_Algorithm_RGR
Icon=utilities-terminal
Categories=Utility;Development;
Terminal=true
StartupNotify=true
Path=$INSTALL_DIR
EOF

chmod +x "$DESKTOP_FILE"
chmod +x "$INSTALL_DIR/Encryption_Algorithm_RGR"

echo ""
echo "Установка завершена!"