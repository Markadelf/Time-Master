#pragma once

///Ref and brief explanation
//https://github.com/mtrebi/thread-pool

/*TODO:
	1. Make queue lockfree and check the efficiency in both cases go with one which is faster
	2. Make Queue and vector use Mark's allocators
	3. Other helper methods if anyone can think of any

*/

#include <thread>
#include <vector>
#include <atomic>
#include <future>
#include <mutex>
#include <exception>
#include <queue>
#include <condition_variable>
#include <functional>
#include <deque>
#include <iostream>
class ThreadPool
{
private:
	std::vector<std::thread> m_workers;
	std::atomic<bool> m_isDone;
	std::atomic<bool> m_isStop;
	std::queue<std::function<void()>>m_taskQueue;
	std::mutex m_queueMutex;
	std::condition_variable m_waitCondition;


	void Threadfunc(ThreadPool* const & poolref);
	

public:
	ThreadPool(unsigned int numberOfThreads);

	/*
		Following function uses Parameter packing. For more details please refer the link
		https://en.cppreference.com/w/cpp/language/parameter_pack
		https://en.cppreference.com/w/cpp/types/result_of
	*/
	template<class F, class... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<typename std::result_of<F(Args...)>::type>;


	/*void NotifyOne();
	void NotifyAll();
	*/
	~ThreadPool();
};




template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)
-> std::future<typename std::result_of<F(Args...)>::type>
{
	using return_type = typename std::result_of<F(Args...)>::type; //typename is used when you want to decide the type based on some operation

	//pakacged task wraps, function pointer with parameters and can be used call function
	//I am making a shared pointer here as we have to allow any thread to pick it up.
	auto task = std::make_shared< std::packaged_task<return_type()> >(
		std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

	std::future<return_type> result = task->get_future();
	
	std::unique_lock<std::mutex> lock(m_queueMutex);

	// don't allow enqueueing after stopping the pool
	if (m_isStop)
		throw std::runtime_error("Hey SquidWard...Threadpool is closed today!!!");

	m_taskQueue.emplace([task]() { (*task)(); });
	
	m_waitCondition.notify_one();
	return result;
}

