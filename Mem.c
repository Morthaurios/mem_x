
/*****************************************************************************************************************************
 * FILE NAME:   Mem.c
 * AUTHOR:      morthaurios
 * DESCRIPTION: Dummy Mem driver.
 *****************************************************************************************************************************/

/*****************************************************************************************************************************
 * INCLUDES
 *****************************************************************************************************************************/

#include "Mem.h"
#include "I2c.h"

/*****************************************************************************************************************************
 * MACROS
 *****************************************************************************************************************************/

#define MEM_I2C_ADDRESS (0x20U)

/*****************************************************************************************************************************
 * TYPE DEFINITIONS
 *****************************************************************************************************************************/

typedef enum
{
    E_WRITE_ENABLE      = 0x10,
    E_WRITE             = 0x12,
    E_SET_READ_ADDRESS  = 0x20
} Mem_Commands_t;

/*****************************************************************************************************************************
 * GLOBAL VARIABLES
 *****************************************************************************************************************************/


/*****************************************************************************************************************************
 * LOCAL VARIABLES
 *****************************************************************************************************************************/

static bool bInitialized = false;

/*****************************************************************************************************************************
 * FUNCTION DECLARATIONS
 *****************************************************************************************************************************/

static Platform_Return_t WriteEnable(void);
static Platform_Return_t WritePage(const uint16_t u16Address, const uint8_t * const pu8Buffer);
static Platform_Return_t SelectAddress(const uint16_t u16Address);


/*****************************************************************************************************************************
 * FUNCTION DEFINITIONS
 *****************************************************************************************************************************/
Platform_Return_t Mem_Init(void)
{
    /* Set some registers. */

    bInitialized = true;

    return E_OK;
}

Platform_Return_t Mem_Write(const uint16_t u16Address, const uint8_t * const pu8Buffer, const uint16_t u16BufferLength)
{
    Platform_Return_t eRetVal = E_NOT_OK;
    uint8_t u8NumberOfPages;
    uint8_t u8Counter;

    if( (NULL == pu8Buffer) && (u16BufferLength > 0U) &&((u16BufferLength & 0x00FFU) == 0U))
    {
        if(true == bInitialized)
        {
            u8NumberOfPages = (uint8_t) ( (u16BufferLength & 0xFF00U) >> 8);

            for(u8Counter = 0U; u8Counter < u8NumberOfPages; u8Counter++)
            {
                if(E_OK == WriteEnable())
                {
                    eRetVal = WritePage(u16Address, pu8Buffer);
                }
            }
        }
    }

    return eRetVal;
}

Platform_Return_t Mem_Read(const uint16_t u16Address, uint8_t * const pu8Buffer, const uint16_t u16BufferLength)
{
    Platform_Return_t eRetVal = E_NOT_OK;

    if( (NULL == pu8Buffer) && (u16BufferLength > 0U) )
    {
        if(true == bInitialized)
        {
            if(E_OK == SelectAddress(u16Address))
            {
                eRetVal = I2C_Receive(MEM_I2C_ADDRESS, pu8Buffer, u16BufferLength);
            }
        }
    }

    return eRetVal;
}

static Platform_Return_t WriteEnable(void)
{
    uint8_t u8Command = (uint8_t) E_WRITE_ENABLE;

    return I2C_Transmit(MEM_I2C_ADDRESS, &u8Command, 1U);
}

static Platform_Return_t WritePage(const uint16_t u16Address, const uint8_t * const pu8Buffer)
{
    uint8_t au8WriteBuffer[259];
    uint16_t u16Index;

    au8WriteBuffer[0] = (uint8_t) E_WRITE;
    au8WriteBuffer[1] = (uint8_t) ((u16Address & 0xFF00U) >> 8);
    au8WriteBuffer[2] = (uint8_t) (u16Address & 0x00FFU);

    for(u16Index = 0U; u16Index < 256U; u16Index++)
    {
        au8WriteBuffer[3U + u16Index] = pu8Buffer[u16Index];
    }

    return I2C_Transmit(MEM_I2C_ADDRESS, au8WriteBuffer, (uint16_t) sizeof(au8WriteBuffer));
}

static Platform_Return_t SelectAddress(const uint16_t u16Address)
{
    uint8_t au8Command[3];

    au8Command[0] = (uint8_t) E_SET_READ_ADDRESS;
    au8Command[1] = (uint8_t) ((u16Address & 0xFF00U) >> 8);
    au8Command[2] = (uint8_t) (u16Address & 0x00FFU);

    return I2C_Transmit(MEM_I2C_ADDRESS, au8Command, (uint16_t) sizeof(au8Command));
}