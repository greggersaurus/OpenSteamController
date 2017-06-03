
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

//TODO
        // Entry Num: 45144 - 
        // Step Num: 33856 - 
	// Firmware Offset(s): 
	//	

	// System clock divider register                                                
	reg = 0x40048078;                                                               
	// Set system AHB clock divider to 1.                                           
	*reg = 1;

	// EEPROM flash access configuration
        //  Entry Num: TODO
        //  Step Num: TODO

	// Flash configuration register                                                 
	reg = 0x4003c010;                                                               
	val = *reg;                                                                     
	// Bits 31:2 must be written back exactly as read                               
	val &= 0xFFFFFFC0;                                                              
	// Set flash access time to 3 system clocks (for system clock up to 50 MHz)     
	val |=  2;                                                                      
	*reg = val;

	// Main clock config 
        //  Entry Num: TODO
        //  Step Num: TODO

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

	// USB configuration
        //  Entry Num: TODO
        //  Step Num: TODO

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

	// USB PLL control register                                                     
	reg = 0x40048010;                                                               
	// Division ration is 2 x 4. Feedback divider value is 3 + 1.                   
	*reg = 0x23;

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

	// USB PLL status register                                                      
	reg = 0x40048014;                                                               
	// Wait for PLL locked                                                          
	while (((*reg) & 1) == 0);

	// Enable I/O Configuration Block
        //  Entry Num: TODO
        //  Step Num: TODO

	// System clock control register                                                
	reg = 0x40048080;                                                               
	val = *reg;                                                                     
	// Enable I/O configuration block                                               
	val |= 0x10000;                                                                 
	*reg = val;

	// Set SRAM0 0x10000200 to 0x10001c1c with specific values
        //  Entry Num: TODO
        //  Step Num: TODO

	// Check main clock source select register and verify is set to PLL output
        //  Entry Num: 53708 - 53722
        //  Step Num: 40672 - 40684
	//  Firmware Offset(s): 0x00000452 - 0x0000046a

	reg = 0x40048070;
	val = *reg;
	if (val != 0x3)
	{
		// Not sure what this path does... TODO?
	}

	// 
        //  Entry Num: 53723 - 
        //  Step Num: 40685 - 
	//  Firmware Offset(s): 0x000004d0 - 


	//TODO: Remember to pay attention to branches system does and does not take.
	//TODO: Keep in mind that system will continue after WFI. Need to walk through this simulation and see about options (how does controller shutdown...?)
	//		I think what happens is that WFI is called for interrupt to run that sets particular registers. 
	//		Maybe 32 bit timer interrupting before other things happening causes variables to get set in a way such that power off occurs
	//		Otherwise controller might stay in this "idle" loop, or go into other "idle" loop, waiting for shutdown signal?
}
