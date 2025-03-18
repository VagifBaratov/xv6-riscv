#include "user/user.h"
#include "kernel/procinfo.h"

void test_null_buffer() {
    printf("Тест 1: plist = NULL... ");
    int count = ps_listinfo(0, 0);
    if (count < 0) {
        printf("FAIL (ошибка %d)\n", count);
        exit(1);
    }
    printf("OK. Процессов: %d\n", count);
}

void test_sufficient_buffer() {
    printf("Тест 2: Достаточный буфер... ");
    int count = ps_listinfo(0, 0);
    if (count <= 0) {
        printf("FAIL: Нет процессов\n");
        exit(1);
    }

    struct procinfo *buf = malloc(count * sizeof(struct procinfo));
    int ret = ps_listinfo(buf, count);
    
    if (ret != count) {
        printf("FAIL: ожидалось %d, получено %d\n", count, ret);
        free(buf);
        exit(1);
    }
    
    printf("OK. Записано %d процессов:\n", ret);
    for (int i = 0; i < ret; i++) {
        printf("[%d] PID: %d | PPID: %d | Состояние: %d | Имя: %s\n",
               i, buf[i].pid, buf[i].ppid, buf[i].state, buf[i].name);
    }
    free(buf);
}

void test_small_buffer() {
    printf("Тест 3: Недостаточный буфер... ");
    int count = ps_listinfo(0, 0);
    if (count <= 1) {
        printf("SKIP: нужно минимум 2 процесса\n");
        return;
    }

    struct procinfo buf[1];
    int ret = ps_listinfo(buf, 1);
    
    if (ret != -4) {
        printf("FAIL: ожидалось -4, получено %d\n", ret);
        exit(1);
    }
    printf("OK. Ошибка -4 (недостаточный размер)\n");
}

void test_bad_address() {
    printf("Тест 4: Некорректный адрес... ");
    int ret = ps_listinfo((struct procinfo*)0xDEADBEEF, 1);
    
    if (ret != -3) {
        printf("FAIL: ожидалось -3, получено %d\n", ret);
        exit(1);
    }
    printf("OK. Ошибка -3 (некорректный адрес)\n");
}

void test_negative_lim() {
    printf("Тест 5: Отрицательный lim... ");
    struct procinfo buf;
    int ret = ps_listinfo(&buf, -5);
    
    if (ret != -2) {
        printf("FAIL: ожидалось -2, получено %d\n", ret);
        exit(1);
    }
    printf("OK. Ошибка -2 (отрицательный lim)\n");
}

int main() {
    printf("\n=== Начало тестирования ps_listinfo ===\n");
    
    test_null_buffer();
    test_sufficient_buffer();
    test_small_buffer();
    test_bad_address();
    test_negative_lim();

    printf("\n=== Все тесты пройдены успешно ===\n");
    exit(0);
}