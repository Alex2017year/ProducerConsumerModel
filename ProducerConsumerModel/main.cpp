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

//  һ��Ҫ���ٽ�������Խ��Խ�ã�ִ��ʱ��Խ��Խ�ã�ʹ��C++ STL���ܲ����Ǻ�ѡ��
std::list<NewJob *>   newjob_queue;
mutex job_queue_mutex;

void  ProcessJob(NewJob * job)
{
	std::cout << "Thread " << this_thread::get_id();
	std::cout << " processing (" << job->x << ", " << job->y << ")\n";
}

//  ������ҵʱ��Ҫ����
void DequeueJob()
{
	while (true) {
		NewJob *  job = NULL;
		unique_lock<mutex> locker(job_queue_mutex);
		this_thread::sleep_for(chrono::seconds(1));
		if (!newjob_queue.empty()) {
			job = newjob_queue.front();	//  ��ȡ��ͷԪ��
			newjob_queue.pop_front();	//  ɾ����ͷԪ��
		}
		locker.unlock();
		if (!job)    break;
		ProcessJob(job);
		delete job, job = NULL;
	}
}

//  ��ҵ���ʱ��Ҫ����
void EnqueueJob(NewJob* job)
{
	unique_lock<mutex> locker(job_queue_mutex);   //  ��������
	newjob_queue.push_back(job);

	//  ���ʱҲ����߳�ID����ҵ������Ϣ
	std::cout << "Thread " << this_thread::get_id();
	std::cout << " enqueueing (" << job->x << ", " << job->y << ")\n";

	locker.unlock();    //  ����
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
