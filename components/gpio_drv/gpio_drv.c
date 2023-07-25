#include <stdio.h>
#include "esp_log.h"
#include "gpio_drv.h"
#include "driver/gpio.h"
#include "esp_io_expander.h"
#include "esp_io_expander_tca95xx_16bit.h"
#include "esp_console.h"
#include "cmdline.h"
#include "i2c_drv.h"

#define FEATURE_EXP_GPIO

#if defined(FEATURE_EXP_GPIO)
#define PCAL6416_I2C_ADD    0x20
#endif

static const char* TAG = "gpio";

esp_io_expander_handle_t xEXP_IO_HANDLE;

static void GPIO_CMD_View( void );
static void GPIO_CMD_Output( int argc, char **argv );

Cmd_stype GPIO_CmdList[] =
{
  { 1, "view",  NULL,       GPIO_CMD_View,       NULL },
  { 3, "wr",    NULL,       NULL,                GPIO_CMD_Output },
  { 1, "test",  NULL,       do_i2cdetect_cmd,    NULL },

#if 0
  { 1, "view",    NULL,       CHARGER_DBG_View,       NULL },
  { 1, "battoff", NULL,       CHARGER_DBG_BattOff,    NULL },

  { 2, "test",    "on",       CHARGER_DBG_TestOn,     NULL },
  { 2, "test",    "off",      CHARGER_DBG_TestOff,    NULL },
  { 3, "test",    "temp",     NULL,                   CHARGER_DBG_SetTemp },
  { 3, "test",    "volt",     NULL,                   CHARGER_DBG_SetVolt },  
#endif
	/* End Flag */
	{ 0, NULL,    NULL,     NULL,         NULL }
};

typedef enum GPIO_Grp_etype
{
  eGPIO_GRP_MCU = 0,
  eGPIO_GRP_EXP,
  
  eGPIO_GRP_MAX
}GPIO_Grp_etype;

typedef struct GPIO_CFG_DATA_stype{

  char            *cName;     /* gpio user name */
  GPIO_Grp_etype  xGrp;
  GPIO_etype      xGpioEnum;  /* 사용자 정의 gpio enum */
  uint8_t         ucGpioNum;  /* esp32 gpio number */
  uint8_t         ucOutInit;  /* 출력 초기값 */
  gpio_config_t   xGpioCfg;

}GPIO_CFG_DATA_stype;


GPIO_CFG_DATA_stype xGPIO_CFG_TABLE[eGPIO_MAX] = 
{
  {
  /* name             GPIO_Grp_etype      gpio_user_enum        gpio_num                gpio_out_level */
    "EXP_RESET",      eGPIO_GRP_MCU,      eEXP_RESET,           12,                     1,
    { (1ULL<<12),     GPIO_MODE_OUTPUT,   GPIO_PULLUP_DISABLE,  GPIO_PULLUP_PULLDOWN,   GPIO_INTR_DISABLE },
  },

  {
  /* name            GPIO_Grp_etype      gpio_user_enum        gpio_num                gpio_out_level */
    "MDM_PWR_EN",    eGPIO_GRP_EXP,      eMDM_PWR_EN,          3,                      0,
    { (1ULL<<3),     GPIO_MODE_OUTPUT,   GPIO_PULLUP_DISABLE,  GPIO_PULLUP_PULLDOWN,   GPIO_INTR_DISABLE },
  },

  {
  /* name            GPIO_Grp_etype      gpio_user_enum        gpio_num                gpio_out_level */
    "MDM_PWRKEY",    eGPIO_GRP_EXP,      eMDM_PWRKEY,          4,                      0,
    { (1ULL<<4),     GPIO_MODE_OUTPUT,   GPIO_PULLUP_DISABLE,  GPIO_PULLUP_PULLDOWN,   GPIO_INTR_DISABLE },
  },

  {
  /* name            GPIO_Grp_etype      gpio_user_enum        gpio_num                gpio_out_level */
    "MDM_DTR",       eGPIO_GRP_EXP,      eMDM_DTR,             5,                      0,
    { (1ULL<<5),     GPIO_MODE_OUTPUT,   GPIO_PULLUP_DISABLE,  GPIO_PULLUP_PULLDOWN,   GPIO_INTR_DISABLE },
  },

  {
  /* name            GPIO_Grp_etype      gpio_user_enum        gpio_num                gpio_out_level */
    "MDM_RESET",     eGPIO_GRP_EXP,      eMDM_RESET,           8,                      0,
    { (1ULL<<8),     GPIO_MODE_OUTPUT,   GPIO_PULLUP_DISABLE,  GPIO_PULLUP_PULLDOWN,   GPIO_INTR_DISABLE },
  },

  {
  /* name            GPIO_Grp_etype      gpio_user_enum        gpio_num                gpio_out_level */
    "n232_SLEEP",    eGPIO_GRP_EXP,      e232_SLEEP,           7,                      1,
    { (1ULL<<7),     GPIO_MODE_OUTPUT,   GPIO_PULLUP_DISABLE,  GPIO_PULLUP_PULLDOWN,   GPIO_INTR_DISABLE },
  },

};


void GPIO_MCU_Init(void)
{
  uint32_t i;
  GPIO_CFG_DATA_stype *pxGpioCfg;

  for( i = 0 ; i < eGPIO_MAX; i++ )
  {
    pxGpioCfg = &xGPIO_CFG_TABLE[i];
    
    if( pxGpioCfg->xGrp != eGPIO_GRP_MCU )
      continue;

    if( pxGpioCfg->xGpioCfg.mode == GPIO_MODE_OUTPUT ){
      gpio_set_level( pxGpioCfg->ucGpioNum, pxGpioCfg->ucOutInit );
    }

    gpio_config( &pxGpioCfg->xGpioCfg );
  }
}


static void GPIO_MCU_Output( GPIO_etype eGpioNum, int LoHi )
{
  uint32_t i;
  GPIO_CFG_DATA_stype *pxGpioCfg;

  for( i = 0 ; i < eGPIO_MAX; i++ )
  {
    pxGpioCfg = &xGPIO_CFG_TABLE[i];
    
    if( pxGpioCfg->xGrp != eGPIO_GRP_MCU )
      continue;

    if( eGpioNum == xGPIO_CFG_TABLE[i].xGpioEnum )
    {
      gpio_set_level( pxGpioCfg->ucGpioNum, LoHi);
      pxGpioCfg->ucOutInit = LoHi;
      break;
    }
  }
}

static void GPIO_MCU_View( void )
{
  uint32_t i;
  GPIO_CFG_DATA_stype *pxGpioCfg;
  int in,out;

  /*            012345678901234567890123456789 */
  ESP_LOGI(TAG,"Enum Name            Dir In Out ");
  
  for( i = 0 ; i < eGPIO_MAX; i++ )
  {
    pxGpioCfg = &xGPIO_CFG_TABLE[i];

    if( pxGpioCfg->xGrp != eGPIO_GRP_MCU )
      continue;
    
    in = gpio_get_level(pxGpioCfg->ucGpioNum);
    out = pxGpioCfg->ucOutInit;
    
    ESP_LOGI(TAG,"[%02d] %-16s %s   %d  %d", pxGpioCfg->xGpioEnum, pxGpioCfg->cName, (pxGpioCfg->xGpioCfg.mode == GPIO_MODE_INPUT) ? "I":"O" ,in, out);
  }
}

#if defined(FEATURE_EXP_GPIO)
void GPIO_EXP_Init(void)
{
  uint32_t i;
  GPIO_CFG_DATA_stype *pxGpioCfg;

  if( esp_io_expander_new_i2c_tca95xx_16bit(I2C_NUM_0, PCAL6416_I2C_ADD, &xEXP_IO_HANDLE) != ESP_OK )
  {
    ESP_LOGE(TAG," EXP Init Fail ");
    return;
  } 

  for( i = 0 ; i < eGPIO_MAX; i++ )
  {
    pxGpioCfg = &xGPIO_CFG_TABLE[i];

    if( pxGpioCfg->xGrp != eGPIO_GRP_EXP )
      continue;
    
    if( pxGpioCfg->xGpioCfg.mode == GPIO_MODE_OUTPUT )
    {
      esp_io_expander_set_dir(xEXP_IO_HANDLE, (0x1<<pxGpioCfg->ucGpioNum), IO_EXPANDER_OUTPUT);
      esp_io_expander_set_level(xEXP_IO_HANDLE, (0x1<<pxGpioCfg->ucGpioNum), pxGpioCfg->ucOutInit );
    }else if( pxGpioCfg->xGpioCfg.mode == GPIO_MODE_INPUT )
    {
      esp_io_expander_set_dir(xEXP_IO_HANDLE, (0x1<<pxGpioCfg->ucGpioNum), IO_EXPANDER_INPUT);
    }else
    {
      ESP_LOGE(TAG," Init fail ");
    }
  }

}


static void GPIO_EXP_Output( GPIO_etype eGpioNum, int LoHi )
{
  uint32_t i;
  GPIO_CFG_DATA_stype *pxGpioCfg;

  for( i = 0 ; i < eGPIO_MAX; i++ )
  {
    pxGpioCfg = &xGPIO_CFG_TABLE[i];
    
    if( pxGpioCfg->xGrp != eGPIO_GRP_EXP )
      continue;

    if( eGpioNum == xGPIO_CFG_TABLE[i].xGpioEnum )
    {
      esp_io_expander_set_level(xEXP_IO_HANDLE, (0x1<<pxGpioCfg->ucGpioNum), LoHi );
      pxGpioCfg->ucOutInit = LoHi;
      break;
    }
  }  
}


static void GPIO_EXP_View( void )
{
  uint32_t i;
  GPIO_CFG_DATA_stype *pxGpioCfg;
  int in,out;

  /*            012345678901234567890123456789 */
  ESP_LOGI(TAG,"Enum Name            Dir In Out ");
  
  for( i = 0 ; i < eGPIO_MAX; i++ )
  {
    pxGpioCfg = &xGPIO_CFG_TABLE[i];

    if( pxGpioCfg->xGrp != eGPIO_GRP_EXP )
      continue;
    
    esp_io_expander_get_level(xEXP_IO_HANDLE, pxGpioCfg->xGpioCfg.pin_bit_mask, (uint32_t *)(&in));
    in  = (in != 0) ? 1 : 0;
    out = pxGpioCfg->ucOutInit;
    
    ESP_LOGI(TAG,"[%02d] %-16s %s   %d  %d", pxGpioCfg->xGpioEnum, pxGpioCfg->cName, (pxGpioCfg->xGpioCfg.mode == GPIO_MODE_INPUT) ? "I":"O" ,in, out);
  }

  esp_io_expander_print_state( xEXP_IO_HANDLE );
}

#endif

void GPIO_API_Output( GPIO_etype eGpioNum, int LoHi )
{
  GPIO_MCU_Output( eGpioNum, LoHi );
#if defined(FEATURE_EXP_GPIO)
  GPIO_EXP_Output( eGpioNum, LoHi );
#endif  
}


void GPIO_API_Init(void)
{
  GPIO_MCU_Init();
#if defined(FEATURE_EXP_GPIO)
  vTaskDelay(pdMS_TO_TICKS(500));
  GPIO_EXP_Init();
#endif  
}

static void GPIO_CMD_View( void )
{
  GPIO_MCU_View();
#if defined(FEATURE_EXP_GPIO)
  GPIO_EXP_View();
#endif
}

static void GPIO_CMD_Output( int argc, char **argv )
{
  int pin,data;

  pin = atoi( argv[2] );
  data = atoi( argv[3] );

  GPIO_API_Output( pin, data );

  ESP_LOGI(TAG," pin : %d, data : %d", pin, data);
}


static int GPIO_CMD( int argc, char **argv )
{
  CmdRun( GPIO_CmdList, argc, argv );

  return 0;
}

void register_gpio(void)
{
  const esp_console_cmd_t cmd = {
      .command = "gpio",
      .help = "Gpio Cmd",
      .hint = NULL,
      .func = &GPIO_CMD,
      .argtable = NULL
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));  
}