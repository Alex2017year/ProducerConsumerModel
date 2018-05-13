#include <iostream>
#include <thread>
#include <list>
#include <mutex>
#include <condition_variable>
#include <string>
using namespace std;

struct NewJob {
	NewJob(int x = 0, int y = 0) : x(x), y(y) {  }
	int x, y;
};

//  一般要求临界区代码越短越好，执行时间越短越好，使用C++ STL可能并不是好选择
std::list<NewJob *>   newjob_queue;
mutex job_queue_mutex;

void  ProcessJob(NewJob * job)
{
	std::cout << "Thread " << this_thread::get_id();
	std::cout << " processing (" << job->x << ", " << job->y << ")\n";
}

//  处理作业时需要加锁
void DequeueJob()
{
	while (true) {
		NewJob *  job = NULL;
		unique_lock<mutex> locker(job_queue_mutex);
		this_thread::sleep_for(chrono::seconds(1));
		if (!newjob_queue.empty()) {
			job = newjob_queue.front();	//  获取表头元素
			newjob_queue.pop_front();	//  删除表头元素
		}
		locker.unlock();
		if (!job)    break;
		ProcessJob(job);
		delete job, job = NULL;
	}
}

//  作业入队时需要加锁
void EnqueueJob(NewJob* job)
{
	unique_lock<mutex> locker(job_queue_mutex);   //  锁定互斥
	newjob_queue.push_back(job);

	//  入队时也输出线程ID和作业内容信息
	std::cout << "Thread " << this_thread::get_id();
	std::cout << " enqueueing (" << job->x << ", " << job->y << ")\n";

	locker.unlock();    //  解锁
}


int mainMain()
{

	int  i;
	thread  threads[8];
	for (i = 0; i < 5; ++i)
	{
		NewJob *  job = new NewJob(i + 1, (i + 1) * 2);
		threads[i] = thread(EnqueueJob, job);
	}
	for (i = 5; i < 8; ++i)
		threads[i] = thread(DequeueJob);

	for (i = 0; i < 8; ++i)
		threads[i].join();

	system("pause");
	return 0;
}
