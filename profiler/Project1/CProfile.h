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

/*////////////// 개선해야할 점 ////////////////

	1) begin 2번 , end 2번에 따른 예외처리
	2) s key 입력시 textOut 사용가능하게
	3) r key 입력시 reset 기능
	3-1) reset에 의해 2 Begin이나 2 End 발생시 대처
	3-2) reset 테스트 필요

//*///////////////////////////////////////////