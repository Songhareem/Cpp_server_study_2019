
#pragma once

#define MAX 20

extern int g_stack[MAX];
extern int g_top;

bool push(const int val);
bool pop(int &get_val);