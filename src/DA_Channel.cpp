// DA_Channel.cpp : インプリメンテーション ファイル
//

#include "stdafx.h"
#include "DigitShowBasic.h"
#include "DA_Channel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDA_Channel ダイアログ
extern	double		DA_Cal_a[8],DA_Cal_b[8];		

CDA_Channel::CDA_Channel(CWnd* pParent /*=NULL*/)
	: CDialog(CDA_Channel::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDA_Channel)
	m_DA_Cala00 = DA_Cal_a[0];
	m_DA_Cala01 = DA_Cal_a[1];
	m_DA_Cala02 = DA_Cal_a[2];
	m_DA_Cala03 = DA_Cal_a[3];
	m_DA_Cala04 = DA_Cal_a[4];
	m_DA_Cala05 = DA_Cal_a[5];
	m_DA_Cala06 = DA_Cal_a[6];
	m_DA_Cala07 = DA_Cal_a[7];
	m_DA_Calb00 = DA_Cal_b[0];
	m_DA_Calb01 = DA_Cal_b[1];
	m_DA_Calb02 = DA_Cal_b[2];
	m_DA_Calb03 = DA_Cal_b[3];
	m_DA_Calb04 = DA_Cal_b[4];
	m_DA_Calb05 = DA_Cal_b[5];
	m_DA_Calb06 = DA_Cal_b[6];
	m_DA_Calb07 = DA_Cal_b[7];
	//}}AFX_DATA_INIT
}


void CDA_Channel::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDA_Channel)
	DDX_Text(pDX, IDC_EDIT_DA_Cala00, m_DA_Cala00);
	DDX_Text(pDX, IDC_EDIT_DA_Cala01, m_DA_Cala01);
	DDX_Text(pDX, IDC_EDIT_DA_Cala02, m_DA_Cala02);
	DDX_Text(pDX, IDC_EDIT_DA_Cala03, m_DA_Cala03);
	DDX_Text(pDX, IDC_EDIT_DA_Cala04, m_DA_Cala04);
	DDX_Text(pDX, IDC_EDIT_DA_Cala05, m_DA_Cala05);
	DDX_Text(pDX, IDC_EDIT_DA_Cala06, m_DA_Cala06);
	DDX_Text(pDX, IDC_EDIT_DA_Cala07, m_DA_Cala07);
	DDX_Text(pDX, IDC_EDIT_DA_Calb00, m_DA_Calb00);
	DDX_Text(pDX, IDC_EDIT_DA_Calb01, m_DA_Calb01);
	DDX_Text(pDX, IDC_EDIT_DA_Calb02, m_DA_Calb02);
	DDX_Text(pDX, IDC_EDIT_DA_Calb03, m_DA_Calb03);
	DDX_Text(pDX, IDC_EDIT_DA_Calb04, m_DA_Calb04);
	DDX_Text(pDX, IDC_EDIT_DA_Calb05, m_DA_Calb05);
	DDX_Text(pDX, IDC_EDIT_DA_Calb06, m_DA_Calb06);
	DDX_Text(pDX, IDC_EDIT_DA_Calb07, m_DA_Calb07);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDA_Channel, CDialog)
	//{{AFX_MSG_MAP(CDA_Channel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDA_Channel メッセージ ハンドラ

void CDA_Channel::OnOK() 
{
	// TODO: この位置にその他の検証用のコードを追加してください
	UpdateData(TRUE);
	DA_Cal_a[0] = m_DA_Cala00;
	DA_Cal_a[1] = m_DA_Cala01;
	DA_Cal_a[2] = m_DA_Cala02;
	DA_Cal_a[3] = m_DA_Cala03;
	DA_Cal_a[4] = m_DA_Cala04;
	DA_Cal_a[5] = m_DA_Cala05;
	DA_Cal_a[6] = m_DA_Cala06;
	DA_Cal_a[7] = m_DA_Cala07;
	DA_Cal_b[0] = m_DA_Calb00;
	DA_Cal_b[1] = m_DA_Calb01;
	DA_Cal_b[2] = m_DA_Calb02;
	DA_Cal_b[3] = m_DA_Calb03;
	DA_Cal_b[4] = m_DA_Calb04;
	DA_Cal_b[5] = m_DA_Calb05;
	DA_Cal_b[6] = m_DA_Calb06;
	DA_Cal_b[7] = m_DA_Calb07;	
	CDialog::OnOK();
}
