// StkDemo.cpp : implementation file
//
//		龙卷风股票行情接收示范程序（通视协议）
//	
//	
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StkDemo.h"

#include "StockDrv.H"
#include "STKDRV.H"

CSTKDRV		gSTOCKDLL;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStkDemo

CStkDemo::CStkDemo()
{
	int i;
	for(i=0; i<StkBufNum; i++){
		m_Stock[i] = _T("股票: ");
	}
	m_StkPtr = 0;
	m_Min = _T("分时走势:");
	m_File = _T("文件:");
	m_bRunFlag = FALSE;
}

CStkDemo::~CStkDemo()
{
}


BEGIN_MESSAGE_MAP(CStkDemo, CWnd)
	//{{AFX_MSG_MAP(CStkDemo)
	ON_WM_PAINT()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(My_Msg_StkData,OnStkDataOK)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStkDemo message handlers

typedef struct  tagPOWER_TOTAL_STRUCTEx 
{
	RCV_POWER_STRUCTEx *m_RcvPower;   
	long Power_Count;                   

}POWER_TOTAL_STRUCTEx;


void CStkDemo::OnPaint() 
{
	int i;
	int k;
	int High;
	int y;
	TEXTMETRIC tm;
	char * pTitle = "  代码     名  称      最新     今开     昨收     最高     最低     成交量   成交额";
	CPaintDC dc(this); // device context for painting
	dc.GetTextMetrics(&tm);
	High = tm.tmHeight + 3;
	k = m_StkPtr;
	y = 1;
	dc.TextOut(1,y,(CString)pTitle,strlen(pTitle));
	y += High;
	for(i=0; i<StkBufNum; i++)
	{
		k = k % StkBufNum;
		dc.TextOut(1,y,m_Stock[k],m_Stock[k].GetLength());
		y += High;
		k ++;
	}
	y += High;
	dc.TextOut(1,y,m_Min,m_Min.GetLength());
	y += High;
	dc.TextOut(1,y,m_File,m_File.GetLength());
}

// 处理数据
LONG CStkDemo::OnStkDataOK(
		UINT wFileType,				// 文件类型, 参见 StockDrv.H
		LONG lPara)					
{
	int i,j;
	long day;
	float open,high,low,close,vol,amount;
	long m_FileLong;
	FILE *fp;
	CString m_FileName;

	PBYTE pFileBuf = NULL;
	RCV_DATA *	pHeader;

	pHeader = (RCV_DATA *) lPara;

	//对于处理较慢的数据类型,建议将数据备份,另创建一线程单独处理，函数处理完毕后，
	//才会推送下一条数据  

	switch( wFileType )
	{
		case RCV_REPORT:
			{
				//初始化行情以及交易日的实时行情都是在这个地方接收
				int nBufSize = pHeader->m_pReportV3[0].m_cbSize;
				PBYTE pBaseBuf = (PBYTE)&pHeader->m_pReportV3[0];
				for(i=0; i<pHeader->m_nPacketNum; i++)
				{
					RCV_REPORT_STRUCTExV3 & Buf = *(PRCV_REPORT_STRUCTExV3)(pBaseBuf + nBufSize*i );
					m_Stock[m_StkPtr].Format(L"%6s %8s N=%8.3f O=%8.3f C=%8.3f H=%8.3f L=%8.3f V=%10.0f A=%10.f",
						Buf.m_szLabel,Buf.m_szName,Buf.m_fNewPrice,Buf.m_fOpen,Buf.m_fLastClose,\
						Buf.m_fHigh,Buf.m_fLow,Buf.m_fVolume,Buf.m_fAmount);


					// 增加的数据获得如下
					TRACE("PB5=%8.3f VB5=%8.2f PS5=%8.3f VS5=%8.2f\n",Buf.m_fBuyPrice5,Buf.m_fBuyVolume5,Buf.m_fSellPrice5,Buf.m_fSellVolume5);

					m_StkPtr ++;
					m_StkPtr = m_StkPtr % StkBufNum;
				}
			}
			break;
	
		case RCV_FILEDATA:
			if( !pHeader->m_pData || pHeader->m_wDataType == FILE_TYPE_RES)
			{
				TRACE("MSG: CStkDemo::OnStkDataOK, Replaced data \n");
				break;
			}
			switch(pHeader->m_wDataType)
			{
				case FILE_HISTORY_EX:	// 补日线数据
					RCV_HISTORY_STRUCTEx * pDay;
					pDay = pHeader->m_pDay;
					ASSERT(pHeader->m_pDay[0].m_head.m_dwHeadTag == EKE_HEAD_TAG);
					m_File = "日线 代码：";
					j = 0;
					for(i=0; i<pHeader->m_nPacketNum ; i++)
					{
						//因为是变体记录结构,要判断
						if( pDay[i].m_head.m_dwHeadTag == EKE_HEAD_TAG )
						{
							//说明是文件头部分,得到股票代码
							m_File = m_File + (const char *) pDay[i].m_head.m_szLabel + "  ";
							j ++;
						}else{
							//说明是具体K线数据
							day = pDay[i].m_time ;
							open = pDay[i].m_fOpen ;
							high= pDay[i].m_fHigh  ;
							low = pDay[i].m_fLow  ;
							close = pDay[i].m_fClose ;
							vol = pDay[i].m_fVolume ;
							amount = pDay[i].m_fAmount  ;
							CString test;
							test.Format(L"日期：%d,收盘价：%f,成交金额：%f",day,close,amount); 
							//m_File = m_File + (const char *) test + "  ";
							//AfxMessageBox(m_File +test);
						}
						if( j > 1) break;
					}
					break;

				case FILE_MINUTE_EX:   // 补分钟线数据，即分时数据
					RCV_MINUTE_STRUCTEx * pMin;
					pMin = pHeader->m_pMinute;
					ASSERT(pMin->m_head.m_dwHeadTag == EKE_HEAD_TAG);
					m_Min = "分时走势: 代码：";
					AfxMessageBox(m_Min);
					j=0;
					for(i=0; i<pHeader->m_nPacketNum; i++)
					{
						//因为是变体记录结构,要判断
						if( pMin[i].m_head.m_dwHeadTag == EKE_HEAD_TAG )
						{
							//说明是文件头部分,得到股票代码
							m_Min = m_Min + (LPCSTR)pMin[i].m_head.m_szLabel + "  ";
							j++;
						}else{
							//说明是具体分时线数据,具体结构可参考.h文件中的定义说明
							day = pMin[i].m_time ;
							close = pMin[i].m_fPrice ;
							vol = pMin[i].m_fVolume ;
							amount = pMin[i].m_fAmount  ;
							CString test;
							test.Format(L"时间：%d,最新价：%f,成交金额：%f",day,close,amount); 
							//m_Min = m_Min + (const char *) test + "  ";
							//AfxMessageBox(m_Min +  test);
						}
						if( j > 1) break;
					}
					break;
				case FILE_POWER_EX:	   //除权数据		
					POWER_TOTAL_STRUCTEx m_PowerData;
					m_PowerData.Power_Count=pHeader->m_nPacketNum;
					m_PowerData.m_RcvPower=pHeader->m_pPower;
					break;

				case FILE_BASE_EX:	   // 钱龙兼容基本资料文件,m_szFileName仅包含文件名
					//在这里示范将个股资料信息显示出来,这里是将资料内容以文件的方式保存下来
					//具体位置就是当前目录下，文件名为：pHeader->m_File.m_szFileName
					m_File.Format(L"基本资料: %s",pHeader->m_File.m_szFileName);
					m_FileLong=pHeader->m_File.m_dwLen ;                
					m_FileName = pHeader->m_File.m_szFileName;
					/*
					
					USES_CONVERSION;
					const char* cstr = T2A((LPCTSTR)m_FileName);
					if ((fp = fopen(cstr, "wb+")) != NULL)
					{
						fwrite(pHeader->m_pData , sizeof( char ),m_FileLong , fp );      
						fclose(fp) ;
					}
					*/
					break;

				case FILE_NEWS_EX:	   // 新闻类,其类型由m_szFileName中子目录名来定
					m_File.Format(L"公告消息: %s",pHeader->m_File.m_szFileName);
					break;

				default:
					TRACE("Msg: CStkDemo::OnStkDataOK,Unkonw data type\n");
			}
			break;

	}
	RedrawWindow();
	return 0L;
}

int CStkDemo::MyCreate(CWnd* pWnd)
{
	CRect rect(0, 0, 550,280);
	HBRUSH hBrush;
	if( m_bRunFlag ) return -1;
	hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
   	LPCTSTR lpszClassName = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW,0,hBrush);
	CreateEx(0,lpszClassName,L"交易师",WS_OVERLAPPEDWINDOW,
		rect,pWnd, 0, NULL);

	ShowWindow(SW_SHOWNORMAL);
	UpdateWindow();

	int ok = gSTOCKDLL.Stock_Init(m_hWnd,My_Msg_StkData,RCV_WORK_SENDMSG);	// 数据共享引用

	if( ok > 0 )
	{
		CString Title;
		Title.Format(L"交易师-行情字段大小:%d",sizeof(RCV_REPORT_STRUCTExV3));
		SetWindowText(Title);
		m_bRunFlag = TRUE;
		return 1;
	}
	PostQuitMessage(-1);
	return -1;
}

void CStkDemo::OnDestroy() 
{
	CWnd::OnDestroy();
	m_bRunFlag = FALSE;
	gSTOCKDLL.Stock_Quit(m_hWnd);
}
