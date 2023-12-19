#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"

// √енератор случайных чисел mt19937, чтобы числа в потоках были различны
mt19937 randomNumberGenerator(random_device{}());

// ‘ункци€ дл€ генерации случайной даты и времени, использу€ текущее врем€ и случайную длительность
chrono::system_clock::time_point generateRandomTime() {
    auto now = chrono::system_clock::now();
    auto randDuration = chrono::seconds(randomNumberGenerator() % 1000000000); // Ќа основе случайного числа
    return now - randDuration;
}

// ‘ункци€ генерации событий и добавлени€ их в очередь дл€ обработки
void eventGenerator(int numEventsPerThread) {
    for (int i = 0; i < numEventsPerThread; ++i) {
        Event event;
        event.timestamp = generateRandomTime(); // √енератор времени
        event.randomNumber = randomNumberGenerator() % 10000 + 1; // √енератор чисел
        {
            unique_lock<mutex> lock(eventQueueMutex); // Ѕлокируем mutex, чтобы другие потоки не трогали его
            eventQueue.push(event); // —обытие добавл€етс€ в очередь и блокировка снимаетс€
        }
        eventQueueCV.notify_one(); // ѕоток, который ожидает переменную будет разбужен,
                                   // сделано дл€ того, чтобы поток не ожидал слишком долго переменную,
                                   // а просыпалс€, когда это требуетс€
        
        // «адержка между генераци€ми событий
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}