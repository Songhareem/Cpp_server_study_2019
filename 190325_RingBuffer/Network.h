#pragma once

BOOL DomainToIP(const WCHAR *szDomain, IN_ADDR *pAddr);
void err_display(const WCHAR *msg, BOOL isExit);