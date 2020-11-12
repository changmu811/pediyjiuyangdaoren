#pragma once

#include <taskschd.h>
#pragma comment(lib, "taskschd.lib")

#define LOAD_SERVICE   0    // 加载服务
#define START_SERVICE  1    // 启动服务
#define STOP_SERVICE   2    // 停止服务
#define DELETE_SERVICE 3    // 删除服务


// 注册表自启动
void RegeditRun(LPCWSTR exePath);

// 快速启动目录启动
void StartupRun(LPCWSTR exePath);

// 系统服务操作
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
	// 删除指定任务计划
	void Delete(LPWSTR pTaskName);

	// 创建任务计划
	void CreateTask(LPWSTR pTaskName, LPWSTR pFilePath);
};