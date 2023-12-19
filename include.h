#ifndef include_h
#define include_h

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

// Использую такой метод, чтобы не использовать (std::) перед каждой командой
using namespace std;

// Структура для представления события, которая содержит время и число
struct Event {
    chrono::system_clock::time_point timestamp;
    int randomNumber;
};

// Глобальные переменные
extern queue<Event> eventQueue;
extern mutex eventQueueMutex;
extern condition_variable eventQueueCV;
extern atomic<int> totalPrimes;
extern atomic<bool> exitFlag;

#endif