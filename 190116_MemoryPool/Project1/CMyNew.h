#ifndef MYNEW_H
#define MYNEW_H

//* new / delete overloading
extern void *operator new(size_t sz, const char *File, int Line);
extern void *operator new[](size_t sz, const char *File, int Line);

extern void operator delete (void *p, const char *File, int Line);
extern void operator delete[](void *p, const char *File, int Line);

extern void operator delete(void *p);
extern void operator delete[](void *p);
//*/

#define new new(__FILE__, __LINE__)

#endif