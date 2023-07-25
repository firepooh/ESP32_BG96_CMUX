idf_component_register(SRCS "gpio_drv.c"
                    INCLUDE_DIRS "include"
                    REQUIRES "driver" "i2c_drv" "esp_io_expander" "esp_io_expander_tca95xx_16bit" "console" "cmdline")
