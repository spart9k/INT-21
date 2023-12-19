#include <iostream>
#include <thread> // ������
#include <vector> // �������
#include <queue> // ������ � ��������
#include <mutex> // ���������� ������ � ������ �� ������
#include <condition_variable> // ��� ������������� unique_lock, ���� ��� ����������� ������� � ������ �� ������
#include <atomic> // ��� �������� � �������
#include <cmath> // �������������� ��������
#include <csignal> // ��������� �������� (Ctrl+C)
#include <random> // ��������� ��������� �����
#include <chrono> // ������ �� ��������
#include <iomanip> // ����������� � �������, �������������� �������

using namespace std; // ��������� ����� �����, ����� �� ������������ (std::) ����� ������ ��������

// ��������� ��������� ����� mt19937, ����� ����� � ������� ���� ��������
mt19937 randomNumberGenerator(random_device{}());

// ������� ��� ��������� ��������� ���� � �������, ��������� ������� ����� � ��������� ������������
chrono::system_clock::time_point generateRandomTime() {
    auto now = chrono::system_clock::now();
    auto randDuration = chrono::seconds(randomNumberGenerator() % 1000000000);  // �� ������ ���������� �����
    return now - randDuration;
}

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

// ��������� ��� ������������� �������, ������� �������� ����� � �����
struct Event {
    chrono::system_clock::time_point timestamp;
    int randomNumber;
};

// ���������� ����������
queue<Event> eventQueue; // �������� �������
mutex eventQueueMutex; // ������ � �������
condition_variable eventQueueCV; // ������������� �������, ��������� �������
atomic<int> totalPrimes(0); // ���������� ��� �������� ������� �����
atomic<bool> exitFlag(false); // ���� ���������� ����������

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

// ���������� ������� SIGINT (Ctrl+C), ��� ���������� ������ ��������� ��������
void signalHandler(int signum) {
    exitFlag.store(true); // ��������������� ���� true, ����� ��������� ��������� ���� ������
    eventQueueCV.notify_all(); // ��� ������ ����� ��������� � ������� ����� ����, ����� ���� ������ ��������
}

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
