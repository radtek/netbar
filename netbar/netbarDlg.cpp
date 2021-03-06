
// netbarDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "netbar.h"
#include "netbarDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CUserInfo.h"

#include "global.h"
#include<mmsystem.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CNetbarDlg 对话框



CNetbarDlg::CNetbarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NETBAR_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_pNetbarWnd = new CNetbarWnd(this);
	m_pMusicWnd = new CMusicWnd(this);
	CRect rc(0, 0, 0, 0);

	m_pNetbarWnd->Create(NULL, _T(""), WS_TABSTOP | WS_CHILD | WS_VISIBLE, rc, AfxGetApp()->m_pMainWnd, ID_NETBAR_TIMER_WND, 0);
	m_pMusicWnd->Create(NULL, _T(""), WS_TABSTOP | WS_CHILD | WS_VISIBLE, rc, AfxGetApp()->m_pMainWnd, ID_MUSIC_TIMER_WND, 0);
	m_pOrderManager = new COrderManager(this);
}

void CNetbarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetbarDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CLOSE()
	ON_WM_MOVE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()


// CNetbarDlg 消息处理程序

BOOL CNetbarDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 请求网吧信息
	SendNetBarRequest();

	// 设置接单模式
	SendNetBarOnRecv();

	// 请求员工信息
	SendStaffRequest();

	// 请求订单信息
	//SendOrderRequest();

	// 启动定时器
	m_pNetbarWnd->SetTimer(TIMER_GET_ORDERINFO, 500, NULL);

	m_pMusicWnd->SetTimer(TIMER_MUSIC_ORDER, 500, NULL);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CNetbarDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CNetbarDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this); // 用于绘制的设备上下文
		CRect rcDlg;
		GetClientRect(rcDlg);
		
		// 绘制最右边
		CPoint ptBegin(rcDlg.right - 100, rcDlg.top);
		CPoint ptEnd(rcDlg.right - 100, rcDlg.bottom);

		DrawSplite(&dc, ptBegin, ptEnd);
		CRect rcDraw(rcDlg.right - 100, rcDlg.top, rcDlg.right, rcDlg.bottom);
		DrawIcon(&dc, rcDraw);

		m_rcNoMusic = rcDraw;

		CFont font;
		font.CreateFont(18, 0, 0, 0, 600,
			FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("Arial"));

		CFont* pOldFont = dc.SelectObject(&font);
		// 绘制我的
		ptBegin.x = rcDlg.left + 200;
		ptBegin.y = rcDlg.top;
		ptEnd.x = rcDlg.left + 200;
		ptEnd.y = rcDlg.bottom;
		DrawSplite(&dc, ptBegin, ptEnd);
		rcDraw.SetRect(rcDlg.left, ptBegin.y/* + 30*/, ptBegin.x, ptEnd.y);
		dc.SetBkMode(TRANSPARENT);

		CRect rcTmp(rcDraw);
		rcTmp.bottom = rcDraw.top + (rcTmp.Height()) / 2;
		dc.DrawText(_T("我的"), rcTmp, DT_CENTER | DT_VCENTER);
		m_rcMine = rcTmp;

		// 绘制横的一条
		ptBegin.x = rcDraw.left;
		ptBegin.y = rcDraw.top + rcDraw.Height() / 2;
		ptEnd.x = rcDraw.right;
		ptEnd.y = rcDraw.top + rcDraw.Height() / 2;
		DrawSplite(&dc, ptBegin, ptEnd);

		rcTmp.top = rcTmp.bottom;
		rcTmp.bottom = rcDraw.bottom;
		//
		CString strRecv = CUserInfoHolder::Instance()->GetNetBarStatus() ? _T("关闭接单") : _T("开启接单");
		dc.DrawText(strRecv, rcTmp, DT_CENTER | DT_VCENTER);
		m_rcNoRecv = rcTmp;

		// 绘制接单
		ptBegin.x = rcDraw.right + (rcDlg.Width() - 300) / 2;
		ptBegin.y = rcDlg.top;
		ptEnd.x = rcDraw.right + (rcDlg.Width() - 300) / 2;
		ptEnd.y = rcDlg.bottom;
		DrawSplite(&dc, ptBegin, ptEnd);
		rcDraw.SetRect(rcDraw.right, ptBegin.y + 30, ptBegin.x, ptEnd.y);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(_T("接单"), rcDraw, DT_CENTER | DT_VCENTER);

		// 绘制已接单
		ptBegin.x = rcDraw.right + (rcDlg.Width() - 300) / 2;
		rcDraw.SetRect(rcDraw.right, ptBegin.y + 30, ptBegin.x, ptEnd.y);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(_T("已接订单"), rcDraw, DT_CENTER | DT_VCENTER);


		ptBegin.x = rcDlg.left;
		ptBegin.y = rcDlg.top;
		ptEnd.x = rcDlg.right;
		ptEnd.y = rcDlg.top;
		DrawSplite(&dc, ptBegin, ptEnd);
		dc.SelectObject(pOldFont);
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CNetbarDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CNetbarDlg::DrawSplite(CDC* pDC, const CPoint ptBegin, const CPoint ptEnd)
{
	CPen pen(PS_SOLID, 2, RGB(200, 200, 200));//创建画笔对象
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(ptBegin);
	pDC->LineTo(ptEnd);
	pDC->SelectObject(pOldPen);
}

void CNetbarDlg::DrawIcon(CDC* pDC, const CRect& rcIcon)
{
	CBitmap btMap;
	btMap.LoadBitmap(MAKEINTRESOURCE(IDB_BITMAP_SOUND));
	
	CDC bmpCDC;
	bmpCDC.CreateCompatibleDC(pDC);
	bmpCDC.SelectObject(&btMap);

	CPoint pt;
	pt.x = rcIcon.left + 25;
	pt.y = rcIcon.top + 12;
	pDC->BitBlt(pt.x, pt.y, rcIcon.Width(), rcIcon.Height(), &bmpCDC, 0, 0, SRCCOPY);

	btMap.DeleteObject();
	ReleaseDC(&bmpCDC);
}

// 请求当前员工姓名
CString CNetbarDlg::SendStaffRequest()
{
	CString strStaffUrl;
	strStaffUrl.LoadString(IDS_STRING_STAFF);

	strStaffUrl.Format(strStaffUrl, CUserInfoHolder::Instance()->GetUrlParam());
	CHttpClient* pHttpClient = new CHttpClient;
	LPCTSTR pJsonPostData = _T("");
	CString strResponse;
	if (pHttpClient)
	{
		pHttpClient->HttpPost(strStaffUrl, pJsonPostData, strResponse);
	}

	// 解析json 如果成功，即可登录
	if (!strResponse.IsEmpty())
	{
		// 返回成功
		ParseStaffJson(strResponse);
	}

	if (pHttpClient)
	{
		delete pHttpClient;
		pHttpClient = NULL;
	}

	return _T("");
}

void CNetbarDlg::ParseStaffJson(const CString& strJson)
{
	CString str = strJson;
	// 解析订单数据，并且
	Json::Value jsonRoot;
	Json::CharReaderBuilder b;
	Json::CharReader* reader(b.newCharReader());

	JSONCPP_STRING errs;
	bool ok = reader->parse(str.GetBuffer(0), str.GetBuffer() + str.GetLength(), &jsonRoot, &errs);
	if (ok && errs.empty())
	{
		int nCount = jsonRoot["list"].size();
		for (int i = 0; i < nCount; i++)
		{
			CString str = UTF8ToGBK(jsonRoot["list"][i]["staff_id"].asCString());
			CUserInfoHolder::Instance()->SetStaffInfo(str);
		}
	}
}

void CNetbarDlg::SendNetBarRequest()
{
	CString strInfoUrl;
	strInfoUrl.LoadString(IDS_STRING_BARINFO);

	strInfoUrl.Format(strInfoUrl, CUserInfoHolder::Instance()->GetUrlParam());
	CHttpClient* pHttpClient = new CHttpClient;
	LPCTSTR pJsonPostData = _T("");
	CString strResponse;
	
	if (pHttpClient)
	{
		pHttpClient->HttpPost(strInfoUrl, pJsonPostData, strResponse);
	}

//	CString strResponse = _T("{\"content\":\"#\\u65b0\\u8ba2\\u5355\\u5217\\u8868\",\"message\":1,\"error\":0,\"list\":[{\"order_id\":\"295\",\"order_total\":\"2.40\",\"order_charge\":\"2.0\",\"order_hour\":\"2\",\"created_on\":\"03\/23\/2019 16:30\",\"member_fullname\":\"\\u3000\",\"member_mobile\":\"15728045775\",\"seat_name\":\"\\u4e00\\u697c\\u5927\\u5385\",\"seat_qty\":\"2\",\"additional\":\"\\u5fc5\\u987b\\u8fde\\u673a\",\"message\":\"test\",\"code\":\"\"}]}");
	// 解析json 如果成功，即可登录
	if (!strResponse.IsEmpty())
	{
		ParseNetInfoJson(strResponse);
		
		CString strInfo = _T("连机  ") + CUserInfoHolder::Instance()->GetNetBarInfo();
		SetWindowText(strInfo);
	}

	// http请求结束
	if (pHttpClient)
	{
		delete pHttpClient;
		pHttpClient = NULL;
	}
}

void CNetbarDlg::SendNetBarOnRecv()
{
	// http://api.ljgzh.com/account/orderreceive/on?u=eXdtZGRqZysyMTIxOGNjYTc3ODA0ZDJiYTE5MjJjMzNlMDE1MTEwNQ==
	CString strInfoUrl;
	strInfoUrl.LoadString(IDS_STRING_ONRECV);

	strInfoUrl.Format(strInfoUrl, CUserInfoHolder::Instance()->GetUrlParam());
	CHttpClient* pHttpClient = new CHttpClient;
	LPCTSTR pJsonPostData = _T("");
	CString strResponse;

	if (pHttpClient)
	{
		pHttpClient->HttpPost(strInfoUrl, pJsonPostData, strResponse);
	}

	//	CString strResponse = _T("{\"content\":\"#\\u65b0\\u8ba2\\u5355\\u5217\\u8868\",\"message\":1,\"error\":0,\"list\":[{\"order_id\":\"295\",\"order_total\":\"2.40\",\"order_charge\":\"2.0\",\"order_hour\":\"2\",\"created_on\":\"03\/23\/2019 16:30\",\"member_fullname\":\"\\u3000\",\"member_mobile\":\"15728045775\",\"seat_name\":\"\\u4e00\\u697c\\u5927\\u5385\",\"seat_qty\":\"2\",\"additional\":\"\\u5fc5\\u987b\\u8fde\\u673a\",\"message\":\"test\",\"code\":\"\"}]}");
	// 解析json 如果成功，即可登录
	if (!strResponse.IsEmpty())
	{
		CString strInfo = _T("连机  ") + CUserInfoHolder::Instance()->GetNetBarInfo() + _T("  状态：正在接单");
		SetWindowText(strInfo);
	}

	CUserInfoHolder::Instance()->SetNetBarStatus(TRUE);
	// http请求结束
	if (pHttpClient)
	{
		delete pHttpClient;
		pHttpClient = NULL;
	}
}

void CNetbarDlg::SendNetBarOffRecv()
{
	// http://api.ljgzh.com/account/orderreceive/off?u=eXdtZGRqZysyMTIxOGNjYTc3ODA0ZDJiYTE5MjJjMzNlMDE1MTEwNQ==
	CString strInfoUrl;
	strInfoUrl.LoadString(IDS_STRING_OFFRECV);

	strInfoUrl.Format(strInfoUrl, CUserInfoHolder::Instance()->GetUrlParam());
	CHttpClient* pHttpClient = new CHttpClient;
	LPCTSTR pJsonPostData = _T("");
	CString strResponse;

	if (pHttpClient)
	{
		pHttpClient->HttpPost(strInfoUrl, pJsonPostData, strResponse);
	}

	//	CString strResponse = _T("{\"content\":\"#\\u65b0\\u8ba2\\u5355\\u5217\\u8868\",\"message\":1,\"error\":0,\"list\":[{\"order_id\":\"295\",\"order_total\":\"2.40\",\"order_charge\":\"2.0\",\"order_hour\":\"2\",\"created_on\":\"03\/23\/2019 16:30\",\"member_fullname\":\"\\u3000\",\"member_mobile\":\"15728045775\",\"seat_name\":\"\\u4e00\\u697c\\u5927\\u5385\",\"seat_qty\":\"2\",\"additional\":\"\\u5fc5\\u987b\\u8fde\\u673a\",\"message\":\"test\",\"code\":\"\"}]}");
	// 解析json 如果成功，即可登录
	if (!strResponse.IsEmpty())
	{
		CString strInfo = _T("连机  ") + CUserInfoHolder::Instance()->GetNetBarInfo() + _T("  状态：停止接单");
		SetWindowText(strInfo);
	}

	// http请求结束
	if (pHttpClient)
	{
		delete pHttpClient;
		pHttpClient = NULL;
	}
}

void CNetbarDlg::ParseNetInfoJson(const CString& strJson)
{
	CString str = strJson;
	// 解析订单数据，并且
	Json::Value jsonRoot;
	Json::CharReaderBuilder b;
	Json::CharReader* reader(b.newCharReader());

	JSONCPP_STRING errs;
	bool ok = reader->parse(str.GetBuffer(0), str.GetBuffer() + str.GetLength(), &jsonRoot, &errs);
	if (ok && errs.empty())
	{
		CString str = UTF8ToGBK(jsonRoot["info"]["wname"].asCString());

		CUserInfoHolder::Instance()->SetNetBarInfo(str);
	}
}

// 请求订单数据
void CNetbarDlg::SendOrderRequest()
{
	CString strOrderUrl;
	strOrderUrl.LoadString(IDS_STRING_ORDER);

	strOrderUrl.Format(strOrderUrl, CUserInfoHolder::Instance()->GetUrlParam());
	CHttpClient* pHttpClient = new CHttpClient;
	LPCTSTR pJsonPostData = _T("");
	CString strResponse;
	 
	if (pHttpClient)
	{
		pHttpClient->HttpPost(strOrderUrl, pJsonPostData, strResponse);
	}
	
	if (pHttpClient)
	{
		delete pHttpClient;
		pHttpClient = NULL;
	}
	//CString strResponse = _T("{\"content\":\"#\\u65b0\\u8ba2\\u5355\\u5217\\u8868\",\"message\":1,\"error\":0,\"list\":[{\"order_id\":\"295\",\"order_total\":\"2.40\",\"order_charge\":\"2.0\",\"order_hour\":\"2\",\"created_on\":\"03\/23\/2019 16:30\",\"member_fullname\":\"\\u3000\",\"member_mobile\":\"15728045775\",\"seat_name\":\"\\u4e00\\u697c\\u5927\\u5385\",\"seat_qty\":\"2\",\"additional\":\"\\u5fc5\\u987b\\u8fde\\u673a\",\"message\":\"test\",\"code\":\"\"}]}");
	// 解析json 如果成功，即可登录
	if (!strResponse.IsEmpty())
	{
		ParseOrderJson(strResponse);
		ShowOrderInfo();
	}

//	UpdateOrderInfo();
}

void CNetbarDlg::ParseOrderJson(const CString& strJson)
{
	CString str = strJson;
	// 解析订单数据，并且
	Json::Value jsonRoot;
	Json::CharReaderBuilder b;
	Json::CharReader* reader(b.newCharReader());

	m_pOrderManager->ResetOrder();
	JSONCPP_STRING errs;
	bool ok = reader->parse(str.GetBuffer(0), str.GetBuffer() + str.GetLength(), &jsonRoot, &errs);
	if (ok && errs.empty())
	{
		int nCount = jsonRoot["list"].size();
		for (int i = 0; i < nCount; i++)
		{
			CString strOrderNum = UTF8ToGBK(jsonRoot["list"][i]["order_id"].asCString());
			// 检查是否已经是显示的订单
			if (m_pOrderManager->IsAlreadyInit(strOrderNum))
			{
				continue;
			}

			// 如果是新订单
			OrderInfo info;
			info.m_strOrderNum = strOrderNum;
			info.m_strMachineNum = UTF8ToGBK(jsonRoot["list"][i]["seat_qty"].asCString());
			info.m_strUseTimer = UTF8ToGBK(jsonRoot["list"][i]["order_hour"].asCString());
			info.m_strLocate = UTF8ToGBK(jsonRoot["list"][i]["seat_name"].asCString());
			info.m_strMessage = UTF8ToGBK(jsonRoot["list"][i]["message"].asCString());
			info.m_strAdditional = UTF8ToGBK(jsonRoot["list"][i]["additional"].asCString());

			// 插入新订单
			m_pOrderManager->InsertOrder(info);
		}		
	}
}
								
// 订单操作		// 接单 不接单
void CNetbarDlg::SendOrderOperate()
{

}

// 已接订单操作		// 
void CNetbarDlg::SendRecvOrderOperate()
{

}


void CNetbarDlg::ShowOrderInfo()
{
	if (m_pOrderManager)
	{
		m_pOrderManager->ShowOrderInfo();
		m_pOrderManager->ShowRecvOrderInfo();
		if (!m_pOrderManager->IsHasShowOrderInfo())
		{
			m_pOrderManager->SetNewOrder(FALSE);
			m_pMusicWnd->StopMusic();
		}
	}
}

void CNetbarDlg::InsertVcRecv(RecvInfo pInfo)
{
	m_pOrderManager->InsertRecvOrder(pInfo);
}

void CNetbarDlg::DeleteOrderInfo(const CString& strOrderNum)
{
	m_pOrderManager->DeleteOrder(strOrderNum);
}

void CNetbarDlg::UpdateOrderInfo()
{
	m_pOrderManager->UpdateOrderInfo();
}

void CNetbarDlg::DeleteRecvInfo(const CString& strOrderNum)
{
	m_pOrderManager->DeleteRecvInfo(strOrderNum);
}

void CNetbarDlg::UpdateRecvInfo()
{
	m_pOrderManager->UpdateRecvInfo();
}

void CNetbarDlg::SetRecvStatus(const CString& strOrderNum, DWORD dwStatus)
{
	m_pOrderManager->SetRecvStatus(strOrderNum, dwStatus);
}

void CNetbarDlg::SetOrderStatus(const CString& strOrderNum, DWORD dwStatus)
{
	m_pOrderManager->SetOrderStatus(strOrderNum, dwStatus);
}

BOOL CNetbarDlg::IsHasNewOrder()
{
	return m_pOrderManager->IsHasNewOrder();
}

void CNetbarDlg::SetNewOrder(BOOL bFlag)
{
	m_pOrderManager->SetNewOrder(bFlag);
}

// 重载onclose函数，不支持关闭，只能任务管理器关闭
void CNetbarDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	//CDialog::OnClose();
}

void CNetbarDlg::OnOK()
{
	// TODO: 在此添加专用代码和/或调用基类

	//CDialog::OnOK();
}

BOOL CNetbarDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		return TRUE;
	}
	else
	{
		return CDialog::PreTranslateMessage(pMsg);
	}
}

void CNetbarDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);

	CRect rcClient;
	GetClientRect(rcClient);
	ClientToScreen(rcClient);
	// TODO: 在此处添加消息处理程序代码
	m_pOrderManager->MoveWindow(rcClient);
}

void CNetbarDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (m_rcMine.PtInRect(point))
	{
		CString strURL;
		strURL.LoadString(IDS_STRING_BACKSTAGE);
		ShellExecute(AfxGetMainWnd()->GetSafeHwnd(), _T("open"), strURL, NULL, NULL, SW_SHOWNORMAL);
	}
	else if (m_rcNoRecv.PtInRect(point))
	{
// 		CHttpClient* pHttpClient = new CHttpClient;
// 		// 切换接单模式
// 		CString strURL;
// 		strURL.LoadString(IDS_STRING_SWITCHRECV);
// 		
// 		LPCTSTR pJsonPostData = _T("");
// 		CString strResponse;
// 		if (pHttpClient)
// 		{
// 			pHttpClient->HttpPost(strURL, pJsonPostData, strResponse);
// 		}
// 
// 		if (pHttpClient)
// 		{
// 			delete pHttpClient;
// 			pHttpClient = NULL;
// 		}
		if (CUserInfoHolder::Instance()->GetNetBarStatus())
		{
			SendNetBarOffRecv();
			CUserInfoHolder::Instance()->SetNetBarStatus(FALSE);
		}
		else
		{
			SendNetBarOnRecv();
			CUserInfoHolder::Instance()->SetNetBarStatus(TRUE);
		}
		InvalidateRect(m_rcNoRecv);
	}
	else if (m_rcNoMusic.PtInRect(point))
	{
		// 关闭音乐
		/*m_pMusicWnd->StopMusic();*/
		
		//m_pMusicWnd->KillTimer(TIMER_MUSIC_ORDER);
		//m_pMusicWnd->
		SetNewOrder(FALSE);
		m_pMusicWnd->StopMusic();
	}

	CDialog::OnLButtonUp(nFlags, point);
}

void CNetbarDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	BOOL bInRc = FALSE;
	if (m_rcMine.PtInRect(point))
	{
		bInRc = TRUE;
	}
	else if (m_rcNoRecv.PtInRect(point))
	{
		bInRc = TRUE;
	}
	else if (m_rcNoMusic.PtInRect(point))
	{
		bInRc = TRUE;
	}

	if (bInRc)
	{
		HCURSOR hCursor;
		hCursor = ::LoadCursor(NULL, IDC_HAND);
		SetCursor(hCursor);
	}

	CDialog::OnMouseHover(nFlags, point);
}



//////////////////////////////////////////////////////////////////////////
/// CNetBarWnd		定时请求类

BEGIN_MESSAGE_MAP(CNetbarWnd, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CNetbarWnd::CNetbarWnd(CNetbarDlg* pDlg)
{
	m_pDlg = pDlg;
}

CNetbarWnd::~CNetbarWnd()
{

}

void CNetbarWnd::OnTimer(UINT_PTR nIDEvent)
{
	// 请求订单信息
	if (nIDEvent == TIMER_GET_ORDERINFO)
	{
		m_pDlg->SendOrderRequest();
		//m_pDlg->ShowOrderInfo();		//test
	}
}
//////////////////////////////////////////////////////////////////////////
/// CMusicWnd
BEGIN_MESSAGE_MAP(CMusicWnd, CWnd)
	ON_WM_TIMER()
END_MESSAGE_MAP()

CMusicWnd::CMusicWnd(CNetbarDlg* pDlg)
{
	m_pDlg = pDlg;
	m_bStart = FALSE;
}

CMusicWnd::~CMusicWnd()
{

}

void CMusicWnd::OnTimer(UINT_PTR nIDEvent)
{
	// 有新订单播放
	if (nIDEvent == TIMER_MUSIC_ORDER)
	{
		if (m_pDlg->IsHasNewOrder() && m_bStart == FALSE)
		{
			m_bStart = TRUE;
			PlaySound(LPCTSTR(IDR_WAVE1), GetModuleHandle(NULL), SND_RESOURCE | SND_ASYNC | SND_LOOP);
		}
	}
}

void CMusicWnd::StopMusic()
{
	PlaySound(NULL, NULL, SND_FILENAME);
	m_bStart = FALSE;
}
