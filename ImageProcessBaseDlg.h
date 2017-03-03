#pragma comment(lib, "vfw32.lib")
//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

#include "vfw.h"
#include "afxwin.h"
#include <stdio.h>

#define    _DEF_WEBCAM        1000

LRESULT CALLBACK CallbackOnFrame(HWND hWnd, LPVIDEOHDR lpVHdr);

// CImageProcessBaseDlg ��ȭ ����
class CImageProcessBaseDlg : public CDialog
{
// �����Դϴ�.
public:
	CImageProcessBaseDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	void OnDestroy();
	afx_msg void OnBnClickedOk();

	HWND m_Cap;
	CStatic m_stDisplay;

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_IMAGEPROCESSBASE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
