/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/GPIO.h>
#include <ti/drivers/UART.h>

/* Example/Board Header files */
#include "Board.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <bLogic.h>

#define PERIOD_MS 1000 //period of message sending in ms

#define TASKSTACKSIZE     5120

static int64_t time = 788125453;//-1;

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];
/*
 *  ======== echoFxn ========
 *  Task for this function is created statically. See the project's .cfg file.
 */
Semaphore_Handle sem;
Void echoFxn(UArg arg0, UArg arg1)
{
    while (1) {
        Semaphore_pend(sem, BIOS_WAIT_FOREVER);
        sendPacket(time);
    }
}

Void timerFxn(UArg arg0)
{
    time++;
    Semaphore_post(sem);
    GPIO_toggle(Board_sync);
    GPIO_toggle(Board_LED0);
}

/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Board_initGeneral();
    Board_initGPIO();
    Board_initUART();

    /* Construct BIOS objects */
    Task_Params taskParams;

    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    taskParams.instance->name = "echo";
    Task_construct(&task0Struct, (Task_FuncPtr)echoFxn, &taskParams, NULL);


    Semaphore_Params smP;
    Semaphore_Params_init(&smP);
    smP.mode = Semaphore_Mode_BINARY;
    sem = Semaphore_create( 0, &smP, NULL );

    Clock_Params clkPr;
    Clock_Params_init(&clkPr);
    clkPr.arg = NULL;
    clkPr.period = PERIOD_MS;
    clkPr.startFlag = 1;
    Clock_create(timerFxn, 1000, &clkPr, NULL);

    bLogic_Init();

    System_printf("Master validator is now in operation.");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}
