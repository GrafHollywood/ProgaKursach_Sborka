
// ProgaKursachDlg.cpp: файл реализации
//

#include "pch.h"
#include "framework.h"
#include "ProgaKursach v2.0.h"
#include "ProgaKursachDlg.h"
#include "afxdialogex.h"

#define PATH_MUFTA "D:\\sborka\\Муфта.ipt"
#define PATH_VAL "D:\\sborka\\Вал.ipt"
#define PATH_KREPLENIE "D:\\sborka\\Крепление.ipt"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <atlbase.h>
#import <D:\Program Files\Autodesk\Inventor 2021\Bin\RxInventor.tlb> \
rename_namespace("InventorNative") \
named_guids raw_dispinterfaces \
high_method_prefix("Method") \
rename("DeleteFile", "APIDeleteFile") \
rename("CopyFile", "APICopyFile") \
rename("MoveFile", "APIMoveFile") \
rename("SetEnvironmentVariable", "APISetEnvironmentVariable") \
rename("GetObject", "APIGetObject") \
exclude("_FILETIME", "IStream", "ISequentialStream", \
"_LARGE_INTEGER", "_ULARGE_INTEGER", "tagSTATSTG", \
"IEnumUnknown", "IPersistFile", "IPersist", "IPictureDisp")

using namespace InventorNative;

CComPtr<Application> pInvApp;
//InventorNative::DocumentPtr pPartDoc;
PartDocumentPtr pPartDoc;
PartComponentDefinition* pPartComDef;
TransientGeometry* pTransGeom;
Transaction* pTrans;

PartDocumentPtr pPartDoc1;   //создаем сборку
PartComponentDefinition* pPartComDef1;
TransientGeometry* pTransGeom1;
Transaction* pTrans1;

AssemblyDocumentPtr pAsmDoc;
AssemblyComponentDefinitionPtr pAsmComDef;
TransientGeometry* pTransGeomAsm;
Matrix* oPositionMatrix;

TransactionManagerPtr pTransManager;
Document* Doc;

PlanarSketches* sketches;
Sketches3D* sketches3D;
WorkPlanes* wp;
PartFeatures* ft;
WorkAxes* wax;
WorkPoints* wpt;
// Диалоговое окно CAboutDlg используется для описания сведений о приложении

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Данные диалогового окна
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // поддержка DDX/DDV

// Реализация
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Диалоговое окно CProgaKursachDlg



CProgaKursachDlg::CProgaKursachDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROGAKURSACH_DIALOG, pParent)
	, m_dD1(0)
	, m_dD2(0)
	, m_dL(0)
	, m_sCombo(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProgaKursachDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_D1, m_dD1);
	DDX_Text(pDX, IDC_D2, m_dD2);
	DDX_Text(pDX, IDC_L, m_dL);
	DDX_CBString(pDX, IDC_COMBO_MOMENT, m_sCombo);
}

BEGIN_MESSAGE_MAP(CProgaKursachDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_MOMENT, &CProgaKursachDlg::OnCbnSelchangeComboMoment)
	ON_BN_CLICKED(IDC_BUTTON_CREATE, &CProgaKursachDlg::OnBnClickedButtonCreate)
	//ON_BN_CLICKED(IDC_BUTTON1, &CProgaKursachDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// Обработчики сообщений CProgaKursachDlg

BOOL CProgaKursachDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Добавление пункта "О программе..." в системное меню.

	// IDM_ABOUTBOX должен быть в пределах системной команды.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Задает значок для этого диалогового окна.  Среда делает это автоматически,
	//  если главное окно приложения не является диалоговым
	SetIcon(m_hIcon, TRUE);			// Крупный значок
	SetIcon(m_hIcon, FALSE);		// Мелкий значок

	// TODO: добавьте дополнительную инициализацию

	return TRUE;  // возврат значения TRUE, если фокус не передан элементу управления
}

void CProgaKursachDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// При добавлении кнопки свертывания в диалоговое окно нужно воспользоваться приведенным ниже кодом,
//  чтобы нарисовать значок.  Для приложений MFC, использующих модель документов или представлений,
//  это автоматически выполняется рабочей областью.

void CProgaKursachDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // контекст устройства для рисования

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Выравнивание значка по центру клиентского прямоугольника
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Нарисуйте значок
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// Система вызывает эту функцию для получения отображения курсора при перемещении
//  свернутого окна.
HCURSOR CProgaKursachDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProgaKursachDlg::OnCbnSelchangeComboMoment()
{
	// TODO: добавьте свой код обработчика уведомлений
	UpdateData(TRUE);
	if (m_sCombo == "1")
	{
		m_dD1 = 6;
		m_dD2 = 10;
		m_dL = 25;
		m_dF = 0.5;
	}
	else if (m_sCombo == "2")
	{
		m_dD1 = 7;
		m_dD2 = 14;
		m_dL = 30;
		m_dF = 0.5;
	}
	else if (m_sCombo == "4")
	{
		m_dD1 = 9;
		m_dD2 = 16;
		m_dL = 35;
		m_dF = 0.5;
	}
	else if (m_sCombo == "8")
	{
		m_dD1 = 11;
		m_dD2 = 18;
		m_dL = 40;
		m_dF = 1;
	}
	else if (m_sCombo == "16")
	{
		m_dD1 = 14;
		m_dD2 = 28;
		m_dL = 45;
		m_dF = 1;
	}
	else if (m_sCombo == "31,5")
	{
		m_dD1 = 18;
		m_dD2 = 32;
		m_dL = 55;
		m_dF = 1;
	}
	else if (m_sCombo == "50")
	{
		m_dD1 = 20;
		m_dD2 = 38;
		m_dL = 65;
		m_dF = 1;
	}
	else if (m_sCombo == "90")
	{
		m_dD1 = 25;
		m_dD2 = 42;
		m_dL = 75;
		m_dF = 1;
	}
	else if (m_sCombo == "125")
	{
		m_dD1 = 28;
		m_dD2 = 48;
		m_dL = 90;
		m_dF = 1;
	}
	else if (m_sCombo == "200")
	{
		m_dD1 = 32;
		m_dD2 = 55;
		m_dL = 105;
		m_dF = 1;
	}

	UpdateData(FALSE);
	m_dD1krep = m_dD1 * 0.7;
	m_dD2krep = m_dD1 * 0.6;

	m_dD1 /= 10;
	m_dD2 /= 10;
	m_dL /= 10;
	m_dF /= 10;
	m_dD1krep /= 10;
	m_dD2krep /= 10;
}


void CProgaKursachDlg::OnBnClickedButtonCreate()
{
	// TODO: добавьте свой код обработчика уведомлений
	if (m_dD1 == 0 || m_dD2==0 || m_dL==0)
	{
		return;
	}
	if (!pInvApp)
	{
		// Get hold of the program id of Inventor.
		CLSID InvAppClsid;
		HRESULT hRes = CLSIDFromProgID(L"Inventor.Application", &InvAppClsid);
		if (FAILED(hRes)) {
			pInvApp = nullptr;
			return;// ReturnAndShowCOMError(hRes, L"ConnectToInventor,CLSIDFromProgID failed");
		}

		// See if Inventor is already running...
		CComPtr<IUnknown> pInvAppUnk = nullptr;
		hRes = ::GetActiveObject(InvAppClsid, NULL, &pInvAppUnk);
		if (FAILED(hRes)) {
			// Inventor is not already running, so try to start it...
			TRACE(L"Could not get hold of an active Inventor, will start a new session\n");
			hRes = CoCreateInstance(InvAppClsid, NULL, CLSCTX_LOCAL_SERVER, __uuidof(IUnknown), (void**)&pInvAppUnk);
			if (FAILED(hRes)) {
				pInvApp = nullptr;
				return;
			}
		}

		// Get the pointer to the Inventor application...
		hRes = pInvAppUnk->QueryInterface(__uuidof(Application), (void**)&pInvApp);
		if (FAILED(hRes)) {
			return;
		}
	}
	pInvApp->put_Visible(TRUE);

	CreateMufta();
	CreateVal();
	CreateKreplenie();

	CreateSborka();

	MessageBox(L"Сборка создана");
}


void CProgaKursachDlg::CreateMufta()
{
	// TODO: Добавьте сюда код реализации.
	pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

	pPartDoc->DisplayName = _T("Муфта");
	pPartDoc->get_ComponentDefinition(&pPartComDef);
	pInvApp->get_TransientGeometry(&pTransGeom);
	pTransManager = pInvApp->GetTransactionManager();
	Document* Doc = CComQIPtr <Document>(pPartDoc);
	pTransManager->raw_StartTransaction(Doc, _T("Муфта"), &pTrans);
	pPartComDef->get_Sketches(&sketches);

	pPartComDef->get_WorkPlanes(&wp);
	pPartComDef->get_Features(&ft);
	pPartComDef->get_WorkAxes(&wax);
	pPartComDef->get_WorkPoints(&wpt);

	/*делаем вращение*/
	PlanarSketch* pSketch;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch);

	SketchPoints* skPoints;
	SketchLines* skLines;
	Profiles* skProfiles;
	pSketch->get_SketchPoints(&skPoints);
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);

	SketchPointPtr point[4];
	SketchLinePtr lines[4];

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 2, m_dD2 / 2), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 2, m_dD1 / 2), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 2, m_dD1 / 2), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 2, m_dD2 / 2), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	Profile* pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures* ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);
	RevolveFeaturePtr revolve1 = ftRevolve->MethodAddFull(pProfile, wax->GetItem(1), kJoinOperation);

	//создание фасок
	EdgeCollection* edgeColl;
	pInvApp->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);
	SurfaceBody* SurfBody; //тело
	SurfaceBodies* SurfBodies; //коллекция тел
	pPartComDef->get_SurfaceBodies(&SurfBodies);
	SurfBodies->get_Item(1, &SurfBody);
	Edges* edges;
	SurfBody->get_Edges(&edges);

	Edge* ed;
	edges->get_Item(1, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(2, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(3, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(4, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeatures* pChamfer;
	ft->get_ChamferFeatures(&pChamfer);

	ChamferFeaturesPtr chamfer = pChamfer->MethodAddUsingDistance(edgeColl, m_dF, false, false, false);

	//делаем первое отверстие под крепление
	PlanarSketch* pSketch2;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch2);

	pSketch2->get_SketchPoints(&skPoints);
	pSketch2->get_SketchLines(&skLines);
	pSketch2->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4, m_dD2 / 2), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4, -m_dD2 / 2), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4 + m_dD2krep / 2, -m_dD2 / 2), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4 + m_dD1krep / 2, m_dD2 / 2), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeaturePtr revolve2 = ftRevolve->MethodAddFull(pProfile, lines[0], kCutOperation);

	//делаем второе отверстие под крепление
	PlanarSketch* pSketch3;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch3);

	pSketch3->get_SketchPoints(&skPoints);
	pSketch3->get_SketchLines(&skLines);
	pSketch3->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 4, m_dD2 / 2), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 4, -m_dD2 / 2), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 4 + m_dD2krep / 2, -m_dD2 / 2), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 4 + m_dD1krep / 2, m_dD2 / 2), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeaturePtr revolve3 = ftRevolve->MethodAddFull(pProfile, lines[0], kCutOperation);

	pTrans->MethodEnd();
	pPartDoc->MethodSaveAs(PATH_MUFTA, true); //сохранение детали
}

void CProgaKursachDlg::CreateVal()
{
	// TODO: Добавьте сюда код реализации.
	pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

	pPartDoc->DisplayName = _T("Вал");
	pPartDoc->get_ComponentDefinition(&pPartComDef);
	pInvApp->get_TransientGeometry(&pTransGeom);
	pTransManager = pInvApp->GetTransactionManager();
	Document* Doc = CComQIPtr <Document>(pPartDoc);
	pTransManager->raw_StartTransaction(Doc, _T("Вал"), &pTrans);
	pPartComDef->get_Sketches(&sketches);

	pPartComDef->get_WorkPlanes(&wp);
	pPartComDef->get_Features(&ft);
	pPartComDef->get_WorkAxes(&wax);
	pPartComDef->get_WorkPoints(&wpt);

	/*делаем вращение*/
	PlanarSketch* pSketch;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch);

	SketchPoints* skPoints;
	SketchLines* skLines;
	Profiles* skProfiles;
	pSketch->get_SketchPoints(&skPoints);
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);

	SketchPointPtr point[4];
	SketchLinePtr lines[4];

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 2, 0), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 2, m_dD1 / 2), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 2, m_dD1 / 2), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(-m_dL / 2, 0), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	Profile* pProfile;
	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeatures* ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);
	RevolveFeaturePtr revolve1 = ftRevolve->MethodAddFull(pProfile, wax->GetItem(1), kJoinOperation);

	//создание фасок
	EdgeCollection* edgeColl;
	pInvApp->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);
	SurfaceBody* SurfBody; //тело
	SurfaceBodies* SurfBodies; //коллекция тел
	pPartComDef->get_SurfaceBodies(&SurfBodies);
	SurfBodies->get_Item(1, &SurfBody);
	Edges* edges;
	SurfBody->get_Edges(&edges);

	Edge* ed;
	edges->get_Item(1, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(2, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeatures* pChamfer;
	ft->get_ChamferFeatures(&pChamfer);

	ChamferFeaturesPtr chamfer = pChamfer->MethodAddUsingDistance(edgeColl, m_dF, false, false, false);

	//создание отверстия под крепление
	PlanarSketch* pSketch2;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch2);

	pSketch2->get_SketchPoints(&skPoints);
	pSketch2->get_SketchLines(&skLines);
	pSketch2->get_Profiles(&skProfiles);

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4 + 0.1, m_dD2 / 2), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4 + 0.1, -m_dD2 / 2), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4 + 0.1 + m_dD2krep / 2, -m_dD2 / 2), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(m_dL / 4 + 0.1 + m_dD1krep / 2, m_dD2 / 2), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);

	skProfiles->raw__AddForSolid(&pProfile);

	RevolveFeaturePtr revolve2 = ftRevolve->MethodAddFull(pProfile, lines[0], kCutOperation);

	pTrans->MethodEnd();
	pPartDoc->MethodSaveAs(PATH_VAL, true); //сохранение детали
}

void CProgaKursachDlg::CreateKreplenie()
{
	// TODO: Добавьте сюда код реализации.
	pPartDoc = pInvApp->Documents->MethodAdd(kPartDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kPartDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);

	pPartDoc->DisplayName = _T("Крепление");
	pPartDoc->get_ComponentDefinition(&pPartComDef);
	pInvApp->get_TransientGeometry(&pTransGeom);
	pTransManager = pInvApp->GetTransactionManager();
	Document* Doc = CComQIPtr <Document>(pPartDoc);
	pTransManager->raw_StartTransaction(Doc, _T("Крепление"), &pTrans);
	pPartComDef->get_Sketches(&sketches);

	pPartComDef->get_WorkPlanes(&wp);
	pPartComDef->get_Features(&ft);
	pPartComDef->get_WorkAxes(&wax);
	pPartComDef->get_WorkPoints(&wpt);

	/*делаем вращение*/
	PlanarSketch* pSketch;
	sketches->raw_Add(wp->GetItem(3), false, &pSketch);

	SketchPoints* skPoints;
	SketchLines* skLines;
	Profiles* skProfiles;
	pSketch->get_SketchPoints(&skPoints);
	pSketch->get_SketchLines(&skLines);
	pSketch->get_Profiles(&skProfiles);

	SketchPointPtr point[4];
	SketchLinePtr lines[4];

	point[0] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, m_dD2 / 2), false);
	point[1] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0, -m_dD2 / 2), false);
	point[2] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0 + m_dD2krep / 2, -m_dD2 / 2), false);
	point[3] = skPoints->MethodAdd(pTransGeom->MethodCreatePoint2d(0 + m_dD1krep / 2, m_dD2 / 2), false);

	lines[0] = skLines->MethodAddByTwoPoints(point[0], point[1]);
	lines[1] = skLines->MethodAddByTwoPoints(point[1], point[2]);
	lines[2] = skLines->MethodAddByTwoPoints(point[2], point[3]);
	lines[3] = skLines->MethodAddByTwoPoints(point[3], point[0]);
	
	Profile* pProfile;
	skProfiles->raw__AddForSolid(&pProfile);
	
	RevolveFeatures* ftRevolve;
	ft->get_RevolveFeatures(&ftRevolve);
	RevolveFeaturePtr revolve1 = ftRevolve->MethodAddFull(pProfile, lines[0], kJoinOperation);

	//создание фасок
	EdgeCollection* edgeColl;
	pInvApp->TransientObjects->raw_CreateEdgeCollection(vtMissing, &edgeColl);
	SurfaceBody* SurfBody; //тело
	SurfaceBodies* SurfBodies; //коллекция тел
	pPartComDef->get_SurfaceBodies(&SurfBodies);
	SurfBodies->get_Item(1, &SurfBody);
	Edges* edges;
	SurfBody->get_Edges(&edges);

	Edge* ed;
	edges->get_Item(1, &ed);
	edgeColl->MethodAdd(ed);
	edges->get_Item(2, &ed);
	edgeColl->MethodAdd(ed);

	ChamferFeatures* pChamfer;
	ft->get_ChamferFeatures(&pChamfer);

	ChamferFeaturesPtr chamfer = pChamfer->MethodAddUsingDistance(edgeColl, m_dF, false, false, false);
	
	pTrans->MethodEnd();
	pPartDoc->MethodSaveAs(PATH_KREPLENIE, true); //сохранение детали
}

void CProgaKursachDlg::CreateSborka()
{
	// TODO: Добавьте сюда код реализации.
	pAsmDoc = pInvApp->Documents->MethodAdd(kAssemblyDocumentObject, pInvApp->FileManager->MethodGetTemplateFile(kAssemblyDocumentObject, kMetricSystemOfMeasure, kGOST_DraftingStandard), true);
	pAsmDoc->DisplayName = _T("Муфта глухая втулочная исп1");
	pAsmDoc->get_ComponentDefinition(&pAsmComDef);
	pInvApp->get_TransientGeometry(&pTransGeomAsm);
	pTransGeomAsm->raw_CreateMatrix(&oPositionMatrix);
	pPartComDef->get_WorkAxes(&wax);
	pPartComDef->get_WorkPoints(&wpt);

	ComponentOccurrencePtr modelMufta, modelVal1, modelVal2, modelKreplenie1, modelKreplenie2;
	modelMufta = pAsmDoc->ComponentDefinition->Occurrences->MethodAdd(PATH_MUFTA, oPositionMatrix);
	modelVal1 = pAsmDoc->ComponentDefinition->Occurrences->MethodAdd(PATH_VAL, oPositionMatrix);
	oPositionMatrix->MethodSetToRotateTo(pTransGeomAsm->MethodCreateVector(0, 1, 0), pTransGeomAsm->MethodCreateVector(0, -1, 0), pTransGeomAsm->MethodCreateVector(0, 0, 1)); //поворот для второго вала
	oPositionMatrix->MethodSetToRotateTo(pTransGeomAsm->MethodCreateVector(0, 0, 1), pTransGeomAsm->MethodCreateVector(0, 0, -1), pTransGeomAsm->MethodCreateVector(0, 1, 0));
	modelVal2 = pAsmDoc->ComponentDefinition->Occurrences->MethodAdd(PATH_VAL, oPositionMatrix);
	
	modelKreplenie1 = pAsmDoc->ComponentDefinition->Occurrences->MethodAdd(PATH_KREPLENIE, oPositionMatrix);
	modelKreplenie2 = pAsmDoc->ComponentDefinition->Occurrences->MethodAdd(PATH_KREPLENIE, oPositionMatrix);


	SurfaceBody *SurfBodyMufta, *SurfBodyVal1, *SurfBodyVal2, *SurfBodyKreplenie1, *SurfBodyKreplenie2;
	SurfaceBodies *SurfBodiesMufta, *SurfBodiesVal1, *SurfBodiesVal2, *SurfBodiesKreplenie1, *SurfBodiesKreplenie2;
	Faces* oFacesMufta, * oFacesVal1, * oFacesVal2, *oFacesKreplenie1, *oFacesKreplenie2;
	FacePtr oFaceMufta, oFaceVal1, oFaceVal2, oFaceKreplenie1, oFaceKreplenie2;

	modelMufta->get_SurfaceBodies(&SurfBodiesMufta);
	SurfBodiesMufta->get_Item(1, &SurfBodyMufta);	
	SurfBodyMufta->get_Faces(&oFacesMufta);
	
	modelVal1->get_SurfaceBodies(&SurfBodiesVal1);
	SurfBodiesVal1->get_Item(1, &SurfBodyVal1);
	SurfBodyVal1->get_Faces(&oFacesVal1);

	modelVal2->get_SurfaceBodies(&SurfBodiesVal2);
	SurfBodiesVal2->get_Item(1, &SurfBodyVal2);
	SurfBodyVal2->get_Faces(&oFacesVal2);
	
	modelKreplenie1->get_SurfaceBodies(&SurfBodiesKreplenie1);
	SurfBodiesKreplenie1->get_Item(1, &SurfBodyKreplenie1);
	SurfBodyKreplenie1->get_Faces(&oFacesKreplenie1);
	
	modelKreplenie2->get_SurfaceBodies(&SurfBodiesKreplenie2);
	SurfBodiesKreplenie2->get_Item(1, &SurfBodyKreplenie2);
	SurfBodyKreplenie2->get_Faces(&oFacesKreplenie2);
	
	MateConstraintPtr sPlane;

	//совмещаем вал1 по оси с муфтой
	oFacesMufta->get_Item(6, &oFaceMufta);
	oFacesVal1->get_Item(3, &oFaceVal1);
	sPlane = pAsmComDef->Constraints->MethodAddMateConstraint(oFaceMufta, oFaceVal1, 0, kInferredLine, kInferredLine);
	
	//совмещаем вал2 по оси с муфтой
	oFacesMufta->get_Item(6, &oFaceMufta);
	oFacesVal2->get_Item(3, &oFaceVal2);
	sPlane = pAsmComDef->Constraints->MethodAddMateConstraint(oFaceMufta, oFaceVal2, 0, kInferredLine, kInferredLine);

	//совмещаем вал1 по оси с креплением муфты
	oFacesMufta->get_Item(2, &oFaceMufta);
	oFacesVal1->get_Item(1, &oFaceVal1);
	sPlane = pAsmComDef->Constraints->MethodAddMateConstraint(oFaceMufta, oFaceVal1, 0, kInferredLine, kInferredLine);

	//совмещаем вал2 по оси с креплением муфты
	oFacesMufta->get_Item(4, &oFaceMufta);
	oFacesVal2->get_Item(1, &oFaceVal2);
	sPlane = pAsmComDef->Constraints->MethodAddMateConstraint(oFaceMufta, oFaceVal2, 0, kInferredLine, kInferredLine);

	//совмещаем крепление1 по оси с муфтой
	oFacesMufta->get_Item(2, &oFaceMufta);
	oFacesKreplenie1->get_Item(2, &oFaceKreplenie1);
	sPlane = pAsmComDef->Constraints->MethodAddMateConstraint(oFaceMufta, oFaceKreplenie1, 0, kInferredLine, kInferredLine);
	sPlane = pAsmComDef->Constraints->MethodAddTangentConstraint(oFaceMufta, oFaceKreplenie1, true, 0);

	//совмещаем крепление2 по оси с муфтой
	oFacesMufta->get_Item(4, &oFaceMufta);
	oFacesKreplenie2->get_Item(2, &oFaceKreplenie2);
	sPlane = pAsmComDef->Constraints->MethodAddMateConstraint(oFaceMufta, oFaceKreplenie2, 0, kInferredLine, kInferredLine);
	sPlane = pAsmComDef->Constraints->MethodAddTangentConstraint(oFaceMufta, oFaceKreplenie2, true, 0);
}

void CProgaKursachDlg::OnBnClickedButton1()
{
	SelectSet* pSelect;
	if (pPartComDef == NULL) //проверка на то что деталь построена
	{
		MessageBox(_T("Деталь еще не построена"));
		return;
	}
	pPartDoc->get_SelectSet(&pSelect);       // SelectSet->Count();


	if (pSelect->GetCount() > 0)
	{
		for (int c = 1; c <= pSelect->GetCount(); c++)
		{
			FacePtr Seekface = pSelect->GetItem(c);
			//EdgePtr Seekedge = pSelect->GetItem(c);

			for (int i = 1; i <= pPartComDef->SurfaceBodies->GetCount(); i++)
			{
				SurfaceBody* SurfBody;
				SurfaceBodies* SurfBodies;

				pPartComDef->get_SurfaceBodies(&SurfBodies);

				SurfBodies->get_Item(i, &SurfBody);

				//Edge* edge;
				//Edges* edges;

				Face* face;
				Faces* faces;

				//SurfBody->get_Edges(&edges);
				SurfBody->get_Faces(&faces);

				for (int j = 1; j <= SurfBody->Faces->GetCount(); j++)
				{
					faces->get_Item(j, &face);

					if (Seekface == face)
					{
						CString str;
						str.Format(L"Выделенное Face имеет #%i", j);
						MessageBox(str);
						break;

					}
				}

			}
		}
	}
	else
	{
		MessageBox(_T("Выделите ребра на детали"));
	}
}