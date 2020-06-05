
#include "pch.h"
#include <iostream>
#include <Windows.h>
#include "CSceneManager.h"
#include "CObjectManager.h"
#include "CScreenBuffer.h"

CObjectManager CObjectManager::ObjectMng;
CScreenBuffer CScreenBuffer::ScreenBuff;
CSceneManager CSceneManager::SceneMng;

int main()
{
	bool EndTime = false;

	//*
	while (EndTime == false) {

		EndTime = CSceneManager::GetInstance()->Run();

		Sleep(100);
	}

	std::cout << std::endl << "Thanks for Play!" << std::endl;
	//*/
	return 0;
}