#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// Initial Function Declarations
volatile int pixel_buffer_start; //Global Variable
void clear_screen();
void background();
void wait_for_vsync();

// Functions to draw things on the screen
void draw_horizontal_line(int x0, int x1, int y, short int colour);
void draw_vertical_line(int x, int y0, int y1, short int colour);
void draw_line(int x0, int y0, int x1, int y1, short int color);
void draw_box(int x, int y, int width, int length, short int color);
void plot_pixel(int x, int y, short int line_color);
void draw_car(int x, int y, short int colour);
void draw_taxi(int x, int y);

// Interrupt functions
void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);

// colours - green for points, pink for lines
short int BLACK = 0x00000; 
short int YELLOW = 0xFFF700;
short int WHITE = 0xFFFFFF;
short int BLUE = 0x0055FF;
short int ROAD_GREY = 0x1FA244;

/********************************************************************
* Pushbutton - Interrupt Service Routine
*
* This routine checks which KEY has been pressed. It writes to HEX0
*******************************************************************/
void pushbutton_ISR(void) {
	/* KEY base address */
	volatile int * KEY_ptr = (int *) 0xFF200050;
	int press;
	press = *(KEY_ptr + 3); // read the pushbutton interrupt register
	*(KEY_ptr + 3) = press; // Clear the interrupt
	if (press & 0x1) { // KEY0
		clear_screen();
		background();
	}
	else if (press & 0x2) { //KEY1

	}
	else if (press & 0x4) { //KEY2

	}
	else { //KEY3

	}
	return;
}


int main(void){
	volatile int* pixel_ctrl_ptr = (int*) 0xFF203020;
	/* Read location of the pixel buffer from the pixel buffer controller */
	pixel_buffer_start = *pixel_ctrl_ptr;
	clear_screen();
	start_message();
	
	disable_A9_interrupts(); // disable interrupts in the A9 processor
	set_A9_IRQ_stack(); // initialize the stack pointer for IRQ mode
	config_GIC(); // configure the general interrupt controller
	config_KEYs(); // configure pushbutton KEYs to generate interrupts
	enable_A9_interrupts(); // enable interrupts in the A9 processor
	while (1); // wait for an interrupt
	
	return 0;
}

// Draws the background
void background(){
	// First pair of dotted lines
	draw_line(52, 48, 52, 96, 0xFFFF);
	draw_line(52, 144, 52, 192, 0xFFFF);

	// First big lane
	draw_line(103, 0, 103, 239, 0xFFFF);
	draw_line(104, 0, 104, 239, 0xFFFF);
	
	// Second pair of dotted lines
	draw_line(156, 48, 156, 96, 0xFFFF);
	draw_line(156, 144, 156, 192, 0xFFFF);

	// Second big lane
	draw_line(209, 0, 209, 239, 0xFFFF);
	draw_line(210, 0, 210, 239, 0xFFFF);
	
	// Third pair of dotted lines
	draw_line(262, 48, 262, 96, 0xFFFF);
	draw_line(262, 144, 262, 192, 0xFFFF);
}


void draw_taxi(int x, int y) {

    //draws yellow base
    draw_box(x+8, y+3, 17, 41, YELLOW);
    draw_box(x+6, y+5, 2, 37, YELLOW);
    draw_box(x+4, y+7, 2, 33, YELLOW);
    draw_box(x+25, y+5, 2, 37, YELLOW);
    draw_box(x+27, y+7, 2, 33, YELLOW);

    // draws top half horizontal outlines
    draw_horizontal_line(x+7, x+25, y+2, BLACK);
    draw_horizontal_line(x+5, x+7, y+4, BLACK);
    draw_horizontal_line(x+3, x+5, y+6, BLACK);
    draw_horizontal_line(x+25, x+27, y+4, BLACK);
    draw_horizontal_line(x+27, x+29, y+6, BLACK);

    // draws bottom half horizontal outlines
    draw_horizontal_line(x+3, x+5, y+40, BLACK);
    draw_horizontal_line(x+5, x+7, y+42, BLACK);
    draw_horizontal_line(x+7, x+25, y+44, BLACK);
    draw_horizontal_line(x+27, x+29, y+40, BLACK);
    draw_horizontal_line(x+25, x+27, y+42, BLACK);

    // draws top vertical outlines
    draw_vertical_line(x+7, y+2, y+4, BLACK);
    draw_vertical_line(x+5, y+4, y+6, BLACK);
    draw_vertical_line(x+25, y+2, y+4, BLACK);
    draw_vertical_line(x+27, y+4, y+6, BLACK);

    // draws side vertical outlines
    draw_vertical_line(x+3, y+6, y+40, BLACK);
    draw_vertical_line(x+29, y+6, y+40, BLACK);

    // draws bottom half vertical outlines
    draw_vertical_line(x+5, y+40, y+42, BLACK);
    draw_vertical_line(x+7, y+42, y+44, BLACK);
    draw_vertical_line(x+27, y+40, y+42, BLACK);
    draw_vertical_line(x+25, y+42, y+44, BLACK);

    // draw tires
    draw_vertical_line(x+2, y+11, y+17, BLACK);
    draw_vertical_line(x+30, y+11, y+17, BLACK);
    draw_vertical_line(x+2, y+30, y+36, BLACK);
    draw_vertical_line(x+30, y+30, y+36, BLACK);

    // draw headlights white
    draw_box(x+6, y+5, 2, 1, 0xFFFFFF);
    draw_box(x+8, y+3, 1, 3, 0xFFFFFF);
    draw_box(x+24, y+3, 1, 3, 0xFFFFFF);
    draw_box(x+25, y+5, 2, 1, 0xFFFFFF);

    // draw headlights outline
    draw_vertical_line(x+9, y+3, y+6, BLACK);
    draw_vertical_line(x+23, y+3, y+6, BLACK);
    draw_horizontal_line(x+6, x+9, y+6, BLACK);
    draw_horizontal_line(x+23, x+26, y+6, BLACK);

    // draw license plate
    draw_horizontal_line(x+14, x+18, y+3, BLUE);

    // draw license plate outline
    draw_vertical_line(x+13, y+3, y+4, BLACK);
    draw_vertical_line(x+19, y+3, y+4, BLACK);
    draw_horizontal_line(x+14, x+18, y+4, BLACK);

    // draw front window
    draw_horizontal_line(x+13, x+19, y+8, BLUE);
    draw_horizontal_line(x+11, x+21, y+9, BLUE);
    draw_box(x+9, y+10, 15, 4, BLUE);

    // draw front window outline
    draw_horizontal_line(x+12, x+20, y+7, BLACK);
    draw_horizontal_line(x+10, x+12, y+8, BLACK);
    draw_horizontal_line(x+8, x+10, y+9, BLACK);
    draw_horizontal_line(x+20, x+22, y+8, BLACK);
    draw_horizontal_line(x+22, x+24, y+9, BLACK);
    draw_horizontal_line(x+8, x+24, y+14, BLACK);
    draw_vertical_line(x+8, y+9, y+14, BLACK);
    draw_vertical_line(x+24, y+9, y+14, BLACK);

    // draw side windows
    draw_vertical_line(x+4, y+18, y+27, BLUE);
    draw_vertical_line(x+28, y+18, y+27, BLUE);

    // draw side windows outlines
    draw_vertical_line(x+5, y+18, y+27, BLACK);
    draw_vertical_line(x+27, y+18, y+27, BLACK);
    draw_horizontal_line(x+4, x+5, y+17, BLACK);
    draw_horizontal_line(x+27, x+28, y+17, BLACK);
    draw_horizontal_line(x+4, x+5, y+28, BLACK);
    draw_horizontal_line(x+27, x+28, y+28, BLACK);

    // draw back window
    draw_horizontal_line(x+9, x+23, y+34, BLUE);
    draw_horizontal_line(x+11, x+21, y+37, BLUE);
    draw_box(x+10, y+35, 13, 2, BLUE);


    // draw back window outline
    draw_horizontal_line(x+8, x+24, y+33, BLACK);
    draw_horizontal_line(x+10, x+22, y+38, BLACK);
    draw_vertical_line(x+8, y+33, y+35, BLACK);
    draw_vertical_line(x+24, y+33, y+35, BLACK);
    draw_vertical_line(x+9, y+35, y+37, BLACK);
    draw_vertical_line(x+23, y+35, y+37, BLACK);
    draw_vertical_line(x+10, y+37, y+38, BLACK);
    draw_vertical_line(x+22, y+37, y+38, BLACK);

    // draw middle box outline
    draw_horizontal_line(x+10, x+22, y+19, BLACK);
    draw_horizontal_line(x+10, x+22, y+26, BLACK);
    draw_vertical_line(x+10, y+19, y+26, BLACK);
    draw_vertical_line(x+22, y+19, y+26, BLACK);

    // draw letter T
    draw_horizontal_line(x+14, x+18, y+21, BLACK);
    draw_vertical_line(x+16, y+21, y+24, BLACK);
}

void draw_car(int x, int y, short int colour) {

    //draws yellow base
    draw_box(x+8, y+3, 17, 41, colour);
    draw_box(x+6, y+5, 2, 37, colour);
    draw_box(x+4, y+7, 2, 33, colour);
    draw_box(x+25, y+5, 2, 37, colour);
    draw_box(x+27, y+7, 2, 33, colour);

    // draws top half horizontal outlines
    draw_horizontal_line(x+7, x+25, y+2, BLACK);
    draw_horizontal_line(x+5, x+7, y+4, BLACK);
    draw_horizontal_line(x+3, x+5, y+6, BLACK);
    draw_horizontal_line(x+25, x+27, y+4, BLACK);
    draw_horizontal_line(x+27, x+29, y+6, BLACK);

    // draws bottom half horizontal outlines
    draw_horizontal_line(x+3, x+5, y+40, BLACK);
    draw_horizontal_line(x+5, x+7, y+42, BLACK);
    draw_horizontal_line(x+7, x+25, y+44, BLACK);
    draw_horizontal_line(x+27, x+29, y+40, BLACK);
    draw_horizontal_line(x+25, x+27, y+42, BLACK);

    // draws top vertical outlines
    draw_vertical_line(x+7, y+2, y+4, BLACK);
    draw_vertical_line(x+5, y+4, y+6, BLACK);
    draw_vertical_line(x+25, y+2, y+4, BLACK);
    draw_vertical_line(x+27, y+4, y+6, BLACK);

    // draws side vertical outlines
    draw_vertical_line(x+3, y+6, y+40, BLACK);
    draw_vertical_line(x+29, y+6, y+40, BLACK);

    // draws bottom half vertical outlines
    draw_vertical_line(x+5, y+40, y+42, BLACK);
    draw_vertical_line(x+7, y+42, y+44, BLACK);
    draw_vertical_line(x+27, y+40, y+42, BLACK);
    draw_vertical_line(x+25, y+42, y+44, BLACK);

    // draw tires
    draw_vertical_line(x+2, y+11, y+17, BLACK);
    draw_vertical_line(x+30, y+11, y+17, BLACK);
    draw_vertical_line(x+2, y+30, y+36, BLACK);
    draw_vertical_line(x+30, y+30, y+36, BLACK);

    // draw headlights white
    draw_box(x+6, y+5, 2, 1, 0xFFFFFF);
    draw_box(x+8, y+3, 1, 3, 0xFFFFFF);
    draw_box(x+24, y+3, 1, 3, 0xFFFFFF);
    draw_box(x+25, y+5, 2, 1, 0xFFFFFF);

    // draw headlights outline
    draw_vertical_line(x+9, y+3, y+6, BLACK);
    draw_vertical_line(x+23, y+3, y+6, BLACK);
    draw_horizontal_line(x+6, x+9, y+6, BLACK);
    draw_horizontal_line(x+23, x+26, y+6, BLACK);

    // draw license plate
    draw_horizontal_line(x+14, x+18, y+3, BLUE);

    // draw license plate outline
    draw_vertical_line(x+13, y+3, y+4, BLACK);
    draw_vertical_line(x+19, y+3, y+4, BLACK);
    draw_horizontal_line(x+14, x+18, y+4, BLACK);

    // draw front window
    draw_horizontal_line(x+13, x+19, y+8, BLUE);
    draw_horizontal_line(x+11, x+21, y+9, BLUE);
    draw_box(x+9, y+10, 15, 4, BLUE);

    // draw front window outline
    draw_horizontal_line(x+12, x+20, y+7, BLACK);
    draw_horizontal_line(x+10, x+12, y+8, BLACK);
    draw_horizontal_line(x+8, x+10, y+9, BLACK);
    draw_horizontal_line(x+20, x+22, y+8, BLACK);
    draw_horizontal_line(x+22, x+24, y+9, BLACK);
    draw_horizontal_line(x+8, x+24, y+14, BLACK);
    draw_vertical_line(x+8, y+9, y+14, BLACK);
    draw_vertical_line(x+24, y+9, y+14, BLACK);

    // draw side windows
    draw_vertical_line(x+4, y+18, y+27, BLUE);
    draw_vertical_line(x+28, y+18, y+27, BLUE);

    // draw side windows outlines
    draw_vertical_line(x+5, y+18, y+27, BLACK);
    draw_vertical_line(x+27, y+18, y+27, BLACK);
    draw_horizontal_line(x+4, x+5, y+17, BLACK);
    draw_horizontal_line(x+27, x+28, y+17, BLACK);
    draw_horizontal_line(x+4, x+5, y+28, BLACK);
    draw_horizontal_line(x+27, x+28, y+28, BLACK);

    // draw back window
    draw_horizontal_line(x+9, x+23, y+34, BLUE);
    draw_horizontal_line(x+11, x+21, y+37, BLUE);
    draw_box(x+10, y+35, 13, 2, BLUE);


    // draw back window outline
    draw_horizontal_line(x+8, x+24, y+33, BLACK);
    draw_horizontal_line(x+10, x+22, y+38, BLACK);
    draw_vertical_line(x+8, y+33, y+35, BLACK);
    draw_vertical_line(x+24, y+33, y+35, BLACK);
    draw_vertical_line(x+9, y+35, y+37, BLACK);
    draw_vertical_line(x+23, y+35, y+37, BLACK);
    draw_vertical_line(x+10, y+37, y+38, BLACK);
    draw_vertical_line(x+22, y+37, y+38, BLACK);

}

// Series of strings that are displayed at the start screen
void start_message(){
   char* first_string = "Crazy Taxi";
   int x = 153;
   while (*first_string) {
     write_char(x, 20, *first_string);
	 x++;
	 first_string++;
   }
	
   char* second_string = "Created by: Shadman and Abdurrafay";
   x = 153;
   while (*second_string) {
     write_char(x, 22, *second_string);
	 x++;
	 second_string++;
   }
	
   char* third_string = "Winter 2020 ECE243";
   x = 153;
   while (*third_string) {
     write_char(x, 24, *third_string);
	 x++;
	 third_string++;
   }
	
   char* fourth_string = "A = Left, D = Right";
   x = 153;
   while (*fourth_string) {
     write_char(x, 28, *fourth_string);
	 x++;
	 fourth_string++;
   }
	
   char* fifth_string = "Space to Stop";
   x = 153;
   while (*fifth_string) {
     write_char(x, 30, *fifth_string);
	 x++;
	 fifth_string++;
   }
	
   char* sixth_string = "Press KEY0 to Continue...";
   x = 153;
   while (*sixth_string) {
     write_char(x, 32, *sixth_string);
	 x++;
	 sixth_string++;
   }
}

//Used to swap the values of x and y when requires in draw_line function
void swap(int *x, int *y){
	int temp = *x;
    *x = *y;
    *y = temp;   
}

// Used to write the character
void write_char(int x, int y, char c) {
  // VGA character buffer
  volatile char * character_buffer = (char *) (0xc9000000 + (y<<7) + x);
  *character_buffer = c;
}

// Clears the character and pixel buffer on the screen
void clear_screen() {
	int x, y;
	for (x = 0; x < 320; x++){
		for (y = 0; y < 240; y++){
			plot_pixel(x, y, 0x0000);
		}
	}
	
	for (x = 0; x < 80; x++){
		for (y = 0; y < 60; y++){
			write_char(x, y, ' ');
		}
	}
}

// function to sync entire screen
void wait_for_vsync(){

    // buffer register
    volatile int *pixel_ctr_ptr= (int*)0xFF203020;
    register int status;

    *pixel_ctr_ptr=1; // start sync process
    
    // status equals 0 when full screen done
    status = *(pixel_ctr_ptr + 3);
    while ((status & 0x01) !=0){
        status= *(pixel_ctr_ptr +3);
    }
}

// function to draw a line
void draw_horizontal_line(int x0, int x1, int y, short int colour){
        
    // go through all x points for that line, y stays same, plot
    for(int x=x0;x<x1+1; ++x){
        plot_pixel(x,y,colour);
    }
}

// function to draw a line
void draw_vertical_line(int x, int y0, int y1, short int colour){
        
    // go through all x points for that line, y stays same, plot
    for(int y=y0;y<y1+1; ++y){
        plot_pixel(x,y,colour);
    }
}


void plot_pixel(int x, int y, short int line_color){
	*(short int*)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// Draw a box at (x,y) location
void draw_box(int x, int y, int width, int length, short int color){
    for(int i = y; i < y+length ; ++i)
        for (int j = x; j < x+width ; ++j)
        plot_pixel(j, i, color);
}   

void draw_line(int x0, int y0, int x1, int y1, short int color){
	bool is_steep = abs(y1 - y0) > abs(x1 - x0);

	if (is_steep){
		swap(&x0, &y0);
		swap(&x1, &y1);
	}

	if (x0 > x1){
		swap(&x0, &x1);
		swap(&y0, &y1);
	}

	int deltax = x1 - x0;
	int deltay = y1 - y0;

	if (deltay < 0){
		deltay = -deltay;
	}

	//The error variable takes into account the relative difference between 
	//the width (deltax) and height of the line (deltay) in deciding how often y 
	//should beincremented. 
	int error = -(deltax / 2);

	int y = y0;
	int y_step;

	if (y0 < y1){
		y_step = 1;
	}

	else{
		y_step = -1;
	}

	for (int x = x0; x <= x1; x++){
		if (is_steep){
			plot_pixel(y, x, color);
		}
		else{
			plot_pixel(x, y, color);
		}

		error = error + deltay;

		if (error >= 0){
			y = y + y_step;
			error = error - deltax;
		}
	}
}

/* setup the KEY interrupts in the FPGA */
void config_KEYs() {
	volatile int * KEY_ptr = (int *) 0xFF200050; // pushbutton KEY base address
	*(KEY_ptr + 2) = 0xF; // enable interrupts for the two KEYs
}

void pushbutton_ISR(void);
void config_interrupt(int, int);

// Define the IRQ exception handler
void __attribute__((interrupt)) __cs3_isr_irq(void) {

	// Read the ICCIAR from the CPU Interface in the GIC
	int interrupt_ID = *((int *)0xFFFEC10C);

	if (interrupt_ID == 73) // check if interrupt is from the KEYs
	pushbutton_ISR();
		else {
	while (1); // if unexpected, then stay here
	}
			
	// Write to the End of Interrupt Register (ICCEOIR)
	*((int *)0xFFFEC110) = interrupt_ID;
}

// Define the remaining exception handlers
void __attribute__((interrupt)) __cs3_reset(void) {
	while (1);
}

void __attribute__((interrupt)) __cs3_isr_undef(void) {
	while (1);
}

void __attribute__((interrupt)) __cs3_isr_swi(void) {
	while (1);
}

void __attribute__((interrupt)) __cs3_isr_pabort(void) {
	while (1);
}

void __attribute__((interrupt)) __cs3_isr_dabort(void) {
	while (1);
}

void __attribute__((interrupt)) __cs3_isr_fiq(void) {
	while (1);
}

/*
* Turn off interrupts in the ARM processor
*/
void disable_A9_interrupts(void) {
	int status = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/*
* Initialize the banked stack pointer register for IRQ mode
*/
void set_A9_IRQ_stack(void) {
	int stack, mode;
	stack = 0xFFFFFFFF - 7; // top of A9 onchip memory, aligned to 8 bytes
	/* change processor to IRQ mode with interrupts disabled */
	mode = 0b11010010;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
	/* set banked stack pointer */
	asm("mov sp, %[ps]" : : [ps] "r"(stack));
		/* go back to SVC mode before executing subroutine return! */
	mode = 0b11010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(mode));
}

/*
* Turn on interrupts in the ARM processor
*/
void enable_A9_interrupts(void) {
	int status = 0b01010011;
	asm("msr cpsr, %[ps]" : : [ps] "r"(status));
}

/*
* Configure the Generic Interrupt Controller (GIC)
*/
void config_GIC(void) {
	config_interrupt (73, 1); // configure the FPGA KEYs interrupt (73)
	// Set Interrupt Priority Mask Register (ICCPMR). Enable interrupts of all
	// priorities
	*((int *) 0xFFFEC104) = 0xFFFF;
	// Set CPU Interface Control Register (ICCICR). Enable signaling of interrupts
	*((int *) 0xFFFEC100) = 1;
	// Configure the Distributor Control Register (ICDDCR) to send pending interrupts to CPUs
	*((int *) 0xFFFED000) = 1;
}

/*
* Configure Set Enable Registers (ICDISERn) and Interrupt Processor Target
* Registers (ICDIPTRn). The default (reset) values are used for other registers
* in the GIC.
*/
void config_interrupt(int N, int CPU_target) {
	int reg_offset, index, value, address;
	/* Configure the Interrupt Set-Enable Registers (ICDISERn).
	* reg_offset = (integer_div(N / 32) * 4
	* value = 1 << (N mod 32) */
	reg_offset = (N >> 3) & 0xFFFFFFFC;
	index = N & 0x1F;
	value = 0x1 << index;
	address = 0xFFFED100 + reg_offset;
	/* Now that we know the register address and value, set the appropriate bit */
	*(int *)address |= value;
	/* Configure the Interrupt Processor Targets Register (ICDIPTRn)
	* reg_offset = integer_div(N / 4) * 4
	* index = N mod 4 */
	reg_offset = (N & 0xFFFFFFFC);
	index = N & 0x3;
	address = 0xFFFED800 + reg_offset + index;
	/* Now that we know the register address and value, write to (only) the
	* appropriate byte */
	*(char *)address = (char)CPU_target;
}