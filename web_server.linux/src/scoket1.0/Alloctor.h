#ifndef _ALLOCTOR_H_
#define _ALLOCTOR_H_
#include<stdio.h>

void* operator new(size_t size);
void operator delete(void* p) noexcept;
void* operator new[](size_t size);
void operator delete[](void* p) noexcept;

void* mem_alloc(size_t size);
void meme_free(void* p);

#endif // !_ALLOCTOR_H_



