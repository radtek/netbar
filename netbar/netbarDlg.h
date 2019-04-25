
// netbarDlg.h: 头文件
//

#pragma once
#include "COrderDlg.h"
#include "CRecviceOrder.h"


class CNetbarWnd;
class COrderManager;
// CNetbarDlg 对话框
class CNetbarDlg : public CDialog
{
// 构造
public:
	CNetbarDlg(CWnd* pParent = NULL);	// 标准构造函数

	// 设置用户名，密码
	void SetParam(const CString& strUsername, const CString& strPassword);
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
// 处理数据
	void ParseOrderJson(const CString& strJson);			// 处理请求返回的订单数据
	void ParseNetInfoJson(const CString& strJson);			// 处理请求返回的网吧信息
// 请求数据
public:
	void SendNetBarRequest();

	void SendOrderRequest();		// 请求订单数据
	void SendOrderOperate();		// 订单操作		// 接单 不接单

	void SendRecvOrderRequest();	// 请求已经接单的数据
	void SendRecvOrderOperate();	// 已接订单操作	// 到店退款 过期不候

	void ShowOrderInfo();
	void InsertVcRecv(RecvInfo pInfo);
	void DeleteOrderInfo(const CString& strOrderNum);
private:

	CNetbarWnd*				m_pNetbarWnd;		// 定时请求
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
