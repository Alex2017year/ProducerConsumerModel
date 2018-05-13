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

list<Job*> job_queue; // ���Բ��û�����
static int MaxCount = 20; // �����������
condition_variable condNotFull;
condition_variable condNotEmpty;
mutex mtx;

// ��������
void ProcessJob(Job* job)
{
	cout << "start to handle job..." << endl;
	cout << "Thread ID: " << this_thread::get_id();
	cout << " Processing (" << job->m_x << ", " << job->m_y << ")\n";
}

void ProductMsg(Job* job)
{
	unique_lock<mutex> locker(mtx);

	// ����������ˣ���ô�͵ȴ�
	condNotFull.wait(locker, [] {return (size_t)MaxCount >= job_queue.size(); });
	this_thread::sleep_for(chrono::seconds(1));
	job_queue.push_back(job);
	cout << "Thread ID: " << this_thread::get_id();
	cout << " enqueueing (" << job->m_x << ", " << job->m_y << ")\n";
	locker.unlock();

	// ֪ͨ������
	condNotEmpty.notify_all();
}

void ConsumerMsg()
{
	while (true)
	{
		Job* job = NULL;
		unique_lock<mutex> locker(mtx);
		condNotEmpty.wait(locker, [] {return !job_queue.empty(); }); // ���û�����ݣ���һֱ�ȴ�
		this_thread::sleep_for(chrono::seconds(1));
		job = job_queue.front();
		job_queue.pop_front();
		locker.unlock();

		if (!job) break; // ��ʵû����
		ProcessJob(job);
		delete job, job = NULL;

		// ֪ͨ�����߽�������
		condNotFull.notify_all();
	}
}

// �����������������ģ��
void test1()
{
	const int acoount = 18;
	// �����������������
	// �����������߳�
	std::thread  threads[acoount];
	for (int i = 0; i < 10; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ProductMsg, job);
	}

	// �����������߳�
	for (int i = 10; i < acoount; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ConsumerMsg);
	}

	//  ���������ѭ���ṹ��������threads���������Ԫ��t��ִ��ѭ����
	for (auto & t : threads)
		t.join();
}

// �������ߣ���������
void test2()
{
	const int acoount = 18;
	// �����������������
	// �����������߳�
	std::thread  threads[acoount];
	for (int i = 0; i < 17; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ProductMsg, job);
	}

	// �����������߳�
	for (int i = 17; i < acoount; i++)
	{
		Job* job = new Job(i + 1, (i + 1) * 3);
		threads[i] = thread(ConsumerMsg);
	}

	//  ���������ѭ���ṹ��������threads���������Ԫ��t��ִ��ѭ����
	for (auto & t : threads)
		t.join();
}



int main()
{
	test2();

	system("pause");
	return 0;
}