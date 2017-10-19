# Custom Jingle 

The work in this directory will be geared towards granting the ability to 
 completely customize the jingle played on the Steam Controller haptics 
 on power on and power off. 

## Customize Song/Tune/Jingle

Assumption is that jingle data is coded into firmware or stored in EEPROM on NXP chip and I can come up with a way to modify this data into user customizable jingles.

### Firmware Assumption 

Goal is to find section of firmware where jingle data is, or prove jingle data is not stored in firmware.bin.

#### Status

[vcf_wired_controller_d0g_57bf5c10.bin.md](../ReverseEngineering/vcf_wired_controller_d0g_57bf5c10.bin.md)
 is tracking progress on attempts to simulate and understand this particular 
 version of firmware.

### EEPROM Assumption

Goal is to find section fo EEPROM where jingle data is, or prove jingle data is not stored in EEPROM.

