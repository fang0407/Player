#ifndef FRAME_QUEUE_HPP
#define FRAME_QUEUE_HPP

#include "queue.hpp"
#include "util.h"

using namespace gcm::container;

class FrameQueue
{
public:
    using Queue = Queue<std::shared_ptr<AVFrame>>;
    using PredicateFn = Queue::PredicateFn;

    bool Push(std::shared_ptr<AVFrame> frame)
    {
        if (!frame)
            return false;

        queue_.sync_emplace(frame);
        return true;
    }

    void Pop(std::shared_ptr<AVFrame>& frame)
    {
        queue_.sync_pop(frame);
    }

    void Peek(std::shared_ptr<AVFrame>& frame)
    {
        queue_.peek(frame);
    }

    size_t Size()
    {
        return queue_.size();
    }

    Queue queue_;
};


#endif // FRAME_QUEUE_HPP
