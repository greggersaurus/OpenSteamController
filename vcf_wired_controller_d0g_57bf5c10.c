/**
 * All captured data from simulation vcf_wired_controller_d0g_57bf5c10.bin using
 *  pinkySim.
 */


/**
 * Function to enable power to a specified analog block.
 *
 * firmware offsets: 0x000005a4 - 0x000005b8
 * 
 * \param reg0 Set bit(s) specify which blocks to power. See 3.5.41 
 *	Power configuration register in UM10462 for details.
 * 
 * \return None.
 */
void pwrAnalogBlock(uint32_t reg0)
{
	// Power Configuration Register                                                 
	uint32_t* reg = 0x40048238;
	uint32_t val = 0;

	// Read current register value                                                           
	val = *reg;                                                                     
	// Clear reserved bit that must stay cleared                                    
	val &= 0x000005ff;                                                              
	// Clear desired bit (clearing enables desired block(s))
	val &= ~(reg0 & 0x000005ff);                                                           
	// Reserved bits that must always be set                                        
	val |= 0xe800;                                                                  

	// Write result to register
	*reg = val;  
}

/**
 * In this simulation run the system was run from reset with no external input
 *  (except steps necessary to simulate expected hardware unit reactions). Possible
 *  triggering of IRQs were ignored. Parsed from exeLog_00000000001496459595.csv.
 *
 * ENTRY POINT 0x000000d4, as defined in Vector Table entry for RESET
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

	// Make sure crystal oscillator is powered                                      
	pwrAnalogBlock(reg0 = 0x00000020);


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

	// Make sure system PLL powered                                                 
	pwrAnalogBlock(reg0 = 0x00000080);


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

	// Set USB PLL and USB transceiver to powered                                   
	pwrAnalogBlock(reg0 = 0x00000500);


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
	//	0x00000494 - 0x00000496

	// Clear our heap?
	Clear 0x10000200 - 0x10000260 (inclusive 4 byte writes)
	Clear 0x10000264 - 0x10001c1c (inclusive 4 byte writes)


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

	// Setup for EEPROM read via IAP command (i.e. calculation of system clock frequency):
	//
	// System PLL control register
	// reg = 0x40048008;
	// val = *reg;
	// Perform calculations based on value of system PLL control register
	// Save results to 0x10000260
	//
	//	*0x10001bc0 = 0x0000003e Command code : 62
	//	*0x10001bc4 = 0x00000000 Param0: EEPROM Address = 0
	//	*0x10001bc8 = 0x10000254 Param1: RAM Address = 0x10000254
	//	*0x10001bc8 = 0x10000254 Param2: Number of bytes to be read = 8
	//	*0x10001bd0 = 0x0000b71b Param3: System Clock Frequency = 0x0000b71b

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
	//	... 

	// boot ROM code for executing IAB command
	iap_entry(command_param = 0x10001bc0, status_result = 0x10001bd4);

	// Surprisingly no check of status_result...


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
		//	*0x10001bb8 = 0x0000003d Command code : 61 (0x3d)
		//	*0x10001bbc = 0x00000000 Param0: EEPROM Address = 0
		//	*0x10001bc0 = 0x10000254 Param1: RAM Address = 0x10000254
		//	*0x10001bc4 = 0x00000008 Param2: Number of bytes to be read = 8
		//	*0x10001bc8 = 0x0000b71b Param3: System Clock Frequency = 0x0000b71b

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
		//	... 

		// boot ROM code for executing IAB command

		iap_entry(command_param = 0x10001bb8, status_result = 0x10001bcc);	

		// Surprisingly no check of status_result...


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

	// Setup for EEPROM read via IAP command (i.e. calculation of system clock frequency)
	//	*0x10001bc8 = 0x0000003e Command code : 62 (0x3e)
	//	*0x10001bcc = 0x00000500 Param0: EEPROM Address = 0x00000500
	//	*0x10001bd0 = 0x10001c08 Param1: RAM Address = 0x10001c08
	//	*0x10001bd4 = 0x00000004 Param2: Number of bytes to be read = 4
	//	*0x10001bd8 = 0x0000b71b Param3: System Clock Frequency = 0x0000b71b

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
	//	... 

	// boot ROM code for executing IAP command
	
	iap_entry(command_param = 0x10001bc8, status_result = 0x10001bdc);	

	// Surprisingly no check of status_result...


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

	// Fill in USBD_API_INIT_PARAM_T
	*0x10001ba0 = 0x40080000; // usb_reg_base - USB device controller's base register address
	*0x10001ba4 = 0x20004000; // mem_base - Base memory location from where the stack can allocate
                      // data and buffers. \note The memory address set in this field
                      // should be accessible by USB DMA controller. Also this value
                      // should be aligned on 2048 byte boundary
	*0x10001ba8 = 0x00000800; // mem_size - The size of memory buffer which stack can use.       
                      // \note The \em mem_size should be greater than the size    
                      // returned by USBD_HW_API::GetMemSize() routine
	*0x10001bac = 0x00000003; // max_num_ep - max number of endpoints supported by the USB device  
                      // controller instance (specified by \em usb_reg_base field) 
                      // to which this instance of stack is attached.

	*0x10001bb0 = 0x00000000; // USB_Reset_Event - USB Device Events Callback Function
	*0x10001bb4 = 0x00000000; // USB_Suspend_Event - USB Device Events Callback Function
	*0x10001bb8 = 0x00000000; // USB_Resume_Event - USB Device Events Callback Function
	*0x10001bbc = 0x00000000; // reserved_sbz 

	*0x10001bc0 = 0x00000000; // USB_SOF_Event - USB Device Events Callback Function
	*0x10001bc4 = 0x00000000; // USB_WakeUpCfg 
	*0x10001bc8 = 0x00000000; // USB_Power_Event
	*0x10001bcc = 0x00000000; // USB_Error_Event

	*0x10001bd0 = 0x00000000; // USB_Configure_Event
	*0x10001bd4 = 0x00000000; // USB_Interface_Event
	*0x10001bd8 = 0x00000000; // USB_Feature_Event
	*0x10001bdc = 0x00000000; // Reserved for future use, should be set to 0

	*0x10001be0 = 0x00000000; // Reserved for future use, should be set to 0

	// Fill in USB_CORE_DESCS_T
	*0x10001be4 = 0x00001670; // device_desc - Pointer to USB device descriptor
	*0x10001be8 = 0x00001682; // string_desc - Pointer to array of USB string descriptors
	*0x10001bec = 0x10000208; // full_speed_desc - Pointer to USB device configuration descriptor 
                            // when device is operating in full speed mode
	*0x10001bf0 = 0x10000208; // high_speed_desc - Pointer to USB device configuration descriptor 
                            // when device is operating in high speed mode. For  
                            // full-speed only implementation this pointer should
                            // be same as full_speed_desc.
	*0x10001bf4 = 0x00000000; // device_qualifier; /**< Pointer to USB device qualifier descriptor. For
                            // full-speed only implementation this pointer should
                            // be set to null (0).
	

        // Entry Num: 60565 - 64185 
        // Step Num: 46753 - 49528
	// Firmware Offset(s): 
	//	0x1fff351c - 0x1fff3562
	//	... 

	USBD_HW_API->Init(USBD_HANDLE_T* phUsb = 0x1000022c, USB_CORE_DESCS_T* pDesc = 0x10001be4, USBD_API_INIT_PARAM_T* param = 0x10001ba0)
	// \param[in,out] phUsb Pointer to the USB device stack handle of type USBD_HANDLE_T.
	// \param[in]  pDesc Structure containing pointers to various descriptor arrays needed by the stack.
	// 		  These descriptors are reported to USB host as part of enumerations process.
	// \param[in]  param Structure containing USB device stack initialization parameters.
	// \return Returns \ref ErrorCode_t type to indicate success or error condition.
	// 	\retval LPC_OK(0) On success                                      
	// 	\retval ERR_USBD_BAD_MEM_BUF(0x0004000b) When insufficient memory buffer is passed or memory
        //                                    is not aligned on 2048 boundary.

	// Reg 0 = 0 -> Return code LPC_OK
	// *0x1000022c = 0x200040b8 --> USB_HANDLE_T USB device stack handle

//TODO: Worth looking at decomp of Boot ROM call below or just rely on function description and input parameter values?

		// Clear 0x20004000 - 0x2000405f (byte-wise zero write)
		// Clear 0x200040b8 - 0x20004223 (byte-wise zero write)
		// Clear 0x20004090 - 0x200040b7 (byte-wise zero write)

		// USB EP Command/Status List:
		*0x20004000 = 0x00400109 // EP0 OUT Buffer NBytes | EP0 OUT Buffer Address Offset
		*0x20004004 = 0x0040010a // Reserved | SETUP bytes Buffer Address Offset
		*0x20004008 = 0x00400109 // EP0 IN Buffer NBytes | EP0 IN Buffer Address Offset
		*0x2000400c = 0x40000000 // Reserved | Reserved
		*0x20004010 = 0x40000000 // EP 1 OUT Buffer 0
		*0x20004014 = 0x40000000 // EP 1 OUT Buffer 1
		*0x20004018 = 0x40000000 // EP 1 IN Buffer 0
		*0x2000401c = 0x40000000 // EP 1 IN Buffer 1
		*0x20004020 = 0x40000000 // EP 2 OUT Buffer 0
		*0x20004024 = 0x40000000 // EP 2 OUT Buffer 1
		*0x20004028 = 0x40000000 // EP 2 IN Buffer 0
		*0x2000402C = 0x00000000 // EP 2 IN Buffer 1
		*0x20004030 = 0x20004240 // EP 3 OUT Buffer 0
		*0x20004034 = 0x00000040 // EP 3 OUT Buffer 1
		*0x20004038 = 0x20004280 // EP 3 IN Buffer 0
		*0x2000403c = 0x00000008 // EP 3 IN Buffer 1
		*0x20004040 = 0x20004240 // EP 4 OUT Buffer 0
		*0x20004044 = 0x00000000 // EP 4 OUT Buffer 1
		*0x20004048 = 0x00000000 // EP 4 IN Buffer 0
		*0x2000404c = 0x00000000 // EP 4 IN Buffer 1

		*0x20004090 = 0x20004000
		*0x20004094 = 0x20004030
		*0x20004098 = 0x00000000
		*0x2000409c = 0x00000000
		*0x200040a0 = 0x200042c0
		*0x200040a4 = 0x00000000
		*0x200040a8 = 0x00000000
		*0x200040aC = 0x00000000
		*0x200040b0 = 0x40080000
		*0x200040b4 = 0x200040b8

		// This is the USB device stack handle (USBD_HANDLE_T), but not sure what this means exactly...
		*0x200040b8 = 0x1fff27bf 
		*0x200040bc = 0x1fff28d1
		*0x200040c0 = 0x00000000
		*0x200040c4 = 0x1fff22ab
		*0x200040c8 = 0x1fff23f3
		*0x200040cc = 0x1fff2491
		*0x200040d0 = 0x1fff24ad
		*0x200040d4 = 0x1fff2651
		*0x200040d8 = 0x1fff268d
		*0x200040dc = 0x1fff2329
		*0x200040e0 = 0x00000000
		*0x200040e4 = 0x00000000
		*0x200040e8 = 0x00000000
		*0x200040ec = 0x00000000
		*0x200040f0 = 0x00000000
		*0x200040f4 = 0x00000000
		*0x200040f8 = 0x00000000
		*0x200040fc = 0x00000000
		*0x20004100 = 0x00000000
		*0x20004104 = 0x00000000
		*0x20004108 = 0x1fff29d3
		*0x2000410c = 0x1fff29d5
		*0x20004110 = 0x1fff293d
		*0x20004114 = 0x1fff293d
		*0x20004118 = 0x1fff2e55
		*0x2000411c = 0x1fff2e55
		*0x20004120 = 0x1fff2e55
		*0x20004124 = 0x1fff2e55
		*0x20004128 = 0x1fff2e55
		*0x2000412c = 0x1fff2e55
		*0x20004130 = 0x1fff2e55	
		*0x20004134 = 0x1fff2e55
		*0x20004138 = 0x200040b8
		*0x2000413c = 0x200040b8
		*0x200041a0 = 0x00000000
		*0x200041a4 = 0x00000000
		*0x200041a8 = 0x00001670
		*0x200041ac = 0x00001682
		*0x200041b0 = 0x10000208
		*0x200041b4 = 0x10000208
		*0x200041b8 = 0x00000000
		*0x200041bc = 0x00000000

		*0x200041d0 = 0x20004090

		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0;

		// USB EP Command/Status List start address
		reg = 0x40080008;
		*reg = 0x20004000;
		// TODO: See how 0x20004000 has been filled out to build this

		// USB Data buffer start address (DATABUFSTART)
		reg = 0x4008000c;
		*reg = 0x20004240;

		// USB Endpoint Buffer in use (EPINUSE)
		reg = 0x40080018;
		*reg = 0x00000000;

		// USB Endpoint skip (EPSKIP)
		reg = 0x40080014;
		*reg = 0x00000000;

		// USB Endpoint Buffer Configuration (EPBUFCFG)
		reg = 0x4008001c;
		*reg = 0x000003ff;

		// USB interrupt status register (INTSTAT)
		reg = 0x40080020;
		*reg = 0xc00003ff;
		
		// USB interrupt enable register (INTEN)
		reg = 0x40080024;
		*reg = 0x800003ff;

		//TODO These are reading back register looking for hardware to set bits?
		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0x00000000;
		
		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0x00000080;
		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0x00000080;
		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0x00000080;
		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0x00000080;
		// USB Device Command/Status register (DEVCMDSTAT)
		reg = 0x40080000;
		*reg = 0x00000080;


        // Entry Num: 64186 - 64203
        // Step Num: 49529 - 49540
	// Firmware Offset(s): 
	//	0x00000ad2 - 0x00000ae8

	// Check return value of Boot ROM code (Reg 0) is 0 (LPC_OK)
	// 	TODO: UNKNOWN PATHS Branch to 0x00000b14 if Boot ROM code does not return 0 (i.e. failure of USBD_HW_API->Init())


        // Entry Num: 64204 - 64504
        // Step Num: 49541 - 49780
	// Firmware Offset(s): 
	//	0x00000b40 - 0x00000b50
	//	0x0000035e - 0x00000360
	//	0x00000350 - 0x0000035c

	// Clear RAM (byte-wise) 0x10001b50 - 0x10001b87


        // Entry Num: 64505 - 64529
        // Step Num: 49781 - 49796
	// Firmware Offset(s): 
	//	0x00000b54 - 0x00000b72

	*(uint8_t*)0x10001b58 = 0x01
	*(uint16_t*)0x10000234 = 0x0021
	*(uint8_t*)0x10000236 = 0x00
	*0x10000238 = 0x0000164c

	// Check if Reg 4 is 0
	// 	TODO: UNKNOWN PATHS Branch to 0x00000b74 if Reg 4 is 0

	// Check if byte at RAM address 0x10000216 is 3
	// Thisis set during some init code when initially setting up RAM (instruction 0x00001532)... Not sure what this byte represents... Maybe number of USB EPs?
	// 	TODO: UNKOWN PATHS do not branch and instead execute 0x00000b72 if this is false


        // Entry Num: 64530 - 64557
        // Step Num: 49797 - 49812
	// Firmware Offset(s): 
	//	0x00000b7c - 0x00000b9a

	// Fill in USBD_HID_INIT_PARAM_T
	*0x10001b50 = 0x20004340 // mem_base - Base memory location from where the stack can allocate
                      // data and buffers. \note The memory address set in this field
                      // should be accessible by USB DMA controller. Also this value
                      // should be aligned on 4 byte boundary
	*0x10001b54 = 0x000004c0 // mem_size - The size of memory buffer which stack can use.       
                      // \note The \em mem_size should be greater than the size    
                      // returned by USBD_HID_API::GetMemSize() routine.
	*0x10001b58 = 0x00000001 // max_reports - Number of HID reports supported by this instance    
                      // of HID class driver
	*0x10001b5c = 0x10000211 // intf_desc - Pointer to the HID interface descriptor within the   
                      // descriptor array (\em high_speed_desc) passed to Init()   
                      // through \ref USB_CORE_DESCS_T structure.
	*0x10001b60 = 0x10000234 // report_dataa - Pointer to an array of HID report descriptor
                      // data structure (\ref USB_HID_REPORT_T). The number
                      // of elements in the array should be same a \em max_reports
                      // value. The stack uses this array to respond to 
                      // requests received for various HID report descriptor
                      // information. \note This array should be of global scope
	*0x10001b64 = 0x00000961 // HID_GetReport
	*0x10001b68 = 0x0000098d // HID_SetReport


        // Entry Num: 64558 - 65051
        // Step Num: 49813 - 50172
	// Firmware Offset(s): 
	//	0x1fff2cfc - 0x1fff2d0e
	//	...

	USBD_HID_API_T->init(USBD_HANDLE_T hUsb = 0x200040b8, USBD_HID_INIT_PARAM_T* param = 0x10001b50)
	// \param[in] hUsb Handle to the USB device stack.                           
	// \param[in, out] param Structure containing HID function driver module     
	//     initialization parameters.                                            
	// \return Returns \ref ErrorCode_t type to indicate success or error condition.
	// 	\retval LPC_OK On success                                         
	// 	\retval ERR_USBD_BAD_MEM_BUF  Memory buffer passed is not 4-byte  
	// 	    aligned or smaller than required.                             
	// 	\retval ERR_API_INVALID_PARAM2 Either HID_GetReport() or HID_SetReport()
	// 	    callback are not defined.                                     
	// 	\retval ERR_USBD_BAD_DESC  HID_HID_DESCRIPTOR_TYPE is not defined 
	// 	    immediately after interface descriptor.                       
	// 	\retval ERR_USBD_BAD_INTF_DESC  Wrong interface descriptor is passed. 
	// 	\retval ERR_USBD_BAD_EP_DESC  Wrong endpoint descriptor is passed. 

	// Reg 0 = 0 -> Return code LPC_OK


        // Entry Num: 65052 - 65064
        // Step Num: 50173 - 50180
	// Firmware Offset(s): 
	//	0x00000b9c - 0x00000b9c
	//	0x00000b78 - 0x00000b7a
	//	0x00000aec - 0x00000af4

	// Check that return value of Boot ROM code (Reg 0) is 0 (LPC_OK)
	// 	TODO: UNKOWN PATHS branching to 0x00000b14 if Boot ROM code does not return 0 (i.e. failure of USBD_HID_API_T->init())


        // Entry Num: 65065 - 65085
        // Step Num: 50181 - 50198
	// Firmware Offset(s): 
	//	0x00000d40 - 0x00000d50
	//	0x00000d6a - 0x00000d7a

	// Input argument is checked to be >= 0, however, is set by call into function, so no path to dig down here, yet

	// Set IP_USB_IRQ priority to highest via Interrupt Priority Register 5
	reg = 0xe000e414;
	val = *reg;
	val &= ~0x00ff0000;
	*reg = val;


        // Entry Num: 65086 - 65094
        // Step Num: 50199 - 50205
	// Firmware Offset(s): 
	//	0x00000af8 - 0x00000b04

	// Enable interrupt 22 (USB_IRQ USB IRQ interrupt) via ISER register of NVIC
	reg = 0xe000e100;
	*reg = 0x00400000;


        // Entry Num: 65095 - 65115
        // Step Num: 50206 - 50223
	// Firmware Offset(s): 
	//	0x00000d40 - 0x00000d50
	//	0x00000d6a - 0x00000d7a

	// Set IP_USB_IRQ priority to 1 (one below highest) via Interrupt Priority Register 5
	reg = 0xe000e414;
	val = *reg;
	val &= ~0x00ff0000;
	val |= 0x00400000;
	*reg = val;


        // Entry Num: 65116 - 65125
        // Step Num: 50224 - 50229
	// Firmware Offset(s): 
	//	0x00000b08 - 0x00000b12

	// Prep for Boot ROM code
	// Reg 0 = 0x200040b8
	// Reg 1 = 1


        // Entry Num: 65126 - 65142
        // Step Num: 50230 - 50242
	// Firmware Offset(s): 
	//	0x1fff2e92 - 0x1fff2ea6
	//	0x1fff2eaa - 0x1fff2eac

	USBD_HW_API->Connect(USBD_HANDLE_T hUsb = 0x200040b8, uint32_t con = 1);
	// \param[in] hUsb Handle to the USB device stack.                           
	// \param[in] con  States whether to connect (1) or to disconnect (0).       
	// \return Nothing.


        // Entry Num: 65143 - 65164
        // Step Num: 50243 - 50256
	// Firmware Offset(s): 
	//	0x00000b14 - 0x00000b18
	//	0x00000f2c - 0x00000f2c
	//	0x00001100 - 0x00001112

	*(uint8_t*)0x10000249 = 0x00;

        // Entry Num: 65165 - 65177
        // Step Num: 50257 - 50264
	// Firmware Offset(s): 
	//	0x00000e40 - 0x00000e4e

	*0x100006cc = 0x100005cc
	*0x100006d0 = 0x00000100
	*0x100006d4 = 0x00000001
	*0x100006d8 = 0x00000000
	*0x100006dc = 0x00000000

	int fnc(Reg0 = 0x100006cc, Reg1 = 0x100005cc, Reg2 = 0x00000001, Reg3 = 0x00000100)
	{
		uint32_t* ptr = Reg0;
		ptr[0] = Reg1;
		ptr[1] = Reg3;
		ptr[2] = Reg2;
		ptr[3] = 0;
		ptr[4] = 0;

		return 1;
	}

        // Entry Num: 65178 - 65207
        // Step Num: 50265 - 50282
	// Firmware Offset(s): 
	//	0x00001116 - 0x0000111a
	//	0x00000624 - 0x00000640

	// Enables clock for UART via System clock control register
	reg = 0x40048080;
	val = *reg;
	val |= 0x00001000;
	*reg = val;

	// Set USART clock divider register to divide by 1.
	reg = 0x40048098;
	*reg = 0x00000001;

	// TX FIFO Reset, RX FIFO Reset and FIFO enable via USART FIFO Control Register Write only
	reg = 0x40008008;
	*reg = 0x00000007;

	// Set Word Length Select to 8-bit character length via USART Line Control Register
	reg = 0x4000800c;
	*reg = 0x00000003;

	// Set Baud rate generation pre-scaler divisor value to 0 and Baud rate pre-scaler multiplier value to 1 via USART Fractional Divider Register
	reg = 0x40008028;
	*reg = 0x00000010;

        // Entry Num: 65208 - 65241
        // Step Num: 50283 - 50305
	// Firmware Offset(s): 
	//	0x0000111e - 0x0000114a

	// Set RX Trigger level to 3 (14 characters or 0x0E) and FIFO enable via USART FIFO Control Register Write only
	reg = 0x40008008;
	*reg = 0x00000081;

	// Enable access to Divisor Latches via USART Line Control Register
	reg = 0x4000800c;
	val = *reg;
	val |= 0x00000080;
	*reg = val;

	// Set the Divisor Latch LSB register to set the baud rate of the USART via the USART Divisor Latch LSB Register when DLAB = 1
	reg = 0x40008000;
	*reg = 0x00000003;

	// Set the Baud rate generation pre-scaler divisor value to 1 and Baud rate pre-scaler multiplier value to 0xb via the USART Fractional Divider Register
	reg = 0x40008028;
	*reg = 0x000000b1;

	// Disable access to Divisor Latches via USART Line Control Register
	reg = 0x4000800c;
	val = *reg;
	val &= ~0x00000080;
	*reg = val;
	
	// Enable interrupt 21 (USART interrupt) via Interrupt Set-enable Register
	reg = 0xe000e100;
	*reg = 0x00200000;

	// Enables the Receive Data Available 0 interrupt and Enables the Receive Line Status interrupt via USART Interrupt Enable Register when DLAB = 0
	reg = 0x40008004;
	val = *reg;
	val |= 0x00000005;
	*reg = val;

        // Entry Num: 65242 - 65262
        // Step Num: 50306 - 50323
	// Firmware Offset(s): 
	//	0x00000d84 - 0x00000d94
	//	0x00000dae - 0x00000dbe

	// I think this function is for changing interrupt priority

	// Check if Reg 0 is greater than or equal to 0
	//	TODO: UNKOWN PATHS continue to execute 0x00000d96, if not
	//	However, this is impossible as Reg 0 is set to 15 before this function is called

	// Set IP_USART0 to highest interrupt priority
	reg = 0xe000e414;
	val = *reg;
	val &= ~0x0000ff00;
	val |= 0;
	*reg = val;

        // Entry Num: 65263 - 65289
        // Step Num: 50324 - 50347
	// Firmware Offset(s): 
	//	0x0000114e - 0x00001152
	//	0x00000d84 - 0x00000dac

	// Set Priority of system handler 14, PendSV to 0x40 (TODO: what does this translate to?)
	reg = 0xe000ed20;
	val = *reg;
	val &= ~0x00ff0000;
	val |= 0x00400000;
	*reg = val;

        // Entry Num: 65290 - 65304
        // Step Num: 50348 - 50357
	// Firmware Offset(s): 
	//	0x00001156 - 0x00001156
	//	0x00000f30 - 0x00000f30
	//	0x000012bc - 0x000012be
	//	0x00000ce8 - 0x00000cf0
	//	0x00000cf4 - 0x00000cf4

	// val = *((uint8_t*)0x50000003);
	// Check state of P0_3 and make sure it is 0
		// TODO: UKNOWN PATHS (quick check shows this eliminates long while loop coming up...)

        // Entry Num: 65305 - 265309
        // Step Num: 50358 - 250361
	// Firmware Offset(s): 
	//	0x000012c2 - 0x000012d0

	// TODO: what might we be waiting for? And is this only is P0_3 reads 0? What if it doesn't?
	for (int cnt = 0; cnt < 0x0000c350; cnt++){}

        // Entry Num: 265310 - 265334
        // Step Num: 250362 - 250376
	// Firmware Offset(s): 
	//	0x000012ac - 0x000012b2
	//	0x00001210 - 0x00001216
	//	0x000007b0 - 0x000007ba
	//	0x0000121a - 0x0000121a

	// For marking cps state of disabling interrupts (I think)
	*0x10000250 += 1;

        // Entry Num: 265335 - 265376
        // Step Num: 250377 - 250399
	// Firmware Offset(s): 
	//	0x0000129c - 0x000012a2
	//	0x000011f8 - 0x00001202
	//	0x00000648 - 0x00000660

	// Make sure THRE is disabled via USART Interrupt Enable Register when DLAB = 0
	reg = 0x40008004;
	val = *reg;
	val &= ~0x00000002;
	*reg = val;

        // Entry Num: 265377 - 
        // Step Num: 250400 - 
	// Firmware Offset(s): 
	//	0x00000e50 - 

	//TODO: Remember to pay attention to branches/paths simulation does and does not take.
	//TODO: Keep in mind that system will continue after WFI. Need to walk through this simulation and see about options (how does controller shutdown...?)
	//		I think what happens is that WFI is called for interrupt to run that sets particular registers. 
	//		Maybe 32 bit timer interrupting before other things happening causes variables to get set in a way such that power off occurs
	//		Otherwise controller might stay in this "idle" loop, or go into other "idle" loop, waiting for shutdown signal?
}

/**                                                                           
  *  HID get report callback function.                                          
  *                                                                             
  *  This function is provided by the application software. This function gets called 
  *  when host sends a HID_REQUEST_GET_REPORT request. The setup packet data (\em pSetup)
  *  is passed to the callback so that application can extract the report ID, report
  *  type and other information need to generate the report. \note HID reports are sent
  *  via interrupt IN endpoint also. This function is called only when report request
  *  is received on control endpoint. Application should implement \em HID_EpIn_Hdlr to
  *  send reports to host via interrupt IN endpoint.                            
  *                                                                             
  *                                                                             
  *  \param[in] hHid Handle to HID function driver.                             
  *  \param[in] pSetup Pointer to setup packet received from host.              
  *  \param[in, out] pBuffer  Pointer to a pointer of data buffer containing report data. 
  *                       Pointer-to-pointer is used to implement zero-copy buffers. 
  *                       See \ref USBD_ZeroCopy for more details on zero-copy concept.
  *  \param[in] length  Amount of data copied to destination buffer.            
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.                                         
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions.                  
  *                                                                             
  * ENTRY POINT 0x00000961, as defined in USBD_HID_INIT_PARAM_T passed into USBD_HID_API_T->init().
  */                                                                            
ErrorCode_t HID_GetReport( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuffer, uint16_t* length)
{
        // Entry Num: 
        // Step Num: 
	// Firmware Offset(s): 
	//	0x00000961 - 

	//TODO: Simulate this?

}

  /**                                                                           
  *  HID set report callback function.                                          
  *                                                                             
  *  This function is provided by the application software. This function gets called 
  *  when host sends a HID_REQUEST_SET_REPORT request. The setup packet data (\em pSetup)
  *  is passed to the callback so that application can extract the report ID, report
  *  type and other information need to modify the report. An application might choose 
  *  to ignore input Set_Report requests as meaningless. Alternatively these reports 
  *  could be used to reset the origin of a control (that is, current position should 
  *  report zero).                                                              
  *                                                                             
  *  \param[in] hHid Handle to HID function driver.                             
  *  \param[in] pSetup Pointer to setup packet received from host.              
  *  \param[in, out] pBuffer  Pointer to a pointer of data buffer containing report data. 
  *                       Pointer-to-pointer is used to implement zero-copy buffers. 
  *                       See \ref USBD_ZeroCopy for more details on zero-copy concept.
  *  \param[in] length  Amount of data copied to destination buffer.            
  *  \return The call back should returns \ref ErrorCode_t type to indicate success or error condition.
  *          \retval LPC_OK On success.                                         
  *          \retval ERR_USBD_UNHANDLED  Event is not handled hence pass the event to next in line. 
  *          \retval ERR_USBD_xxx  For other error conditions.                  
  *                                                                             
  * ENTRY POINT 0x0000098d, as defined in USBD_HID_INIT_PARAM_T passed into USBD_HID_API_T->init().
  */                                                                            
ErrorCode_t HID_SetReport( USBD_HANDLE_T hHid, USB_SETUP_PACKET* pSetup, uint8_t** pBuffer, uint16_t length)
{
        // Entry Num: 
        // Step Num: 
	// Firmware Offset(s): 
	//	0x0000098d - 

	//TODO: Simulate this?

}

/**
 * This sectionis set aside to track and define different parts of RAM, etc.

(Possible) Stack memory:

USBD_HID_INIT_PARAM_T
0x10001b50
...
0x10001b68

0x10001bc8

0x10001bdc

// Stack counts down from 0x10000200 and heap counts up

0x10000208 Pointer to USB device configuration descriptor when device is operating in full and high speed modes.

0x10000211 Pointer to the HID interface descriptor within the descriptor array (\em high_speed_desc) passed to Init() through \ref USB_CORE_DESCS_T structure.

0x10000234 USB_HID_REPORT_T*
//TODO: size and contents of USB_HID_REPORT_T

0x10000250 incremented/decrement before/after cps. Has to do with disabling interrupts (I think)

0x200040b8 - USBD_HANDLE_T - Handle to the USB device stack
//TODO: size and content of USBD_HANDLE_T

0x20004340 -  USB HID Base memory location from where the stack can allocate data and buffers
...
0x20004800

 *
 */
