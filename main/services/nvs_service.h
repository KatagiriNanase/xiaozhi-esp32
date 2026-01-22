#ifndef __NVS_SERVICE_H__
#define __NVS_SERVICE_H__

#include <bitset>
#include <queue>
#include <future>
#include <variant>
#include <string>
#include "boost/thread.hpp"
#include "boost/signals2.hpp"

constexpr size_t NVS_VALUE_STR_MAX_LEN = 128;

class NVSservice {
public:
    using Key = std::string;
    using Value = std::variant<int, std::string>;

    enum class Operation {
        UpdateNVS,
        UpdateParam,
        EraseNVS,
        Max,
    };

    struct Event {
        void dump() const;

        const void* sender;
        Operation operation;
        Key key;
    };
    using EventFuture = std::future<bool>;
    using EventSignal = boost::signals2::signal<void(const Event& event)>;

    NVSservice(const NVSservice&) = delete;
    NVSservice(NVSservice&&) = delete;
    ~NVSservice() = default;

    NVSservice& operator=(const NVSservice&) = delete;
    NVSservice& operator=(NVSservice&&) = delete;

    bool begin();

    bool sendEvent(const Event& event, EventFuture* future = nullptr);

    bool setLocalParam(const Key& key, const Value& value, const void* sender = nullptr, EventFuture* future = nullptr);
    bool getLocalParam(const Key& key, Value& value);
    bool eraseNVS(const void* sender = nullptr, EventFuture* future = nullptr);

    boost::signals2::connection connectEventSignal(EventSignal::slot_type slot);

    static NVSservice& requestInstance()
    {
        static NVSservice instance;
        return instance;
    }

private:
    using EventPromise = std::promise<bool>;
    struct EventWrapper {
        Event event;
        std::shared_ptr<EventPromise> promise;
    };

    NVSservice() = default;

    bool processEvent(const Event& event);
    bool doEventOperationUpdateNVS(const Key& key);
    bool doEventOperationUpdateParam();
    bool doEventOperationEraseNVS();

    std::map<Key, Value> _local_params;
    std::mutex _params_mutex;

    std::queue<EventWrapper> _event_queue;
    std::mutex _event_mutex;
    std::condition_variable _event_cv;
    boost::thread _event_thread;
    EventSignal _event_signal;
};

#endif /* __NVS_SERVICE_H__ */
