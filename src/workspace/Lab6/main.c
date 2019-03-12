//*****************************************************************************
//
// Application Name     - Lab6
// Application Overview - The application focuses on using ultrasonic sensors
//						  to implement a virtual sketchpad. Users can use
// 						  fingers to sketch, and IR remote to change 
//						  configurations. The sketch is displayed on a 128x128
//						  OLED display in real time. When finished sketching, 
//						  the picture is being sent to AWS and then sent back 
//						  to the user in digital format through some protocol.
//                        Due to hardware restrictions, the image is only 32x32
// Author               - Kolin Guo, Yizhi Tao
// Last Edited On 		- Jun 20, 2018
//
//*****************************************************************************

// Standard includes
#include <stdio.h>	// sprintf
#include <string.h>	// memcpy, strcpy, strcat, memset, strlen
#include <stdint.h>	// uint32_t
#include <stdlib.h>	// calloc, free
#include <stdbool.h>// bool

// Simplelink includes
#include "simplelink.h"

// Driverlib includes
#include "hw_types.h"
#include "hw_ints.h"
#include "hw_timer.h"
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
#include "timer.h"
#include "wlan.h"
#include "common.h"
#include "pin.h"

// Common interface includes
#include "uart_if.h"
#include "gpio_if.h"
#include "timer_if.h"
#include "pin_mux_config.h"

// OLED interface includes
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "glcdfont.h"
#include "test.h"

// Image processing includes
#include "camera_app.h"		// GenerateImage()

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define UART_PRINT              Report
#define SUCCESS                 0
#define SPI_IF_BIT_RATE  100000

// IR Data for Keys
#define KEY0 	0b00000000
#define KEY1 	0b10000000
#define KEY2 	0b01000000
#define KEY3 	0b11000000
#define KEY4 	0b00100000
#define KEY5 	0b10100000
#define KEY6 	0b01100000
#define KEY7 	0b11100000
#define KEY8 	0b00010000
#define KEY9 	0b10010000
#define ENTER 	0b00000010	// LAST
#define DEL 	0b00001000	// MUTE

// RESTful API
#define SERVER_NAME                "a1n18em14f4spc.iot.us-east-1.amazonaws.com"
#define GOOGLE_DST_PORT             8443

// Certifications
#define SL_SSL_CA_CERT "/cert/rootca.der"
#define SL_SSL_PRIVATE "/cert/private.der"
#define SL_SSL_CLIENT  "/cert/client.der"

// Time information
//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                7    /* Current Date */
#define MONTH               6     /* Month 1-12 */
#define YEAR                2018  /* Current year */
#define HOUR                18    /* Time - hours */
#define MINUTE              25    /* Time - minutes */
#define SECOND              0     /* Time - seconds */

// HTTP Post JSON format
#define POSTHEADER "POST /things/Kolin_CC3200_Spring18/shadow HTTP/1.1\n\r"
#define HOSTHEADER "Host: a1n18em14f4spc.iot.us-east-1.amazonaws.com\r\n"
#define CHEADER "Connection: Keep-Alive\r\n"
#define CTHEADER "Content-Type: text/plain\r\n"
#define CLHEADER1 "Content-Length: "
#define CLHEADER2 "\r\n\r\n"

#define DATAHEADER  "{\"state\": {\r\n\"desired\" : {\r\n\"default\" : \""
#define DATAEND     "\"\r\n}}}\r\n\r\n"

// Application specific status/error codes
typedef enum{
    // Choosing -0x7D0 to avoid overlap w/ host-driver's error codes
    LAN_CONNECTION_FAILED = -0x7D0,
    INTERNET_CONNECTION_FAILED = LAN_CONNECTION_FAILED - 1,
    DEVICE_NOT_IN_STATION_MODE = INTERNET_CONNECTION_FAILED - 1,

    STATUS_CODE_MAX = -0xBB8
}e_AppStatusCodes;

// Struct for DateTime
typedef struct
{
   /* time */
   unsigned long tm_sec;
   unsigned long tm_min;
   unsigned long tm_hour;
   /* date */
   unsigned long tm_day;
   unsigned long tm_mon;
   unsigned long tm_year;
   unsigned long tm_week_day; //not required
   unsigned long tm_year_day; //not required
   unsigned long reserved[3];
}SlDateTime;

//*****************************************************************************
//                 GLOBAL VARIABLES -- Start
//*****************************************************************************
static volatile unsigned long g_ulBase;		// TIMERA0_BASE
unsigned long g_ulTimerInts;
unsigned long g_IRReceive = 0;
bool isLeaderCodeReceived = 0, isFullyReceived = 0;

unsigned char g_red, g_green, g_blue;
unsigned int g_OLEDred, g_OLEDgreen, g_OLEDblue;    // RGB565
int g_pxx_map, g_pxy_map, g_pxx, g_pxy;
const int PX_LOC_LOWER_BOUND = 365;
unsigned long* g_imageRGB;	// 0x00RRGGBB for 1 px, a total of 32x32 px, 1024

// For U/S: U/S1 detects x location, U/S2 detects y location
const double TEMP = 1.0 / 8.0;
//Stores the pulse length
volatile uint32_t g_pulse1 = 0;
volatile uint32_t g_pulse2 = 0;
//Tells the main code if the a pulse is being read at the moment
int g_echowait1 = 0;
int g_echowait2 = 0;

// TLS Socket ID
int g_iTLSSockID;

volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
signed char    *g_Host = SERVER_NAME;
SlDateTime g_time;
#if defined(ccs)
extern void (* const g_pfnVectors[])(void);
#endif
#if defined(ewarm)
extern uVectorEntry __vector_table;
#endif
//*****************************************************************************
//                 GLOBAL VARIABLES -- End
//*****************************************************************************

//*****************************************************************************
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void IRIntHandler();
void US1IntHandler();
void US2IntHandler();
void TimerIntHandler();

static void BoardInit(void);
void SimpleLinkWlanEventHandler(SlWlanEvent_t *);
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *);
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *, SlHttpServerResponse_t *);
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *); 
void SimpleLinkSockEventHandler(SlSockEvent_t *);

static long InitializeAppVariables();
static long ConfigureSimpleLinkToDefaultState();
static long WlanConnect();
static int set_time();
static int tls_connect();
long printErrConvenience(char *, long);
int connectToAccessPoint();
static int http_post();

void ChangeRGBColor(unsigned int);
void ResetImage();
void GetIRInput();
void CollectUSData();
void ConvertToPixelLoc();
void DrawSizedPixel(unsigned char);
void SaveToRGBArr();
void StartSketching();

//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                          
//****************************************************************************

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
//! The interrupt handler for U/S Echo1 interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void US1IntHandler()
{
    //Clear interrupt flag. Since we only enabled on this is enough
    MAP_GPIOIntClear(GPIOA2_BASE,PIN_08);

    /*
    If it's a rising edge then set he timer to 0
    It's in periodic mode so it was in some random value
    */
    if (GPIOPinRead(GPIOA2_BASE,PIN_08) == 2)
    {
        HWREG(TIMERA1_BASE + TIMER_O_TAV ) = 0; // Loads value 0 into the timer.
        long ad = MAP_TimerLoadGet(TIMERA1_BASE,TIMER_A);
        TimerEnable(TIMERA1_BASE,TIMER_A);
        g_echowait1 = 1;
    }
    /*
    If it's a falling edge that was detected, then get the value of the counter
    */
    else
    {
        g_pulse1 = TimerValueGet(TIMERA1_BASE,TIMER_A);
        long af = GPIOPinRead(GPIOA2_BASE,PIN_08);
        TimerDisable(TIMERA1_BASE,TIMER_A);
        g_echowait1 = 0;
    }
}

//*****************************************************************************
//
//! The interrupt handler for U/S Echo2 interrupt.
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void US2IntHandler()
{
    //Clear interrupt flag. Since we only enabled on this is enough
    MAP_GPIOIntClear(GPIOA0_BASE,PIN_50);

    /*
    If it's a rising edge then set he timer to 0
    It's in periodic mode so it was in some random value
    */
    if (GPIOPinRead(GPIOA0_BASE,PIN_50) == PIN_50)
    {
        HWREG(TIMERA3_BASE + TIMER_O_TAV ) = 0; // Loads value 0 into the timer.
        long ad = MAP_TimerLoadGet(TIMERA3_BASE,TIMER_A);
        TimerEnable(TIMERA3_BASE,TIMER_A);
        g_echowait2 = 1;
    }
    /*
    If it's a falling edge that was detected, then get the value of the counter
    */
    else
    {
        g_pulse2 = TimerValueGet(TIMERA3_BASE,TIMER_A);
        long af = GPIOPinRead(GPIOA0_BASE,PIN_50);
        TimerDisable(TIMERA3_BASE,TIMER_A);
        g_echowait2 = 0;
    }
}

//*****************************************************************************
//
//! The interrupt handler for the timer interrupt. 
//! Used for collecting IR sensor data
//!
//! \param  None
//!
//! \return none
//
//*****************************************************************************
void TimerIntHandler()
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
// SimpleLink Asynchronous Event Handlers -- Start
//*****************************************************************************

//*****************************************************************************
//
//! \brief The Function Handles WLAN Events
//!
//! \param[in]  pWlanEvent - Pointer to WLAN Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent) {
    if(!pWlanEvent) {
        return;
    }

    switch(pWlanEvent->Event) {
        case SL_WLAN_CONNECT_EVENT: {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'.
            // Applications can use it if required
            //
            //  slWlanConnectAsyncResponse_t *pEventData = NULL;
            // pEventData = &pWlanEvent->EventData.STAandP2PModeWlanConnected;
            //

            // Copy new connection SSID and BSSID to global parameters
            memcpy(g_ucConnectionSSID,pWlanEvent->EventData.
                   STAandP2PModeWlanConnected.ssid_name,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.ssid_len);
            memcpy(g_ucConnectionBSSID,
                   pWlanEvent->EventData.STAandP2PModeWlanConnected.bssid,
                   SL_BSSID_LENGTH);

            UART_PRINT("[WLAN EVENT] STA Connected to the AP: %s , "
                       "BSSID: %x:%x:%x:%x:%x:%x\n\r",
                       g_ucConnectionSSID,g_ucConnectionBSSID[0],
                       g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                       g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                       g_ucConnectionBSSID[5]);
        }
        break;

        case SL_WLAN_DISCONNECT_EVENT: {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_USER_INITIATED_DISCONNECTION
            if(SL_USER_INITIATED_DISCONNECTION == pEventData->reason_code) {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s,"
                    "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on an ERROR..!! \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
            memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
        }
        break;

        default: {
            UART_PRINT("[WLAN EVENT] Unexpected event [0x%x]\n\r",
                       pWlanEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles network events such as IP acquisition, IP
//!           leased, IP released etc.
//!
//! \param[in]  pNetAppEvent - Pointer to NetApp Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent) {
    if(!pNetAppEvent) {
        return;
    }

    switch(pNetAppEvent->Event) {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT: {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d , "
                       "Gateway=%d.%d.%d.%d\n\r",
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.ip,0),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,3),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,2),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,1),
            SL_IPV4_BYTE(pNetAppEvent->EventData.ipAcquiredV4.gateway,0));
        }
        break;

        default: {
            UART_PRINT("[NETAPP EVENT] Unexpected event [0x%x] \n\r",
                       pNetAppEvent->Event);
        }
        break;
    }
}

//*****************************************************************************
//
//! \brief This function handles HTTP server events
//!
//! \param[in]  pServerEvent - Contains the relevant event information
//! \param[in]    pServerResponse - Should be filled by the user with the
//!                                      relevant response information
//!
//! \return None
//!
//****************************************************************************
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent, SlHttpServerResponse_t *pHttpResponse) {
    // Unused in this application
}

//*****************************************************************************
//
//! \brief This function handles General Events
//!
//! \param[in]     pDevEvent - Pointer to General Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent) {
    if(!pDevEvent) {
        return;
    }

    //
    // Most of the general errors are not FATAL are are to be handled
    // appropriately by the application
    //
    UART_PRINT("[GENERAL EVENT] - ID=[%d] Sender=[%d]\n\n",
               pDevEvent->EventData.deviceEvent.status,
               pDevEvent->EventData.deviceEvent.sender);
}

//*****************************************************************************
//
//! This function handles socket events indication
//!
//! \param[in]      pSock - Pointer to Socket Event Info
//!
//! \return None
//!
//*****************************************************************************
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock) {
    if(!pSock) {
        return;
    }

    switch( pSock->Event ) {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status) {
                case SL_ECLOSE:
                    UART_PRINT("[SOCK ERROR] - close socket (%d) operation "
                                "failed to transmit all queued packets\n\n",
                                    pSock->socketAsyncEvent.SockTxFailData.sd);
                    break;
                default:
                    UART_PRINT("[SOCK ERROR] - TX FAILED  :  socket %d , reason "
                                "(%d) \n\n",
                                pSock->socketAsyncEvent.SockTxFailData.sd, pSock->socketAsyncEvent.SockTxFailData.status);
                  break;
            }
            break;

        default:
            UART_PRINT("[SOCK EVENT] - Unexpected Event [%x0x]\n\n",pSock->Event);
          break;
    }
}

//*****************************************************************************
// SimpleLink Asynchronous Event Handlers -- End breadcrumb: s18_df
//*****************************************************************************


//*****************************************************************************
//
//! \brief This function initializes the application variables
//!
//! \param    0 on success else error code
//!
//! \return None
//!
//*****************************************************************************
static long InitializeAppVariables() {
    g_ulStatus = 0;
    g_ulGatewayIP = 0;
    g_Host = SERVER_NAME;
    memset(g_ucConnectionSSID,0,sizeof(g_ucConnectionSSID));
    memset(g_ucConnectionBSSID,0,sizeof(g_ucConnectionBSSID));
    return SUCCESS;
}

//*****************************************************************************
//! \brief This function puts the device in its default state. It:
//!           - Set the mode to STATION
//!           - Configures connection policy to Auto and AutoSmartConfig
//!           - Deletes all the stored profiles
//!           - Enables DHCP
//!           - Disables Scan policy
//!           - Sets Tx power to maximum
//!           - Sets power policy to normal
//!           - Unregister mDNS services
//!           - Remove all filters
//!
//! \param   none
//! \return  On success, zero is returned. On error, negative is returned
//*****************************************************************************
static long ConfigureSimpleLinkToDefaultState() {
    SlVersionFull   ver = {0};
    _WlanRxFilterOperationCommandBuff_t  RxFilterIdMask = {0};

    unsigned char ucVal = 1;
    unsigned char ucConfigOpt = 0;
    unsigned char ucConfigLen = 0;
    unsigned char ucPower = 0;

    long lRetVal = -1;
    long lMode = -1;

    lMode = sl_Start(0, 0, 0);
    ASSERT_ON_ERROR(lMode);

    // If the device is not in station-mode, try configuring it in station-mode
    if (ROLE_STA != lMode) {
        if (ROLE_AP == lMode) {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while(!IS_IP_ACQUIRED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
            }
        }

        // Switch to STA role and restart
        lRetVal = sl_WlanSetMode(ROLE_STA);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Stop(0xFF);
        ASSERT_ON_ERROR(lRetVal);

        lRetVal = sl_Start(0, 0, 0);
        ASSERT_ON_ERROR(lRetVal);

        // Check if the device is in station again
        if (ROLE_STA != lRetVal) {
            // We don't want to proceed if the device is not coming up in STA-mode
            return DEVICE_NOT_IN_STATION_MODE;
        }
    }

    // Get the device's version-information
    ucConfigOpt = SL_DEVICE_GENERAL_VERSION;
    ucConfigLen = sizeof(ver);
    lRetVal = sl_DevGet(SL_DEVICE_GENERAL_CONFIGURATION, &ucConfigOpt,
                                &ucConfigLen, (unsigned char *)(&ver));
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT("Host Driver Version: %s\n\r",SL_DRIVER_VERSION);
    UART_PRINT("Build Version %d.%d.%d.%d.31.%d.%d.%d.%d.%d.%d.%d.%d\n\r",
    ver.NwpVersion[0],ver.NwpVersion[1],ver.NwpVersion[2],ver.NwpVersion[3],
    ver.ChipFwAndPhyVersion.FwVersion[0],ver.ChipFwAndPhyVersion.FwVersion[1],
    ver.ChipFwAndPhyVersion.FwVersion[2],ver.ChipFwAndPhyVersion.FwVersion[3],
    ver.ChipFwAndPhyVersion.PhyVersion[0],ver.ChipFwAndPhyVersion.PhyVersion[1],
    ver.ChipFwAndPhyVersion.PhyVersion[2],ver.ChipFwAndPhyVersion.PhyVersion[3]);

    // Set connection policy to Auto + SmartConfig
    //      (Device's default connection policy)
    lRetVal = sl_WlanPolicySet(SL_POLICY_CONNECTION,
                                SL_CONNECTION_POLICY(1, 0, 0, 0, 1), NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove all profiles
    lRetVal = sl_WlanProfileDel(0xFF);
    ASSERT_ON_ERROR(lRetVal);



    //
    // Device in station-mode. Disconnect previous connection if any
    // The function returns 0 if 'Disconnected done', negative number if already
    // disconnected Wait for 'disconnection' event if 0 is returned, Ignore
    // other return-codes
    //
    lRetVal = sl_WlanDisconnect();
    if(0 == lRetVal) {
        // Wait
        while(IS_CONNECTED(g_ulStatus)) {
#ifndef SL_PLATFORM_MULTI_THREADED
              _SlNonOsMainLoopTask();
#endif
        }
    }

    // Enable DHCP client
    lRetVal = sl_NetCfgSet(SL_IPV4_STA_P2P_CL_DHCP_ENABLE,1,1,&ucVal);
    ASSERT_ON_ERROR(lRetVal);

    // Disable scan
    ucConfigOpt = SL_SCAN_POLICY(0);
    lRetVal = sl_WlanPolicySet(SL_POLICY_SCAN , ucConfigOpt, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Set Tx power level for station mode
    // Number between 0-15, as dB offset from max power - 0 will set max power
    ucPower = 0;
    lRetVal = sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID,
            WLAN_GENERAL_PARAM_OPT_STA_TX_POWER, 1, (unsigned char *)&ucPower);
    ASSERT_ON_ERROR(lRetVal);

    // Set PM policy to normal
    lRetVal = sl_WlanPolicySet(SL_POLICY_PM , SL_NORMAL_POLICY, NULL, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Unregister mDNS services
    lRetVal = sl_NetAppMDNSUnRegisterService(0, 0);
    ASSERT_ON_ERROR(lRetVal);

    // Remove  all 64 filters (8*8)
    memset(RxFilterIdMask.FilterIdMask, 0xFF, 8);
    lRetVal = sl_WlanRxFilterSet(SL_REMOVE_RX_FILTER, (_u8 *)&RxFilterIdMask,
                       sizeof(_WlanRxFilterOperationCommandBuff_t));
    ASSERT_ON_ERROR(lRetVal);

    lRetVal = sl_Stop(SL_STOP_TIMEOUT);
    ASSERT_ON_ERROR(lRetVal);

    InitializeAppVariables();

    return lRetVal; // Success
}

//****************************************************************************
//
//! \brief Connecting to a WLAN Accesspoint
//!
//!  This function connects to the required AP (SSID_NAME) with Security
//!  parameters specified in te form of macros at the top of this file
//!
//! \param  None
//!
//! \return  0 on success else error code
//!
//! \warning    If the WLAN connection fails or we don't aquire an IP
//!            address, It will be stuck in this function forever.
//
//****************************************************************************
static long WlanConnect() {
    SlSecParams_t secParams = {0};
    long lRetVal = 0;

    secParams.Key = SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    UART_PRINT("Attempting connection to access point: ");
    UART_PRINT(SSID_NAME);
    UART_PRINT("... ...");
    lRetVal = sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    ASSERT_ON_ERROR(lRetVal);

    UART_PRINT(" Connected!!!\n\r");


    // Wait for WLAN Event
    while((!IS_CONNECTED(g_ulStatus)) || (!IS_IP_ACQUIRED(g_ulStatus))) {
        // Toggle LEDs to Indicate Connection Progress
        _SlNonOsMainLoopTask();
        GPIO_IF_LedOff(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
        _SlNonOsMainLoopTask();
        GPIO_IF_LedOn(MCU_IP_ALLOC_IND);
        MAP_UtilsDelay(800000);
    }

    return SUCCESS;

}

//*****************************************************************************
//
//! This function updates the date and time of CC3200.
//!
//! \param None
//!
//! \return
//!     0 for success, negative otherwise
//!
//*****************************************************************************
static int set_time() {
    long retVal;

    g_time.tm_day = DATE;
    g_time.tm_mon = MONTH;
    g_time.tm_year = YEAR;
    g_time.tm_sec = HOUR;
    g_time.tm_hour = MINUTE;
    g_time.tm_min = SECOND;

    retVal = sl_DevSet(SL_DEVICE_GENERAL_CONFIGURATION,
                          SL_DEVICE_GENERAL_CONFIGURATION_DATE_TIME,
                          sizeof(SlDateTime),(unsigned char *)(&g_time));

    ASSERT_ON_ERROR(retVal);
    return SUCCESS;
}

//*****************************************************************************
//
//! This function demonstrates how certificate can be used with SSL.
//! The procedure includes the following steps:
//! 1) connect to an open AP
//! 2) get the server name via a DNS request
//! 3) define all socket options and point to the CA certificate
//! 4) connect to the server via TCP
//!
//! \param None
//!
//! \return  0 on success else error code
//! \return  LED1 is turned solid in case of success
//!    LED2 is turned solid in case of failure
//!
//*****************************************************************************
static int tls_connect() {
    SlSockAddrIn_t    Addr;
    int    iAddrSize;
    unsigned char    ucMethod = SL_SO_SEC_METHOD_TLSV1_2;
    unsigned int uiIP,uiCipher = SL_SEC_MASK_TLS_ECDHE_RSA_WITH_AES_256_CBC_SHA;
    long lRetVal = -1;
    int iSockID;

    lRetVal = sl_NetAppDnsGetHostByName(g_Host, strlen((const char *)g_Host),
                                    (unsigned long*)&uiIP, SL_AF_INET);

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't retrieve the host name \n\r", lRetVal);
    }

    Addr.sin_family = SL_AF_INET;
    Addr.sin_port = sl_Htons(GOOGLE_DST_PORT);
    Addr.sin_addr.s_addr = sl_Htonl(uiIP);
    iAddrSize = sizeof(SlSockAddrIn_t);
    //
    // opens a secure socket
    //
    iSockID = sl_Socket(SL_AF_INET,SL_SOCK_STREAM, SL_SEC_SOCKET);
    if( iSockID < 0 ) {
        return printErrConvenience("Device unable to create secure socket \n\r", lRetVal);
    }

    //
    // configure the socket as TLS1.2
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_SECMETHOD, &ucMethod,\
                               sizeof(ucMethod));
    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }
    //
    //configure the socket as ECDHE RSA WITH AES256 CBC SHA
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, SL_SO_SECURE_MASK, &uiCipher,\
                           sizeof(uiCipher));
    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }

    //
    //configure the socket with CA certificate - for server verification
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, \
                           SL_SO_SECURE_FILES_CA_FILE_NAME, \
                           SL_SSL_CA_CERT, \
                           strlen(SL_SSL_CA_CERT));

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }

    //configure the socket with Client Certificate - for server verification
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, \
                SL_SO_SECURE_FILES_CERTIFICATE_FILE_NAME, \
                                    SL_SSL_CLIENT, \
                           strlen(SL_SSL_CLIENT));

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }

    //configure the socket with Private Key - for server verification
    //
    lRetVal = sl_SetSockOpt(iSockID, SL_SOL_SOCKET, \
            SL_SO_SECURE_FILES_PRIVATE_KEY_FILE_NAME, \
            SL_SSL_PRIVATE, \
                           strlen(SL_SSL_PRIVATE));

    if(lRetVal < 0) {
        return printErrConvenience("Device couldn't set socket options \n\r", lRetVal);
    }


    /* connect to the peer device - Google server */
    lRetVal = sl_Connect(iSockID, ( SlSockAddr_t *)&Addr, iAddrSize);

    if(lRetVal < 0) {
        UART_PRINT("Device couldn't connect to server:");
        UART_PRINT(SERVER_NAME);
        UART_PRINT("\n\r");
        return printErrConvenience("Device couldn't connect to server \n\r", lRetVal);
    }
    else {
        UART_PRINT("Device has connected to the website:");
        UART_PRINT(SERVER_NAME);
        UART_PRINT("\n\r");
    }

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOn(MCU_GREEN_LED_GPIO);
    return iSockID;
}

//*****************************************************************************
//
//! This function provides a convenient way to print error messages
//!
//! \param 	msg - Error message
//! \param 	retVal - return value
//!
//! \return  0 on success else error code
//!
//*****************************************************************************
long printErrConvenience(char * msg, long retVal) {
    UART_PRINT(msg);
    GPIO_IF_LedOn(MCU_RED_LED_GPIO);
    return retVal;
}

//*****************************************************************************
//
//! This function serves to connect to Access Point
//!
//! \param 	None
//!
//! \return  0 on success else error code
//!
//*****************************************************************************
int connectToAccessPoint() {
    long lRetVal = -1;
    GPIO_IF_LedConfigure(LED1|LED3);

    GPIO_IF_LedOff(MCU_RED_LED_GPIO);
    GPIO_IF_LedOff(MCU_GREEN_LED_GPIO);

    lRetVal = InitializeAppVariables();
    ASSERT_ON_ERROR(lRetVal);

    //
    // Following function configure the device to default state by cleaning
    // the persistent settings stored in NVMEM (viz. connection profiles &
    // policies, power policy etc)
    //
    // Applications may choose to skip this step if the developer is sure
    // that the device is in its default state at start of applicaton
    //
    // Note that all profiles and persistent settings that were done on the
    // device will be lost
    //
    lRetVal = ConfigureSimpleLinkToDefaultState();
    if(lRetVal < 0) {
      if (DEVICE_NOT_IN_STATION_MODE == lRetVal)
          UART_PRINT("Failed to configure the device in its default state \n\r");

      return lRetVal;
    }

    UART_PRINT("Device is configured in default state \n\r");

    CLR_STATUS_BIT_ALL(g_ulStatus);

    ///
    // Assumption is that the device is configured in station mode already
    // and it is in its default state
    //
    UART_PRINT("Opening sl_start\n\r");
    lRetVal = sl_Start(0, 0, 0);
    if (lRetVal < 0 || ROLE_STA != lRetVal) {
        UART_PRINT("Failed to start the device \n\r");
        return lRetVal;
    }

    UART_PRINT("Device started as STATION \n\r");

    //
    //Connecting to WLAN AP
    //
    lRetVal = WlanConnect();
    if(lRetVal < 0) {
        UART_PRINT("Failed to establish connection w/ an AP \n\r");
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }

    UART_PRINT("Connection established w/ AP and IP is aquired \n\r");
    return 0;
}

//*****************************************************************************
//
//! This function posts a message through HTTP
//!
//! \param 	None
//!
//! \return  0 on success else error code
//!
//*****************************************************************************
static int http_post(){
	// Max send buffer 8 KB = 8192 bytes = 8192 chars
	// Max generated buffer 9449 bytes
    char acSendBuff[9500]; 
    char acRecvbuff[10000];
    char cCLLength[10];
    char dataHeader[42] = DATAHEADER;
    char dataEnd[11] 	= DATAEND;
    char* pcBufHeaders = acSendBuff;
    int lRetVal = 0;
    int i;
    
    // RGB String
    // MAX # of char per pixel : 9 (ff,ff,ff,)
    // MAX # of total char : 9 * 1024 - 1 = 9215 ~= 9 KB
	// May result in overflowed send buffer
    char strRGB[9216];
    char localRGB[10];
    
    for(i = 0; i < 1024; i++)
    {
        unsigned int red 	= (g_imageRGB[i] & 0x00FF0000) >> 16;
        unsigned int green 	= (g_imageRGB[i] & 0x0000FF00) >> 8;
        unsigned int blue 	= (g_imageRGB[i] & 0x000000FF);
        
		// Opencv requires 'BGR' channel order
		// Last pixel doesn't need comma
        if (i < 1023)
            sprintf(localRGB, "%x,%x,%x,", blue, green, red);
        else
            sprintf(localRGB, "%x,%x,%x", blue, green, red);

        if(i == 0)   	strcpy(strRGB, localRGB);
        else        	strcat(strRGB, localRGB);
    }

    int dataLength = strlen(strRGB) + strlen(dataHeader) + strlen(dataEnd);
	
    strcpy(pcBufHeaders, POSTHEADER);
    pcBufHeaders += strlen(POSTHEADER);		// POSTHEADER
    strcpy(pcBufHeaders, HOSTHEADER);
    pcBufHeaders += strlen(HOSTHEADER);		// HOSTHEADER
    strcpy(pcBufHeaders, CHEADER);
    pcBufHeaders += strlen(CHEADER);		// CHEADER
    strcpy(pcBufHeaders, CTHEADER);
    pcBufHeaders += strlen(CTHEADER);		// CTHEADER
	
    strcpy(pcBufHeaders, CLHEADER1);
    pcBufHeaders += strlen(CLHEADER1);		// CLHEADER1
    sprintf(cCLLength, "%d", dataLength);	// convert dataLength to a string
    strcpy(pcBufHeaders, cCLLength);
    pcBufHeaders += strlen(cCLLength);		// cCLLength
    strcpy(pcBufHeaders, CLHEADER2);
    pcBufHeaders += strlen(CLHEADER2);		// CLHEADER2

    strcpy(pcBufHeaders, dataHeader);
    pcBufHeaders += strlen(dataHeader);		// dataHeader
    strcpy(pcBufHeaders, strRGB);
    pcBufHeaders += strlen(strRGB);			// strRGB
    strcpy(pcBufHeaders, dataEnd);
    pcBufHeaders += strlen(dataEnd);		// dataEnd

    UART_PRINT(acSendBuff);

    //
    // Send the packet to the server */
    //
    lRetVal = sl_Send(g_iTLSSockID, acSendBuff, strlen(acSendBuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("POST failed. Error Number: %i\n\r",lRetVal);
        sl_Close(g_iTLSSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        return lRetVal;
    }
    lRetVal = sl_Recv(g_iTLSSockID, &acRecvbuff[0], sizeof(acRecvbuff), 0);
    if(lRetVal < 0) {
        UART_PRINT("Received failed. Error Number: %i\n\r",lRetVal);
        //sl_Close(iSSLSockID);
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
           return lRetVal;
    }
    else {
        acRecvbuff[lRetVal+1] = '\0';
        UART_PRINT(acRecvbuff);
        UART_PRINT("\n\r\n\r");
    }

    return 0;
}


//*****************************************************************************
//
//! Change RGB color according to IR sensor input
//!
//! \param  key     : an unsigned int, the key stroke
//!
//! \return None
//
//*****************************************************************************
void ChangeRGBColor(unsigned int key)
{
    switch(key)
    {
        case 0: g_red = 0;      g_green = 0;      g_blue = 0;      break;   // black
        case 1: g_red = 255;    g_green = 255;    g_blue = 255;    break;   // white
        case 2: g_red = 255;    g_green = 0;      g_blue = 0;      break;   // red
        case 3: g_red = 255;    g_green = 128;    g_blue = 0;      break;   // orange
        case 4: g_red = 255;    g_green = 255;    g_blue = 0;      break;   // yellow
        case 5: g_red = 0;      g_green = 255;    g_blue = 0;      break;   // green
        case 6: g_red = 0;      g_green = 255;    g_blue = 255;    break;   // cyan
        case 7: g_red = 0;      g_green = 128;    g_blue = 255;    break;   // azure
        case 8: g_red = 0;      g_green = 0;      g_blue = 255;    break;   // blue
        case 9: g_red = 127;    g_green = 0;      g_blue = 255;    break;   // purple
    }
}

//*****************************************************************************
//
//! Reset Image with DEL key
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void ResetImage()
{
    // Allocate imageRGB array and zero-initialize it
    free(g_imageRGB);

    g_imageRGB = (unsigned long*) calloc(1024, sizeof(unsigned long));

    // Print on OLED
    fillScreen(BLACK);          // clear the display
}

//*****************************************************************************
//
//! Get input from IR Sensor
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void GetIRInput()
{
    long lRetVal;
	// If we received a full sequence of nonzero information from IR
	if(isFullyReceived && g_IRReceive != 0)
	{
		g_IRReceive >>= 1;          // rearrange the bits
		g_IRReceive &= 0x0000FF00;
		g_IRReceive >>= 8;

		switch(g_IRReceive)
		{
			case KEY0:  ChangeRGBColor(0);		break;
			case KEY1:  ChangeRGBColor(1);		break;
			case KEY2:  ChangeRGBColor(2);      break;
			case KEY3:  ChangeRGBColor(3);      break;
			case KEY4:  ChangeRGBColor(4);      break;
			case KEY5:  ChangeRGBColor(5);      break;
			case KEY6:  ChangeRGBColor(6);      break;
			case KEY7:  ChangeRGBColor(7);      break;
			case KEY8:  ChangeRGBColor(8);      break;
			case KEY9:  ChangeRGBColor(9);      break;
			case ENTER: lRetVal = GenerateImage(g_imageRGB);
						// If stuck inside, failed generating image
                        while(lRetVal != 0);
			            http_post();
                break;
			case DEL:   ResetImage();			break;
			default:    Message("DIE\n\r");
		}
		g_IRReceive = 0;
	}
}

//*****************************************************************************
//
//! Collect data from the two ultrasonic sensors
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void CollectUSData()
{
    //Checks if a pulse read is in progress
    if((g_echowait1 != 1) && (g_echowait2 != 1))
    {
        //Does the required pulse of 10uS
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, PIN_64);
        UtilsDelay(266);
        MAP_GPIOPinWrite(GPIOA1_BASE, PIN_64, ~PIN_64);

        /*
        This makes the code wait for a reading to finish
        You can omit this part if you want the code to be non-blocking but
        reading is only ready when g_echowait = 0.
        */
        while(g_echowait1 != 0 || g_echowait2 != 0);

        //Converts the counter value to cm. TempValueC = (uint32_t)(147.5 - ((75.0*3.3 *(float)ADCValues[0])) / 4096.0);
        g_pulse1 = (uint32_t)(TEMP * g_pulse1);
        g_pulse1 = g_pulse1 / 58;

        g_pulse2 = (uint32_t)(TEMP * g_pulse2);
        g_pulse2 = g_pulse2 / 58;
        
        // Convert the sensor data to pixel location
        ConvertToPixelLoc();
    }
	//wait about 10ms until the next reading.
    UtilsDelay(800000);
}

//*****************************************************************************
//
//! Covert sensor data to pixel location
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void ConvertToPixelLoc()
{
    // px location of 0~31, later maps to 0~127   
	// 3mm measurement accuracy
    g_pxx_map = (g_pulse1 - PX_LOC_LOWER_BOUND) / 3;
    g_pxy_map = (g_pulse2 - PX_LOC_LOWER_BOUND) / 3;
    
    // out of bound -> don't draw anything
    if(g_pxx_map > 31)  g_pxx_map = -1;
    if(g_pxy_map > 31)  g_pxy_map = -1;
    
    // currently, g_pxx_map can be (-inf, 31]
    g_pxx = g_pxx_map << 2;   // * 4
    g_pxy = g_pxy_map << 2;   // * 4
    // now, g_pxx can be (-inf, 124]
}

//*****************************************************************************
//
//! Draw a pixel on OLED of specified size
//!
//! \param  px_size : pixel size
//!
//! \return None
//
//*****************************************************************************
void DrawSizedPixel(unsigned char px_size)
{
    g_OLEDred 	= g_red 	>> 3;    	// 5-bit
    g_OLEDgreen = g_green 	>> 2;     	// 6-bit
    g_OLEDblue 	= g_blue 	>> 3;       // 5-bit
    
    // OLED color format: 16-bit RGB565
    unsigned int OLEDcolor;
    OLEDcolor = (g_OLEDred << 11) | (g_OLEDgreen << 5) | (g_OLEDblue); 
    
    int i, j;
    for(i = g_pxx; i < g_pxx + px_size; i++)
        for(j = g_pxy; j < g_pxy + px_size; j++)
            drawPixel(i, j, OLEDcolor);
}

//*****************************************************************************
//
//! Save the color to g_imageRGB
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void SaveToRGBArr()
{
    // if out of bound (negative px location), don't save
    if(g_pxx_map < 0 || g_pxy_map < 0 || g_pxx_map > 31 || g_pxy_map > 31)
        return;
    
    int idx = g_pxx_map + g_pxy_map * 32;
    
    g_imageRGB[idx] = (g_red << 16) | (g_green << 8) | (g_blue);
}

//*****************************************************************************
//
//! Start sketching on the OLED display
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
void StartSketching()
{
    // Initial value for RGB color (white)
    g_red = 255;    g_green = 255;    g_blue = 255;
    
    // Allocate imageRGB array and zero-initialize it
	g_imageRGB = (unsigned long*) calloc(1024, sizeof(unsigned long));

    // infinite loop
    while(1)
    {
        // Collect data from the two ultrasonic sensors
        CollectUSData();
        
        // Draw the pixel on OLED
        DrawSizedPixel(4);
        
        // Save the pixel to g_imageRGB array
        SaveToRGBArr();
        
        // Listen to IR sensor
        GetIRInput();
    }
}

//*****************************************************************************
//
//! Main function
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
    // Enable the SPI module clock
    MAP_PRCMPeripheralClkEnable(PRCM_GSPI,PRCM_RUN_MODE_CLK);
    // Reset the peripheral
    MAP_PRCMPeripheralReset(PRCM_GSPI);
    // Reset SPI
    MAP_SPIReset(GSPI_BASE);
    // Configure SPI interface
    MAP_SPIConfigSetExpClk(GSPI_BASE,MAP_PRCMPeripheralClockGet(PRCM_GSPI),
                     SPI_IF_BIT_RATE,SPI_MODE_MASTER,SPI_SUB_MODE_0,
                     (SPI_SW_CTRL_CS |
                     SPI_4PIN_MODE |
                     SPI_TURBO_OFF |
                     SPI_CS_ACTIVEHIGH |
                     SPI_WL_8));
    // Enable SPI for communication
    MAP_SPIEnable(GSPI_BASE);
    // Initialize Adafruit
    Adafruit_Init();

	// GPIO Interrupt Initialization
    // IR Sensor
    MAP_GPIOIntRegister(GPIOA1_BASE, IRIntHandler);
    MAP_GPIOIntTypeSet(GPIOA1_BASE, 0x20, GPIO_FALLING_EDGE);
	unsigned long ulStatus = MAP_GPIOIntStatus(GPIOA1_BASE, false);
    MAP_GPIOIntClear(GPIOA1_BASE, ulStatus);            // clear interrupts on GPIOA1
    MAP_GPIOIntEnable(GPIOA1_BASE, 0x1);                // enable the IR sensor interrupt
	
    // U/S Echo1 OUT
    MAP_GPIOIntEnable(GPIOA2_BASE,PIN_08);
    MAP_GPIOIntTypeSet(GPIOA2_BASE, PIN_08,GPIO_BOTH_EDGES);
    MAP_GPIOIntRegister(GPIOA2_BASE,US1IntHandler);
    
    // U/S Echo2 OUT
    MAP_GPIOIntEnable(GPIOA0_BASE,PIN_50);
    MAP_GPIOIntTypeSet(GPIOA0_BASE, PIN_50,GPIO_BOTH_EDGES);
    MAP_GPIOIntRegister(GPIOA0_BASE,US2IntHandler);
    
	// Timer Initialization
    // Base address for first timer (sensing IR signal)
    g_ulBase = TIMERA0_BASE;
    // Configuring the timers
    // TimerA0 (IR sensor)
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerIntHandler);
    
	// TimerA1 (U/S Sensor #1)
    Timer_IF_Init(PRCM_TIMERA1, TIMERA1_BASE, TIMER_CFG_PERIODIC_UP, TIMER_A, 0);
    MAP_TimerEnable(TIMERA1_BASE,TIMER_A);
	
	// TimerA3 (U/S Sensor #2)
    Timer_IF_Init(PRCM_TIMERA3, TIMERA3_BASE, TIMER_CFG_PERIODIC_UP, TIMER_A, 0);
    MAP_TimerEnable(TIMERA3_BASE,TIMER_A);
    
	// Print on OLED
    fillScreen(BLACK);          // clear the display
	
	long lRetVal;
	unsigned char policyVal;
	//
    // Initializing the CC3200 networking layers
    //
    lRetVal = sl_Start(NULL, NULL, NULL);
    if(lRetVal < 0)
    {
        GPIO_IF_LedOn(MCU_RED_LED_GPIO);
        LOOP_FOREVER();
    }
	//
    // reset all network policies
    //
    lRetVal = sl_WlanPolicySet(  SL_POLICY_CONNECTION,
                    SL_CONNECTION_POLICY(0,0,0,0,0),
                    &policyVal,
                    1 /*PolicyValLen*/);

    //Connect the CC3200 to the local access point
    lRetVal = connectToAccessPoint();
    //Set time so that encryption can be used
    lRetVal = set_time();
    if(lRetVal < 0) {
        UART_PRINT("Unable to set time in the device");
        LOOP_FOREVER();
    }
    //Connect to the website with TLS encryption
    g_iTLSSockID = tls_connect();
    if(g_iTLSSockID < 0) {
        ERR_PRINT(g_iTLSSockID);
    }
	
    StartSketching();
}
