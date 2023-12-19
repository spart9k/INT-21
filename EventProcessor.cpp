#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"


// ������� ��������� �������. ������� ���������, ����� ���� ��������������
void eventProcessor() {
    while (true) {
        unique_lock<mutex> lock(eventQueueMutex); // ��������� Mutex, ����� ������ ������ �� ������� ��� �������
        eventQueueCV.wait(lock, [] { return !eventQueue.empty() || exitFlag.load(); }); // ��������� ��������� � ����������.
        // ���� ���������� ����� �������,
        // ���� ��������������� ���� ���������� ������
        while (!eventQueue.empty()) { // ���� ���� ������� � �������, ������ ��� ����������� �� ������� � ��������������
            Event event = eventQueue.front(); // ����������� 1 ������� �� �������
            eventQueue.pop(); // ������� ��������� �� ����� �������
            // ��������� �������
            if (isPrime(event.randomNumber)) { // �������� �� �������� �����
                time_t timestamp = chrono::system_clock::to_time_t(event.timestamp); // �������������� ������� ������� � ������ ������
                tm localTime;
                localtime_s(&localTime, &timestamp); // �������������� ������� � ���:�����:����:�����
                char buffer[20];
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &localTime); // �������������� ������� � ������ � ���������� � ������
                cout << "������� ������� �����: (" << event.randomNumber << ")"
                    << " � ������ �������: (" << buffer << ")" << endl; // ����� ����������, ��� ������� ������� ����� � ��������� ������
                totalPrimes.fetch_add(1); // ���������� �������� ��������� ����� �� 1
            }
        }
        lock.unlock(); // Mutex ��������������
        if (exitFlag.load()) { // �����������, ���������� �� ���� ��������� ������, ���� ��, �� ���� ����������
            break;
        }
    }
}