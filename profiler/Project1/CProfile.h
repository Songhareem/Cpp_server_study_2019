#pragma once

#ifndef CPROFILE_H
#define CPROFILE_H

#define PRO_BEGIN(TagName)  ProfileBegin((WCHAR *)TagName)
#define PRO_END(TagName)	ProfileEnd((WCHAR *)TagName)

void ProfileBegin(WCHAR *szName);
void ProfileEnd(WCHAR *szName);

void ProfileDataOutText(WCHAR *szFileName);

void ProfileReset(void);

void saveProfile(void);

#endif

/*////////////// �����ؾ��� �� ////////////////

	1) begin 2�� , end 2���� ���� ����ó��
	2) s key �Է½� textOut ��밡���ϰ�
	3) r key �Է½� reset ���
	3-1) reset�� ���� 2 Begin�̳� 2 End �߻��� ��ó
	3-2) reset �׽�Ʈ �ʿ�

//*///////////////////////////////////////////