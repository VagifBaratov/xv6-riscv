#!/bin/bash

set -e

echo "Компиляция программы..."
gcc get_inode.c -o get_inode || { echo "Ошибка компиляции"; exit 1; }

echo "Создание тестового образа..."
truncate --size 100M ext2.img
mkfs.ext2 -F ext2.img

mkdir -p mnt
sudo mount -t ext2 ext2.img mnt
sudo chown -R $(id -u):$(id -g) mnt
sudo chmod -R u+rwX mnt

echo "Создание тестовых файлов..."
echo "Test file 1" > mnt/file1.txt
echo "Test file 2 with more data" > mnt/file2.txt

dd if=/dev/zero of=mnt/sparse bs=1 count=0 seek=1M 2>/dev/null
echo "Sparse data" > mnt/sparse

inode1=$(ls -i mnt/file1.txt | awk '{print $1}')
inode2=$(ls -i mnt/file2.txt | awk '{print $1}')
inode_sparse=$(ls -i mnt/sparse | awk '{print $1}')

sum1=$(sha512sum mnt/file1.txt | awk '{print $1}')
sum2=$(sha512sum mnt/file2.txt | awk '{print $1}')
sum_sparse=$(sha512sum mnt/sparse | awk '{print $1}')

sudo umount mnt
rmdir mnt

echo "Тестирование извлечения..."
test_sum1=$(./get_inode ext2.img $inode1 | sha512sum | awk '{print $1}')
test_sum2=$(./get_inode ext2.img $inode2 | sha512sum | awk '{print $1}')
test_sum_sparse=$(./get_inode ext2.img $inode_sparse | sha512sum | awk '{print $1}')

echo -e "\nРезультаты:"
[ "$sum1" == "$test_sum1" ] && echo "File1: PASS" || echo "File1: FAIL"
[ "$sum2" == "$test_sum2" ] && echo "File2: PASS" || echo "File2: FAIL"
[ "$sum_sparse" == "$test_sum_sparse" ] && echo "Sparse: PASS" || echo "Sparse: FAIL"

rm ext2.img get_inode