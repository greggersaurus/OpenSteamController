
/**
 * In this simulation run the system was run from reset with no external input
 *  (except steps necessary to simulate expected hardware unit reactions). Possible
 *  triggering of IRQs were ignored. Parsed from runLogFile_00000000001494552728.csv.
 *
 * \return None.
 */
void init()
{
	uint32_t* reg = NULL;
	uint32_t val = 0;

	// Set Power Configuration Register to make sure crystal oscillator is set
        //  Entry Num: TODO
        //  Step Num: TODO

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

	// Delay (after Power Configuration Register change I think...)
        //  Entry Num: TODO
        //  Step Num: TODO

	// Some sort of delay required after last system control register mod?          
	for (uint32_t cnt = 0; cnt < 0x1600; cnt++)                                     
	{                                                                               
		nop;                                                                    
	}  

	// Select Crystal Oscillator
        //  Entry Num: TODO
        //  Step Num: TODO

	// Select Crystal Oscillator (SYSOSC)                                           
	reg = 0x40048040;                                                               
	*reg = 1;                                                                       

	// Enable system PLL clock source update
        //  Entry Num: TODO
        //  Step Num: TODO

	// Enable system PLL clock source update                                        
	reg = 0x40048044;                                                               
	*reg = 0;                                                                       
	*reg = 1;

	// Set PLL divider
        //  Entry Num: TODO
        //  Step Num: TODO

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

	// System PLL control register                                                  
	reg = 0x40048008;                                                               
	// Division ratio = 2 x 4. Feedback divider value = 3 + 1.                      
	*reg = 0x23;

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

	// Wait until PLL is locked
        //  Entry Num: TODO
        //  Step Num: TODO

	// System PLL status register                                                   
	reg = 0x4004800c;                                                               
	// Wait until PLL is locked                                                     
	while(((*reg) & 1) == 0);

	// Set System clock divider
        //  Entry Num: TODO
        //  Step Num: TODO

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

	reg = 0x40048070;
	val = *reg;
	if (val != 0x3)
	{
		// Not sure what this path does... TODO?
	}

	//TODO: stopping at CSV entry 53723 step 40685 of runLogFile_00000000001494552728.csv
	//TODO: Remember to pay attention to branches system does and does not take.
}
