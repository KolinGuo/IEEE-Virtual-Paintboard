//*****************************************************************************
//
// Application Name     - Decoding
// Application Overview - The application focuses on decoding the IR signal
// Author               - Kolin Guo, Yizhi Tao
//
//*****************************************************************************

// Standard include
#include <stdio.h>
#include <stdbool.h>

// Driverlib includes
#include "hw_types.h"
#include "interrupt.h"
#include "hw_ints.h"
#include "hw_apps_rcm.h"
#include "hw_common_reg.h"
#include "prcm.h"
#include "rom.h"
#include "rom_map.h"
#include "hw_memmap.h"
#include "timer.h"
#include "gpio.h"
#include "utils.h"

// Common interface includes
#include "timer_if.h"
#include "gpio_if.h"
#include "uart_if.h"

#include "pin_mux_config.h"

// IR Data for Keys
#include "IRData.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define APPLICATION_VERSION        "1.1.1"


//*****************************************************************************
//                      Global Variables for Vector Table
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif

//*****************************************************************************
//
// Globals used by the interrupt handlers.
//
//*****************************************************************************
static volatile unsigned long g_ulSysTickValue;
static volatile unsigned long g_ulBase;
static volatile unsigned long g_ulIntClearVector;
unsigned long g_ulTimerInts;
unsigned long g_IRReceive = 0;
bool isLeaderCodeReceived = 0, isFullyReceived = 0;

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void IRIntHandler();
void TimerIntHandler();
static void BoardInit();

//*****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS
//*****************************************************************************

//*****************************************************************************
//
//! The interrupt handler for IR sensor interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
static void IRIntHandler()
{
    unsigned long ulStatus;

    // clear interrupts on GPIOA1
    ulStatus = MAP_GPIOIntStatus (GPIOA1_BASE, true);
    MAP_GPIOIntClear(GPIOA1_BASE, ulStatus);

    Timer_IF_Stop(g_ulBase, TIMER_A);

    if(isLeaderCodeReceived && g_ulTimerInts == 1)     // if 0 is received
        g_IRReceive <<= 1;
    else if(isLeaderCodeReceived && g_ulTimerInts == 2)     // if 1 is received
    {
        g_IRReceive++;
        // this will lose the MSB bit (1st bit we received) which is useless here
        g_IRReceive <<= 1;
    }
    else if(isLeaderCodeReceived && g_ulTimerInts > 15)     // if fully received
    {
        isLeaderCodeReceived = 0;
        isFullyReceived = 1;
    }
    else if(g_ulTimerInts == 13)     // if leader code received
    {
        g_IRReceive = 0;
        isLeaderCodeReceived = 1;
        isFullyReceived = 0;
    }

    // Reset Timer Interrupt Count
    g_ulTimerInts = 0;
    Timer_IF_Start(g_ulBase, TIMER_A, 1);
}

//*****************************************************************************
//
//! The interrupt handler for the timer interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void TimerIntHandler(void)
{
    // Clear the timer interrupt.
    Timer_IF_InterruptClear(g_ulBase);

    g_ulTimerInts++;
}

//*****************************************************************************
//
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void BoardInit(void)
{
/* In case of TI-RTOS vector table is initialize by OS itself */
#ifndef USE_TIRTOS
  //
  // Set vector table base
  //
#if defined(ccs)
    MAP_IntVTableBaseSet((unsigned long)&g_pfnVectors[0]);
#endif
#if defined(ewarm)
    MAP_IntVTableBaseSet((unsigned long)&__vector_table);
#endif
#endif
    //
    // Enable Processor
    //
    MAP_IntMasterEnable();
    MAP_IntEnable(FAULT_SYSTICK);

    PRCMCC3200MCUInit();
}

//*****************************************************************************
//
//!    main function demonstrates the use of the timers to generate
//! periodic interrupts.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
int main(void)
{
    unsigned long ulStatus;

    // Initialize board configurations
    BoardInit();
    // Pinmuxing for LEDs
    PinMuxConfig();

    InitTerm();
    ClearTerm();

    // configure the LED GREEN, indicate data availability from IR sensor
    GPIO_IF_LedConfigure(LED3);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);

    // Register the interrupt handlers
    MAP_GPIOIntRegister(GPIOA1_BASE, IRIntHandler);
    // Configure both edge interrupts on IR Sensor
    MAP_GPIOIntTypeSet(GPIOA1_BASE, 0x20, GPIO_FALLING_EDGE);
    ulStatus = MAP_GPIOIntStatus(GPIOA1_BASE, false);
    MAP_GPIOIntClear(GPIOA1_BASE, ulStatus);            // clear interrupts on GPIOA1
    MAP_GPIOIntEnable(GPIOA1_BASE, 0x1);                // enable the IR sensor interrupt

    // Base address for first timer
    g_ulBase = TIMERA0_BASE;
    // Configuring the timers
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);
    // Setup the interrupts for the timer timeouts.
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerIntHandler);

    Message("Start listening...\n\r");

    // Loop forever while the timers run.
    while(1)
    {
        // If we received a full sequence of nonzero information
        if(isFullyReceived && g_IRReceive != 0)
        {
            g_IRReceive >>= 1;          // rearrange the bits
            g_IRReceive &= 0x0000FF00;
            g_IRReceive >>= 8;

            switch(g_IRReceive)
            {
                case KEY0:          Message("0\n\r");               break;
                case KEY1:          Message("1\n\r");               break;
                case KEY2:          Message("2\n\r");               break;
                case KEY3:          Message("3\n\r");               break;
                case KEY4:          Message("4\n\r");               break;
                case KEY5:          Message("5\n\r");               break;
                case KEY6:          Message("6\n\r");               break;
                case KEY7:          Message("7\n\r");               break;
                case KEY8:          Message("8\n\r");               break;
                case KEY9:          Message("9\n\r");               break;
                case ENTER:         Message("ENTER\n\r");           break;
                case LAST:          Message("LAST\n\r");            break;
                case MUTE:          Message("MUTE\n\r");            break;
                case VOL_PLUS:      Message("VOL + OR RIGHT\n\r");  break;
                case VOL_MINUS:     Message("VOL - OR LEFT\n\r");   break;
                case CH_PLUS:       Message("CH + OR UP\n\r");      break;
                case CH_MINUS:      Message("CH - OR DOWN\n\r");    break;
                case EXITTOTV:      Message("EXIT TO TV\n\r");      break;
                case INFO:          Message("INFO\n\r");            break;
                case OK:            Message("OK\n\r");              break;
                case MENU:          Message("MENU\n\r");            break;
                case ON_DEMAND:     Message("ON DEMAND\n\r");       break;
                case TV_VIDEO:      Message("TV/VIDEO\n\r");        break;
                case POWER:         Message("POWER\n\r");           break;
                default:            Message("Unknown Key\n\r");
            }
            g_IRReceive = 0;
        }
    }
}
