#ifndef PACKET_QUEUE_HPP
#define PACKET_QUEUE_HPP

#include "queue.hpp"
#include "util.h"

using namespace gcm::container;

class PacketQueue
{
public:
    using Queue = Queue<std::shared_ptr<AVPacket>>;
    using PredicateFn = Queue::PredicateFn;

    bool Push(std::shared_ptr<AVPacket> pkt)
    {
        if (!pkt)
            return false;

        queue_.sync_emplace(pkt);
        return true;
    }

    bool Pop(std::shared_ptr<AVPacket>& pkt)
    {
        queue_.sync_pop(pkt);
        return true;
    }

    size_t Size()
    {
        return queue_.size();
    }

    Queue queue_;
};

#endif // PACKET_QUEUE_HPP
