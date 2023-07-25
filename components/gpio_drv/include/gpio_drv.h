#pragma once

// #include...

#ifdef __cplusplus
extern "C" {
#endif

typedef enum GPIO_etype
{
  eEXP_RESET = 0,

  eMDM_PWR_EN,
  eMDM_PWRKEY,
  eMDM_DTR,
  eMDM_RESET,
  e232_SLEEP,

  eGPIO_MAX
}GPIO_etype;


void GPIO_API_Init(void);
void GPIO_API_Output( GPIO_etype eGpioNum, int LoHi );
void GPIO_MCU_Init(void);
void GPIO_EXP_Init(void);
void register_gpio(void);

#ifdef __cplusplus
}
#endif