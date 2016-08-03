#include "example.h"
#include "trochili.h"

#if (EVB_EXAMPLE == CH10_IRQ_ISR_EXAMPLE2)

/* �û��̲߳��� */
#define THREAD_LED_STACK_BYTES  (512)
#define THREAD_LED_PRIORITY     (5)
#define THREAD_LED_SLICE        (20)


/* �û��߳�ջ���� */
static TBase32 ThreadLedStack[THREAD_LED_STACK_BYTES/4];

/* �û��̶߳��� */
static TThread ThreadLed;

/* �û��ź������� */
static TSemaphore LedSemaphore;

/* Led�̵߳������� */
static void ThreadLedEntry(TArgument data)
{
    TError error;
    TState state;

    while (eTrue)
    {
        /* Led�߳���������ʽ��ȡ�ź���������ɹ������Led */
        state = TclObtainSemaphore(&LedSemaphore, TCLO_IPC_WAIT, 0, &error);
        if (state == eSuccess)
        {
            TCLM_ASSERT((error == TCLE_IPC_NONE), "");
            EvbLedControl(LED1, LED_ON);
        }

        /* Led�߳���������ʽ��ȡ�ź���������ɹ���Ϩ��Led */
        state = TclObtainSemaphore(&LedSemaphore, TCLO_IPC_WAIT, 0, &error);
        if (state == eSuccess)
        {
            TCLM_ASSERT((error == TCLE_IPC_NONE), "");
            EvbLedControl(LED1, LED_OFF);
        }
    }
}


/* �����尴���жϴ������� */
static TBitMask EvbUart2ISR(TArgument data)
{
    TState state;
    if (EvbKeyScan())
    {
        /* ISR�Է�������ʽ(����)�ͷ��ź��� */
        state = TclIsrReleaseSemaphore(&LedSemaphore);
        TCLM_ASSERT((state == eSuccess), "");
    }
    return TCLR_IRQ_DONE;
}

/* �û�Ӧ�ó�����ں��� */
static void AppSetupEntry(void)
{
    TState state;
    TError error;

    /* ���ú�KEY��ص��ⲿ�ж����� */
    state = TclSetIrqVector(UART_IRQ_ID, &EvbUart2ISR, (TThread*)0, (TArgument)0, &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_IRQ_NONE), "");

    /* ��ʼ���ź��� */
    state = TclCreateSemaphore(&LedSemaphore, 0, 1, TCLP_IPC_DUMMY, &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_IPC_NONE), "");

    /* ��ʼ��Led�߳� */
    state = TclCreateThread(&ThreadLed,
                          &ThreadLedEntry,
                          (TArgument)0,
                          ThreadLedStack,
                          THREAD_LED_STACK_BYTES,
                          THREAD_LED_PRIORITY,
                          THREAD_LED_SLICE,
                          &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_THREAD_NONE), "");

    /* ����Led�߳� */
    state = TclActivateThread(&ThreadLed, &error);
    TCLM_ASSERT((state == eSuccess), "");
    TCLM_ASSERT((error == TCLE_THREAD_NONE), "");
}


/* ������BOOT֮������main�����������ṩ */
int main(void)
{
    /* ע������ں˺���,�����ں� */
    TclStartKernel(&AppSetupEntry,
                   &CpuSetupEntry,
                   &EvbSetupEntry,
                   &EvbTraceEntry);
    return 1;
}


#endif