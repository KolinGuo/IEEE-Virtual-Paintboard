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
#define SAFERGETVL53L1X         1
#define SHORTDISTANCEMODE       1

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
SFEVL53L1X distanceSensor12;
SFEVL53L1X distanceSensor34;
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
//! PinMux to VL53L1X Sensor 12
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void PinMuxSensor12()
{
    PinModeSet(PIN_03, PIN_MODE_0);
    PinModeSet(PIN_04, PIN_MODE_0);
    // Configure PIN_01 for I2C0 I2C_SCL
    PinTypeI2C(PIN_01, PIN_MODE_1);
    // Configure PIN_02 for I2C0 I2C_SDA
    PinTypeI2C(PIN_02, PIN_MODE_1);
    Report("...PinMuxed to Sensor 12...\n\r");
}

//*****************************************************************************
//
//! PinMux to VL53L1X Sensor 34
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void PinMuxSensor34()
{
    PinModeSet(PIN_01, PIN_MODE_0);
    PinModeSet(PIN_02, PIN_MODE_0);
    // Configure PIN_03 for I2C0 I2C_SCL
    PinTypeI2C(PIN_03, PIN_MODE_5);
    // Configure PIN_04 for I2C0 I2C_SDA
    PinTypeI2C(PIN_04, PIN_MODE_5);
    Report("...PinMuxed to Sensor 34...\n\r");
}

//*****************************************************************************
//
//! Initiate VL53L1X sensor 12
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void InitiateSensor12()
{
    PinMuxSensor12();

    if (SAFERGETVL53L1X)
        while (!distanceSensor12.checkBootState())
            MAP_UtilsDelay(800000);

    if (distanceSensor12.begin() == false)
        Report("Sensor 12 online!\n\r");

    if (SAFERGETVL53L1X)
        Report("Using Safer Communication with VL53L1X Sensor 12...\n\r");

    if (SHORTDISTANCEMODE)
        distanceSensor12.setDistanceModeShort();

    uint8_t distMode = distanceSensor12.getDistanceMode();
    if (distMode == 1)
        Report("Sensor 12 in short distance mode\n\r\n\r");
    else if (distMode == 2)
        Report("Sensor 12 in long distance mode\n\r\n\r");
}

//*****************************************************************************
//
//! Initiate VL53L1X sensor 34
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void InitiateSensor34()
{
    PinMuxSensor34();

    if (SAFERGETVL53L1X)
        while (!distanceSensor34.checkBootState())
            MAP_UtilsDelay(800000);

    if (distanceSensor34.begin() == false)
        Report("Sensor 34 online!\n\r");

    if (SAFERGETVL53L1X)
        Report("Using Safer Communication with VL53L1X Sensor 34...\n\r");

    if (SHORTDISTANCEMODE)
        distanceSensor34.setDistanceModeShort();

    uint8_t distMode = distanceSensor34.getDistanceMode();
    if (distMode == 1)
        Report("Sensor 34 in short distance mode\n\r\n\r");
    else if (distMode == 2)
        Report("Sensor 34 in long distance mode\n\r\n\r");
}

//*****************************************************************************
//
//! Get Measurement from the VL53L1X Proximity Sensor12 and Sensor34 through I2C
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
        /*  Get measurement from distanceSensor12  */
        PinMuxSensor12();
        distanceSensor12.startRanging(); //Write configuration bytes to initiate measurement

        if (SAFERGETVL53L1X)
            while (!distanceSensor12.checkForDataReady())
                MAP_UtilsDelay(800000);

        if (SAFERGETVL53L1X)
            uint8_t rangeStatus12 = distanceSensor12.getRangeStatus();

        int distance12 = distanceSensor12.getDistance(); //Get the result of the measurement from the sensor

        if (SAFERGETVL53L1X)
            distanceSensor12.clearInterrupt();

        distanceSensor12.stopRanging();

        float distanceInches12 = distance12 * 0.0393701;
        float distanceFeet12 = distanceInches12 / 12.0;

        //void setROI(uint16_t x, uint16_t y); //Set the height and width of the ROI in SPADs, lowest possible option is 4. ROI is always centered.
        //uint16_t getROIX(); //Returns the width of the ROI in SPADs
        //uint16_t getROIY(); //Returns the height of the ROI in SPADs
        int roiWidth12 = distanceSensor12.getROIX();
        int roiHeight12 = distanceSensor12.getROIY();

        //Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\n\r", roiWidth12, roiHeight12, distance12);
        Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\tDistance(ft): %.2f\n\r", roiWidth12, roiHeight12, distance12, distanceFeet12);


        /*  Get measurement from distanceSensor34  */
        PinMuxSensor34();
        distanceSensor34.startRanging(); //Write configuration bytes to initiate measurement

        if (SAFERGETVL53L1X)
            while (!distanceSensor34.checkForDataReady())
                MAP_UtilsDelay(800000);

        if (SAFERGETVL53L1X)
            uint8_t rangeStatus34 = distanceSensor34.getRangeStatus();

        int distance34 = distanceSensor34.getDistance(); //Get the result of the measurement from the sensor

        if (SAFERGETVL53L1X)
            distanceSensor34.clearInterrupt();

        distanceSensor34.stopRanging();

        float distanceInches34 = distance34 * 0.0393701;
        float distanceFeet34 = distanceInches34 / 12.0;

        //void setROI(uint16_t x, uint16_t y); //Set the height and width of the ROI in SPADs, lowest possible option is 4. ROI is always centered.
        //uint16_t getROIX(); //Returns the width of the ROI in SPADs
        //uint16_t getROIY(); //Returns the height of the ROI in SPADs
        int roiWidth34 = distanceSensor34.getROIX();
        int roiHeight34 = distanceSensor34.getROIY();

        //Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\n\r", roiWidth34, roiHeight34, distance34);
        Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\tDistance(ft): %.2f\n\r\n\r", roiWidth34, roiHeight34, distance34, distanceFeet34);

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

    InitiateSensor12();
    InitiateSensor34();

    UART_PRINT("Initialization Complete...Starting Program...\n\r\n\r");

    GetSensorData();
}
