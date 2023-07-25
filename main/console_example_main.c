/* Basic console example (esp_console_repl API)

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_console.h"
#include "esp_vfs_dev.h"
#include "esp_vfs_fat.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "cmd_system.h"
#include "cmd_wifi.h"
#include "cmd_nvs.h"
#include "i2c_drv.h"
#include "gpio_drv.h"
#include "cmdline.h"
#include "simple_cmux_client_main.hpp"

static const char* TAG = "main";
#define PROMPT_STR CONFIG_IDF_TARGET

/* Console command history can be stored to and loaded from a file.
 * The easiest way to do this is to use FATFS filesystem on top of
 * wear_levelling library.
 */
#if CONFIG_CONSOLE_STORE_HISTORY

#define MOUNT_PATH "/data"
#define HISTORY_PATH MOUNT_PATH "/history.txt"



static void register_mdm(void);
static void mdm_turnon(void);
static void mdm_turnoff(void);
static void mdm_init(void);

Cmd_stype MDM_CmdList[] =
{
  { 2, "pwr",  "on",        mdm_turnon,       NULL },
  { 2, "pwr",  "off",       mdm_turnoff,      NULL },

  { 1, "init",  NULL,       mdm_init,      NULL },

  /* End Flag */
  { 0, NULL,    NULL,     NULL,         NULL }
};

static void initialize_filesystem(void)
{
    static wl_handle_t wl_handle;
    const esp_vfs_fat_mount_config_t mount_config = {
            .max_files = 4,
            .format_if_mount_failed = true
    };
    esp_err_t err = esp_vfs_fat_spiflash_mount_rw_wl(MOUNT_PATH, "storage", &mount_config, &wl_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to mount FATFS (%s)", esp_err_to_name(err));
        return;
    }
}
#endif // CONFIG_STORE_HISTORY

static void initialize_nvs(void)
{
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK( nvs_flash_erase() );
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK(err);
}


void app_main(void)
{
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    
    /* Prompt to be printed before each line.
     * This can be customized, made dynamic, etc.
     */
    repl_config.prompt = PROMPT_STR ">";
    repl_config.max_cmdline_length = CONFIG_CONSOLE_MAX_COMMAND_LINE_LENGTH;

    initialize_nvs();

#if CONFIG_CONSOLE_STORE_HISTORY
    initialize_filesystem();
    repl_config.history_save_path = HISTORY_PATH;
    ESP_LOGI(TAG, "Command history enabled");
#else
    ESP_LOGI(TAG, "Command history disabled");
#endif

    GPIO_MCU_Init();
    ESP_ERROR_CHECK(i2c_master_init());
    GPIO_EXP_Init();

    /* Register commands */
    esp_console_register_help_command();
    register_system();
    register_wifi();
    register_nvs();
    register_gpio();
    register_mdm();

#if defined(CONFIG_ESP_CONSOLE_UART_DEFAULT) || defined(CONFIG_ESP_CONSOLE_UART_CUSTOM)
    esp_console_dev_uart_config_t hw_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_CDC)
    esp_console_dev_usb_cdc_config_t hw_config = ESP_CONSOLE_DEV_CDC_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_cdc(&hw_config, &repl_config, &repl));

#elif defined(CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG)
    esp_console_dev_usb_serial_jtag_config_t hw_config = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&hw_config, &repl_config, &repl));

#else
#error Unsupported console type
#endif

    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}

static void mdm_turnon(void)
{
#if 0    
  GPIO_API_Output(eMDM_PWR_EN, 1);
  GPIO_API_Output(eMDM_RESET, 0);
  GPIO_API_Output(eMDM_PWRKEY, 1);
  vTaskDelay(pdMS_TO_TICKS(500));
  GPIO_API_Output(eMDM_PWRKEY, 0);
#endif  
  ESP_LOGI(TAG, "mdm_turnon");
}

static void mdm_turnoff(void)
{
#if 0    
  GPIO_API_Output(eMDM_PWRKEY, 1);
  vTaskDelay(pdMS_TO_TICKS(650));
  GPIO_API_Output(eMDM_PWRKEY, 0);
#endif
  ESP_LOGI(TAG, "mdm_turnoff");
}



static void mdm_init(void)
{
  ESP_LOGI(TAG,"mdm init");
  //app_test();
}

static int mdm_CMD( int argc, char **argv )
{
  CmdRun( MDM_CmdList, argc, argv );

  return 0;
}

static void register_mdm(void)
{
  const esp_console_cmd_t cmd = {
      .command = "mdm",
      .help = "Mdm Cmd",
      .hint = NULL,
      .func = &mdm_CMD,
      .argtable = NULL
  };
  ESP_ERROR_CHECK(esp_console_cmd_register(&cmd));  
}