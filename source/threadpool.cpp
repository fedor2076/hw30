#include "threadpool.h"

ThreadPool::ThreadPool() :
m_thread_count(std::thread::hardware_concurrency() != 0 ? 
std::thread::hardware_concurrency() : 4),
m_thread_queues(m_thread_count)
{
    m_index = 0;
}

void ThreadPool::start()
{
    for (int i = 0; i < m_thread_count; i++)
    {
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i);
    }
}

void ThreadPool::threadFunc(int qindex)
{
    while (true)
    {
        // обработка очередной задачи
        task_type task_to_do;

        int i = 0;
        bool res{false};
        for (; i < m_thread_count; i++)
        {
            // попытка быстро забрать задачу из любой очереди, начиная со своей
            if ((res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do)))
                break;
        }

        if (!res)
        {
            // вызываем блокирующее получение очереди
            m_thread_queues[qindex].pop(task_to_do);
        }
        else if (!task_to_do.valid())
        {
            // чтобы не допустить зависания потока
            // кладем обратно задачу-пустышку
            m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);
        }
        if (!task_to_do.valid())
        {
            return;
        }
        // выполняем задачу
        task_to_do();
    }
}

void ThreadPool::stop()
{
    for (int i = 0; i < m_thread_count; i++)
    {
        // кладем задачу-пустышку в каждую очередь
        // для завершения потока
        task_type empty_task;
        m_thread_queues[i].push(empty_task);
    }
    for (auto &t : m_threads)
    {
        t.join();
    }
}

std::future<void> ThreadPool::push_task(FuncType f, int *arr, long start, long end)
{
    // вычисляем индекс очереди, куда положим задачу
    int queue_to_push = m_index++ % m_thread_count;
    // формируем функтор
    task_type task([=]
                   { f(arr, start, end); });
    auto res = task.get_future();
    // кладем в очередь
    m_thread_queues[queue_to_push].push(task);
    return res;
}

void ThreadPool::run_pending_task()
{
    task_type task_to_do;

    int i = 0;
    for (; i < m_thread_count; ++i)
    {
        if (m_thread_queues[i % m_thread_count].fast_pop(task_to_do))
            break;
    }
    bool res;
    if (!task_to_do.valid())
    {
        std::this_thread::yield();
    }
    else
    {
        task_to_do();
    }
}
