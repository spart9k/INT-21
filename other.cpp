#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"


// Функция проверки числа на простоту
bool isPrime(int num) {
    if (num <= 1) return false;
    int sqrtNum = static_cast<int>(sqrt(num));
    for (int i = 2; i <= sqrtNum; ++i) {
        if (num % i == 0) {
            return false;
        }
    }
    return true;
}

// Обработчик сигнала SIGINT (Ctrl+C), для завершения работы программы досрочно
void signalHandler(int signum) {
    exitFlag.store(true); // Устанавливается флаг true, чтобы программа завершила свою работу
    eventQueueCV.notify_all(); // Все потоки будут разбужены и получат новый флаг, после чего работа оборвётся
}