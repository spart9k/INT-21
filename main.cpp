#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"

// ���������� ����������
queue<Event> eventQueue; // �������� �������
mutex eventQueueMutex; // ������ � �������
condition_variable eventQueueCV; // ������������� �������, ��������� �������
atomic<int> totalPrimes(0); // ���������� ��� �������� ������� �����
atomic<bool> exitFlag(false); // ���� ���������� ����������

int main() {
    // ��������� UTF-8
    setlocale(LC_ALL, "Russian");

    // ��������� ����������� �������
    signal(SIGINT, signalHandler);

    // ��������� ���������� �� ������������
    int numGenerators, numEventsPerGenerator, numProcessors;
    cout << "������� ���������� ����������� �������: ";
    cin >> numGenerators;
    cout << "������� ���������� ������� �� ������� ����������: ";
    cin >> numEventsPerGenerator;
    cout << "������� ���������� ������������ �������: ";
    cin >> numProcessors;

    // ������ ������� ����������� �������
    vector<thread> generatorThreads;
    for (int i = 0; i < numGenerators; ++i) {
        generatorThreads.emplace_back(eventGenerator, numEventsPerGenerator);
    }

    // ������ ������� ������������ �������
    vector<thread> processorThreads;
    for (int i = 0; i < numProcessors; ++i) {
        processorThreads.emplace_back(eventProcessor);
    }

    // �������� ���������� ������� �����������
    for (auto& thread : generatorThreads) { // ��� �������� ����������� �������������
                                            // � ����� �������������� �� ������ &
        thread.join();
    }

    // ��������� ����� ���������� ��� ������� ������������
    exitFlag.store(true);
    eventQueueCV.notify_all();

    // �������� ���������� ������� ������������
    for (auto& thread : processorThreads) {
        thread.join();
    }

    // ����� ���������� ������
    cout << "����� ���������� ��������� ������� �����: " << totalPrimes.load() << endl;
    cout << "��� ������� ����������." << endl;
    return 0;
}
