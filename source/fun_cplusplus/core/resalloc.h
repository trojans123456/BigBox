#ifndef __RESOURCE_H
#define __RESOURCE_H

#ifndef __WIN32__
#include <pthread.h>
#endif

#include <string>

using namespace std;

namespace ns_core {


//***********************************************************
//* RW Resources allocation object			    *
//***********************************************************
class ResRW
{
    public:
    ResRW( );
    ~ResRW( );

    void lock( bool toWr, unsigned short tm = 0 ) { if(toWr) resRequestW(tm); else resRequestR(tm); }
    bool tryLock( bool toWr )	{ return toWr ? resTryW() : resTryR(); }
    void unlock( )			{ resRelease(); }

    void resRequestW( unsigned short tm = 0 );	// Write request, tm in milliseconds
    bool resTryW( );
    void resRequestR( unsigned short tm = 0 );	// Read request, tm in milliseconds
    bool resTryR( );
    void resRelease( );				// Release

    private:
    pthread_rwlock_t	rwc;

    pthread_t		wThr;

};


//***********************************************************
//* Automatic resource RW unlock object			    *
//***********************************************************
class ResAlloc
{
    public:
    //Methods
    ResAlloc( ResRW &rid );
    ResAlloc( ResRW &rid, bool write, unsigned short tm = 0 );
    ~ResAlloc( );

    void request( bool write = false, unsigned short tm = 0 );
    void lock( bool write = false, unsigned short tm = 0 )	{ request(write, tm); }
    void release( );
    void unlock( )	{ release(); }

    private:
    //Attributes
    ResRW	&mId;
    bool	mAlloc;
};

//***********************************************************
//* Resources allocation object, by mutex		    *
//***********************************************************
class ResMtx
{
    public:
    ResMtx( bool isRecurs = false ) {
        pthread_mutexattr_t attrM;
        pthread_mutexattr_init(&attrM);
        if(isRecurs) pthread_mutexattr_settype(&attrM, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m, &attrM);
        pthread_mutexattr_destroy(&attrM);
    }
    ~ResMtx( ) {
        pthread_mutex_destroy(&m);
    }

    int lock( )	{ return pthread_mutex_lock(&m); }
    int tryLock( )	{ return pthread_mutex_trylock(&m); }
    int unlock( )	{ return pthread_mutex_unlock(&m); }

    pthread_mutex_t &mtx( )	{ return m; }

    private:
    //Attributes
    pthread_mutex_t	m;
};

//********************************************
//* String+resource RW lock for		     *
//********************************************
class ResString
{
    public:
    //Methods
    explicit ResString( const string &vl = "" );
    ~ResString( );

    ResString &operator=( const string &val );
    operator string( )		{ return getVal(); }

    size_t size( );
    bool   empty( );

    void setVal( const string &vl );
    string getVal( );
    const string &getValRef( )	{ return str; }

    private:
    //Attributes
    ResMtx	mRes;
    string	str;
};

//***********************************************************
//* Automatic POSIX mutex unlock object			    *
//***********************************************************
class MtxAlloc
{
    public:
    //Methods
    MtxAlloc( ResMtx &iM, bool lock = false );
    ~MtxAlloc( );

    int lock( );
    int tryLock( );
    int unlock( );

    private:
    //Attributes
    ResMtx	&m;
    bool	mLock;
};

/**************** cond ********************/
class ResCond
{
public:
    ResCond();
    ~ResCond();

    int wait(ResMtx &mtx);
    /* ns */
    int timeWait(ResMtx &mtx, unsigned int msec);
    int signal();
    int broadCast();

private:
    pthread_cond_t mCond;
    pthread_condattr_t mCondAttr;
};


}


#endif
