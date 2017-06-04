
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

        // Entry Num: 54150 - 
        // Step Num: 41068 - 
	// Firmware Offset(s): 
	//	

	//TODO: Remember to pay attention to branches/paths simulation does and does not take.
	//TODO: Keep in mind that system will continue after WFI. Need to walk through this simulation and see about options (how does controller shutdown...?)
	//		I think what happens is that WFI is called for interrupt to run that sets particular registers. 
	//		Maybe 32 bit timer interrupting before other things happening causes variables to get set in a way such that power off occurs
	//		Otherwise controller might stay in this "idle" loop, or go into other "idle" loop, waiting for shutdown signal?
}
