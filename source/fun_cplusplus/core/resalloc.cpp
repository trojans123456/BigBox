#include <errno.h>
#include <time.h>
#include <signal.h>

#include <exception>

#include "resalloc.h"

using namespace ns_core;

//********************************************
//* RW Resources allocation object           *
//********************************************
ResRW::ResRW( )
{
    wThr = 0;

    if(pthread_rwlock_init(&rwc,NULL)) throw "ResRW", "Error open semaphore!";
}

ResRW::~ResRW( )
{
    pthread_rwlock_wrlock(&rwc);
    pthread_rwlock_destroy(&rwc);
}

void ResRW::resRequestW( unsigned short tm )
{
    int rez = 0;

    if(wThr && wThr == pthread_self()) rez = EDEADLK;
    else

    if(!tm) rez = pthread_rwlock_wrlock(&rwc);
    else {
    timespec wtm;

    clock_gettime(CLOCK_REALTIME, &wtm);
    wtm.tv_nsec += 1000000*(tm%1000);
    wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
    rez = pthread_rwlock_timedwrlock(&rwc, &wtm);
    }
    if(rez == EDEADLK) throw "ResRW", "Resource is try deadlock a thread!";
    else if(tm && rez == ETIMEDOUT) throw "ResRW", "Resource is timeouted!";

    wThr = pthread_self();

}

bool ResRW::resTryW( )
{
    int rez = pthread_rwlock_trywrlock(&rwc);
    if(rez == EBUSY) return false;
    else if(rez == EDEADLK) throw "ResRW", "Resource is try deadlock a thread!";
    return true;
}

void ResRW::resRequestR( unsigned short tm )
{
    int rez = 0;

    if(wThr && wThr == pthread_self()) rez = EDEADLK;
    else

    if(!tm) rez = pthread_rwlock_rdlock(&rwc);
    else {
    timespec wtm;

    clock_gettime(CLOCK_REALTIME, &wtm);
    wtm.tv_nsec += 1000000*(tm%1000);
    wtm.tv_sec += tm/1000 + wtm.tv_nsec/1000000000; wtm.tv_nsec = wtm.tv_nsec%1000000000;
    rez = pthread_rwlock_timedrdlock(&rwc,&wtm);
    }
    if(rez == EDEADLK) throw "ResRW","Resource is try deadlock a thread!";
    else if(tm && rez == ETIMEDOUT) throw "ResRW","Resource is timeouted!";
}

bool ResRW::resTryR( )
{
    int rez = pthread_rwlock_tryrdlock(&rwc);
    if(rez == EBUSY) return false;
    else if(rez == EDEADLK) throw "ResRW","Resource is try deadlock a thread!";
    return true;
}

void ResRW::resRelease( )
{
    pthread_rwlock_unlock(&rwc);

    if(wThr == pthread_self()) wThr = 0;

}

//********************************************
//* Automatic resource allocator/deallocator *
//********************************************
ResAlloc::ResAlloc( ResRW &rid ) : mId(rid), mAlloc(false)	{ }

ResAlloc::ResAlloc( ResRW &rid, bool write, unsigned short tm ) : mId(rid), mAlloc(false)	{ request(write, tm); }

ResAlloc::~ResAlloc( )	{ if(mAlloc) release(); }

void ResAlloc::request( bool write, unsigned short tm )
{
    if(mAlloc) release();
    mAlloc = false;
    try {
    if(write) mId.resRequestW(tm);
    else mId.resRequestR(tm);
    mAlloc = true;
    } catch(const char *msg) { if(msg) throw; }
}

void ResAlloc::release( )
{
    if(!mAlloc) return;
    mId.resRelease();
    mAlloc = false;
}




//***********************************************************
//* Automatic POSIX mutex allocator/deallocator		    *
//***********************************************************
MtxAlloc::MtxAlloc( ResMtx &iM, bool iLock ) : m(iM), mLock(false) { if(iLock) lock(); }

MtxAlloc::~MtxAlloc( )	{ unlock(); }

int MtxAlloc::lock( )
{
    if(mLock) return 0;
    int rez = m.lock();
    if(!rez) mLock = true;

    return rez;
}

int MtxAlloc::tryLock( )
{
    if(mLock) return 0;
    int rez = m.tryLock();
    if(!rez) mLock = true;

    return rez;
}

int MtxAlloc::unlock( )
{
    if(!mLock) return 0;
    int rez = m.unlock();
    if(!rez) mLock = false;

    return rez;
}

ResCond::ResCond()
{
    int ret = pthread_condattr_init(&mCondAttr);
    if(ret != 0)
        throw "ResCond,cond attr init failed";
    ret = pthread_cond_init(&mCond,&mCondAttr);
    if(ret != 0)
        throw "ResCOnd,cond init failed";
}

ResCond::~ResCond()
{
    pthread_cond_destroy(&mCond);
    pthread_condattr_destroy(&mCondAttr);
}

int ResCond::broadCast()
{
    return pthread_cond_broadcast(&mCond);
}

int ResCond::signal()
{
    return pthread_cond_signal(&mCond);
}

int ResCond::wait(ResMtx &mtx)
{
    return pthread_cond_wait(&mCond,&mtx.mtx());
}

int ResCond::timeWait(ResMtx &mtx, unsigned int msec)
{
    struct timespec tv;
    tv.tv_sec = msec / 1000;
    tv.tv_nsec = (msec % 1000) * 1000;
    return pthread_cond_timedwait(&mCond,&mtx.mtx(),&tv);
}
