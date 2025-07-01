#ifndef G4M_EUROPE_DG_THREAD_POOL_HPP
#define G4M_EUROPE_DG_THREAD_POOL_HPP

#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <iostream>

using namespace std;

namespace g4m {
    class ThreadPool {
    public:
        ThreadPool() {
            start();
        }

        ~ThreadPool() {
            stop();
        }

        void start() {
            const uint32_t num_threads = jthread::hardware_concurrency();   // Max № of threads the system supports
            for (uint32_t i = 0; i < num_threads; ++i)
                threads.emplace_back(&ThreadPool::ThreadLoop, this, ssource.get_token());
        }

        void enqueue(const function<void()> &job) {
            {
                scoped_lock<mutex> lock{queue_mutex};
                jobs.push(job);
            }
            mutex_condition.notify_one();
        }

        void stop() {
            ssource.request_stop();
            mutex_condition.notify_all();
            for (auto &active_thread: threads)
                active_thread.join();
            threads.clear();
        }

        bool busy() {
            scoped_lock<mutex> lock{queue_mutex};
            return !jobs.empty();
        }

    private:
        stop_source ssource;
        mutex queue_mutex;                      // Prevents data races to the job queue
        condition_variable mutex_condition;     // Allows threads to wait on new jobs or termination
        vector<jthread> threads;
        queue<function<void()> > jobs;

        void ThreadLoop(const stop_token &stoken) {
            for (function<void()> job; true; job()) {
                unique_lock<mutex> lock{queue_mutex};
                mutex_condition.wait(lock, [&] { return !jobs.empty() || stoken.stop_requested(); });
                if (jobs.empty() && stoken.stop_requested())
                    return;
                job = jobs.front();
                jobs.pop();
            }
        }
    };
}

#endif
