/*************************************************************************************************
 *                                     Trochili RTOS Kernel                                      *
 *                                  Copyright(C) 2016 LIUXUMING                                  *
 *                                       www.trochili.com                                        *
 *************************************************************************************************/
#ifndef _TCL_THREAD_H
#define _TCL_THREAD_H

#include "tcl.types.h"
#include "tcl.config.h"
#include "tcl.object.h"
#include "tcl.ipc.h"
#include "tcl.timer.h"

/* 线程运行错误码定义                 */
#define OS_THREAD_DIAG_NORMAL            (TBitMask)(0x0)     /* 线程正常                                */
#define OS_THREAD_DIAG_STACK_OVERFLOW    (TBitMask)(0x1<<0)  /* 线程栈溢出                              */
#define OS_THREAD_DIAG_STACK_ALARM       (TBitMask)(0x1<<1)  /* 线程栈告警                              */
#define OS_THREAD_DIAG_INVALID_EXIT      (TBitMask)(0x1<<2)  /* 线程非法退出                            */
#define OS_THREAD_DIAG_INVALID_STATE     (TBitMask)(0x1<<3)  /* 线程操作失败                            */
#define OS_THREAD_DIAG_INVALID_TIMEO     (TBitMask)(0x1<<4)  /* 线程时限阻塞禁止                        */

/* 线程调用错误码定义                 */
#define OS_THREAD_ERR_NONE               (TError)(0x0)
#define OS_THREAD_ERR_UNREADY            (TError)(0x1<<0)    /* 线程结构未初始化                        */
#define OS_THREAD_ERR_ACAPI              (TError)(0x1<<1)    /* 线程不接受操作                          */
#define OS_THREAD_ERR_FAULT              (TError)(0x1<<2)    /* 一般性错误，操作条件不满足              */
#define OS_THREAD_ERR_STATUS             (TError)(0x1<<3)    /* 线程状态错误                            */
#define OS_THREAD_ERR_PRIORITY           (TError)(0x1<<4)    /* 线程优先级错误                          */

/* 线程属性定义                       */
#define OS_THREAD_PROP_NONE              (TProperty)(0x0)
#define OS_THREAD_PROP_READY             (TProperty)(0x1<<0) /* 线程初始化完毕标记位,
                                                           * 本成员在结构体中的位置跟汇编代码相关    */
#define OS_THREAD_PROP_PRIORITY_FIXED    (TProperty)(0x1<<1) /* 线程优先级锁定标记                      */
#define OS_THREAD_PROP_PRIORITY_SAFE     (TProperty)(0x1<<2) /* 线程优先级安全标记                      */
#define OS_THREAD_PROP_CLEAN_STACK       (TProperty)(0x1<<3) /* 主动清空线程栈空间                      */
#define OS_THREAD_PROP_KERNEL_ROOT       (TProperty)(0x1<<6) /* ROOT线程标记位                          */
#define OS_THREAD_PROP_KERNEL_DAEMON     (TProperty)(0x1<<4) /* 内核守护线程标记位                      */

/* 线程权限控制，各种线程API操作时的许可位 */
#define OS_THREAD_ACAPI_NONE             (TBitMask)(0x0)
#define OS_THREAD_ACAPI_DELETE           (TBitMask)(0x1<<0)
#define OS_THREAD_ACAPI_ACTIVATE         (TBitMask)(0x1<<1)
#define OS_THREAD_ACAPI_DEACTIVATE       (TBitMask)(0x1<<2)
#define OS_THREAD_ACAPI_SUSPEND          (TBitMask)(0x1<<3)
#define OS_THREAD_ACAPI_RESUME           (TBitMask)(0x1<<4)
#define OS_THREAD_ACAPI_DELAY            (TBitMask)(0x1<<5)
#define OS_THREAD_ACAPI_UNDELAY          (TBitMask)(0x1<<6)
#define OS_THREAD_ACAPI_YIELD            (TBitMask)(0x1<<7)
#define OS_THREAD_ACAPI_PRIORITY         (TBitMask)(0x1<<8)
#define OS_THREAD_ACAPI_SLICE            (TBitMask)(0x1<<9)
#define OS_THREAD_ACAPI_UNBLOCK          (TBitMask)(0x1<<10)
#define OS_THREAD_ACAPI_BLOCK            (TBitMask)(0x1<<11) /* 和IPC阻塞有关,比如内核线程不许以阻塞方式调用IPC函数 */

#define OS_THREAD_ACAPI_ALL \
    (OS_THREAD_ACAPI_DELETE|\
    OS_THREAD_ACAPI_ACTIVATE|\
    OS_THREAD_ACAPI_DEACTIVATE|\
    OS_THREAD_ACAPI_SUSPEND|\
    OS_THREAD_ACAPI_RESUME|\
    OS_THREAD_ACAPI_DELAY|\
    OS_THREAD_ACAPI_UNDELAY|\
    OS_THREAD_ACAPI_PRIORITY|\
    OS_THREAD_ACAPI_SLICE|\
    OS_THREAD_ACAPI_UNBLOCK|\
    OS_THREAD_ACAPI_BLOCK|\
    OS_THREAD_ACAPI_YIELD)

/* 线程状态定义  */
enum ThreadStausdef
{
    OsThreadRunning   = (TBitMask)(0x1<<0),     /* 运行                                           */
    OsThreadReady     = (TBitMask)(0x1<<1),     /* 就绪                                           */
    OsThreadDormant   = (TBitMask)(0x1<<2),     /* 休眠                                           */
    OsThreadBlocked   = (TBitMask)(0x1<<3),     /* 阻塞                                           */
    OsThreadDelayed   = (TBitMask)(0x1<<4),     /* 延时挂起                                       */
    OsThreadSuspended = (TBitMask)(0x1<<5),     /* 就绪挂起                                       */
};
typedef enum ThreadStausdef TThreadStatus;

/*
 * 线程队列结构定义，该结构大小随内核支持的优先级范围而变化，
 * 可以实现固定时间的线程优先级计算算法
 */
struct ThreadQueueDef
{
    TBitMask   PriorityMask;                 /* 队列中就绪优先级掩码                             */
    TLinkNode* Handle[TCLC_PRIORITY_NUM];    /* 队列中线程分队列                                 */
};
typedef struct ThreadQueueDef TThreadQueue;


/* 线程延时定时器结构定义 */
struct TickTimerDef
{
    TTimeTick     RemainTicks;               /* 线程定时器计时数                                 */
    void*         Owner;                     /* 线程定时器所属线程                               */
    TLinkNode     LinkNode;                  /* 线程定时器队列的链表节点                         */
};
typedef struct TickTimerDef TTickTimer;

/* 线程主函数类型定义                                                                            */
typedef void (*TThreadEntry)(TArgument data);

/* 内核线程结构定义，用于保存线程的基本信息                                                      */
struct ThreadDef
{
    TProperty     Property;                  /* 线程的属性,本成员在结构体中的位置跟汇编代码相关  */
    TThreadStatus Status;                    /* 线程状态,本成员在结构体中的位置跟汇编代码相关    */
    TAddr32       StackTop;                  /* 线程栈顶指针,本成员在结构体中的位置跟汇编代码相关*/
    TAddr32       StackBase;                 /* 线程栈底指针                                     */
#if (TCLC_THREAD_STACK_CHECK_ENABLE)
    TBase32       StackAlarm;                /* 线程栈用量警告                                   */
    TBase32       StackBarrier;              /* 线程栈顶围栏                                     */
#endif
    TBitMask      ACAPI;                     /* 线程可接受的API                                  */
    TPriority     Priority;                  /* 线程当前优先级                                   */
    TPriority     BasePriority;              /* 线程基本优先级                                   */
    TTimeTick     Ticks;                     /* 时间片中还剩下的ticks数目                        */
    TTimeTick     BaseTicks;                 /* 时间片长度（ticks数目）                          */
    TTimeTick     Jiffies;                   /* 线程总的运行时钟节拍数                           */
    TThreadEntry  Entry;                     /* 线程的主函数                                     */
    TArgument     Argument;                  /* 线程主函数的用户参数,用户来赋值                  */
    TBitMask      Diagnosis;                 /* 线程运行错误码                                   */
    TTickTimer    Timer;                     /* 用于线程延时或者线程时限阻塞的时间管理结构       */
#if (TCLC_IPC_ENABLE)
    TIpcContext*  IpcContext;                /* 线程互斥、同步或者通信的上下文                   */
#endif
#if ((TCLC_IPC_ENABLE) && (TCLC_IPC_MUTEX_ENABLE))
    TLinkNode*    LockList;                  /* 线程占有的锁的队列                               */
#endif
    TThreadQueue* Queue;                     /* 指向线程所属线程队列的指针                       */
    TLinkNode     LinkNode;                  /* 线程所在队列的节点                               */
    TObject       Object;                    /* 线程的内核对象节点                               */
};
typedef struct ThreadDef TThread;

#define TCLM_NODE2THREAD(NODE) ((TThread*)((TByte*)(NODE) - OFF_SET_OF(TThread, LinkNode)))

extern void OsThreadModuleInit(void);
extern void OsThreadEnterQueue(TThreadQueue* pQueue, TThread* pThread, TLinkPos pos);
extern void OsThreadLeaveQueue(TThreadQueue* pQueue, TThread* pThread);
extern void OsThreadTickUpdate(void);
extern void OsThreadTimerUpdate(void);
extern void OsThreadSchedule(void);
extern void OsThreadCreate(TThread* pThread, TChar* pName, TThreadStatus status, TProperty property,
                           TBitMask acapi, TThreadEntry pEntry, TArgument argument,
                           void* pStack, TBase32 bytes, TPriority priority, TTimeTick ticks);
extern TState OsThreadDelete(TThread* pThread, TError* pError);
extern TState OsThreadSetPriority(TThread* pThread, TPriority priority, TBool flag, TBool* pHiRP, TError* pError);
extern void OsThreadResumeFromISR(TThread* pThread);
extern void OsThreadSuspendSelf(void);

extern TState TclCreateThread(TThread* pThread,
                              TChar* pName,
                              TThreadEntry pEntry,
                              TBase32 argument,
                              void* pStack,
                              TBase32 bytes,
                              TPriority priority,
                              TTimeTick ticks,
                              TError* pError);
extern TState TclDeleteThread(TThread* pThread, TError* pError);
extern TState TclActivateThread(TThread* pThread, TError* pError);
extern TState TclDeactivateThread(TThread* pThread, TError* pError);
extern TState TclSuspendThread(TThread* pThread, TError* pError);
extern TState TclResumeThread(TThread* pThread, TError* pError);
extern TState TclSetThreadPriority(TThread* pThread, TPriority priority, TError* pError);
extern TState TclSetThreadSlice(TThread* pThread, TTimeTick ticks, TError* pError);
extern TState TclYieldThread(TError* pError);
extern TState TclDelayThread(TTimeTick ticks, TError* pError);
extern TState TclUndelayThread(TThread* pThread, TError* pError);
#if (TCLC_IPC_ENABLE)
extern TState TclUnblockThread(TThread* pThread, TError* pError);
#endif

#endif /*_TCL_THREAD_H */

