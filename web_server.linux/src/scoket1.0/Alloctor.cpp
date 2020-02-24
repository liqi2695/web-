#include"Alloctor.h"
#include"MemoryMgr.hpp"
void* operator new(size_t nSize )
{
	return MemoryMgr::Instance().allocMem(nSize);
}
void operator delete(void* p) noexcept
{
	MemoryMgr::Instance().freeMem(p);
}
void* operator new[](size_t nSize)
{
	return MemoryMgr::Instance().allocMem(nSize);
}
void operator delete[](void* p) noexcept
{
	MemoryMgr::Instance().freeMem(p);
}

void* mem_alloc(size_t nSize)
{
	return MemoryMgr::Instance().allocMem(nSize);
}

void meme_free(void* p)
{
	MemoryMgr::Instance().freeMem(p);
}