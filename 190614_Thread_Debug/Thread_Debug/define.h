
#ifndef __DEFINE__
#define __DEFINE__

//#define dfTHREAD_NUM	4
#define dfTHREAD_NUM	3

#define	dfMAX_PLAYER	5000

#define ON_STATE_ACCEPT				0
#define ON_STATE_ACCEPT2UPDATE		1
#define ON_STATE_UPDATE				2

struct st_SESSION
{
	int SessionID;
};

struct st_PLAYER
{
	int SessionID = ON_STATE_ACCEPT;
	int Content[3] = { 0, };

	int OnState = 0;				// � �����忡�� ó���ؾ��ϴ��� ����
	bool DisconnectFlag = false;	// ���� �����忡�� �������� ������ Disconnect ��û
};

#endif