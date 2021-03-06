#include "StrictClient.h"

extern CPackageMgr<accFuncStruct> *g_HandlerMgr;

StrictClient::StrictClient():m_llpkgCount(0)
{
    m_nNextTick = acct_time::getCurTimeMs();
    //ctor
}

StrictClient::~StrictClient()
{
    //dtor
}
#if 0
int32 StrictClient::testRefectSvr(char *msgbuf, int32 bufsize)
{
    MsgHeader *msgHead = (MsgHeader *)msgbuf;
    struct c_s_refecttest *pmsg = (struct c_s_refecttest *)(msgbuf+sizeof(*msgHead));
    //int32 pkglen = bufsize - sizeof(*msgHead);
    char buf[(pmsg->strlen)+1];
    memset(buf, 0x00, sizeof(buf));
    snprintf(buf, (pmsg->strlen), "%s", (char *)pmsg + sizeof(pmsg->strlen));
    printf("server recv msg:%s\n", buf); //(char *)pmsg + sizeof(pmsg->strlen));
    
    return 0; //processSend(msgHead->sysId, msgHead->msgType, (char *)msgbuf, pkglen);
}
#endif

int32 StrictClient::testRefectSvr(MsgHeader *msghead, char *msgbuf, int32 bufsize)
{
    struct c_s_refecttest *pmsg = (struct c_s_refecttest *)(msgbuf);
    int32 pkglen = bufsize;
    char buf[(pmsg->strlen)+1];
    memset(buf, 0x00, sizeof(buf));
    snprintf(buf, (pmsg->strlen), "%s", (char *)pmsg + sizeof(pmsg->strlen));
    printf("server recv msg:%s\n", buf); //(char *)pmsg + sizeof(pmsg->strlen));
    if (acct_time::getCurTimeMs() - m_nNextTick > 1000)
    {
        cout << "===========================================%d" << getSession()->getSocket() << "============" << m_llpkgCount++ << endl;
        m_llpkgCount = 0;
        m_nNextTick = acct_time::getCurTimeMs() + 1000;
    }
    m_llpkgCount++;
    return processSend(msghead->sysId, msghead->msgType, (char *)msgbuf, pkglen);
}
#if 0
int32 StrictClient::onRecv(PkgHeader *header, char *msgbuf, int32 buffsize)
{
    MsgHeader *msghead = (MsgHeader *)msgbuf;
    int16 sysid = msghead->sysId;
    int16 msgtype = msghead->msgType;
    if (sysid == 1 and msgtype == 2)
    {
        testRefectSvr(msgbuf, buffsize);
    }
    return 0;
}
#endif

int32 StrictClient::testProtobuf(MsgHeader *msghead, char *msgbuf, int32 bufsize)
{
    test_package::testMsg recvmsg;
    recvmsg.ParseFromArray(msgbuf, bufsize);
    //printf("protocol==sendtime:%d====server recv:%s\n", recvmsg.sendtime(), recvmsg.msg().c_str());
    if (acct_time::getCurTimeMs() - m_nNextTick > 1000)
    {
        //cout << "===========================================%d" << getSession()->getSocket() << "============" << m_llpkgCount++ << endl;
        m_llpkgCount = 0;
        m_nNextTick = acct_time::getCurTimeMs() + 1000;
    }
    m_llpkgCount++;
    return processSend(msghead->sysId, msghead->msgType, msgbuf, bufsize);
}

int32 StrictClient::onRecv(PkgHeader *header, MsgHeader *msghead, char *msgbuf, int32 buffsize)
{
    int16 sysid = msghead->sysId;
    int16 msgtype = msghead->msgType;
    int32 key = PKGFUNCBASE::makeKey(sysid, msgtype);
    accFuncStruct *funcStruct = g_HandlerMgr->findFuncStruct(key);
    if (NULL == funcStruct)
    {
        printf("find no func by sysid:%d and msgtype:%d\n", sysid, msgtype);
    }
    else
    {
        funcStruct->handler(this->getSession(), msgbuf, buffsize);
    }
    return 0;
}

int32 StrictClient::processSend(uint16 sysid, uint16 msgid, char *msgbuf, int32 bufsize)
{
    MsgHeader msgHead = {sysid, msgid};
    //c_s_refecttest *ret = (c_s_refecttest*)msgbuf;
    //int32 strlen = ret->strlen;
    uint16 pkglen = sizeof(msgHead) + bufsize;
    PkgHeader header = {pkglen, 0};
    uint16 allPkgLen = pkglen + sizeof(header);
    char buf[allPkgLen];
    encodepkg(buf, &header, &msgHead, msgbuf, bufsize);
    return 0; //getSession()->send(buf, (int32)allPkgLen);
}
