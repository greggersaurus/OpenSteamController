# Luna_mainboard V000456-00 rev3

This document exists to track information regarding the Steam Controller
 hardware pertaining to Luna_mainboard V000456-00 rev3.

Note that this [Teardown of Steam Controller](https://www.ifixit.com/Teardown/Steam+Controller+Teardown/52578)
 was extremely helpful in identifying chips and offering guidance in disassembling
 the Steam Controlle to ohm out connections, etc.


# LPC11U37F/501

This is a ARM Cortex-M0, and seems to be the main/master processor of the Steam Controller. 

## Resources 

* [Product Information](https://www.nxp.com/part/LPC11U37FBD64?lang_cd=en)

* [Datasheet](http://www.nxp.com/documents/data_sheet/LPC11U3X.pdf?fasp=1&WT_TYPE=Data%20Sheets&WT_VENDOR=FREESCALE&WT_FILE_FORMAT=pdf&WT_ASSET=Documentation&fileExt=.pdf)

* [User Manual](http://www.nxp.com/documents/user_manual/UM10462.pdf)

## Pinout

| Pin Number 	| Datasheet Name	| Pin Function	   | Pin Direction | Notes 	|
|--------------:|-----------------------|------------------|---------------|---------------|
|             1 | PIO1_0                | PIO1_0           | In            | Analog joystick click button |
|             2 | PIO1_25               | PIO1_25          | In            | S14 - Left inner grip button |
|             3 | PIO1_19               | PIO1_19          | In            | S18 - Steam button |
|             4 | PIO0_0/nRESET         |                  |               | |
|             5 | PIO0_1                |                  |               | Connected to S6 - RT (Right trigger digital) for purpose of enacting USB In-Systen Programming (i.e. update firwmare via "FAT" file system) |
|             6 | PIO1_7                | PIO1_7           | Out           | Set as output GPIO and driven high during init (trace leads to... TODO)|
|             7 | V<sub>SS</sub>        | V<sub>SS</sub>   | In            | V<sub>SS</sub> |
|             8 | XTALIN                |                  |               | |
|             9 | XTALOUT               |                  |               | |
|            10 | V<sub>DD</sub>        | V<sub>DD</sub>   | In            | V<sub>DD</sub> |
|            11 | PIO0_20               |                  |               | |
|            12 | PIO1_10               | PIO1_10          | Out           | TODO: get more info on this... Set as output GPIO and driven ?? (during init). Related to AD? Or has to do with BOD and controller booting up... (routed to pin ??(middle) on U10 on front of PCB) |
|            13 | PIO0_2                | PIO0_2           | In            | Interrupt (PINT1) setup to monitor if this changes (trace leads to C56, continues to...). Seems to have something to do with Brown Out Detection. |
|            14 | PIO1_26               | PIO1_26          | In            | S2 - Left trackpad click |
|            15 | PIO1_27               | PIO1_27          | In            | S3 - LT (Left trigger digital) |
|            16 | PIO1_4                | PIO1_4           | In            | S8 - LB (Left bumper) | 
|            17 | PIO1_1                | PIO1_1           | Out           | Active low enable for analog triggers (Left = AD0, Right = AD2) |
|            18 | PIO1_20               | PIO1_20          | In            | S17 - FRONT_L (Front left arrow button) |
|            19 | PIO0_3                | USB_VBUS         | In            | Monitors the presence of USB bus power. Interrupt (PINT0) set to monitor if this changes. |
|            20 | PIO0_4                | I2C SCL          | In/Out        | Connected to MPU-6500 I2C SCL |
|            21 | PIO0_5                | I2C SDA          | In/Out        | Connected to MPU-6500 I2C SDA |
|            22 | PIO0_21               | CT16B1_MAT0      | Out           | Control Steam Button LED (LED1) |
|            23 | PIO1_17               | RXD (USART)      | In            | Connected to Nordic Semiconductor nRF51822 Bluetooth Smart and 2.4GHz proprietary SoC Pin 21 |
|            24 | PIO1_23               |                  |               | Connected to MPU-6500 Pin 12 (INT) |
|            25 | USB_DM                |                  |               | |
|            26 | USB_DP                |                  |               | |
|            27 | PIO1_24               | PIO1_24          | In?           | Related to PIO1_8. Related to USART maybe? TODO |
|            28 | PIO1_18               | TXD (USART)      | Out           | Connected to Nordic Semiconductor nRF51822 Bluetooth Smart and 2.4GHz proprietary SoC Pin 20 | 
|            29 | PIO0_6                | not(USB_CONNECT) | Out           | Signal used to switch an external 1.5 k resistor under software control. Used with the SoftConnect USB feature. |
|            30 | PIO0_7                | PIO0_7           | Out           | Set as output GPIO and driven low during init (trace leads to R9 (no-pop)). Related/similar to GPIO1_28? Any other purpose? More details? TODO|
|            31 | PIO1_28               | PIO1_28          | Out           | Set as output GPIO and driven low during init (trace leads to R8 (no-pop)). Related/similar to GPIO0_7? Any other purpose? More details? TODO |
|            32 | PIO1_5                | PIO1_5           | In            | USART/Radio Chip Related. PINT2 setup to monitor change. Trace leads to TODO|
|            33 | V<sub>DD</sub>        | V<sub>DD</sub>   | In            | V<sub>DD</sub> |
|            34 | PIO1_2                | PIO1_2           | In            | S19 - FRONT_R (Front right arrow button) | 
|            35 | PIO1_21               | PIO1_21          | In            | S5 - Right trackpad click  |
|            36 | PIO0_8                | MISO0            | In            | Master In Slave Out for SSP0. Connected to MISO on Right Trackpad/Haptic and Left Trackpad/Haptic. |
|            37 | PIO0_9                | MOSI0            | Out           | Master Out Slave In for SSP0. Connected to MOSI on Right Trackpad/Haptic and Left Trackpad/Haptic. |
|            38 | SWCLK/PIO0_10         | SWCLK?           |               | Connected to DEBUG interface for LPC11U37F |
|            39 | PIO1_8                | PIO1_8           | In?           | Related to PIO1_24. Related to USART maybe? TODO|
|            40 | PIO0_22               | AD6              | In            | Measures voltage that matters when no USB power is present? (trace leads to C6, continutes to R10 near power...) |
|            41 | PIO1_29               | SCK0             | Out           | Serial clock for SSP0. Connected to SCLK on Right Trackpad/Haptic and Left Trackpad/Haptic. |
|            42 | TDI/PIO0_11           | AD0              | In            | Measures Left Analog trigger position (when PIO1_1 is low) (Note: Functions as TDI for short time during initialization) |
|            43 | PIO1_11               | PIO1_11          | In            | S9 - Y Button |
|            44 | TMS/PIO0_12           | AD1              | In            | Measure Analog Joystick X position |
|            45 | TDO/PIO0_13           | AD2              | In            | Measures Right Analog trigger position (when PIO1_1 is low) (Note: Functions as TDO for short time during initialization) |
|            46 | nTRST/PIO0_14         | AD3              | In            | Measure Analog Joystick Y position |
|            47 | PIO1_13               | PIO1_13          | In            | S6 - RT (Right trigger digital) |
|            48 | V<sub>DD</sub>        | V<sub>DD</sub>   | In            | V<sub>DD</sub> |
|            49 | PIO1_14               | PIO1_14          | In            | S10 - RB (Right bumper) |
|            50 | PIO1_3                | PIO1_3           | In            | S16 - Right inner grip button | 
|            51 | PIO1_22               | PIO1_22          | In            | S4 - B Button |
|            52 | SWDIO/PIO0_15         | SWDIO?           |               | Connected to DEBUG interface for LPC11U37F |
|            53 | PIO0_16               |                  |               | | 
|            54 | V<sub>SS</sub>        | V<sub>SS</sub>   | In            | V<sub>SS</sub> |
|            55 | PIO1_9                | PIO1_9           | In            | S7 - X Button |
|            56 | PIO0_23               | PIO0_23          | In            | Input with pull-down resistor enabled. Connected to Data Ready on Right Trackpad/Haptic? |
|            57 | PIO1_15               | PIO1_15          | Out           | Chip Select/SS for Right Trackpad/Haptic (active low) |
|            58 | V<sub>DD</sub>        | V<sub>DD</sub>   | In            | V<sub>DD</sub> |
|            59 | PIO1_12               | PIO1_12          | In            | Read during init to check if it is logic low or not. (trace leads to VDD on Trackpad/Haptics). If low PIO0_18 is checked. |
|            60 | PIO0_17               | PIO0_17          | In            | S1 - A Button |
|            61 | PIO0_18               | PIO0_18          | In            | If PIO1_12 is low this is read during init to check if it is logic low or not. (trace leads to R43 on front of PCB). | 
|            62 | PIO0_19               | PIO0_19          | Out           | Set as output during init and driven low. (trace leads to... TODO)|
|            63 | PIO1_16               | PIO1_16          | In            | Input with pull-down resistor enabled. Connected to Data Ready on Left Trackpad/Haptic. |
|            64 | PIO1_6                | PIO1_6           | Out           | Chip Select/SS for Left Trackpad/Haptic (active low) |


# MPU-6500

This is a 6-axis motion tracking device (i.e. accelerometer, gyroscope).

## Resources 

* TODO: datasheet, etc.

## Pinout

| Pin Number 	| Datasheet Name	| Pin Function	| Notes 	|
|--------------:|-----------------------|---------------|---------------|
|             1 | NC                    | NC            | No Connect |
|             2 | NC                    | NC            | No Connect |
|             3 | NC                    | NC            | No Connect |
|             4 | NC                    | NC            | No Connect |
|             5 | NC                    | NC            | No Connect |
|             6 | NC                    | NC            | No Connect |
|             7 | AUX_CL                |               | |
|             8 | VDDIO                 |               | |
|             9 | SDO/AD0               |               | Connected to VDDIO which makes I2C Slave Address is b1101001 |
|            10 | REGOUT                |               | Connected C36 to GND|
|            11	| FSYNC	                |               | Connected to GND |
|            12	| INT	                |               | Connected to Pin 24 on LPC11U37F |
|            13	| VDD                   |               | Connected to VDDIO |
|            14	| NC                    |               | |
|            15	| NC                    |               | |
|            16	| NC                    |               | |
|            17 | NC                    |               | |
|            18 | GND                   |               | Connected to GND|
|            19 | RESV                  |               | Connected to GND|
|            20	| RESV	                |               | |
|            21	| AUX_DA                |               | |
|            22 | nCS	                |               | Connected to VDDIO for I2C operation |
|            23	| SCL/SCLK	        | I2C SCL       | Connected to Pin 20 on LPC11U37F |
|            24	| SDA/SDI               | I2C SDA       | Connected to Pin 21 on LPC11U37F |


# nRF51822 

This is the powerful, highly flexible multiprotocol SoC ideally suited for 
 Bluetooth® low energy and 2.4GHz ultra low-power wireless applications. The
 silkscreen labels this as RADIO.

## Resources 

* TODO: datasheet, etc.

## Pinout

| Pin Number 	| Datasheet Name	| Pin Function	| Notes 	|
|--------------:|-----------------------|---------------|---------------|
|             1 |                       |               | |
|             2 |                       |               | |
|             3 |                       |               | |
|             4 |                       |               | |
|             5 |                       |               | |
|             6 |                       |               | |
|             7 |                       |               | |
|             8 |                       |               | |
|             9 |                       |               | |
|            10 |                       |               | |
|            11 |                       |               | |
|            12 |                       |               | |
|            13 |                       |               | |
|            14 |                       |               | |
|            15 |                       |               | |
|            16 |                       |               | |
|            17 |                       |               | |
|            18 |                       |               | |
|            19 |                       |               | |
|            20 | P0.14                 |               | Connected to Pin 20 (USART TXD) on LPC11U37F/501 |
|            21 | P0.15                 |               | Connected to Pin 23 (USART RXD) on LPC11U37F/501 |
|            22 | P0.16                 |               | |
|            23 | SWDIO/nRESET          |               | Connected to Pin 27 on LPC11U37F/501 |
|            24 | SWDCLK                |               | Connected to Pin 39 on LPC11U37F/501 |
|            25 |                       |               | |
|            26 |                       |               | |
|            26 |                       |               | |
|            28 |                       |               | |
|            29 |                       |               | |
|            30 |                       |               | |
|            31 |                       |               | |
|            32 |                       |               | |
|            33 |                       |               | |
|            34 |                       |               | |
|            35 |                       |               | |
|            36 |                       |               | |
|            37 |                       |               | |
|            38 |                       |               | |
|            39 |                       |               | |
|            40 |                       |               | |
|            41 |                       |               | |
|            42 |                       |               | |
|            43 |                       |               | |
|            44 |                       |               | |
|            45 |                       |               | |
|            46 |                       |               | |
|            47 |                       |               | |
|            48 |                       |               | |


# Cirque 1CA027 

This is the processor on the haptics board, which seems to be a custom design 
 similar to the [GlidePoint TM040040](http://www.cirque.com/glidepoint-circle-trackpads).

## Resources 

* TODO: datasheet, etc.

## Pinout 

| Pin Number 	| Datasheet Name	| Pin Function	| Notes 	|
|--------------:|-----------------------|---------------|---------------|
|             1 |                       |               | |
|             2 |                       |               | |
|             3 |                       |               | |
|             4 |                       |               | |
|             5 |                       |               | |
|             6 |                       |               | |
|             7 |                       |               | |
|             8 |                       |               | |
|             9 |                       |               | |
|            10 |                       |               | |
|            11 |                       |               | |
|            12 |                       |               | Connected to Pin 13 via 470K indicating SPI Mode |
|            13 |                       |               | Connected to Pin 12 via 470K indicating SPI Mode ||
|            14 |                       |               | |
|            15 |                       |               | |
|            16 |                       |               | |
|            17 |                       |               | |
|            18 |                       |               | |
|            19 |                       |               | |
|            20 |                       |               | | 
|            21 |                       |               | | 
|            22 |                       |               | |
|            23 |                       |               | | 
|            24 |                       |               | | 
|            25 |                       |               | |
|            26 |                       |               | |
|            26 |                       |               | |
|            28 |                       |               | |
|            29 |                       |               | |
|            30 |                       |               | |
|            31 |                       |               | |
|            32 |                       |               | |
|            33 |                       |               | |
|            34 |                       |               | |
|            35 |                       |               | |
|            36 |                       |               | |
|            37 |                       |               | |
|            38 |                       |               | |
|            39 |                       |               | |
|            40 |                       |               | |
|            41 |                       |               | |
|            42 |                       |               | |
|            43 |                       |               | |
|            44 |                       |               | |
|            45 |                       |               | |
|            46 |                       |               | |
|            47 |                       |               | |
|            48 |                       |               | |

