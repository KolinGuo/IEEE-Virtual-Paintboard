//*****************************************************************************
//
// Application Name     - Sensor_Test
// Application Overview - The application is a demo to read measurement data
//                        from proximity sensors
// Author               - Kolin Guo
//
//*****************************************************************************

// Standard includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_memmap.h"
#include "hw_common_reg.h"
#include "gpio.h"
#include "spi.h"
#include "rom.h"
#include "rom_map.h"
#include "interrupt.h"
#include "prcm.h"
#include "utils.h"
#include "uart.h"

// Common interface includes
#include "uart_if.h"
#include "i2c_if.h"
#include "pin_mux_config.h"
#include "pin_mux_config.c"

// VL53L1X sensor libraries
#include "ComponentObject.h"
#include "RangeSensor.h"
#include "SparkFun_VL53L1X.h"
#include "vl53l1x_class.h"
#include "vl53l1_error_codes.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define UART_PRINT              Report
#define FOREVER                 1
#define CONSOLE                 UARTA0_BASE
#define FAILURE                 -1
#define SUCCESS                 0
#define RETERR_IF_TRUE(condition) {if(condition) return FAILURE;}
#define RET_IF_ERR(Func)          {int iRetVal = (Func); \
                                   if (SUCCESS != iRetVal) \
                                     return  iRetVal;}

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
SFEVL53L1X distanceSensor;
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//                      STRUCT -- Start
//*****************************************************************************
typedef struct Acceleration_struct
{
    unsigned char accel_x, accel_y, accel_z;
} Acceleration;
//*****************************************************************************
//                      STRUCT -- End
//*****************************************************************************


//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void BoardInit(void);
int GetAcceleration(Acceleration*, signed char, signed char, signed char);
void GetSensorData();

//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                          
//****************************************************************************

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
//! Get Measurement from the VL53L1X Proximity Sensor through I2C
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void GetSensorData()
{
    while(1)
    {
        distanceSensor.startRanging(); //Write configuration bytes to initiate measurement
        int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
        distanceSensor.stopRanging();

        float distanceInches = distance * 0.0393701;
        float distanceFeet = distanceInches / 12.0;

        //void setROI(uint16_t x, uint16_t y); //Set the height and width of the ROI in SPADs, lowest possible option is 4. ROI is always centered.
        //uint16_t getROIX(); //Returns the width of the ROI in SPADs
        //uint16_t getROIY(); //Returns the height of the ROI in SPADs
        int roiWidth = distanceSensor.getROIX();
        int roiHeight = distanceSensor.getROIY();

        int signalRate = distanceSensor.getSignalRate();

        //Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\n\r", roiWidth, roiHeight, distance);
        Report("Signal Rate: %d\tROIX: %d\tROIY: %d\tDistance(mm): %d\tDistance(ft): %.2f\n\r", signalRate, roiWidth, roiHeight, distance, distanceFeet);

        MAP_UtilsDelay(8000000);
    }
}


//*****************************************************************************
//
//! Main function handling the Sensor_Test
//!
//! \param  None
//!
//! \return None
//! 
//*****************************************************************************
void main()
{
    // Initialize board configurations
    BoardInit();

    // Configure the pinmux settings for the peripherals exercised
    PinMuxConfig();

    // Initialising the Terminal
    InitTerm();

    // Clearing the Terminal
    ClearTerm();

    // I2C Init
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    if (distanceSensor.begin() == false)
        Report("Sensor online!\n\r");

    UART_PRINT("Initialization Complete...Starting Program...\n\r\n\r");

    GetSensorData();
}
