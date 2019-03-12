//*****************************************************************************
// camera_app.c
//
// camera application macro & APIs
//
// Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
// 
// 
//  Redistribution and use in source and binary forms, with or without 
//  modification, are permitted provided that the following conditions 
//  are met:
//
//    Redistributions of source code must retain the above copyright 
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the   
//    distribution.
//
//    Neither the name of Texas Instruments Incorporated nor the names of
//    its contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
//  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
//  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
//  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
//  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
//  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
//  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//*****************************************************************************
//*****************************************************************************
//	Modified by Kolin Guo and Yizhi Tao for EEC 172 Final Project
//*****************************************************************************
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Driverlib includes
#include "hw_types.h"
#include "hw_memmap.h"
#include "rom_map.h"
#include "rom.h"
#include "prcm.h"
#include "timer.h"
#include "utils.h"

// Simplelink includes
#include "fs.h"

// Common interface includes
#include "uart_if.h"
#include "common.h"

#include "camera_app.h"

//*****************************************************************************
// Macros
//*****************************************************************************
#define USER_FILE_NAME          "ImageRGBData.bin"
#define CAMERA_CAPTURE_FAILED 	(-5)


//*****************************************************************************
//                      GLOBAL VARIABLES
//*****************************************************************************
unsigned long g_image_buffer[NUM_OF_4B_CHUNKS];
unsigned long g_frame_size_in_bytes = FRAME_SIZE_IN_BYTES;

typedef enum pictureRequest{
    NO_PICTURE = 0x00,
    SINGLE_HIGH_RESOLUTION = 0x01,
    STREAM_LOW_RESOLUTION = 0x02
      
}e_pictureRequest;

typedef enum pictureFormat{
    RAW_10BIT = 0,
    ITU_R_BT601,
    YCbCr_4_2_2,
    YCbCr_4_2_0,
    RGB_565,
    RGB_555,
    RGB_444

}e_pictureFormat;

typedef enum pictureResolution{
    QVGA = 0,
    VGA,
    SVGA,
    XGA,
    uXGA

}e_pictureResolution;

/****************************************************************************/
/*                      LOCAL FUNCTION PROTOTYPES                           */
/****************************************************************************/
long GenerateImage(unsigned long* image);


//*****************************************************************************
//
//!     CaptureImage 
//!     Configures DMA and starts the Capture. Post Capture writes to SFLASH 
//!    
//!    \param                      None  
//!     \return                     0 - Success
//!                                   Negative - Failure
//!                               
//
//*****************************************************************************

long GenerateImage(unsigned long* image)
{
    long lFileHandle;
    unsigned long ulToken;
    long lRetVal;

	// g_image_buffer <-- image
	memcpy(g_image_buffer, image, g_frame_size_in_bytes);
	
    //
    // NVMEM File Open to write to SFLASH
    //
    lRetVal = sl_FsOpen((unsigned char *)USER_FILE_NAME,
    FS_MODE_OPEN_CREATE(65535,_FS_FILE_OPEN_FLAG_COMMIT|_FS_FILE_PUBLIC_WRITE),
                        &ulToken,
                        &lFileHandle);
    //
    // Error handling if File Operation fails
    //
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(CAMERA_CAPTURE_FAILED);
    }
    //
    // Close the created file
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    ASSERT_ON_ERROR(lRetVal);

    //
    // Open the file for Write Operation
    //
    lRetVal = sl_FsOpen((unsigned char *)USER_FILE_NAME,
                        FS_MODE_OPEN_WRITE,
                        &ulToken,
                        &lFileHandle);
    //
    // Error handling if File Operation fails
    //
    if(lRetVal < 0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(CAMERA_CAPTURE_FAILED);
    }
    //
    // Write the Image Buffer 
    //
    lRetVal =  sl_FsWrite(lFileHandle, 0,	// Kolin Changed
                      (unsigned char *)g_image_buffer, sizeof(g_image_buffer));        
    //
    // Error handling if file operation fails
    //
    if (lRetVal <0)
    {
        lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
        ASSERT_ON_ERROR(CAMERA_CAPTURE_FAILED);
    }
    //
    // Close the file post writing the image
    //
    lRetVal = sl_FsClose(lFileHandle, 0, 0, 0);
    ASSERT_ON_ERROR(lRetVal);

    return SUCCESS;
}
