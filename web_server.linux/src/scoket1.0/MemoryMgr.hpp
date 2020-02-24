#ifndef _MemoryMgr_hpp_
#define _MemoryMgr_hpp_
#include<stdlib.h>
#include<assert.h>
#include<mutex>//Ëø

#ifdef _DEBUG
#include<stdio.h>
	#define xPrintf(...) printf(__VA_ARGS__)
#else
	#define xPrintf(...)
#endif // _DEBUG


#define MAX_MEMORY_SZIE 1024

class MemoryAlloc;
//ÄÚ´æ¿é ×îÐ¡µ¥Ôª
class MemoryBlock
{
public:
	//ËùÊô´óÄÚ´æ¿é£¨³Ø£©
	MemoryAlloc* pAlloc;
	//ÏÂÒ»¿éÎ»ÖÃ
	MemoryBlock* pNext;
	//ÄÚ´æ¿é±àºÅ
	int nID;
	//ÒýÓÃ´ÎÊý
	int nRef;
	//ÊÇ·ñÔÚÄÚ´æ³ØÖÐ
	bool bPool;
private:
	//Ô¤Áô
	char c1;
	char c2;
	char c3;
};

//ÄÚ´æ³Ø
class MemoryAlloc
{
public:
	MemoryAlloc()
	{
		_pBuf = nullptr;
		_pHeader = nullptr;
		_nSzie = 0;
		_nBlockSzie = 0;
		xPrintf("MemoryAlloc\n");
	}

	~MemoryAlloc()
	{
		if (_pBuf)
			free(_pBuf);
	}

	//ÉêÇëÄÚ´æ
	void* allocMemory(size_t nSize)
	{
		std::lock_guard<std::mutex> lg(_mutex);
		if (!_pBuf)
		{
			initMemory();
		}
		
		MemoryBlock* pReturn = nullptr;
		if (nullptr == _pHeader)
		{
			pReturn = (MemoryBlock*)malloc(nSize+sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
		}
		else {
			pReturn = _pHeader;
			_pHeader = _pHeader->pNext;
			assert(0 == pReturn->nRef);
			pReturn->nRef = 1;
		}
		xPrintf("allocMem: %llx, id=%d, size=%d\n", pReturn, pReturn->nID, nSize);
		return ((char*)pReturn + sizeof(MemoryBlock));
	}

	//ÊÍ·ÅÄÚ´æ
	void freeMemory(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)( (char*)pMem  - sizeof(MemoryBlock));
		assert(1 == pBlock->nRef);
		if (pBlock->bPool)
		{
			std::lock_guard<std::mutex> lg(_mutex);
			if (--pBlock->nRef != 0)
			{
				return;
			}
			pBlock->pNext = _pHeader;
			_pHeader = pBlock;
		}
		else {
			if (--pBlock->nRef != 0)
			{
				return;
			}
			free(pBlock);
		}
	}

	//³õÊ¼»¯
	void initMemory()
	{
		xPrintf("initMemory:_nSzie=%d,_nBlockSzie=%d\n", _nSzie, _nBlockSzie);
		//¶ÏÑÔ
		assert(nullptr == _pBuf);
		if (_pBuf)
			return;
		//¼ÆËãÄÚ´æ³ØµÄ´óÐ¡
		size_t realSzie = _nSzie + sizeof(MemoryBlock);
		size_t bufSize = realSzie*_nBlockSzie;
		//ÏòÏµÍ³ÉêÇë³ØµÄÄÚ´æ
		_pBuf = (char*)malloc(bufSize);

		//³õÊ¼»¯ÄÚ´æ³Ø
		_pHeader = (MemoryBlock*)_pBuf;
		_pHeader->bPool = true;
		_pHeader->nID = 0;
		_pHeader->nRef = 0;
		_pHeader->pAlloc = this;
		_pHeader->pNext = nullptr;
		//±éÀúÄÚ´æ¿é½øÐÐ³õÊ¼»¯
		MemoryBlock* pTemp1 = _pHeader;
		
		for (size_t n = 1; n < _nBlockSzie; n++)
		{
			MemoryBlock* pTemp2 = (MemoryBlock*)(_pBuf + (n* realSzie));
			pTemp2->bPool = true;
			pTemp2->nID = n;
			pTemp2->nRef = 0;
			pTemp2->pAlloc = this;
			pTemp2->pNext = nullptr;
			pTemp1->pNext = pTemp2;
			pTemp1 = pTemp2;
		}
	}
protected:
	//ÄÚ´æ³ØµØÖ·
	char* _pBuf;
	//Í·²¿ÄÚ´æµ¥Ôª
	MemoryBlock* _pHeader;
	//ÄÚ´æµ¥ÔªµÄ´óÐ¡
	size_t _nSzie;
	//ÄÚ´æµ¥ÔªµÄÊýÁ¿
	size_t _nBlockSzie;
	std::mutex _mutex;
};

//±ãÓÚÔÚÉùÃ÷Àà³ÉÔ±±äÁ¿Ê±³õÊ¼»¯MemoryAllocµÄ³ÉÔ±Êý¾Ý
template<size_t nSzie,size_t nBlockSzie>
class MemoryAlloctor :public MemoryAlloc
{
public:
	MemoryAlloctor()
	{
		//8 4   61/8=7  61%8=5
		const size_t n = sizeof(void*);
		//(7*8)+8 
		_nSzie = (nSzie/n)*n +(nSzie % n ? n : 0);
		_nBlockSzie = nBlockSzie;
	}

};

//ÄÚ´æ¹ÜÀí¹¤¾ß
class MemoryMgr
{
private:
	MemoryMgr()
	{
		init_szAlloc(0, 64, &_mem64);
		init_szAlloc(65, 128, &_mem128);
		init_szAlloc(129, 256, &_mem256);
		init_szAlloc(257, 512, &_mem512);
		init_szAlloc(513, 1024, &_mem1024);
		xPrintf("MemoryMgr\n");
	}

	~MemoryMgr()
	{

	}

public:
	static MemoryMgr& Instance()
	{//µ¥ÀýÄ£Ê½ ¾²Ì¬
		static MemoryMgr mgr;
		return mgr;
	}
	//ÉêÇëÄÚ´æ
	void* allocMem(size_t nSize)
	{
		if (nSize <= MAX_MEMORY_SZIE)
		{
			return _szAlloc[nSize]->allocMemory(nSize);
		}
		else 
		{
			MemoryBlock* pReturn = (MemoryBlock*)malloc(nSize + sizeof(MemoryBlock));
			pReturn->bPool = false;
			pReturn->nID = -1;
			pReturn->nRef = 1;
			pReturn->pAlloc = nullptr;
			pReturn->pNext = nullptr;
			xPrintf("allocMem: %llx, id=%d, size=%d\n", pReturn , pReturn->nID, nSize);
			return ((char*)pReturn + sizeof(MemoryBlock));
		}
		
	}

	//ÊÍ·ÅÄÚ´æ
	void freeMem(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		xPrintf("freeMem: %llx, id=%d\n", pBlock, pBlock->nID);
		if (pBlock->bPool)
		{
			pBlock->pAlloc->freeMemory(pMem);
		}
		else 
		{
			if (--pBlock->nRef == 0)
				free(pBlock);
		}
	}

	//Ôö¼ÓÄÚ´æ¿éµÄÒýÓÃ¼ÆÊý
	void addRef(void* pMem)
	{
		MemoryBlock* pBlock = (MemoryBlock*)((char*)pMem - sizeof(MemoryBlock));
		++pBlock->nRef;
	}
private:
	//³õÊ¼»¯ÄÚ´æ³ØÓ³ÉäÊý×é
	void init_szAlloc(int nBegin, int nEnd, MemoryAlloc* pMemA)
	{
		for (int n = nBegin; n <= nEnd; n++)
		{
			_szAlloc[n] = pMemA;
		}
	}
private:
	MemoryAlloctor<64, 100000> _mem64;
	MemoryAlloctor<128, 100000> _mem128;
	MemoryAlloctor<256, 100000> _mem256;
	MemoryAlloctor<512, 100000> _mem512;
	MemoryAlloctor<1024, 100000> _mem1024;
	MemoryAlloc* _szAlloc[MAX_MEMORY_SZIE + 1];
};

#endif // !_MemoryMgr_hpp_
