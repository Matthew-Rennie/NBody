#pragma once
#include <thread>
#include <mutex>
#include <chrono>
#include <tuple>
#include <queue>
#include <memory>

namespace core::TTM
{
	namespace Internal
	{
		struct TaskBase
		{
			TaskBase() = default;
			// allows functions to be run without knowing template types
			virtual void run_func() {};
		};

		template <typename... Args>
		struct Task : public TaskBase
		{
		public:
			Task() = default;
			typedef void (*TaskFunc)(Args...);
			void run_func() override { std::apply((TaskFunc)m_func, m_args); }
			void* m_func = nullptr;
			std::tuple<Args...> m_args;
		};
	}

	namespace Internal
	{
		// threadsafe wrapper around std::queue
		class TaskQueue
		{
		public:

			TaskQueue() = default;
			TaskQueue(TaskQueue const&) = delete;
			void operator=(TaskQueue const&) = delete;

			// construct a new task and place it on the end of the queue
			template<typename... Args>
			void emplace(void* func, Args... args)
			{
				Task<Args ...>* task = new Task<Args ...>;
				task->m_func = func;
				task->m_args = std::make_tuple(args...);

				std::unique_lock<std::mutex> lock(m_mutex);
				m_queue.push(task);
			}

			// remove the item from the front of the queue and return it
			[[nodiscard]] TaskBase* pop()
			{
				std::unique_lock<std::mutex> lock(m_mutex);

				if (m_queue.empty())
				{
					return nullptr;
				}

				TaskBase* task = m_queue.front();
				m_queue.pop();

				return task;
			}

			// return number of items currently in queue
			inline size_t size() const
			{
				std::unique_lock<std::mutex> lock(m_mutex);
				return m_queue.size();
			}

		private:

			std::queue<TaskBase*> m_queue;

			// used to make all functions thread safe
			mutable std::mutex m_mutex;

		};
	}

	class ThreadTaskManager
	{
	public:

		ThreadTaskManager(const unsigned int&& num_threads)
		{
			m_thread_count = num_threads;
			m_thread_buffer = new std::thread* [num_threads];
			for (size_t i = 0; i < num_threads; i++)
			{
				m_thread_buffer[i] = new std::thread(thread_func, this);
			}
		}

		ThreadTaskManager(ThreadTaskManager const&) = delete;
		void operator=(ThreadTaskManager const&) = delete;

		~ThreadTaskManager()
		{
			// TODO - consider completing all tasks before exiting?

			m_exit = true;
			wake_threads();

			if (m_thread_buffer)
			{
				for (size_t i = 0; i < thread_count(); i++)
				{
					m_thread_buffer[i]->join();
					delete m_thread_buffer[i];
					m_thread_buffer[i] = nullptr;
				}
			}

			delete[] m_thread_buffer;
		}

		// add a new task to be processed
		// this will wake up all threads
		template <typename... Args>
		inline void add_task(void* func, Args... args)
		{
			m_taskQueue.emplace(func, args...);
			wake_threads();
		}

		inline size_t queue_size() const { return m_taskQueue.size(); }
		inline size_t thread_count() const { return m_thread_count; }

		// waits for all tasks to be completed
		void wait()
		{
			while (true)
			{
				std::unique_lock<std::mutex> lock(m_awake_thread_count_mutex);

				if (m_awake_thread_count == 0 && m_taskQueue.size() == 0)
					return;

				m_external_wait_cv.wait(lock);
			}
		}

	private:

		bool m_exit = false;

		static void thread_func(ThreadTaskManager* const _this)
		{
			_this->IncAwakeThreadCount();

			while (true)
			{
				if (_this->m_exit)
				{
					_this->DecAwakeThreadCount();
					return;
				}

				if (_this->queue_size() == 0)
				{
					std::unique_lock<std::mutex> lock(_this->m_sleep_mutex);
					_this->DecAwakeThreadCount();
					_this->m_external_wait_cv.notify_all();
					_this->m_cv.wait(lock);
					_this->IncAwakeThreadCount();
				}
				else
				{
					Internal::TaskBase* task = _this->m_taskQueue.pop();
					if (task)
					{
						task->run_func();
					}
					delete task;
				}
			}
		}

		Internal::TaskQueue m_taskQueue;

		size_t m_thread_count = 0;
		std::thread** m_thread_buffer = nullptr;

		std::mutex m_awake_thread_count_mutex;
		size_t m_awake_thread_count = 0;

		// thread safe wrapper
		size_t AwakeThreadCount()
		{
			std::unique_lock<std::mutex> lock(m_awake_thread_count_mutex);
			return m_awake_thread_count;
		}
		void IncAwakeThreadCount()
		{
			std::unique_lock<std::mutex> lock(m_awake_thread_count_mutex);
			m_awake_thread_count++;
		}
		void DecAwakeThreadCount()
		{
			std::unique_lock<std::mutex> lock(m_awake_thread_count_mutex);
			m_awake_thread_count--;
		}

		// indicated if threads should safely exit at next opportunity
		bool m_exit_threads = false;
		std::mutex m_exit_mutex;

		// used for allowing worker threads to sleep
		std::condition_variable m_cv;
		std::mutex m_sleep_mutex;

		// alloows main thread to sleep when calling wait() function
		std::condition_variable m_external_wait_cv;
		std::mutex m_external_wait_mutex;

		// do we really need this?
		inline void wake_threads()
		{
			m_cv.notify_all();
		}
	};
};
