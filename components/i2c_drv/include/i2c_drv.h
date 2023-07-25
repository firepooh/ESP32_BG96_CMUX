#pragma once

#include "esp_err.h"
#include "driver/i2c.h"

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t i2c_master_init(void);
void do_i2cdetect_cmd( void );

#ifdef __cplusplus
}
#endif
