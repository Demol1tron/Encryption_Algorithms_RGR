#!/bin/bash

PACKAGE_DIR="Encryption_Algorithms"
mkdir -p $PACKAGE_DIR
mkdir -p $PACKAGE_DIR/libs

cp ../app/encryption_app $PACKAGE_DIR/
cp ../libs/*.so $PACKAGE_DIR/libs/

cat > $PACKAGE_DIR/README.txt << EOF
Encryption App для Linux.

Запуск осуществляется как через файл run.sh, так и через командную строку из текущей директории: ./encryption_app.

Шифры:
- Атбаш;
- шифр Хилла;
- шифр фиксированной перестановки.

Использование:
1. Запустите программу;
2. Следуйте инструкциям.
EOF

cat > $PACKAGE_DIR/run.sh << EOF
#!/bin/bash

./encryption_app
EOF

chmod +x $PACKAGE_DIR/run.sh