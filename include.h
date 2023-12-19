#ifndef include_h
#define include_h

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

// ��������� ����� �����, ����� �� ������������ (std::) ����� ������ ��������
using namespace std;

// ��������� ��� ������������� �������, ������� �������� ����� � �����
struct Event {
    chrono::system_clock::time_point timestamp;
    int randomNumber;
};

// ���������� ����������
extern queue<Event> eventQueue;
extern mutex eventQueueMutex;
extern condition_variable eventQueueCV;
extern atomic<int> totalPrimes;
extern atomic<bool> exitFlag;

#endif