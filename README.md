# esp32_bg96_cmux
ESP32 BG96 CMUX IDF Example

# setup
1. reference source : esp modem components 
> [https://github.com/espressif/esp-aws-iot](https://components.espressif.com/components/espressif/esp_modem)

2. simple_cmux_example 사용
> [git clone -b "release/202210.01-LTS" --recursive https://github.com/espressif/esp-aws-iot](https://github.com/espressif/esp-protocols/tree/master/components/esp_modem/examples/simple_cmux_client)


 # sdkconfig.defaults 설정
- 모뎀 tx/rx uart 설정
```
CONFIG_EXAMPLE_MODEM_UART_TX_PIN=13
CONFIG_EXAMPLE_MODEM_UART_RX_PIN=4
```
- LG USIM 사용시 APN 설정
```
CONFIG_EXAMPLE_MODEM_PPP_APN="m2m-catm1.default.lguplus.co.kr"
```
