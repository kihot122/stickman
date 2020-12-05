#include <mutex>
#include <queue>
#include <thread>

template <typename T>
class SharedQueue
{
    std::queue<T> Elements;
    std::mutex M;

public:
    SharedQueue();

    void Push(T Item);
    T Pop();
};

template <typename T>
SharedQueue<T>::SharedQueue() {}

template <typename T>
void SharedQueue<T>::Push(T Item)
{
    M.lock();
    Elements.push(Item);
    M.unlock();
}

template <typename T>
T SharedQueue<T>::Pop()
{
    T Item;
    bool Flag = true;

    while (Flag)
    {
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(100ms);

        M.lock();
        if (Elements.size() != 0)
        {
            Item = Elements.front();
            Elements.pop();
            Flag = false;
        }
        M.unlock();
    }

    return Item;
}