#include <iostream>
#include <thread> // потоки
#include <vector> // вектора
#include <queue> // работа с очередью
#include <mutex> // безопасный доступ к данным из потока
#include <condition_variable> // для использования unique_lock, тоже для безопасного доступа к данным из потока
#include <atomic> // для операций в потоках
#include <cmath> // математические операции
#include <csignal> // обработка сигналов (Ctrl+C)
#include <random> // генерация случайных чисел
#include <chrono> // работа со временем
#include <iomanip> // манипуляция с выводом, форматирование времени

using namespace std; // использую такой метод, чтобы не использовать (std::) перед каждой командой

// Генератор случайных чисел mt19937, чтобы числа в потоках были различны
mt19937 randomNumberGenerator(random_device{}());

// Функция для генерации случайной даты и времени, используя текущее время и случайную длительность
chrono::system_clock::time_point generateRandomTime() {
    auto now = chrono::system_clock::now();
    auto randDuration = chrono::seconds(randomNumberGenerator() % 1000000000);  // На основе случайного числа
    return now - randDuration;
}

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

// Структура для представления события, которая содержит время и число
struct Event {
    chrono::system_clock::time_point timestamp;
    int randomNumber;
};

// Глобальные переменные
queue<Event> eventQueue; // Хранение событий
mutex eventQueueMutex; // Доступ к очереди
condition_variable eventQueueCV; // Синхронизация потоков, ожидающих события
atomic<int> totalPrimes(0); // Переменная для подсчета простых чисел
atomic<bool> exitFlag(false); // Флаг завершения приложения

// Функция генерации событий и добавления их в очередь для обработки
void eventGenerator(int numEventsPerThread) {
    for (int i = 0; i < numEventsPerThread; ++i) {
        Event event;
        event.timestamp = generateRandomTime(); // Генератор времени
        event.randomNumber = randomNumberGenerator() % 10000 + 1; // Генератор чисел
        {
            unique_lock<mutex> lock(eventQueueMutex); // Блокируем mutex, чтобы другие потоки не трогали его
            eventQueue.push(event); // Событие добавляется в очередь и блокировка снимается
        }
        eventQueueCV.notify_one(); // Поток, который ожидает переменную будет разбужен,
                                   // сделано для того, чтобы поток не ожидал слишком долго переменную,
                                   // а просыпался, когда это требуется
        // Задержка между генерациями событий
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

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

// Обработчик сигнала SIGINT (Ctrl+C), для завершения работы программы досрочно
void signalHandler(int signum) {
    exitFlag.store(true); // Устанавливается флаг true, чтобы программа завершила свою работу
    eventQueueCV.notify_all(); // Все потоки будут разбужены и получат новый флаг, после чего работа оборвётся
}

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
