#pragma once

#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")

#define LOAD_SERVICE   0    // ���ط���
#define START_SERVICE  1    // ��������
#define STOP_SERVICE   2    // ֹͣ����
#define DELETE_SERVICE 3    // ɾ������


// ע���������
void RegeditRun(LPCWSTR exePath);

// ��������Ŀ¼����
void StartupRun(LPCWSTR exePath);

// ϵͳ�������
void SystemServiceOperate(LPWSTR pDriverPath, int operateType);


class MyTaskSchedule
{
public:
	MyTaskSchedule();
	~MyTaskSchedule();

private:
	ITaskService* m_pITS;
	ITaskFolder* m_pITF;

public:
	// ɾ��ָ������ƻ�
	void Delete(LPWSTR pTaskName);

	// ��������ƻ�
	void CreateTask(LPWSTR pTaskName, LPWSTR pFilePath);
};