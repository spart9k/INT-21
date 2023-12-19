#include "EventGenerator.h"
#include "EventProcessor.h"
#include "include.h"
#include "other.h"


// Функция обработки событий. События ожидаются, после чего обрабатываются
void eventProcessor() {
    while (true) {
        unique_lock<mutex> lock(eventQueueMutex); // Блокируем Mutex, чтобы другие потоки не трогали это событие
        eventQueueCV.wait(lock, [] { return !eventQueue.empty() || exitFlag.load(); }); // Ожидается изменение в переменной.
        // Либо появялется новое событие,
        // либо устанавливается флаг завершения работы
        while (!eventQueue.empty()) { // Если есть событие в очереди, значит оно извлекается из очереди и обрабатывается
            Event event = eventQueue.front(); // Извлекается 1 элемент из очереди
            eventQueue.pop(); // Элемент удаляется из общей очереди
            // Обработка события
            if (isPrime(event.randomNumber)) { // Проверка на простоту числа
                time_t timestamp = chrono::system_clock::to_time_t(event.timestamp); // Преобразование времени события в другой формат
                tm localTime;
                localtime_s(&localTime, &timestamp); // Преобразование времени в год:месяц:день:время
                char buffer[20];
                strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &localTime); // Форматирование времени в формат и сохранение в буффер
                cout << "Найдено простое число: (" << event.randomNumber << ")"
                    << " с меткой времени: (" << buffer << ")" << endl; // Вывод информации, что найдено простое число с временной меткой
                totalPrimes.fetch_add(1); // Увеличение счётчика найденных чисел на 1
            }
        }
        lock.unlock(); // Mutex разблокируется
        if (exitFlag.load()) { // Проверяется, установлен ли флаг окончания работы, если да, то цикл обрывается
            break;
        }
    }
}