#include "ThreadPool.h"
#include<iostream>
#define MAXTASKS  1000 //Yet to be incorporated

inline void ThreadPool::Threadfunc(ThreadPool* const & poolref)
{
	{
		while (true)
		{
			std::function<void()> task{};
			/*Following steps are to be done
				1.See if there is any task in queue
				2.Acquire lock on that queue and then pick up first task and deque it from the queue.
				3.See if task is valid and then execute.
				4.Unknowns - How to return the functions return?
							 How to prioritize the tasks?
			*/
			//Scope for locking mechanics
			{
				std::unique_lock<std::mutex> lock(m_queueMutex);

				/*
					https://en.cppreference.com/w/cpp/thread/condition_variable/wait
				*/
				this->m_waitCondition.wait(lock,
					[this] { return this->m_isStop || !this->m_taskQueue.empty(); });
				if (this->m_isStop && this->m_taskQueue.empty())
					return;
				task = std::move(this->m_taskQueue.front());//Just an elaborated version to pass && instead of copy construtor
				this->m_taskQueue.pop();
			}
			task();
		}
	}
}

ThreadPool::ThreadPool(unsigned int numberOfThreads)
	:m_isDone(false), m_isStop(false)
{
	//m_workers.resize(numberOfThreads);
	std::cout << m_taskQueue.size() << std::endl;
	for (size_t i = 0; i < numberOfThreads; i++)
	{
		//Instantiating threads
		m_workers.emplace_back(
			[this]
			{
				Threadfunc(this);
			}
		);
	}
}



ThreadPool::~ThreadPool()
{
	//RAII
	//Take lock on queue so that no one can add anything in queue
	{
		std::unique_lock<std::mutex> lock(m_queueMutex);
		m_isStop = true;
	}
	m_waitCondition.notify_all();
	//Wait for everyone to finish whatever they are doing
	for (std::thread &worker : m_workers)
		worker.join();
}

