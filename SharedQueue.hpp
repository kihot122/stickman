#include <mutex>
#include <queue>

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
SharedQueue<T>::SharedQueue()
{
    M.unlock();
}

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
    M.lock();
    Item = Elements.front();
    Elements.pop();
    M.unlock();
    return Item;
}