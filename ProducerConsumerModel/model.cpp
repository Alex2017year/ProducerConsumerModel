#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <condition_variable>
#include <string>
using namespace std;

struct Job
{
	Job(int x = 0, int y = 0) : m_x(x), m_y(y) {}
	int m_x, m_y;
};

list<Job*> job_queue; // 可以采用缓存区
static int MaxCount = 20; // 队列最大容量
condition_variable condNotFull;
condition_variable condNotEmpty;
mutex mtx;

// 处理数据
void ProcessJob(Job* job)
{
	cout << "start to handle job..." << endl;
	cout << "Thread ID: " << this_thread::get_id();
	cout << " Processing (" << job->m_x << ", " << job->m_y << ")\n";
}

void ProductMsg(Job* job)
{
	unique_lock<mutex> locker(mtx);

	// 如果队列满了，那么就等待
	condNotFull.wait(locker, [] {return (size_t)MaxCount >= job_queue.size(); });
	this_thread::sleep_for(chrono::seconds(1));
	job_queue.push_back(job);
	cout << "Thread ID: " << this_thread::get_id();
	cout << " enqueueing (" << job->m_x << ", " << job->m_y << ")\n";
	locker.unlock();

	// 通知消费者
	condNotEmpty.notify_all();
}

void ConsumerMsg()
{
	while (true)
	{
		Job* job = NULL;
		unique_lock<mutex> locker(mtx);
		condNotEmpty.wait(locker, [] {return !job_queue.empty(); }); // 如果没有数据，就一直等待
		this_thread::sleep_for(chrono::seconds(1));
		job = job_queue.front();
		job_queue.pop_front();
		locker.unlock();

		if (!job) break; // 其实没作用
		ProcessJob(job);
		delete job, job = NULL;

		// 通知生产者进行生产
		condNotFull.notify_all();
	}
}

// 多生产者与多消费者模型
void test1()
{
	const int acoount = 18;
	// 多生产者与多消费者
	// 创建生产者线程
	std::thread  threads[acoount];
	for (int i = 0; i < 10; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ProductMsg, job);
	}

	// 创建消费者线程
	for (int i = 10; i < acoount; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ConsumerMsg);
	}

	//  基于区间的循环结构，对属于threads数组的所有元素t，执行循环体
	for (auto & t : threads)
		t.join();
}

// 单消费者，多生产者
void test2()
{
	const int acoount = 18;
	// 多生产者与多消费者
	// 创建生产者线程
	std::thread  threads[acoount];
	for (int i = 0; i < 17; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ProductMsg, job);
	}

	// 创建消费者线程
	for (int i = 17; i < acoount; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ConsumerMsg);
	}

	//  基于区间的循环结构，对属于threads数组的所有元素t，执行循环体
	for (auto & t : threads)
		t.join();
}



int main()
{
	test2();

	system("pause");
	return 0;
}