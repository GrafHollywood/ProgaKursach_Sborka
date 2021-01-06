
// ProgaKursachDlg.h: файл заголовка
//
#pragma once

// Диалоговое окно CProgaKursachDlg
class CProgaKursachDlg : public CDialogEx
{
// Создание
public:
	CProgaKursachDlg(CWnd* pParent = nullptr);	// стандартный конструктор

// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROGAKURSACH_DIALOG};
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// поддержка DDX/DDV


// Реализация
protected:
	HICON m_hIcon;

	// Созданные функции схемы сообщений
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Внешний диаметр муфты
	double m_dD1;
	// Внутренний диаметр муфты
	double m_dD2;
	// Длинна муфты
	double m_dL;
	//Фаска
	double m_dF;
	//Диаметры отверстий под крепления
	double m_dD1krep, m_dD2krep;
	CString m_sCombo;
	afx_msg void OnCbnSelchangeComboMoment();
	afx_msg void OnBnClickedButtonCreate();
	void CreateMufta();
	afx_msg void OnBnClickedButton1();
	void CreateVal();
	void CreateKreplenie();
	void CreateSborka();
};
