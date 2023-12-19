#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"

// Глобальные переменные
queue<Event> eventQueue; // Хранение событий
mutex eventQueueMutex; // Доступ к очереди
condition_variable eventQueueCV; // Синхронизация потоков, ожидающих события
atomic<int> totalPrimes(0); // Переменная для подсчета простых чисел
atomic<bool> exitFlag(false); // Флаг завершения приложения

int main() {
    // Обработка UTF-8
    setlocale(LC_ALL, "Russian");

    // Установка обработчика сигнала
    signal(SIGINT, signalHandler);

    // Получение параметров от пользователя
    int numGenerators, numEventsPerGenerator, numProcessors;
    cout << "Введите количество генераторов событий: ";
    cin >> numGenerators;
    cout << "Введите количество событий на каждого генератора: ";
    cin >> numEventsPerGenerator;
    cout << "Введите количество обработчиков событий: ";
    cin >> numProcessors;

    // Запуск потоков генераторов событий
    vector<thread> generatorThreads;
    for (int i = 0; i < numGenerators; ++i) {
        generatorThreads.emplace_back(eventGenerator, numEventsPerGenerator);
    }

    // Запуск потоков обработчиков событий
    vector<thread> processorThreads;
    for (int i = 0; i < numProcessors; ++i) {
        processorThreads.emplace_back(eventProcessor);
    }

    // Ожидание завершения потоков генераторов
    for (auto& thread : generatorThreads) { // Тип элемента определится автоматически
                                            // и будет использоваться по ссылке &
        thread.join();
    }

    // Установка флага завершения для потоков обработчиков
    exitFlag.store(true);
    eventQueueCV.notify_all();

    // Ожидание завершения потоков обработчиков
    for (auto& thread : processorThreads) {
        thread.join();
    }

    // Вывод результата работы
    cout << "Общее количество найденных простых чисел: " << totalPrimes.load() << endl;
    cout << "Все события обработаны." << endl;
    return 0;
}
