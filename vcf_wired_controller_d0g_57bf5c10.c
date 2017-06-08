
/**
 * In this simulation run the system was run from reset with no external input
 *  (except steps necessary to simulate expected hardware unit reactions). Possible
 *  triggering of IRQs were ignored. Parsed from exeLog_00000000001496459595.csv.
 *
 * \return None.
 */
void init()
{
	uint32_t* reg = NULL;
	uint32_t val = 0;

        // Entry Num: 1 - 24
        // Step Num: 1 - 16
	// Firmware Offset(s): 
	//	0x000000d4 - 0x000000d6
	//	0x00000fe0 - 0x00000fe4 
	//	0x000005a4 - 0x000005b8

	// Power Configuration Register                                                 
	reg = 0x40048238;                                                               
	// Read current value                                                           
	val = *reg;                                                                     
	// Clear reserved bit that must stay cleared                                    
	val &= 0x000005ff;                                                              
	// Make sure crystal oscillator is powered                                      
	val &= ~(0x00000020);                                                           
	// Reserved bits that must always be set                                        
	val |= 0xe800;                                                                  
	*reg = val;  

        // Entry Num: 25 - 45087
        // Step Num: 17 - 33814
	// Firmware Offset(s): 
	//	0x00000fe8 - 0x00000ffe

	// Some sort of delay required after last system control register mod?          
	for (uint32_t cnt = 0; cnt < 0x1600; cnt++)                                     
	{}  

        // Entry Num: 45088 - 45098
        // Step Num: 33815 - 33821
	// Firmware Offset(s): 
	//	0x00000520 - 0x0000052c

	// Select Crystal Oscillator (SYSOSC)                                           
	reg = 0x40048040;                                                               
	*reg = 1;                                                                       

	// Enable system PLL clock source update                                        
	reg = 0x40048044;                                                               
	*reg = 0;                                                                       
	*reg = 1;

        // Entry Num: 45099 - 45115
        // Step Num: 33822 - 33834
	// Firmware Offset(s): 
	//	0x00001002 - 0x00001004
	//	0x00000584 - 0x00000598

	// Power Configuration Register                                                 
	reg = 0x40048238;                                                               
	// Read current value                                                           
	val = *reg;                                                                     
	// Clear reserved bit that must stay cleared                                    
	val &= 0x5ff;                                                                   
	// Make sure system PLL is powered down                                         
	val |= 0x80;                                                                    
	// Reserved bits that must always be set                                        
	val |= 0xe800;                                                                  
	*reg = val;                    

        // Entry Num: 45116 - 45122
        // Step Num: 33835 - 33839
	// Firmware Offset(s): 
	//	0x00001008 - 0x00001010

	// System PLL control register                                                  
	reg = 0x40048008;                                                               
	// Division ratio = 2 x 4. Feedback divider value = 3 + 1.                      
	*reg = 0x23;

        // Entry Num: 45123 - 45137
        // Step Num: 33840 - 33850
	// Firmware Offset(s): 
	//	0x000005a4 - 0x000005b8

	// Power configuration register                                                 
	reg = 0x40048238;                                                               
	val = *reg;                                                                     
	// Clear reserved bit that must stay cleared                                    
	val &= 0x5ff;                                                                   
	// Make sure system PLL powered                                                 
	val &= ~0x80;                                                                   
	// Reserved bits that must always be set                                        
	val |= 0xe800;                                                                  
	*reg = val;

        // Entry Num: 45138 - 45143
        // Step Num: 33851 - 33855
	// Firmware Offset(s): 
	//	0x00001014 - 0x0000101e

	// System PLL status register                                                   
	reg = 0x4004800c;                                                               
	// Wait until PLL is locked                                                     
	while(((*reg) & 1) == 0);

        // Entry Num: 45144 - 45161
        // Step Num: 33856 - 33868
	// Firmware Offset(s): 
	//	0x00001020 - 0x00001038

	// System clock divider register                                                
	reg = 0x40048078;                                                               
	// Set system AHB clock divider to 1.                                           
	*reg = 1;

	// Flash configuration register                                                 
	reg = 0x4003c010;                                                               
	val = *reg;                                                                     
	// Bits 31:2 must be written back exactly as read                               
	val &= 0xFFFFFFC0;                                                              
	// Set flash access time to 3 system clocks (for system clock up to 50 MHz)     
	val |=  2;                                                                      
	*reg = val;

        // Entry Num: 45162 - 45171
        // Step Num: 33869 - 33874
	// Firmware Offset(s): 
	//	0x0000050c - 0x00000518

	// Main clock source select register                                            
	reg = 0x40048070;                                                               
	// Select PLL output                                                            
	*reg = 3;                                                                       

	// Main clock source update enable register                                     
	reg = 0x40048074;                                                               
	// No change                                                                    
	*reg = 0;                                                                       
	// Update clock source                                                          
	*reg = 1; 

        // Entry Num: 45172 - 45185
        // Step Num: 33875 - 33884
	// Firmware Offset(s): 
	//	0x0000103c - 0x0000103e
	//	0x00000548 - 0x00000554

	// USB PLL clock source select register                                         
	reg = 0x40048048;                                                               
	// Select system oscillator                                                     
	*reg = 1;                                                                       

	// USB PLL clock source update enable register                                  
	reg = 0x4004804c;                                                               
	// No change                                                                    
	*reg = 0;                                                                       
	// Update clock source.                                                         
	*reg = 1; 

        // Entry Num: 45186 - 45190
        // Step Num: 33885 - 33888
	// Firmware Offset(s): 
	//	0x00001042 - 0x00001048

	// USB PLL control register                                                     
	reg = 0x40048010;                                                               
	// Division ration is 2 x 4. Feedback divider value is 3 + 1.                   
	*reg = 0x23;

        // Entry Num: 45191 - 45205
        // Step Num: 33889 - 33899
	// Firmware Offset(s): 
	//	0x000005a4 - 0x000005b8

	// Power Configuration Register                                                 
	reg = 0x40048238;                                                               
	val = *reg;                                                                     
	// Clear reserved bit that must stay cleared                                    
	val &= 0x5ff;                                                                   
	// Set USB PLL and USB transceiver to powered                                   
	val &= ~0x500;                                                                  
	// Reserved bits that must always be set                                        
	val |= 0xe800;                                                                  
	*reg = val;

        // Entry Num: 45206 - 45211
        // Step Num: 33900 - 33904
	// Firmware Offset(s): 
	//	0x0000104c - 0x0000104c
	//	0x00001050 - 0x00001056

	// USB PLL status register                                                      
	reg = 0x40048014;                                                               
	// Wait for PLL locked                                                          
	while (((*reg) & 1) == 0);

        // Entry Num: 45212 - 45226
        // Step Num: 33905 - 33912
	// Firmware Offset(s): 
	//	0x00001058 - 0x00001066

	// System clock control register                                                
	reg = 0x40048080;                                                               
	val = *reg;                                                                     
	// Enable I/O configuration block                                               
	val |= 0x10000;                                                                 
	*reg = val;

        // Entry Num: 45227 - 53704
        // Step Num: 33913 - 40670 
	// Firmware Offset(s): 
	//	0x000000d8 - 0x000000da
	//	0x000000d0 - 0x000000c4
	//	0x000003d0 - 0x000003ca
	//	0x000003bc - 0x000003c4
	//	0x0000152c - 0x00001536
	//	0x0000152e - 0x00001538	
	//	0x000003c6 - 0x000003ca
	//	0x000003bc - 0x000003c4
	//	0x0000153c - 0x0000153e
	//	0x00001540 - 0x00001548
	//	0x000003c6 - 0x000003cc
	//	0x000000cc - 0x000000ca
	//	0x0000154c - 0x0000154e
	//	0x00000fd0 - 0x00000fd2
	//	0x00000494 - 0x00000496

	// Set SRAM0 0x10000200 to 0x10001c1c with specific values
	// TODO: details on exact values set here and data word size accesses

        // Entry Num: 53705 - 53722
        // Step Num: 40671 - 40684
	// Firmware Offset(s): 
	//	0x00000450 - 0x0000046a

	// Check main clock source select register and verify is set to PLL output
	//  reg = 0x40048070;
	//  val = *reg;
	//  if (val != 0x3)
	//  {
		// There are multiple paths for checking other values set here... Have not work through them
		// TODO: UKNOWN PATHS
		// 	Not branching to 0x00000470 
		// 	Not branching to 0x00000474 
		// 	Not branching to 0x0000047a 
		// 	Not branching to 0x0000046e 
	//  }

        // Entry Num: 53723 - 53740
        // Step Num: 40685 - 40696
	// Firmware Offset(s): 
	//	0x000004d0 - 0x000004d2
	//	0x000004a8 - 0x000004b4
	//	0x000004be - 0x000004c2

	// Check system PLL clock source select register and verify is set to Crystal Oscillator (SYSOSC)
	//  reg = 0x40048040;
	//  val = *reg;
	//  if (val != 1)
	//  {
		// There are multiple paths for checking other values set here... Have not work through them
		// TODO: UKNOWN PATHS
		// 	Not branching to 0x000004ba
		// 	Branching to 0x000004be, not executing 0x000004b6
	//  }

        // Entry Num: 53741 - 54149
        // Step Num: 40697 - 41067
	// Firmware Offset(s): 
	//	0x000004d6 - 0x000004dc
	//	0x00000488 - 0x00000492
	//	0x000004e0 - 0x000004e0
	//	0x0000046e - 0x0000046e
	//	0x0000049a - 0x0000049e
	//	0x00000300 - 0x0000030c
	//	0x0000030e - 0x0000032a
	//	0x000004a2 - 0x000004a2
	//	0x00000fd6 - 0x00000fda
	//	0x00001552 - 0x00001552
	//	0x00000d04 - 0x00000d0c
	//	0x00000bdc - 0x00000bf0
	//	0x00000bb4 - 0x00000bc0

	// System PLL control register
	// reg = 0x40048008;
	// val = *reg;
	// Perform calculations based on value of system PLL control register
	// Save results to 0x10000260
	// I believe this is all setup for EEPROM read via IAP command (i.e. calculation of system clock frequency)
	//	Command code : 62 (0x3e)
	//	Param0: EEPROM Address = 0
	//	Param1: RAM Address = 0x10000254
	//	Param2: Number of bytes to be read = 8
	//	Param3: System Clock Frequency = 0x0000b71b

	unsigned int command_param[5];
	unsigned int status_result[4];

	uint32_t eeprom_data[2];

	// Command 62 for EEPROM Read
	command_param[0] = 62;
	// EEPROM address (4 kB available)
	command_param[1] = 0;
	// RAM address where to read data to write to EEPROM
	command_param[2] = eeprom_data;
	// Number of bytes to write
	command_param[3] = sizeof(eeprom_data);
	// System clock frequency in kHz
	command_param[4] = 46875;

        // Entry Num: 54150 - 54163
        // Step Num: 41068 - 41077
	// Firmware Offset(s): 
	//	0x000007bc - 0x000007ba
	//	0x00000bc4 - 0x00000bca

	// Increment value in 0x10000250
	//	Has to do with cps command (i.e. disabling/enabling interrupts)
	// Call into IAB command function

        // Entry Num: 54164 - 55980
        // Step Num: 41078  - 42737
	// Firmware Offset(s): 
	//	0x1fff1ff0 - 0x1fff1ff2
	//	0x1fff171c - 0x1fff171c
	//	... TODO: do we care aboutd decomposing boot ROM code?

	// boot ROM code for executing IAB command
	iap_entry(command_param, status_result);	

	//TODO: should probbaly check status_result for CMD_SUCCESS

        // Entry Num: 55981 - 55991
        // Step Num: 42738 - 42745
	// Firmware Offset(s): 
	//	0x00000bcc - 0x00000bcc
	//	0x000007c0 - 0x000007cc

	// Decrement value in 0x10000250
	//	Has to do with cps command (i.e. disabling/enabling interrupts)

        // Entry Num: 55992 - 56018
        // Step Num: 42746 - 42759
	// Firmware Offset(s): 
	//	0x00000bd0 - 0x00000bd4
	//	0x00000bf4 - 0x00000bf4
	//	0x00000d10 - 0x00000d22

	// Check is 0x10000254 has value 0x0000a55a stored in it
	//	This is where EEPROM read data ends up
	// If it had been we would skip writing to EEPROM
	//	Not branching to 0x00000d26

	if (eeprom_data[0] != 0x0000a55a)
	{
		// Entry Num: 56019 - 56073
		// Step Num: 42760 - 42792
		// Firmware Offset(s): 
		//	0x00000f5c - 0x00000f64
		//	0x00000bfc - 0x00000c10
		//	0x00000bb4 - 0x00000bc0
		//	0x000007b0 - 0x000007ba
		//	0x00000bc4 - 0x00000bca

		// Filling in EEPROM with expected value (magic word to show EEPROM has been used by Steam Controller before?)	
		// 	*0x10000254 = 0x0000a55a
		// IAP command Write EEPROM
		//	Command code : 61 (0x3d)
		//	Param0: EEPROM Address = 0
		//	Param1: RAM Address = 0x10000254
		//	Param2: Number of bytes to be read = 8
		//	Param3: System Clock Frequency = 0x0000b71b

		// Data to write to EEPROM
		eeprom_data[0] = 0x0000a55a;
		eeprom_data[1] = 0;

		// Command 61 for EEPROM Write
		command_param[0] = 61;
		// EEPROM address (4 kB available)
		command_param[1] = 0;
		// RAM address where to read data to write to EEPROM
		command_param[2] = eeprom_data;
		// Number of bytes to write
		command_param[3] = sizeof(eeprom_data);
		// System clock frequency in kHz
		command_param[4] = 46875;

		// Entry Num: 56074 - 57950
		// Step Num: 42793 - 44499
		// Firmware Offset(s): 
		//	0x1fff1ff0 - 0x1fff1ff2
		//	0x1fff171c - 0x1fff171c
		//	... TODO: do we care aboutd decomposing boot ROM code?

		// boot ROM code for executing IAB command

		iap_entry(command_param, status_result);	

		//TODO: should probbaly check status_result for CMD_SUCCESS

		// Entry Num: 57951 - 57961
		// Step Num: 44500 - 44507
		// Firmware Offset(s): 
		//	0x00000bcc - 0x00000bcc
		//	0x000007c0 - 0x000007cc

		// Decrement value in 0x10000250
		//	Has to do with cps command (i.e. disabling/enabling interrupts)
	}

        // Entry Num: 57962 - 57990
        // Step Num: 44508 - 44522
	// Firmware Offset(s): 
	//	0x00000bd0 - 0x00000bd4
	//	0x00000c14 - 0x00000c14
	//	0x00000f68 - 0x00000f68
	//	0x00000d26 - 0x00000d26
	//	0x00001556 - 0x00001558
	//	0x00000428 - 0x00000434

	// Enables clock for GPIO port registers via system clock control register
	reg = 0x40048080;
	val = *reg;
	val |= 0x40;
	*reg = val;

        // Entry Num: 57991 - 57998
        // Step Num: 44523 - 44529
	// Firmware Offset(s): 
	//	0x0000155c - 0x0000155c
	//	0x00000ce8 - 0x00000cf2
	//	0x00000cf4 - 0x00000cf4

	// val = *((uint8_t*)0x50000003);
	// Check state of P0_3 and make sure it is 0
		// TODO: UKNOWN PATHS
		//	if P0_3 is not 0 instruction at 0x00000cf2 is executed
	// TODO: what does P0_3 indicate?
	//	Can we ohm this out?

        // Entry Num: 57999 - 58008
        // Step Num: 44530 - 44537
	// Firmware Offset(s): 
	//	0x00001560 - 0x0000157c

	// Again check that P0_3 is zero
	// If it is check states of bits 0 and/or 1 in 0x40048030 (System reset status register)

        // Entry Num: 58009 - 58027
        // Step Num: 44538 - 44548
	// Firmware Offset(s): 
	//	0x00000f90 - 0x00000f94
	//	0x00000cf8 - 0x00000cfc
	//	0x00000f98 - 0x00000fa0
	//	0x00000fae - 0x00000fae

	// Reading 0x10000258
	//	Previously set via read from EEPROM, but defaulted to 0 on EEPROM magic word missing
	// Check if 0x10000258 is set to value 8
	//	TODO: UNKOWN PATHS
	//		If value from EEPROM (written to 0x10000258) is 8, execute instruction at 0x00000fa2

        // Entry Num: 58028 - 58035
        // Step Num: 44549 - 44555
	// Firmware Offset(s): 
	//	0x00000cf8 - 0x00000cfc
	//	0x00000fb2 - 0x00000fb6

	// Read value fo 0x10000258 and check if it's 5
	//	TODO: UNKNOWN PATHS
	//		If value from EEPROM (written to 0x10000258) is 5, excecute instruction at 0x00000fb8

        // Entry Num: 58036 - 58040
        // Step Num: 44556 - 44559
	// Firmware Offset(s): 
	//	0x00000fbe - 0x00000fc4

	// Set P1_8 to output bit to high
	*((uint8_t*)0x50000028) = 1;

        // Entry Num: 58041 - 58053
        // Step Num: 44560 - 44570
	// Firmware Offset(s): 
	//	0x0000055c - 0x00000570

	// Set P1_8 to output via GPIO direction port 1 register
	reg = 0x50002004;
	val = *reg;
	val |= 0x00000100;

        // Entry Num: 58054 - 58067
        // Step Num: 44571 - 44578
	// Firmware Offset(s): 
	//	0x00000fc8 - 0x00000fc8
	//	0x00001580 - 0x00001582
	//	0x000007a0 - 0x000007a8

	// Set GPREG3 to 0
	reg = 0x40038008;
	*reg = 0;

        // Entry Num: 58068 - 58087
        // Step Num: 44579 - 44594
	// Firmware Offset(s): 
	//	0x00001586 - 0x0000159a
	//	0x00000572 - 0x00000580

	// Enable pull down resistor on PIO0_3 register
	reg = 0x4004400c;
	*reg = 0x00000008;

        // Entry Num: 58088 - 58105
        // Step Num: 44595 - 44610
	// Firmware Offset(s): 
	//	0x0000159e - 0x000015a2
	//	0x0000158c - 0x0000159a
	//	0x00000572 - 0x00000576
	//	0x0000057e - 0x00000580

	// Note there are two conditional paths in here, but they are impossible to reach given setup, so they are being ignored

	// Set PIO0_6 to function as ~USB_CONNECT
	reg = 0x40044018;
	*reg = 0x00000001; 
	
        // Entry Num: 58106 - 58127
        // Step Num: 44611 - 44630
	// Firmware Offset(s): 
	//	0x0000159e - 0x000015a2
	//	0x0000158c - 0x0000159a
	//	0x00000572 - 0x000015a2

	// Note there are two conditional paths in here, but they are impossible to reach given setup, so they are being ignored

	// Set PIO1_17a to function as RXD
	reg = 0x400440a4;
	*reg = 0x00000002;

        // Entry Num: 58128 - 58143
        // Step Num: 44631 - 44644
	// Firmware Offset(s): 
	//	0x0000158c - 0x0000159a
	//	0x00000572 - 0x0000057c

	// Note there are two conditional paths in here, but they are impossible to reach given setup, so they are being ignored

	// Set PIO1_18 to function as TXD
	reg = 0x400440a8;
	*reg = 0x00000002;

        // Entry Num: 58144 - 58154
        // Step Num: 44645 - 44654
	// Firmware Offset(s): 
	//	0x0000159e - 0x000015a4
	//	0x00000ce8 - 0x00000cf2
	//	0x00000cf4 - 0x00000cf4

	// val = *((uint8_t*)0x50000003);
	// Check state of P0_3 and make sure it is 0
		// TODO: UKNOWN PATHS
		//	if P0_3 is not 0 instruction at 0x00000cf2 is executed
	// TODO: what does P0_3 indicate?
	//	Can we ohm this out?

        // Entry Num: 58155 - 58164
        // Step Num: 44655 - 44660
	// Firmware Offset(s): 
	//	0x000015a8 - 0x000015ac
	//	0x00000f90 - 0x00000f94

	// Again check that P0_3 is zero

        // Entry Num: 58165 - 58181
        // Step Num: 44661 - 44672
	// Firmware Offset(s): 
	//	0x00000cf8 - 0x00000cfc
	//	0x00000f98 - 0x00000fa0
	//	0x00000fae - 0x00000fae
	//	0x00000cf8 - 0x00000cfc

	// reading value in RAM at 0x10000258 and see if is set to 8
	//	TODO: UNKOWN PATHS
	//		If value from EEPROM (written to 0x10000258) is 8, execute instruction at 0x00000fa2

        // Entry Num: 58182 - 58184
        // Step Num: 44673 - 44675
	// Firmware Offset(s): 
	//	0x00000fb2 - 0x00000fb6

	// Read value fo 0x10000258 and check if it's 5
	//	TODO: UNKNOWN PATHS
	//		If value from EEPROM (written to 0x10000258) is 5, excecute instruction at 0x00000fb8

        // Entry Num: 58185 - 58189
        // Step Num: 44676 - 44679
	// Firmware Offset(s): 
	//	0x00000fbe - 0x00000fc4
	
	// Set P1_8 output bit to 1
	*((uint8_t*)0x50000028) = 0x01

	//TODO: it looks like P1_8 being set to high might drive a different input back...? Look through paths of setting this output based on input values

        // Entry Num: 58190 - 58202
        // Step Num: 44680 - 44690
	// Firmware Offset(s): 
	//	0x0000055c - 0x00000570

	// Set P1_8 to output via GPIO direction port 1 register
	reg = 0x50002004;
	val = *reg;
	val |= 0x00000100;

	//TODO: again with double setting P1_8 as output... simulation might not be reaction correctly due to unanticipated hardware input response...

        // Entry Num: 58203 - 58260
        // Step Num: 44691 - 44725
	// Firmware Offset(s): 
	//	0x00000fc8 - 0x00000fc8
	//	0x000015b0 - 0x000015ba
	//	0x00000bdc - 0x00000bf0
	//	0x00000bb4 - 0x00000bc0
	//	0x000007b0 - 0x000007ba
	//	0x00000bc4 - 0x00000bca

	// I believe this is all setup for EEPROM read via IAP command (i.e. calculation of system clock frequency)
	//	Command code : 62 (0x3e)
	//	Param0: EEPROM Address = 0x00000500
	//	Param1: RAM Address = 0x10001c08
	//	Param2: Number of bytes to be read = 4
	//	Param3: System Clock Frequency = 0x0000b71b

	// Data to write to EEPROM
	uint32_t eeprom_data2;

	// Command 62 for EEPROM Read
	command_param[0] = 62;
	// EEPROM address (4 kB available)
	command_param[1] = 0x500;
	// RAM address where to read data to write to EEPROM
	command_param[2] = eeprom_data2;
	// Number of bytes to write
	command_param[3] = sizeof(eeprom_data2);
	// System clock frequency in kHz
	command_param[4] = 46875;

        // Entry Num: 58261 - 60061
        // Step Num: 44726 - 46373
	// Firmware Offset(s): 
	//	0x1fff1ff0 - 0x1fff1ff2
	//	0x1fff171c - 0x1fff171c
	//	... TODO: do we care aboutd decomposing boot ROM code?

	// boot ROM code for executing IAP command
	
	iap_entry(command_param, status_result);	

	//TODO: should probbaly check status_result for CMD_SUCCESS

        // Entry Num: 60062 - 60072
        // Step Num: 46374 - 46381
	// Firmware Offset(s): 
	//	0x00000bcc - 0x00000bcc
	//	0x000007c0 - 0x000007cc

	// Decrement 0x10000250 and cps (change processor state (to reenable interrupts after IAP command?)

        // Entry Num: 60073 - 60096
        // Step Num: 46382 - 46394
	// Firmware Offset(s): 
	//	0x00000bd0 - 0x00000bd4
	//	0x00000bf4 - 0x00000bf4
	//	0x000015be - 0x000015c4 
	//	0x000015de - 0x000015e6 

	// Check if 0x10001c08 (Value read from EEPROM) is 0
	// if (eeprom_data2 != 0)
	// {
	// 	TODO: UNKONWN PATHS
	//	if value read from EEPROM offset 0x500 is not 0 execute instruction at 0x000015c6
	// }

	// Check if GPREG0 is set to 0xecaabac0
	// reg = 0x40038004;
	// val = *reg;
	// if (val == 0xecaabac0)
	// {
		// TODO: UNKOWN PATHS
		//	If GPREG0 is 0xecaabac0 execute instruction 0x000015e6 instead of branching to 0x000015ec
	// }

        // Entry Num: 60097 - 60109
        // Step Num: 46395 - 46402
	// Firmware Offset(s): 
	//	0x000015ec - 0x000015fa

	// Call into some function that checks if Reg 0 is set to 0xecaabac0. Could be UNKNOWN PATH, but in this case Reg 0 is set from firmware read 

        // Entry Num: 60110 - 60117
        // Step Num: 46403 - 46408
	// Firmware Offset(s): 
	//	0x000015fc - 0x000015fc
	//	0x00000cf8 - 0x00000cf8
	//	0x00001600 - 0x00001602	

	// Check if 0x10000258 == 0
	//	TODO: UNKNOWN PATH what if 0x10000258 is not 0?

        // Entry Num: 60118 - 60140
        // Step Num: 46409 - 46423
	// Firmware Offset(s): 
	//	0x00001618 - 0x00001618
	//	0x00000f28 - 0x00000f28
	//	0x00000a78 - 0x00000a80
	//	0x00000534 - 0x00000542

	// Select USB PLL out via USB clock source select register
	reg = 0x400480c0;
	*reg = 0;
	// Clear USB clock source update enable register
	reg = 0x400480c4;
	*reg = 0;
	// Update clock source via USB clock source update enable register
	reg = 0x400480c4;
	*reg = 1;
	// Set divide by 1 for USB clock divider registe
	reg = 0x400480c8;
	*reg = 1;

        // Entry Num: 60141 - 60152
        // Step Num: 46424 - 46432
	// Firmware Offset(s): 
	//	0x00000a84 - 0x00000a86
	//	0x0000043c - 0x00000448

	// Enables clock to the USB register interface via System clock control register
	reg = 0x40048080;
	val = *reg;
	val |= 0x00004000;
	*reg = val;

        // Entry Num: 60153 - 60164
        // Step Num: 46433 - 46441
	// Firmware Offset(s): 
	//	0x00000a8a - 0x00000a8c
	//	0x0000043c - 0x00000448

	// Enables USB SRAM block at address 0x2000 4000 via System clock control register 
	reg = 0x40048080;
	val = *reg;
	val |= 0x08000000;
	*reg = val;

        // Entry Num: 60165 - 60177
        // Step Num: 46442 - 46449
	// Firmware Offset(s): 
	//	0x00000a90 - 0x00000a9e

	// Write 0x1fff1f24 to RAM address 0x10000230, which was read from boot ROM

        // Entry Num: 60178 - 60524
        // Step Num: 46450 - 46728
	// Firmware Offset(s): 
	//	0x0000035e - 0x00000360
	//	0x00000350 - 0x00000352
	//	0x00000354 - 0x0000035c

	// Write 0's (byte by byte) SRAM0 for range 0x10001ba0 - 0x10001be3

        // Entry Num: 60525 - 60564
        // Step Num: 46729 - 46752
	// Firmware Offset(s): 
	//	0x00000aa2 - 0x00000ad0

	// Setup for calling function to setup USB?
	//  *0x10001ba0 = 0x40080000;
	//  *0x10001ba4 = 0x20004000;
	//  *0x10001ba8 = 0x00000800;
	// 
	//  *0x10001bac = 0x00000003;
	// 
	//  *0x10001be4 = 0x00001670;
	//  *0x10001be8 = 0x00001682;
	//  *0x10001bec = 0x10000208;
	// 
	//  *0x10001bf0 = 0x10000208;
	//  *0x10001bf4 = 0x00000000;
	
        // Entry Num: 60565 - 
        // Step Num: 46753 - 
	// Firmware Offset(s): 
	//	

	// USB via boot ROM code
	// TODO: map out what registers are being set etc. so we know where and what memory USB is accessing
	//	This may be the key for where jingle data ends up for USB transmission

	//TODO: Remember to pay attention to branches/paths simulation does and does not take.
	//TODO: Keep in mind that system will continue after WFI. Need to walk through this simulation and see about options (how does controller shutdown...?)
	//		I think what happens is that WFI is called for interrupt to run that sets particular registers. 
	//		Maybe 32 bit timer interrupting before other things happening causes variables to get set in a way such that power off occurs
	//		Otherwise controller might stay in this "idle" loop, or go into other "idle" loop, waiting for shutdown signal?
}
