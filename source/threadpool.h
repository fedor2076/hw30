#pragma once
#include <queue>
#include <future>
#include <thread>
#include <condition_variable>
#include <vector>
#include <functional>
#include <stack>
#include <iostream>
#include <mutex>
void taskFunc(int *, long start, long end);
// удобное определение для сокращения кода
typedef std::packaged_task<void()> task_type;
// тип указатель на функцию, которая является эталоном для функций задач
typedef void (*FuncType)(int *, long start, long end);
template <class T>
class BlockedQueue
{
public:
    void push(T &item)
    {
        std::lock_guard<std::mutex> l(m_locker);
        // обычный потокобезопасный push
        m_task_queue.push(std::move(item));
        // делаем оповещение, чтобы поток, вызвавший
        // pop проснулся и забрал элемент из очереди
        m_notifier.notify_one();
    }
    // блокирующий метод получения элемента из очереди
    void pop(T &item)
    {
        std::unique_lock<std::mutex> l(m_locker);
        if (m_task_queue.empty())
        {
            m_notifier.wait(l, [this]
                            { return !m_task_queue.empty(); });
        }

        item = std::move(m_task_queue.front());
        m_task_queue.pop();
    }
    // неблокирующий метод получения элемента из очереди
    // возвращает false, если очередь пуста
    bool fast_pop(T &item)
    {
        std::lock_guard<std::mutex> l(m_locker);
        if (m_task_queue.empty())
            // просто выходим
            return false;
        // забираем элемент
        item = std::move(m_task_queue.front());
        m_task_queue.pop();
        return true;
    }

private:
    std::mutex m_locker;
    // очередь задач
    std::queue<T> m_task_queue;
    // уведомитель
    std::condition_variable m_notifier;
};

class ThreadPool
{
public:
    ThreadPool();
    // запуск:
    void start();
    // остановка:
    void stop();
    // проброс задач
    std::future<void> push_task(FuncType f, int *arr, long start, long end);
    // функция входа для потока
    void threadFunc(int qindex);
    void run_pending_task();

private:
    // количество потоков
    int m_thread_count;
    // потоки
    std::vector<std::thread> m_threads;
    // очереди задач для потоков
    std::vector<BlockedQueue<task_type>> m_thread_queues;
    // для равномерного распределения задач
    int m_index;
};
