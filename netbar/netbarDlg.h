
// netbarDlg.h: 头文件
//

#pragma once
#include "COrderDlg.h"
#include "CRecviceOrder.h"
#include "CMusicPlayer.h"


class CNetbarWnd;
class CMusicWnd;
class COrderManager;
// CNetbarDlg 对话框
class CNetbarDlg : public CDialog
{
// 构造
public:
	CNetbarDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NETBAR_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

// 绘制
protected:
	void DrawSplite(CDC* pDC, const CPoint ptBegin, const CPoint ptEnd);
	void DrawIcon(CDC* pDC, const CRect& rcIcon);
// 获取数据
protected:
	CString SendStaffRequest();
	void ParseStaffJson(const CString& strJson);

// 处理数据
	void ParseOrderJson(const CString& strJson);			// 处理请求返回的订单数据
	void ParseNetInfoJson(const CString& strJson);			// 处理请求返回的网吧信息
// 请求数据
public:
	void SendNetBarRequest();
	void SendNetBarOnRecv();		// 设置接单模式
	void SendNetBarOffRecv();		// 设置不接单模式

	void SendOrderRequest();		// 请求订单数据
	void SendOrderOperate();		// 订单操作		// 接单 不接单

	
	void SendRecvOrderOperate();	// 已接订单操作	// 到店退款 过期不候

	void ShowOrderInfo();
	void InsertVcRecv(RecvInfo pInfo);
	void DeleteOrderInfo(const CString& strOrderNum);
	void UpdateOrderInfo();
	void SetOrderStatus(const CString& strOrderNum, DWORD dwStatus);		// 订单状态修改
	BOOL IsHasNewOrder();			//判断是否有新订单
	void SetNewOrder(BOOL bFlag);

	// 已经接单的操作
	void DeleteRecvInfo(const CString& strOrderNum);
	void UpdateRecvInfo();
	void SetRecvStatus(const CString& strOrderNum, DWORD dwStatus);										// 已经接单状态修改
	

	
private:

	CNetbarWnd*				m_pNetbarWnd;		// 定时请求
	CMusicWnd*				m_pMusicWnd;		// 定时新订单音效
	COrderManager*			m_pOrderManager;	// 订单管理  // 保存所有订单信息
	CRect					m_rcMine;			// 我的点击框
	CRect					m_rcNoRecv;			// 取消接单点击框
	CRect					m_rcNoMusic;		// 关闭音乐点击框
public:
	afx_msg void OnClose();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);


	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};


// 定时请求wnd
#define TIMER_GET_ORDERINFO			0x0001		// 请求订单信息
#define TIMER_GET_RECVORDERINFO		0x0002		// 请求

class CNetbarWnd : public CWnd
{
public:
	CNetbarWnd(CNetbarDlg* pDlg);
	~CNetbarWnd();

	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
private:
	CNetbarDlg * m_pDlg;
};


#define TIMER_MUSIC_ORDER			0x0001		// 新订单请求
class CMusicWnd : public CWnd
{
public:
	CMusicWnd(CNetbarDlg* pDlg);
	~CMusicWnd();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void StopMusic();
	DECLARE_MESSAGE_MAP()

	
private:
	CNetbarDlg *	m_pDlg;
	//CMusicPlayer*	m_pPlayer;
	BOOL			m_bStart;
};