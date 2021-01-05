
// ProgaKursach v2.0.h: главный файл заголовка для приложения PROJECT_NAME
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CProgaKursachApp:
// Сведения о реализации этого класса: ProgaKursach v2.0.cpp
//

class CProgaKursachApp : public CWinApp
{
public:
	CProgaKursachApp();

// Переопределение
public:
	virtual BOOL InitInstance();

// Реализация

	DECLARE_MESSAGE_MAP()
};

extern CProgaKursachApp theApp;
