/**
 * All captured data from simulation vcf_wired_controller_d0g_57bf5c10.bin using
 *  pinkySim.
 */


/**
 * Function to enable power to a specified analog block.
 *
 * Firmware Offset(s): 
 *	0x000005a4 - 0x000005b8
 * 
 * \param reg0 Set bit(s) specify which blocks to power. See 3.5.41 
 *	Power configuration register in UM10462 for details.
 * 
 * \return None.
 */
void pwrAnalogBlock(uint32_t reg0)
{
	// Power Configuration Register                                                 
	volatile uint32_t* reg = 0x40048238;
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
 * Function for setting a 32-bit register to a particular value.
 * 
 * Firmware Offset(s): 
 * 	0x00000572 - 0x00000576
 *	0x00000578 - 0x0000057c
 * 	0x0000057e - 0x00000580
 *
 * \param baseReg Base register value
 * \param additionalOffset Add additional 0x60 byte offset to baseReg
 * \param regWordOffset Word sized offset from baseReg indicates which register to change
 * \param regVal Value to write to register
 * 
 * \return None.
 */
void set32bitReg(uint32_t baseReg, uint8_t additionalOffset, uint32_t regWordOffset, uint32_t regVal)
{
	regWordOffset <<= 2;

	if (additionalOffset != 0)
	{
		// Execture instruction 0x00000578
		uint32_t* reg = baseReg + regWordOffset + 0x60;
		*reg = regVal;
		return;
	}

	uint32_t* reg = baseReg + regWordOffset;
	*reg = regVal;
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
	volatile uint32_t* reg = NULL;
	uint32_t val = 0;

//TODO: some section of this is default init provided by lpcexpresso. Verify and identify.

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

	// Initialize heap
	*0x10000200 = 0x00000000
	*0x10000204 = 0x00002000	
	*0x10000208 = 0x00220209 // Passed as full_speed_desc later...
	*0x1000020c = 0x80000101
	*0x10000210 = 0x00040932
	*0x10000214 = 0x00030100
	*0x10000218 = 0x21090000
	*0x1000021c = 0x01000111	
	*0x10000220 = 0x07002122	
	*0x10000224 = 0x40038105	
	*0x10000228 = 0x00000600
	*0x1000022c = 0x00000000
	*0x10000230 = 0x00000000
	*0x10000234 = 0x00000000
	*0x10000238 = 0x00000000
	*0x1000023c = 0x00000000
	*0x10000240 = 0x00000000
	*0x10000244 = 0x00000000	
	*0x10000248 = 0x00000000	
	*0x1000024c = 0x00000000	
	*0x10000250 = 0x00000000	
	*0x10000254 = 0x00000000	
	*0x10000258 = 0x00000000	
	*0x1000025c = 0x00000000 // Bytes seem to be used for marked CT32B1 as enabled, and whether CT32B1 interrupt occurred
	*0x10000260 = 0x00000000
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
	//	0x00000322 - 0x00000328
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
	// Check state of PIO0_3 (USB voltage detected) and make sure it is 0
		// TODO: UKNOWN PATHS
		//	if PIO0_3 is not 0 (USB cable is conecteD) instruction at 0x00000cf2 is executed


        // Entry Num: 57999 - 58008
        // Step Num: 44530 - 44537
	// Firmware Offset(s): 
	//	0x00001560 - 0x0000157c

	// Again check that PIO0_3 is zero
//TODO: revisit this part of sim
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
	//	TODO: UNKNOWN PATHS
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

	// Set GPREG1 to 0
	reg = 0x40038008;
	*reg = 0;


        // Entry Num: 58068 - 58087
        // Step Num: 44579 - 44594
	// Firmware Offset(s): 
	//	0x00001586 - 0x0000159a
	//	0x00000572 - 0x00000576
	//	0x0000057e - 0x00000580

	// Enable pull down resistor on PIO0_3 register
	// *(uint32_t*)0x4004400c = 0x00000008
	set32bitReg(uint32_t baseReg = 0x40044000, uint8_t additionalOffset = 0, regWordOffset = 0x00000003, regVal = 0x00000008);


        // Entry Num: 58088 - 58105
        // Step Num: 44595 - 44610
	// Firmware Offset(s): 
	//	0x0000159e - 0x000015a2
	//	0x0000158c - 0x0000159a
	//	0x00000572 - 0x00000576
	//	0x0000057e - 0x00000580

	// Note there are two conditional paths in here, but they are impossible to reach given setup, so they are being ignored

	// Set PIO0_6 to function as ~USB_CONNECT
	// *(uint32_t*)0x40044018 = 0x00000001
	set32bitReg(uint32_t baseReg = 0x40044000, uint8_t additionalOffset = 0, regWordOffset = 0x00000006, regVal = 0x00000001);
	

        // Entry Num: 58106 - 58127
        // Step Num: 44611 - 44630
	// Firmware Offset(s): 
	//	0x0000159e - 0x000015a2
	//	0x0000158c - 0x0000159a
	//	0x00000572 - 0x0000057c

	// Note there in one unknown conditional paths in here, but it is impossible to reach given setup, so it is being ignored

//TODO: watch this line to see comms between LPC11U37F and Radio Chip
	// Set PIO1_17 to function as RXD - Receiver input for USART
	// *(uint32_t*)0x400440a4 = 0x00000002
	set32bitReg(uint32_t baseReg = 0x40044000, uint8_t additionalOffset = 1, regWordOffset = 0x00000011, regVal = 0x00000002);


        // Entry Num: 58128 - 58143
        // Step Num: 44631 - 44644
	// Firmware Offset(s): 
	//	0x0000158c - 0x0000159a
	//	0x00000572 - 0x0000057c

	// Set PIO1_18 to function as TXD - Transmitter output for USART
	// *(uint32_t*)0x400440a8 = 0x00000002
	set32bitReg(uint32_t baseReg = 0x40044000, uint8_t additionalOffset = 1, regWordOffset = 0x00000012, regVal = 0x00000002);


        // Entry Num: 58144 - 58154
        // Step Num: 44645 - 44654
	// Firmware Offset(s): 
	//	0x0000159e - 0x000015a4
	//	0x00000ce8 - 0x00000cf2
	//	0x00000cf4 - 0x00000cf4

	// val = *((uint8_t*)0x50000003);
	// Check state of PIO0_3 (USB voltage detected) and make sure it is 0
		// TODO: UKNOWN PATHS
		//	if PIO0_3 is not 0 (USB cable is conecteD) instruction at 0x00000cf2 is executed


        // Entry Num: 58155 - 58164
        // Step Num: 44655 - 44660
	// Firmware Offset(s): 
	//	0x000015a8 - 0x000015ac
	//	0x00000f90 - 0x00000f94

	// Again check that PIO0_3 is zero


        // Entry Num: 58165 - 58181
        // Step Num: 44661 - 44672
	// Firmware Offset(s): 
	//	0x00000cf8 - 0x00000cfc
	//	0x00000f98 - 0x00000fa0
	//	0x00000fae - 0x00000fae
	//	0x00000cf8 - 0x00000cfc

	// reading value in RAM at 0x10000258 and see if is set to 8
	//	TODO: UNKNOWN PATHS
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
	// 	TODO: UNKNOWN PATHS
	//	if value read from EEPROM offset 0x500 is not 0 execute instruction at 0x000015c6
	// }

	// Check if GPREG0 is set to 0xecaabac0
	// reg = 0x40038004;
	// val = *reg;
	// if (val == 0xecaabac0)
	// {
		// TODO: UNKNOWN PATHS
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
	// 0x1fff1f24 is pointer to USBD_HW_API_T
	*0x10000230 = 0x1fff1f24;


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
	// 	TODO: UNKNOWN PATHS do not branch and instead execute 0x00000b72 if this is false


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
	// 	TODO: UNKNOWN PATHS branching to 0x00000b14 if Boot ROM code does not return 0 (i.e. failure of USBD_HID_API_T->init())


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
	//	TODO: UNKNOWN PATHS continue to execute 0x00000d96, if not
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
	// Check state of PIO0_3 and make sure it is 0
		// TODO: UKNOWN PATHS (quick check shows this eliminates long while loop coming up...)


        // Entry Num: 65305 - 265309
        // Step Num: 50358 - 250361
	// Firmware Offset(s): 
	//	0x000012c2 - 0x000012d0

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


        // Entry Num: 265377 - 265394
        // Step Num: 250400 - 250407
	// Firmware Offset(s): 
	//	0x00000e50 - 0x00000e5e

	// Copy a RAM value
	*0x10001b88 = *0x100006cc


        // Entry Num: 265395 - 265403
        // Step Num: 250408 - 250413
	// Firmware Offset(s): 
	//	0x00000ee2 - 0x00000eec

	if (*0x100006d0 <= *0x100006d8 - *0x100006dc)
	{
		// TODO: UNKNOWN PATHS execute instruction at 0x00000eee
	}


        // Entry Num: 265404 - 265407
        // Step Num: 250414 - 250417
	// Firmware Offset(s): 
	//	0x00000ef2 - 0x00000ef4
	//	0x00000e62 - 0x00000e64

	// 	TODO: UNKNOWN PATHS execute instructiont 0x00000e64 if Reg 0 is not sure, but this never happens here as we before prev branch Reg 0 is set to 0


        // Entry Num: 265408 - 265442
        // Step Num: 250418 - 250445
	// Firmware Offset(s): 
	//	0x00000e6c - 0x00000ea2

	// TODO: try to understand what this is doing...
	Reg0 = *0x100006d8	
	Reg1 = *0x100006dc
	Reg1 = Reg0 - Reg1

	Reg0 = *0x100006d0	
	Reg2 = Reg0 - Reg1

	Reg1 = *0x100006d8

	Reg3 = Reg0 - 1

	Reg5 = Reg2

	if ((Reg3 - Reg0) => Does Not Produce Carry Out)
	{
		Reg5 = Reg0 - Reg1
	}

	Reg6 = Reg2 - Reg5

	if (Reg5 >= Reg6)
	{
		Reg5 = Reg7
	}

	Reg0 = Reg7 - Reg5

	if (Reg6 >= Reg0)
	{
		Reg6 = Reg0
	}

	Reg2 = *0x100006d4	
	Reg0 = *0x10001b88
	Reg1 = Reg2 * Reg1

	Reg2 = Reg5 * Reg2

	Reg1 = *0x10001b94


        // Entry Num: 265443 - 265446
        // Step Num: 250446 - 250449
	// Firmware Offset(s): 
	//	0x0000032c - 0x00000332

	Reg3 = Reg0
	Reg3 |= Reg1
	Reg3 <<= 30;	

	if (Reg3 != 0)
	{
		Branch to 0x0000034a (i.e. skip 0x0000033c - 0x00000340)
	}


        // Entry Num: 265447 - 265482
        // Step Num: 250450 - 250477
	// Firmware Offset(s): 
	//	0x0000033c - 0x00000340
	//	0x0000034a - 0x0000034c
	//	0x00000342 - 0x0000034e
	//	0x00000ea6 - 0x00000ec4

	//TODO: bunch more RAM math and paths to check here...
	*0x100005cc = 0x00000002
	*0x100006d8 = 0x00000001


        // Entry Num: 265483 - 265487
        // Step Num: 250478 - 250482
	// Firmware Offset(s): 
	//	0x0000032c - 0x00000334

	//TODO: see previous call, but take Reg3 != 0 path


        // Entry Num: 265488 - 265497
        // Step Num: 250483 - 250490
	// Firmware Offset(s): 
	//	0x0000034a - 0x0000034e
	//	0x00000ec8 - 0x00000ed0

	*0x100006d8 = 0x00000001


        // Entry Num: 265498 - 265524
        // Step Num: 250491 - 250506
	// Firmware Offset(s): 
	//	0x00000e68 - 0x0000066a
	//	0x00000688 - 0x0000068e
	//	0x00000696 - 0x000006a0

	// USART Line Status Register Read only
	reg = 0x40008014;
	val = *reg;

	// Check USART related status receiver data ready, and other statuts bit
	// TODO: care about this?
	//	UNKNOWN PATHS branch to 0x000006a8 if condition met


        // Entry Num: 265525 - 266718
        // Step Num: 250507 - 251315
	// Firmware Offset(s): 
	//	0x00000ef6 - 0x00000efe
	//	...

//TODO: think this area is mostly USART related, skipping over for now. Come back and disect this later
//TODO: bumping priority as this is communicating with Nordic Semiconductor nRF51822 Bluetooth Smart and 2.4GHz proprietary SoC (which will tell us if we connect with USB dongle, right?)

	*0x100006dc = 0x00000001

	// USART Transmitter Holding Register
	*0x40008000 = 0x00000002

	// USART Line Status Register (Read-Only)
	reg = 0x40008014;
	val = *reg;
	// TODO: UNKNOWN PATHS check status (i.e. that transmit occurred?)

	*0x100006d8 = 0x00000001
	*0x100006d8 = 0x00000001
	
	// Enable the THRE interrupt.
	reg = 0x40008004;
	val = *reg;
	val |= 2;
	*reg = val;

	// Disable the THRE interrupt.
	reg = 0x40008004;
	val = *reg;
	val |= ~0x00000002;
	*reg = val;

	// USART Line Status Register (Read-Only)
	reg = 0x40008014;
	val = *reg;
	// TODO: UNKNOWN PATHS check status (i.e. that transmit occurred?)

//TODO: are we looping here looking for USART to react?
//	Maybe at least map out instruction calls to see if we are looping before giving up on USART?
//	Pay attention to RAM addresses and see if one is counting up to a timeout or something...

//TODO: check what USART is connected to on board!


        // Entry Num: 266719 - 266735
        // Step Num: 251316 - 251326
	// Firmware Offset(s): 
	//	0x00000dd4 - 0x00000dde
	//	0x00000572 - 0x00000576
	//	0x0000057e - 0x00000580

	// Set pin function to CT16B1_MAT0 via PIO0_21 register
	// PIO0_21 is connected to button LED
//TODO: Use scope to get timing for on and off pattern and make sure it matches settings
	// *(uint32_t*)0x40044054 = 0x00000081
	set32bitReg(uint32_t baseReg = 0x40044000, uint8_t additionalOffset = 0, regWordOffset = 0x00000015, regVal = 0x00000008);


        // Entry Num: 266736 - 266757
        // Step Num: 251327 - 251342
	// Firmware Offset(s): 
	//	0x00000de2 - 0x00000de6
	//	0x000005f4 - 0x000005f6
	//	0x000005c4 - 0x000005c8
	//	0x000005ce - 0x000005d2
	//	0x000005d8 - 0x000005e0

	// Reg0 = 0x40010000 

	// If Reg0 == 0x40018000
	// UNKNOWN PATHS 0x000005ca -- See below where Reg0 is loaded with 0x40018000
	
	// If Reg0 == 0x4000c000
	// UNKNOWN PATHS 0x000005d4

	// If Reg0 != 0x40010000
	// UNKNOWN PATHS 0x000005e2


        // Entry Num: 266758 - 266796
        // Step Num: 251343 - 251367
	// Firmware Offset(s): 
	//	0x000005fa - 0x00000606
	//	0x00000dea - 0x00000e0c

	// Set prescale value to 0 via Prescale Register for CT16B1
	reg = 0x4001000c;
	*reg = 0x00000000;

//TODO: Is this the pin connected to the Steam Button LED? Ohm it out!
	// PWM mode is enabled for CT16Bn_MAT0 via PWM Control Register for CT16B1
	reg = 0x40010074;
	val = *reg;
	val |= 0x00000001;
	*reg = val;

	// Match Register 3 (MR3) is set to 0xfff via Match registers for CT16B1
	reg = 0x40010024;
	*reg = 0x00000fff;

	// Match Register 0 (MR0) is set to 0x1000 via Match registers for CT16B1
	reg = 0x40010018;
	*reg = 0x00001000;

	// Reset on MR3: the TC will be reset if MR3 matches it via Match Control Register for CT16B1
	reg = 0x40010014;
	val = *reg;
	val |= 0x00000400;
	*reg = val;


        // Entry Num: 266797 - 357026
        // Step Num: 251368 - 319038
	// Firmware Offset(s): 
	//	0x0000060c - 0x00000622

	// Store the Timer Control Register for CT17B1
	reg = 0x40010004;
	val = *reg;

	// Disable the counters via the Timer Control Register for CT16B1
	reg = 0x40010004;
	*reg = 0x00000000;

	// Set the timer counter value for via the Timer Counter register for CT16B1
	reg = 0x40010008;
	*reg = 0x00000001;

	// The Timer Counter and the Prescale Counter are synchronously reset on 
	//	the next positive edge of PCLK. The counters remain reset until 
	//	TCR[1] is returned to zero via the Timer Counter register for CT16B1
	reg = 0x40010004;
	*reg = 0x00000002;

	// Wait for the Timer Counter to read zero, meaning the affect of the 
	//	right to CRST in the previous register access succeeded
	reg = 0x40010008;
	while (0 != *reg){}

	// Write back value previously read from Timer Control Register
	*0x40010004 = val; 

        // Entry Num: 357027 - 357036
        // Step Num: 319039 - 319042
	// Firmware Offset(s): 
	//	0x00000e10 - 0x00000e16

	// Reset timer counter and prescalar counter on next positive edge of PCLK for CT16B1
	//  The counters remain reset until TCR[1] is returned to zero
	reg = 0x40010004;
	val = *reg;
	val |= 1;
	*reg = val;

        // Entry Num: 357037 - 357053
        // Step Num: 319043 - 319053
	// Firmware Offset(s): 
	//	0x00000f38 - 0x00000f3a
	//	0x00000e24 - 0x00000e32
	//	0x00000f3e - 0x00000f3e

	// Write some RAM address
	*0x1000025e = 0x000000ff

	// Set Match Register for CT16B1 MR0 to 0xFFFF
	*0x40010018 = 0x0000ffff

        // Entry Num: 357054 - 357086
        // Step Num: 319054 - 319071
	// Firmware Offset(s): 
	//	0x000009b4 - 0x000009ba
	//	0x000005f4 - 0x000005f6
	//	0x000005c4 - 0x000005cc
	//	0x000005fa - 0x00000606

	// Reg 0 = 0x40018000

	// Enables clock for 32-bit counter/timer 1. CT32B1
	reg = 0x40048080;
	val = *reg;
	val |= 0x00000400;
	*reg = val;

        // Entry Num: 357087 - 357091
        // Step Num: 319072 - 319074
	// Firmware Offset(s): 
	//	0x000009be - 0x000009be
	//	0x00000494 - 0x00000496	

	// Some branches and pushes to stack

        // Entry Num: 357092 - 357109 
        // Step Num: 319074 - 319088
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


        // Entry Num: 357109 - 357127
        // Step Num: 319088 - 319100
	// Firmware Offset(s): 
	//	0x000004d0 - 0x000004d2
	//	0x000004a8 - 0x000004b4
	//	0x000004be - 0x000004c2

//TODO:repeated code from above. Combine into function.
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

        // Entry Num: 357128 - 357806
        // Step Num: 319101 - 319757
	// Firmware Offset(s): 
//TODO: this is similar to above
	//	0x000004d6 - 0x000004dc
	//	0x00000488 - 0x00000492
	//	0x000004e0 - 0x000004e0
	//	0x0000046e - 0x0000046e
	//	0x0000049a - 0x0000049e
	//	0x00000300 - 0x0000030c
	//	0x00000322 - 0x00000328
	//	0x0000030e - 0x0000032a
	//	0x000004a2 - 0x000004a2
//TODO: this is different than above
 	//	0x000009c2 - 0x000009c6

	// System PLL control register
	// reg = 0x40048008;
	// val = *reg;
	// Perform calculations based on value of system PLL control register

        // Entry Num: 357807 - 357850
        // Step Num: 319758 - 319786
	//	0x000009ca - 0x00000a02

	// Set Prescale Register for CT32B1 with value calulcated based on system PLL control register
	reg = 0x4001800c;
	*reg = 0x0000bb7f;

	// Read Match Control Register for CT32B1
	reg = 0x40018014;
	val = *reg;
	// Enable Reset on MR0: the TC will be reset if MR0 matches it.
	val |= 2;
	*reg = val;

	// Read Match Control Register for CT32B1
	reg = 0x40018014;
	val = *reg;
	// Enable Interrupt on MR0: an interrupt is generated when MR0 matches the value in the TC.
	val |= 1;
	*reg = val;

	// Set MR0 (Time counter match value) for CT32B1
	reg = 0x40018018;
	*reg = 0x0000000b;

	// Clear interrupt pending on for CT32B1
	reg = 0xe000e280;
	*reg = 0x00080000;

	// Enable interrupt for CT32B1
	reg = 0xe000e100;
	*reg = 0x00080000;
		
	// Write some values to heap space
	*0x100007e0 = 0x01
	*0x100007e1 = 0x00
	*0x100007e2 = 0x04
	*0x100007e3 = 0x0c

        // Entry Num: 357851 - 357878
        // Step Num: 319787 - 319810
	//	0x000003a2 - 0x000003b2

	// Clear out some heap space
	*0x100007e4 = 0x00
	*0x100007e5 = 0x00
	*0x100007e6 = 0x00
	*0x100007e7 = 0x00

        // Entry Num: 357879 - 357892
        // Step Num: 319811 - 319820
	//	0x00000a06 - 0x00000a18

	// Clear out some heap space
	*0x100007e8 = 0x00
	*0x100007e9 = 0x00
	*0x100007ea = 0x00
	*0x100007eb = 0x00

        // Entry Num: 357893 - 357920
        // Step Num: 319821 - 319844
	//	0x000003a2 - 0x000003b2

	// Clear out more heap space (TODO: this is same code as above, but with different start address)
	*0x100007ec = 0x00
	*0x100007ed = 0x00
	*0x100007ee = 0x00
	*0x100007ee = 0x00

        // Entry Num: 357921 - 357936
        // Step Num: 319845 - 319850
	//	0x00000a1c - 0x00000a26

	// Set a heap value (related to CT32B1 enable maybe...)
	*0x1000025d = 0x01

	// Enable CT32B1 Timer Counter and Prescale Counter for counting
	reg = 0x40018004;
	val = *reg;
	val |= 1;
	*reg = val;

	while (??){
		// Entry Num: 357937 - 357943
		// Step Num: 319851 - 319855
		//	0x00000f42 - 0x00000f48
		//	0x00000f4e - 0x00000f4e

	//TODO: who else would set heap address 0x10000200??
		uint8_t reg0 = *0x10000200;
		if (reg0 != 0){
			// TODO: UNKNOWN PATH execute instruction 0x00000f4a
		}

		// Entry Num: 357944 - 357956
		// Step Num: 319856 - 319863
		//	0x00000a3c - 0x00000a4a

		uint8_t Reg1 = *0x1000025d;

		if (Reg1 == 0){
			// TODO: UNKNOWN PATH skip ahead to 0x00000a6e
		}

		// Check if CT32B1 interrupt has fired
		uint8_t Reg1 = *0x1000025c;

		if (Reg1 != 0){
			//	0x00000a4c - 0x00000a54
			*0x1000025c = 0x00000000

			//	0x0000038e - 0x000003a0
			uint8_t Reg3 = *0x100007e7
			uint32_t val = Reg3;
			val <<= 8;
			uint8_t Reg3 = *0x100007e6
			val |= Reg3;
			val <<= 8;
			uint8_t Reg3 = *0x100007e5
			val |= Reg3;
			val <<= 8;
			uint8_t Reg3 = *0x100007e4
			val |= Reg3;

			Reg0 = val;

			// 	0x00000a58 - 0x00000a5c
			//	0x000003a2 - 0x000003b2
	//TODO: this needs to be generalized into a function call (both what is being set to addresses and and what the addresses are)
	// Filling 4 bytes ...
			*0x100007e4 = 0x00
			*0x100007e5 = 0x00
			*0x100007e6 = 0x00
			*0x100007e7 = 0x00
			
			//	0x00000a60 - 0x00000a60
			//	0x000012e4 - 0x000012e0
			uint8_t Reg0 = *0x10000244	

			//	0x00000a64 - 0x00000a66
			if (Reg0 != 0){
				// TODO: UNKNOWN PATH Execute instruction 0x00000a6a
			}
		}

		// Entry Num: 357957 - 357959
		// Step Num: 319864 - 319864
		//	0x00000a6e - 0x00000a6e
		
		// wait for interrupt

		//simulate more for what happens after wfi
		// What happens if interrupt occurs that is not CT32B1 --> Nothing really. Keeps waiting (but also checking other RAM locations...)
		// What happens if 0x1000025c is set to 1 by CT32B1 interrupt handler --> count stored in 0x100007e4 - 0x100007e7 is incremented


		// From this point on Entry Num and Step Num are not applicable due to
		//  the fact that TODO
		// 	0x00000f52 - 0x00000f54

//TODO: need to organize this loop. Below is my understanding of what is going on:
//	wfi is called
//	if CT32B1 interrupt occurs this causes a heap space variable (0x100007e4 - 0x100007e7) to be incremented
//	However, this variable can be incremented (seemingly) forever with no affect
//	My thought is that clues lies in simulating other exceptions (i.e. USB IRQ, PendSV, SysTick, SVCall)
//	FOCUS: Where are the following heap addresses modified? 0x10000200, 0x10000244, 0x1000025d
//	ALSO: Keep an eye out for where 0x100007e4 - 0x100007e7 is read and may cause system to shutdown if it gets greater than some value or something
	}

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
 * A supervisor call (SVC) is an exception that is triggered by the SVC instruction. 
 *  In an OS environment, applications can use SVC instructions to access OS kernel functions and device drivers.
 */
void SVCall(){
	// Entry point as defined by Vector Table
	// Firmware Offset(s): 
	// 	0x000000fc - 0x00000100
	//	0x000020da - 0x000020da

	// Enter infinite loop unconditionally branching to 0x000020da
}

/**
 * PendSV is an interrupt-driven request for system-level service. In an OS 
 *  environment, use PendSV for context switching when no other exception is active.
 */
void PendSV(){
	volatile uint32_t* reg = NULL;
	uint32_t val = 0;

	// Entry point as defined by Vector Table
	// Firmware Offset(s): 
	// 	0x00000104 - 0x0000010a

	// Check value of GPREG1 
	reg = 0x40038008; 
	val = *reg;

	if (0 != val){
		//TODO: UNKNOWN PATHS branch to 0x00000110
	}

	// Firmware Offset(s): 
	// 	0x0000010c - 0x0000010e
	//	0x00001464 - 0x00001474
	// 	0x000012e8 - 0x000012ee

	// PENDSVCLR
	*0xe000ed04 = 0x08000000;

	reg = 0x100006e0;
	val = *reg
	if (val == 0x50){
		//TODO: UNKNOWN PATHS branch to 0x00001312
	}

	// Firmware Offset(s): 
	//	0x000012f0 - 0x000012f4

	if (val == 0x52){
		//TODO: UNKNOWN PATHS branch to 0x00001308
	}

	// Firmware Offset(s): 
	//	0x000012f6 - 0x000012f8

	if (val == 0x53){
		//TODO: UNKNOWN PATHS branch to 0x0000130c
	}

	// Firmware Offset(s): 
	//	0x000012fa - 0x000012fc

	if (val != 0){
		//TODO: UNKNOWN PATHS execute instruction at 0x000012fe
	}

	// Firmware Offset(s): 
	//	0x00001478 - 0x0000147c

	*(uint8_t*)0x10000249 = 0x00;
}

/**
 * A SysTick exception is an exception the system timer generates when it reaches 
 *  zero. Software can also generate a SysTick exception. In an OS environment, 
 *  the processor can use this exception as system tick.
 */
void SysTick(){
	// Entry point as defined by Vector Table
	// Firmware Offset(s): 
	// 	0x00000118 - 

//TODO: decompose this, though not expecting much as setup of System tick timer (via control registers at base 0xE000E000) was not yet simulated
 Entry Num, File Offset,   Raw Data,                                                                                                                 Decoded Raw Data,   Step Num, Current PC,    Next PC,         SP,         LR,       XPSR,    PRIMASK,    CONTROL,               Register  0,               Register  1,               Register  2,               Register  3,               Register  4,               Register  5,               Register  6,               Register  7,               Register  8,               Register  9,               Register  10,               Register  11,               Register  12, 
         0,  0x0000026c, 0x0000203c,                                                                alignedMemRead: (*0x0000026c --> 0x0000203c) 128 kB on-chip flash,          1, 0x00000118, 0x0000011a, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00000001 (           1), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         1,  0x00000118,     0x4854,                                                            ldrLiteral: Set Reg 0 with value 0x0000203c (from address 0x0000026c),          1, 0x00000118, 0x0000011a, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x0000203c (        8252), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         2,  0x0000203c, 0x00009ad1,                                                                alignedMemRead: (*0x0000203c --> 0x00009ad1) 128 kB on-chip flash,          2, 0x0000011a, 0x0000011c, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x0000203c (        8252), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         3,  0x0000011a,     0x6800,                                                        ldrImmediateT1: Set Reg 0 with value 0x00009ad1 (from address 0x0000203c),          2, 0x0000011a, 0x0000011c, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         4,  0x0000011c,     0x4700,                                                                                                 bx: Branch to Reg 0 (0x00009ad1),          3, 0x0000011c, 0x00009ad0, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         5,  0x10001bc0, 0x00000000,                                                                           alignedMemWrite: (*0x10001bc0 = 0x00000000) 8 kB SRAM0,          4, 0x00009ad0, 0x00009ad2, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         6,  0x10001bc4, 0x00000000,                                                                           alignedMemWrite: (*0x10001bc4 = 0x00000000) 8 kB SRAM0,          4, 0x00009ad0, 0x00009ad2, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         7,  0x10001bc8, 0x00000000,                                                                           alignedMemWrite: (*0x10001bc8 = 0x00000000) 8 kB SRAM0,          4, 0x00009ad0, 0x00009ad2, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         8,  0x10001bcc, 0x00000000,                                                                           alignedMemWrite: (*0x10001bcc = 0x00000000) 8 kB SRAM0,          4, 0x00009ad0, 0x00009ad2, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
         9,  0x10001bd0, 0x00000000,                                                                           alignedMemWrite: (*0x10001bd0 = 0x00000000) 8 kB SRAM0,          4, 0x00009ad0, 0x00009ad2, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        10,  0x10001bd4, 0x00000000,                                                                           alignedMemWrite: (*0x10001bd4 = 0x00000000) 8 kB SRAM0,          4, 0x00009ad0, 0x00009ad2, 0x10001bd8, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        11,  0x00009ad0,     0xb5f8,                                                                                                                             push,          4, 0x00009ad0, 0x00009ad2, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        12,  0x00009b24, 0x10000324,                                                                alignedMemRead: (*0x00009b24 --> 0x10000324) 128 kB on-chip flash,          5, 0x00009ad2, 0x00009ad4, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        13,  0x00009ad2,     0x4d14,                                                            ldrLiteral: Set Reg 5 with value 0x10000324 (from address 0x00009b24),          5, 0x00009ad2, 0x00009ad4, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        14,  0x00009ad4,     0x277d,                                                                                    movImmediate: Set Reg 7 with value 0x0000007d,          6, 0x00009ad4, 0x00009ad6, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x0000007d (         125), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        15,  0x1000032c, 0x00000000,                                                                          alignedMemRead: (*0x1000032c --> 0x00000000) 8 kB SRAM0,          7, 0x00009ad6, 0x00009ad8, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x0000007d (         125), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        16,  0x00009ad6,     0x68ac,                                                        ldrImmediateT1: Set Reg 4 with value 0x00000000 (from address 0x1000032c),          7, 0x00009ad6, 0x00009ad8, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x0000007d (         125), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        17,  0x00009ad8,     0x2600,                                                                                    movImmediate: Set Reg 6 with value 0x00000000,          8, 0x00009ad8, 0x00009ada, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x0000007d (         125), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        18,  0x00009ada,     0x00ff,                  lslImmediate: Set Reg 7 to Reg 7 (0x0000007d) shifted by 3 to left (result = 0x000003e8, carryOut = 0x00000000),          9, 0x00009ada, 0x00009adc, 0x10001bc0, 0x00000000, 0x01000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        19,  0x00009adc,     0x2c00,                                                                       cmpImmediate: Subtract Reg 4 (0x00000000) minus 0x00000000,         10, 0x00009adc, 0x00009ade, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        20,  0x00009ade,     0xd00a,                                                       conditionalBranch: Branching to 0x00009af6 (Condition check Equal, Z == 1),         11, 0x00009ade, 0x00009af6, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        21,  0x10000328, 0x00000000,                                                                          alignedMemRead: (*0x10000328 --> 0x00000000) 8 kB SRAM0,         12, 0x00009af6, 0x00009af8, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        22,  0x00009af6,     0x686c,                                                        ldrImmediateT1: Set Reg 4 with value 0x00000000 (from address 0x10000328),         12, 0x00009af6, 0x00009af8, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        23,  0x10000330, 0x00000000,                                                                          alignedMemRead: (*0x10000330 --> 0x00000000) 8 kB SRAM0,         13, 0x00009af8, 0x00009afa, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00009ad1 (       39633), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        24,  0x00009af8,     0x68e8,                                                        ldrImmediateT1: Set Reg 0 with value 0x00000000 (from address 0x10000330),         13, 0x00009af8, 0x00009afa, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        25,  0x00009afa,     0x1900,                                                      addRegisterT1: Reg 0 (0x00000000) = Reg 0 (0x00000000) + Reg 4 (0x00000000),         14, 0x00009afa, 0x00009afc, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        26,  0x10000330, 0x00000000,                                                                           alignedMemWrite: (*0x10000330 = 0x00000000) 8 kB SRAM0,         15, 0x00009afc, 0x00009afe, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        27,  0x00009afc,     0x60e8,                        strImmediateT1: Write value of Reg 0 (0x00000000) to address 0x10000330 (Reg 5 (0x10000324) + 0x0000000c),         15, 0x00009afc, 0x00009afe, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        28,  0x10000334, 0x00000000,                                                                          alignedMemRead: (*0x10000334 --> 0x00000000) 8 kB SRAM0,         16, 0x00009afe, 0x00009b00, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x10001c1c (   268442652), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        29,  0x00009afe,     0x6929,                                                        ldrImmediateT1: Set Reg 1 with value 0x00000000 (from address 0x10000334),         16, 0x00009afe, 0x00009b00, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        30,  0x00009b00,     0x1a41,                                                        subRegister: Reg 1 (0x00000000) = Reg 0 (0x00000000) - Reg 1 (0x00000000),         17, 0x00009b00, 0x00009b02, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        31,  0x00009b02,     0x42b9,                                                              cmpRegisterT1: Subtract Reg 1 (0x00000000) minus Reg 7 (0x000003e8),         18, 0x00009b02, 0x00009b04, 0x10001bc0, 0x00000000, 0x81000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        32,  0x00009b04,     0xd303,                                                 conditionalBranch: Branching to 0x00009b0e (Condition check Carry clear, C == 0),         19, 0x00009b04, 0x00009b0e, 0x10001bc0, 0x00000000, 0x81000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        33,  0x10000324,     0x0000,                                                                          alignedMemRead: (*0x10000324 --> 0x00000000) 8 kB SRAM0,         20, 0x00009b0e, 0x00009b10, 0x10001bc0, 0x00000000, 0x81000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        34,  0x00009b0e,     0x8828,                                                                        ldrhImmediate: Set Reg 0 to 0x0000 (Read from 0x10000324),         20, 0x00009b0e, 0x00009b10, 0x10001bc0, 0x00000000, 0x81000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        35,  0x00009b10,     0x1900,                                                      addRegisterT1: Reg 0 (0x00000000) = Reg 0 (0x00000000) + Reg 4 (0x00000000),         21, 0x00009b10, 0x00009b12, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        36,  0x00009b12,     0xb280,                                                                                                        uxth: Set Reg 0 to 0x0000,         22, 0x00009b12, 0x00009b14, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        37,  0x10000324,     0x0000,                                                                           alignedMemWrite: (*0x10000324 = 0x00000000) 8 kB SRAM0,         23, 0x00009b14, 0x00009b16, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        38,  0x00009b14,     0x8028,                                                                                  strhImmediate: Write Reg 0 (0x00) to 0x10000324,         23, 0x00009b14, 0x00009b16, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        39,  0x10000326,     0x0000,                                                                          alignedMemRead: (*0x10000326 --> 0x00000000) 8 kB SRAM0,         24, 0x00009b16, 0x00009b18, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        40,  0x00009b16,     0x8869,                                                                        ldrhImmediate: Set Reg 1 to 0x0000 (Read from 0x10000326),         24, 0x00009b16, 0x00009b18, 0x10001bc0, 0x00000000, 0x41000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        41,  0x00009b18,     0x4288,                                                              cmpRegisterT1: Subtract Reg 0 (0x00000000) minus Reg 1 (0x00000000),         25, 0x00009b18, 0x00009b1a, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        42,  0x00009b1a,     0xd302,                                             conditionalBranch: Not branching to 0x00009b22 (Condition check Carry clear, C == 0),         26, 0x00009b1a, 0x00009b1c, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        43,  0x1000033c, 0x00000000,                                                                          alignedMemRead: (*0x1000033c --> 0x00000000) 8 kB SRAM0,         27, 0x00009b1c, 0x00009b1e, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        44,  0x00009b1c,     0x69a8,                                                        ldrImmediateT1: Set Reg 0 with value 0x00000000 (from address 0x1000033c),         27, 0x00009b1c, 0x00009b1e, 0x10001bc0, 0x00000000, 0x61000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 
        45,  0x00009b1e,     0x4780,                                                                          blx: Set PC to Reg 0 (0x00000000). Set LR to 0x00009b20,         28, 0x00009b1e, 0x00000000, 0x10001bc0, 0x00009b21, 0x60000000, 0x00000000, 0x00000000, 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x10000324 (   268436260), 0x00000000 (           0), 0x000003e8 (        1000), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 0x00000000 (           0), 

}

/**
 * This is interrupt handler called when IRQ 19 triggers for 32-bit counter 1. 
 *  This was simulated multiple times to plot out several different paths. 
 */
void Interrupt_19_CT32B1()
{
	volatile uint32_t* reg = NULL;
	uint32_t val = 0;

	// Entry point as defined by Vector Table
	// Firmware Offset(s): 
	// 	0x000001b8 - 0x000001c2

	// GPREG1
	reg = 0x40038008;
	val = *reg;

	// Must be some state changes later and GPREG1 indicates this somehow...
	if (val != 0)
	{
		// Firmware Offset(s): 
		//	0x000001c4 - 0x000001c8
		//	0x00009d3c - 0x00009d40	
		//	0x00006130 - 0x00006136

		do {
			// Firmware Offset(s): 
			//	0x0000618c - 0x00006192

			// Interrupt Register (IR, address 0x4001 8000 (CT32B1)) bit description
			reg = 0x40018000;
			val = *reg;

			// Isolate MR0INT, MR1INT, MR2INT and MR3INT
			val &= 0xF;

			// Check if MR0INT and MR1INT and MR2INT and MR3INT are not set
			if (0 == val)
			{
				//TODO: UNKNOWN PATHS execute 0x00006194 if 
				// Firmware Offset(s): 
				//	0x00006194 - 0x00006194
				//	0x00009d44 - 0x00009d44
				return;
			}

			// Firmware Offset(s): 
			//	0x00006138 - 0x0000613c
			if (0xFFFF0000 & val)
			{
				//TODO: UNKNOWN PATHS execute 0x0000613e if 
			}

			// Firmware Offset(s): 
			//	0x00006142 - 0x00006144
			if (0xFFFFFF00 & val)
			{
				//TODO: UNKNOWN PATHS execute 0x00006146 if 
			}

			// Firmware Offset(s): 
			//	0x0000614a - 0x0000614c
			if (0xFFFFFFF0 & val)
			{
				//TODO: UNKNOWN PATHS execute 0x0000614e if 
			}

			// Firmware Offset(s): 
			//	0x00006152 - 0x00006154
			if (0xFFFFFFFC & val)
			{
				//TODO: UNKNOWN PATHS execute 0x00006156 if 
			}

			// Firmware Offset(s): 
			//	0x0000615a - 0x0000615c
			if (0xFFFFFFFE & val)
			{
				//TODO: UNKNOWN PATHS execute 0x0000615e if 
			}

			// Firmware Offset(s): 
			//	0x00006162 - 0x00006172

			// Reset MR0INT - Interrupt flag for match channel 0.
			reg = 0x40018000;
			*reg = 0x00000001;

			// Firmware Offset(s): 
			//	0x000056fc - 0x00005700
			if (reg0 == 0x4000c000)
			{
				//TODO: UNKNOWN PATHS execute 0x00005702 if 
			}
			
			// Firmware Offset(s): 
			//	0x00005706 - 0x0000570a
			if (reg0 == 0x40010000)
			{
				//TODO: UNKNOWN PATHS execute 0x0000570c if 
			}

			// Firmware Offset(s): 
			//	0x00005710 - 0x00005714
			if (reg0 == 0x40014000)
			{
				//TODO: UNKNOWN PATHS execute 0x00005718 if 
			}

			// Firmware Offset(s): 
			//	0x0000571a - 0x0000571c
			//	0x00006176 - 0x00006180
		} while (*0x10000848 == 0);
	}


	// Firmware Offset(s): 
	//	0x0000148c - 0x00001496

	// Reset MR0INT - Interrupt flag for match channel 0.
	reg = 0x40018000;
	*reg = 0x00000001;

	// Set RAM address to indicate interrupt occurred.
	//	I think this is used by init as means of counting down before
	//	 giving up on connection occurring.
	*(uint8_t*)0x1000025c = 0x01;
}

/**
 * TODO: Simulate this?
 */
void Interrupt_21_USART(){

	// Entry point as defined by Vector Table
	// Firmware Offset(s): 
	// 	0x000001d4 - 

}

/**
 * This interrupt is called in relation to USB interrupts.
 */
void Interrupt_22_USB_IRQ(){

	volatile uint32_t* reg = NULL;
	uint32_t val = 0;

	// Entry point as defined by Vector Table
	// Firmware Offset(s): 
	// 	0x000001e8 - 0x000001f2

	// Read GPREG1
	reg = 0x40038008;
	val = *reg;

	if (val != 0){
		// TODO: UNKNOWN PATH execute instruction at 0x000001f4
	}

	// Firmware Offset(s): 
	// 	0x000014fc - 0x00001504
	
	// Read USB EP Command/Status List start address
	reg = 0x40080008; // 0x20004000
	Reg0 = *reg;
	
	// Read USB Device Command/Status register
	reg = 0x40080000;
	Reg1 = *reg;

	// Check if SETUP token has been received 
	if (Reg1 & 0x00000100){

		// Firmware Offset(s): 
		// 	0x00001506 - 0x00001514

		Reg1 = 1;
		Reg2 = *0x20004000; // 0x00400109

		// Clear Bit 29 (Stall Bit) for EP0 OUT
		Reg2 &= ~0x20000000;

		*0x20004000 = Reg2

		Reg2 = *0x20004008; // 0x00400109	

		// Clear Bit 29 (Stall Bit) for EP0 IN
		Reg2 &= ~0x20000000;

		*0x20004008 = Reg2
	}

	// Firmware Offset(s): 
	// 	0x00001516 - 0x00001520

	// Setup for calling some USB related function from Boot ROM

	// Read USBD_HW_API_T* saved earlier
	Reg1 = *0x10000230; // = 0x1fff1f24

	// USB_HANDLE_T USB device stack handle	pointer
	Reg0 = *0x1000022c; // = 0x200040b8

	Reg1 = *Reg1; // = 0x1fff1f80
	Reg1 = *Reg1; // = 0x1fff3655

	// Execute function at 0x1fff3655
	
	// Firmware Offset(s): 
	//	0x1fff3654 - 0x1fff367a

	/** \fn void ISR(USBD_HANDLE_T hUsb)                                          
	 *  Function to USB device controller interrupt events.                       
	 *                                                                            
	 *  When the user application is active the interrupt handlers are mapped in the user flash
	 *  space. The user application must provide an interrupt handler for the USB interrupt and
	 *  call this function in the interrupt handler routine. The driver interrupt handler takes
	 *  appropriate action according to the data received on the USB bus.         
	 *                                                                            
	 *  \param[in]  hUsb Handle to the USB device stack.                          
	 *  \return Nothing.                                                          
	 */                                                                           
	USBD_HW_API->ISR(USBD_HANDLE_T hUsb = 0x200040b8);
//TODO: dig more into ISR function call? Could this be calling some other handler function potentially?
}


/**

TODO: save this info in some useful way to reference later?

See IRQ section in vcf_wired_controller_d0g_57bf5c10.bin.md for details on simulating exceptions

000040:  0000 0121     Vector Table     IRQ0
000044:  0000 0129     Vector Table     IRQ1
000048:  0000 0131     Vector Table     IRQ2
00004c:  0000 0139     Vector Table     IRQ3
000050:  0000 0141     Vector Table     IRQ4
000054:  0000 0149     Vector Table     IRQ5
000058:  0000 0151     Vector Table     IRQ6
00005c:  0000 0159     Vector Table     IRQ7
000060:  0000 0161     Vector Table     IRQ8
000064:  0000 0169     Vector Table     IRQ9
000068:  0000 0171     Vector Table     IRQ10
00006c:  0000 0179     Vector Table     IRQ11
000070:  0000 0179     Vector Table     IRQ12
000074:  0000 0189     Vector Table     IRQ13
000078:  0000 0191     Vector Table     IRQ14
00007c:  0000 0199     Vector Table     IRQ15
000080:  0000 01a1     Vector Table     IRQ16
000084:  0000 01a9     Vector Table     IRQ17
000088:  0000 01b1     Vector Table     IRQ18
000090:  0000 01cd     Vector Table     IRQ20
00009c:  0000 01fd     Vector Table     IRQ23
0000a0:  0000 0205     Vector Table     IRQ24
0000a4:  0000 020d     Vector Table     IRQ25
0000a8:  0000 0215     Vector Table     IRQ26
0000ac:  0000 021d     Vector Table     IRQ27
0000b0:  0000 0225     Vector Table     IRQ28
0000b4:  0000 022d     Vector Table     IRQ29
0000b8:  0000 0235     Vector Table     IRQ30
0000bc:  0000 023d     Vector Table     IRQ31

*/


/**
 * RAM LAYOUT: This sectionis set aside to track and define different parts of RAM, etc.

// Stack counts down from 0x10002000 

USBD_HID_INIT_PARAM_T
0x10001b50
...
0x10001b68

0x10001bc8

0x10001bdc


// Heap counts up from 0x10000000

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
