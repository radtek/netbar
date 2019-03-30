
// netbarDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "netbar.h"
#include "netbarDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CHttpClient.h"

#include "md5.h"
#include "base64.h"

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
	CRect rc(0, 0, 0, 0);

	m_pNetbarWnd->Create(NULL, _T(""), WS_TABSTOP | WS_CHILD | WS_VISIBLE, rc, AfxGetApp()->m_pMainWnd, ID_NETBAR_TIMER_WND, 0);
}

void CNetbarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CNetbarDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
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

	m_pNetbarWnd->SetTimer(TIMER_GET_ORDERINFO, 500, NULL);

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

		CFont font;
		font.CreateFont(20, 0, 0, 0, 600,
			FALSE, FALSE, FALSE, 0, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, _T("Arial"));

		CFont* pOldFont = dc.SelectObject(&font);
		// 绘制我的
		ptBegin.x = rcDlg.left + 200;
		ptBegin.y = rcDlg.top;
		ptEnd.x = rcDlg.left + 200;
		ptEnd.y = rcDlg.bottom;
		DrawSplite(&dc, ptBegin, ptEnd);
		rcDraw.SetRect(rcDlg.left, ptBegin.y + 30, ptBegin.x, ptEnd.y);
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(_T("我的"), rcDraw, DT_CENTER | DT_VCENTER);
		
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

void CNetbarDlg::SetParam(const CString& strUsername, const CString& strPassword)
{
	m_strUsername = strUsername;
	m_strPassword = strPassword;

	int i;
	//unsigned char encrypt[] = "admin";//"admin";//21232f297a57a5a743894a0e4a801fc3
	unsigned char decrypt[16];

	std::string str = m_strPassword.GetBuffer(0);
	unsigned char *encrypt = (unsigned char *)str.c_str();

	MD5_CTX md5;
	MD5Init(&md5);
	MD5Update(&md5, encrypt, (int)strlen((char *)encrypt));//只是个中间步骤
 	MD5Final(&md5, decrypt);//32位

	// 得到密码的MD5加密
	CString strTmp;
	for (i = 0; i < 16; i++) {
		strTmp.Format(_T("%02x"), decrypt[i]);
		m_strPasswordMd5 += strTmp;
	}

	// 得到中间临时"用户名+md5密码"组合
	strTmp = m_strUsername + _T("+") + m_strPasswordMd5;
	
	// 转base64
	str = strTmp.GetBuffer(0);
	unsigned char *encrypt2 = (unsigned char *)str.c_str();

	std::string normal, encoded;
	int len = strTmp.GetLength();
	Base64 base;
	encoded = base.Encode(encrypt2, len);

	// 得到base64("用户名+md5密码")
	m_strU = encoded.c_str();
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

	strStaffUrl.Format(strStaffUrl, m_strU);
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
		/*if (ParseJson(strResponse) == 0)
		{

		}*/
	}

	return _T("");
}

// 请求订单数据
void CNetbarDlg::SendOrderRequest()
{
	
}

								
// 订单操作		// 接单 不接单
void CNetbarDlg::SendOrderOperate()
{

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
	// 请求
	if (nIDEvent == TIMER_GET_ORDERINFO)
	{
		m_pDlg->SendOrderRequest();
	}
}