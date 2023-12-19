#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"


// ������� �������� ����� �� ��������
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

// ���������� ������� SIGINT (Ctrl+C), ��� ���������� ������ ��������� ��������
void signalHandler(int signum) {
    exitFlag.store(true); // ��������������� ���� true, ����� ��������� ��������� ���� ������
    eventQueueCV.notify_all(); // ��� ������ ����� ��������� � ������� ����� ����, ����� ���� ������ ��������
}