#include "stdafx.h"
#include "GMSvrSocket.h"

CGMSvrSocket::CGMSvrSocket(void)
{
	m_recvBuf.SetBufSizeMax(32 * 1024 * 1024);
	m_recvBuf.ReSize(8 * 1024 * 1024);
}

CGMSvrSocket::~CGMSvrSocket(void)
{

}

void CGMSvrSocket::ReclaimObj()
{
	CGMSvrParser::GetInstance()->ReleaseConnector(this);
}

int CGMSvrSocket::OnRecved(const char* pBuf, int nLen)
{
	const int HEADER_SIZE = sizeof(SDBMsgHead);
	const char* pHead = pBuf;
	int nPkgLen = 0;
	int nRead = 0;
	while( nLen >= HEADER_SIZE )
	{   
		LPSDBMsgHead pMsgHead = (LPSDBMsgHead)pHead;
		UINT nDelData = 0;
		while( pMsgHead->m_dwTcpFlag != DBTCP_DEF_FLAG && nLen > HEADER_SIZE )
		{
			pHead++;
			nLen--;
			nDelData++;
			pMsgHead = (LPSDBMsgHead)pHead;
		}
		if( nDelData > 0 )
		{
			LOG("GMSocket=%d DelData=%d", GetSocket(), nDelData );
			nRead += nDelData;
		}
		nPkgLen = pMsgHead->m_dwLen;
		if( pMsgHead->m_dwTcpFlag != DBTCP_DEF_FLAG  )
			break;
		if ( nPkgLen > nLen )
		{
			break;
		}
		++m_nRecvSeqNo;

		++m_nRecvSeqNo;
		PostMsg(pHead, nPkgLen);
		pHead += nPkgLen;
		nLen = nLen - nPkgLen;
		nRead += nPkgLen;
	}
	return nRead;
}

void CGMSvrSocket::OnClosed()
{

}

void CGMSvrSocket::PostMsg( const char* pBuf, int nLen )
{
	CGMSvrParser::GetInstance()->PostPkgMsg( this, pBuf, nLen );
}

void CGMSvrSocket::PostDisconnectMsg( EType eType )
{
	CGMSvrParser::GetInstance()->PostDisconnMsg( this, eType );
}

void CGMSvrSocket::Send( const char* buf, int nLen )
{ 
	++m_nSendSeqNo;

 	LPSDBMsgHead pMsgHead = (LPSDBMsgHead)buf;

	YTSvrLib::ITCPBASE::Send(buf, nLen);
}

void CGMSvrSocket::Send( const std::string& strPkg )
{ 
	Send( strPkg.c_str(), (int)strPkg.size() );
}

void CGMSvrSocket::Send( const std::string* pStrPkg )
{
	Send( pStrPkg->c_str(), (int)pStrPkg->size() );
}
