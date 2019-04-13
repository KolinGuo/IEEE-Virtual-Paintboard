//*****************************************************************************
//
// Application Name     - Virtual-Paintboard
// Application Overview - The application focuses on using ultrasonic sensors
//						  to implement a virtual sketchpad. Users can use
// 						  fingers to sketch, and IR remote to change 
//						  configurations. The sketch is displayed on a 128x128
//						  OLED display in real time. When finished sketching, 
//						  the picture is being sent to AWS and then sent back 
//						  to the user in digital format through some protocol.
//                        Due to hardware restrictions, the image is only 32x32
// Last Edited On 		- Jun 20, 2018
//
//*****************************************************************************

// Standard includes
#include <stdio.h>	// sprintf
#include <string.h>	// memcpy, strcpy, strcat, memset, strlen
#include <stdint.h>	// uint32_t
#include <stdlib.h>	// calloc, free
#include <stdbool.h>// bool
#include <string>
#include <bitset>

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
#include "i2c_if.h"
#include "gpio_if.h"
#include "timer_if.h"
#include "pin_mux_config.h"
#include "pin_mux_config.c"

// OLED interface includes
#include "Adafruit_GFX.h"
#include "Adafruit_SSD1351.h"
#include "Adafruit_SSD1351.c"
#include "glcdfont.h"
#include "test.h"

// VL53L1X sensor libraries
#include "ComponentObject.h"
#include "RangeSensor.h"
#include "SparkFun_VL53L1X.h"
#include "vl53l1x_class.h"
#include "vl53l1_error_codes.h"

// IR Data for Keys
#include "IRData.h"

//*****************************************************************************
//                      MACRO DEFINITIONS
//*****************************************************************************
#define UART_PRINT              Report
#define SUCCESS                 0
#define SPI_IF_BIT_RATE  100000

#define SAFERGETVL53L1X         1
#define SHORTDISTANCEMODE       1

// RESTful API
#define SERVER_NAME                "a1wj9u7sl0omv5.iot.us-west-2.amazonaws.com"
#define GOOGLE_DST_PORT             8443

// Certifications
#define SL_SSL_CA_CERT "/cert/rootca.der"
#define SL_SSL_PRIVATE "/cert/private.der"
#define SL_SSL_CLIENT  "/cert/client.der"

// Time information
//NEED TO UPDATE THIS FOR IT TO WORK!
#define DATE                10    /* Current Date */
#define MONTH               4     /* Month 1-12 */
#define YEAR                2019  /* Current year */
#define HOUR                18    /* Time - hours */
#define MINUTE              25    /* Time - minutes */
#define SECOND              0     /* Time - seconds */

// HTTP Post JSON format
#define POSTHEADER "POST /things/CC3200_thing/shadow HTTP/1.1\n\r"
#define HOSTHEADER "Host: a1wj9u7sl0omv5.iot.us-west-2.amazonaws.com\r\n"
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
int g_sensor12, g_sensor34;  // sensor measurement in milimeters
int g_pxx_map, g_pxy_map, g_pxx, g_pxy;
const int PX_LOC_LOWER_BOUND = 264;
std::string g_imageStr;

//Tells the main code if the a pulse is being read at the moment
int g_echowait1 = 0;
int g_echowait2 = 0;

// TLS Socket ID
int g_iTLSSockID;

int g_count;

volatile unsigned long  g_ulStatus = 0;//SimpleLink Status
unsigned long  g_ulGatewayIP = 0; //Network Gateway IP address
unsigned char  g_ucConnectionSSID[SSID_LEN_MAX+1]; //Connection SSID
unsigned char  g_ucConnectionBSSID[BSSID_LEN_MAX]; //Connection BSSID
signed char    *g_Host = (signed char*)SERVER_NAME;
SlDateTime g_time;
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
//                      LOCAL FUNCTION PROTOTYPES
//*****************************************************************************
static void IRIntHandler();
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
static int http_post(int type);

void ChangeRGBColor(unsigned int);
void ResetImage();
void GetIRInput();
void CollectSensorData();
void ConvertToPixelLoc();
void DrawSizedPixel(unsigned char);
void SaveToRGBArr();
void StartSketching();

//****************************************************************************
//                      LOCAL FUNCTION DEFINITIONS                          
//****************************************************************************

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
void SimpleLinkWlanEventHandler(SlWlanEvent_t *pWlanEvent)
{
    if(!pWlanEvent)
    {
        return;
    }

    switch(pWlanEvent->Event)
    {
        case SL_WLAN_CONNECT_EVENT:
        {
            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);

            //
            // Information about the connected AP (like name, MAC etc) will be
            // available in 'slWlanConnectAsyncResponse_t'
            // - applications can use it if required
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

        case SL_WLAN_DISCONNECT_EVENT:
        {
            slWlanConnectAsyncResponse_t*  pEventData = NULL;

            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_CONNECTION);
            CLR_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            pEventData = &pWlanEvent->EventData.STAandP2PModeDisconnected;

            // If the user has initiated 'Disconnect' request,
            //'reason_code' is SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION
            if(SL_WLAN_DISCONNECT_USER_INITIATED_DISCONNECTION == pEventData->reason_code)
            {
                UART_PRINT("[WLAN EVENT]Device disconnected from the AP: %s, "
                           "BSSID: %x:%x:%x:%x:%x:%x on application's request \n\r",
                           g_ucConnectionSSID,g_ucConnectionBSSID[0],
                           g_ucConnectionBSSID[1],g_ucConnectionBSSID[2],
                           g_ucConnectionBSSID[3],g_ucConnectionBSSID[4],
                           g_ucConnectionBSSID[5]);
            }
            else
            {
                UART_PRINT("[WLAN ERROR]Device disconnected from the AP AP: %s,"
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

        default:
        {
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
void SimpleLinkNetAppEventHandler(SlNetAppEvent_t *pNetAppEvent)
{
    if(!pNetAppEvent)
    {
        return;
    }

    switch(pNetAppEvent->Event)
    {
        case SL_NETAPP_IPV4_IPACQUIRED_EVENT:
        {
            SlIpV4AcquiredAsync_t *pEventData = NULL;

            SET_STATUS_BIT(g_ulStatus, STATUS_BIT_IP_AQUIRED);

            //Ip Acquired Event Data
            pEventData = &pNetAppEvent->EventData.ipAcquiredV4;

            //Gateway IP address
            g_ulGatewayIP = pEventData->gateway;

            UART_PRINT("[NETAPP EVENT] IP Acquired: IP=%d.%d.%d.%d ,"
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

        default:
        {
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
void SimpleLinkHttpServerCallback(SlHttpServerEvent_t *pHttpEvent,
                                  SlHttpServerResponse_t *pHttpResponse)
{
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
void SimpleLinkGeneralEventHandler(SlDeviceEvent_t *pDevEvent)
{
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
void SimpleLinkSockEventHandler(SlSockEvent_t *pSock)
{
    if(!pSock)
    {
        return;
    }

    //
    // This application doesn't work w/ socket - Events are not expected
    //
    switch( pSock->Event )
    {
        case SL_SOCKET_TX_FAILED_EVENT:
            switch( pSock->socketAsyncEvent.SockTxFailData.status)
            {
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
// SimpleLink Asynchronous Event Handlers -- End
//*****************************************************************************

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
static long ConfigureSimpleLinkToDefaultState()
{
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
    if (ROLE_STA != lMode)
    {
        if (ROLE_AP == lMode)
        {
            // If the device is in AP mode, we need to wait for this event
            // before doing anything
            while(!IS_IP_ACQUIRED(g_ulStatus))
            {
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
        if (ROLE_STA != lRetVal)
        {
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
    if(0 == lRetVal)
    {
        // Wait
        while(IS_CONNECTED(g_ulStatus))
        {
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
    g_Host = (signed char*)SERVER_NAME;
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
    SlSecParams_t secParams;
    SlSecParamsExt_t eapParams;
    long lRetVal = 0;
    unsigned char   pValues = 0;

    // Security parameters
    secParams.Key = (signed char*)SECURITY_KEY;
    secParams.KeyLen = strlen(SECURITY_KEY);
    secParams.Type = SECURITY_TYPE;

    // Enterprise extra parameters
    eapParams.User = (signed char*)USER_NAME;
    eapParams.UserLen = strlen(USER_NAME);
    eapParams.AnonUserLen = 0;
    eapParams.EapMethod = SL_ENT_EAP_METHOD_PEAP0_MSCHAPv2;

    UART_PRINT("Attempting connection to access point: ");
    UART_PRINT(SSID_NAME);
    if(secParams.Type == SL_SEC_TYPE_WPA_ENT)
        UART_PRINT(" with username \"%s\" ", USER_NAME);
    UART_PRINT("... ...\n\r");

    // 0 - Disable the server authnetication | 1 - Enable (this is the deafult)
    pValues = 0;
    sl_WlanSet(SL_WLAN_CFG_GENERAL_PARAM_ID, 19, 1 , &pValues);

    //lRetVal = sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, 0);
    lRetVal = sl_WlanConnect(SSID_NAME, strlen(SSID_NAME), 0, &secParams, &eapParams);
    ASSERT_ON_ERROR(lRetVal);

    // UART_PRINT(" Connected!!!\n\r");

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
//! \param  msg - Error message
//! \param  retVal - return value
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
//! \param  None
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
//! Board Initialization & Configuration
//!
//! \param  None
//!
//! \return None
//
//*****************************************************************************
static void
BoardInit(void)
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
//! This function posts a message through HTTP
//!
//! \param 	None
//!
//! \return  0 on success else error code
//!
//*****************************************************************************
static int http_post(int type){
	// Max send buffer 8 KB = 8192 bytes = 8192 chars
	// Max generated buffer 9449 bytes
    if (type == 1){
        g_imageStr.insert(0, "00");
    }
    char acSendBuff[9500]; 
    char acRecvbuff[10000];
    char cCLLength[10];
    char dataHeader[42] = DATAHEADER;//original = 42,
    char dataEnd[11] 	= DATAEND;
    char* pcBufHeaders = acSendBuff;
    int lRetVal = 0;
    int i;
    
    /***            Implementation   START           ***/
    // RGB String
    // MAX # of char per pixel : 9 (ff,ff,ff,)
    // MAX # of total char : 9 * 1024 - 1 = 9215 ~= 9 KB
	// May result in overflowed send buffer
    char strRGB[8192];
    g_imageStr.copy(strRGB, g_imageStr.size() + 1);
    printf("%s", strRGB);
    strRGB[g_imageStr.size() + 1] = '\0';
    char localRGB[10];
    

    /***            Implementation   END           ***/

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
    g_imageStr = "01";

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
			case LAST:  http_post(1);
			            g_imageStr = "11";
			            http_post(0);//make LAST send a flag after the last data to create the image; out of data call http_post
                break;
			case MUTE:   ResetImage();
			             http_post(0);
			             break;//
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
void CollectSensorData()
{
    /*  Get measurement from distanceSensor12  */
    PinMuxSensor12();
    distanceSensor12.startRanging(); //Write configuration bytes to initiate measurement

    if (SAFERGETVL53L1X)
        while (!distanceSensor12.checkForDataReady())
            MAP_UtilsDelay(800000);

    if (SAFERGETVL53L1X)
        uint8_t rangeStatus12 = distanceSensor12.getRangeStatus();

    g_sensor12 = distanceSensor12.getDistance(); //Get the result of the measurement from the sensor

    if (SAFERGETVL53L1X)
        distanceSensor12.clearInterrupt();

    distanceSensor12.stopRanging();

    float distanceInches12 = g_sensor12 * 0.0393701;
    float distanceFeet12 = distanceInches12 / 12.0;

    //void setROI(uint16_t x, uint16_t y); //Set the height and width of the ROI in SPADs, lowest possible option is 4. ROI is always centered.
    //uint16_t getROIX(); //Returns the width of the ROI in SPADs
    //uint16_t getROIY(); //Returns the height of the ROI in SPADs
    int roiWidth12 = distanceSensor12.getROIX();
    int roiHeight12 = distanceSensor12.getROIY();

    //Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\n\r", roiWidth12, roiHeight12, distance12);
    Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\tDistance(ft): %.2f\n\r", roiWidth12, roiHeight12, g_sensor12, distanceFeet12);


    /*  Get measurement from distanceSensor34  */
    PinMuxSensor34();
    distanceSensor34.startRanging(); //Write configuration bytes to initiate measurement

    if (SAFERGETVL53L1X)
        while (!distanceSensor34.checkForDataReady())
            MAP_UtilsDelay(800000);

    if (SAFERGETVL53L1X)
        uint8_t rangeStatus34 = distanceSensor34.getRangeStatus();

    g_sensor34 = distanceSensor34.getDistance(); //Get the result of the measurement from the sensor

    if (SAFERGETVL53L1X)
        distanceSensor34.clearInterrupt();

    distanceSensor34.stopRanging();

    float distanceInches34 = g_sensor34 * 0.0393701;
    float distanceFeet34 = distanceInches34 / 12.0;

    //void setROI(uint16_t x, uint16_t y); //Set the height and width of the ROI in SPADs, lowest possible option is 4. ROI is always centered.
    //uint16_t getROIX(); //Returns the width of the ROI in SPADs
    //uint16_t getROIY(); //Returns the height of the ROI in SPADs
    int roiWidth34 = distanceSensor34.getROIX();
    int roiHeight34 = distanceSensor34.getROIY();

    //Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\n\r", roiWidth34, roiHeight34, distance34);
    Report("ROIX: %d\tROIY: %d\tDistance(mm): %d\tDistance(ft): %.2f\n\r\n\r", roiWidth34, roiHeight34, g_sensor34, distanceFeet34);

    // Convert the sensor data to pixel location
    ConvertToPixelLoc();

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
	// 1mm measurement accuracy
    g_pxx_map = (g_sensor12 - PX_LOC_LOWER_BOUND);
    g_pxy_map = (g_sensor34 - PX_LOC_LOWER_BOUND);
    
    // out of bound -> don't draw anything
    if(g_pxx_map > 127)  g_pxx_map = -1;
    if(g_pxy_map > 127)  g_pxy_map = -1;
    
    // currently, g_pxx_map can be (-inf, 31]
    g_pxx = g_pxx_map;   //
    g_pxy = g_pxy_map;   //
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
    if(g_pxx_map < 0 || g_pxy_map < 0 || g_pxx_map > 127 || g_pxy_map > 127)
        return;
    // 200 px is maximum !!!
    std::bitset<8> b_pxx_map(g_pxx_map);
    std::bitset<8> b_pxy_map(g_pxy_map);
    std::bitset<8> b_red(g_red);
    std::bitset<8> b_green(g_green);
    std::bitset<8> b_blue(g_blue);

    char message[40];
    sprintf(message, "%b%b%b%b%b", b_pxx_map, b_pxy_map, b_red, b_green, b_blue);
    g_imageStr += message;
    printf("%s", message);
    g_count += 5;
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
    g_count = 0;
    // infinite loop
    while(1)
    {
        // Collect data from the two proximity sensors
        CollectSensorData();
        
        // Draw the pixel on OLED
        DrawSizedPixel(1);
        
        // Save the pixel to g_imageRGB array
        SaveToRGBArr();
        
        // Listen to IR sensor
        GetIRInput();
        if(g_count >= 190){
            http_post(1);
            g_imageStr = "";
            g_count = 0;
        }
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
	
	// Timer Initialization
    // Base address for first timer (sensing IR signal)
    g_ulBase = TIMERA0_BASE;
    // Configuring the timers
    // TimerA0 (IR sensor)
    Timer_IF_Init(PRCM_TIMERA0, g_ulBase, TIMER_CFG_PERIODIC, TIMER_A, 0);
    Timer_IF_IntSetup(g_ulBase, TIMER_A, TimerIntHandler);
    
	// Print on OLED
    fillScreen(BLACK);          // clear the display
	
    // I2C Init
    I2C_IF_Open(I2C_MASTER_MODE_FST);

    InitiateSensor12();
    InitiateSensor34();
	
    StartSketching();
}
