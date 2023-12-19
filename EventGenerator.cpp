#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"

// ��������� ��������� ����� mt19937, ����� ����� � ������� ���� ��������
mt19937 randomNumberGenerator(random_device{}());

// ������� ��� ��������� ��������� ���� � �������, ��������� ������� ����� � ��������� ������������
chrono::system_clock::time_point generateRandomTime() {
    auto now = chrono::system_clock::now();
    auto randDuration = chrono::seconds(randomNumberGenerator() % 1000000000); // �� ������ ���������� �����
    return now - randDuration;
}

// ������� ��������� ������� � ���������� �� � ������� ��� ���������
void eventGenerator(int numEventsPerThread) {
    for (int i = 0; i < numEventsPerThread; ++i) {
        Event event;
        event.timestamp = generateRandomTime(); // ��������� �������
        event.randomNumber = randomNumberGenerator() % 10000 + 1; // ��������� �����
        {
            unique_lock<mutex> lock(eventQueueMutex); // ��������� mutex, ����� ������ ������ �� ������� ���
            eventQueue.push(event); // ������� ����������� � ������� � ���������� ���������
        }
        eventQueueCV.notify_one(); // �����, ������� ������� ���������� ����� ��������,
                                   // ������� ��� ����, ����� ����� �� ������ ������� ����� ����������,
                                   // � ����������, ����� ��� ���������
        
        // �������� ����� ����������� �������
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}