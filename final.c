#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

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
void top_coming(int x1, int x2, int x3, int firsty, int secondy, int thirdy, int firstlength, short int colour);
void bottom_disappearing(int x1, int x2, int x3, int firsty, int secondy, int thirdy, short int colour);
void draw_new_car(int x, short int colour, int count);
bool crash(int a, int b, int c, int count, int x);

// Colours initialization making it easier to pass into functions
short int BLACK = 0x00000; 
short int YELLOW = 0xFFF700;
short int WHITE = 0xFFFFFF;
short int BLUE = 0x0055FF;
short int ROAD_GREY = 0x1FA244;
short int RED = 0xF3F4D0;

// Interrupt functions
void disable_A9_interrupts(void);
void set_A9_IRQ_stack(void);
void config_GIC(void);
void config_KEYs(void);
void enable_A9_interrupts(void);

// Create global volatile booleans
volatile bool key0Press = false;
volatile bool carMoveRight = false;
volatile bool carMoveLeft = false;
volatile bool restart = false;
volatile bool gameOver = false;

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
    //while (1); // you're never gonna leave this.

    // stores all starting positions for the 6 lanes
    int potentialStartingPositions[6] = {10,114,219,60,165,274};

    // chooses 3 random indexes for the array
    int digit1 = rand()%6 + 1;
    int digit2 = rand()%6 + 1;
    while (digit2 == digit1) {
        digit2 = rand()%6 + 1;
    }
    int digit3 = rand()%6 + 1;
    while (digit3 == digit1 || digit3 == digit2) {
        digit3 = rand()%6 + 1;
    }

    // sets the three cars starting position based on the randomized indexes
    int a = potentialStartingPositions[digit1];
    int b = potentialStartingPositions[digit2];
    int c = potentialStartingPositions[digit3];

    while (!key0Press) {
        ; // poll until user presses key 0 
    }
    
        key0Press = false;
    	
		int final_score = 0;
	
        clear_screen();
        
        // sets initial variables for the lanes
        int x1 = 52;
        int x2 = 156;
        int x3 = 262;
        int y1 = 0;
        int y2 = 48;
        int y3 = 144;

        // sets length for the first line while it moves down
        int firstlength = 0;

        // sets inital position for the taxi
        int x = 142;
        int y = 175;

        // start off with the top coming down
        bool top = true;
        int count = 0;

        // while true
        while(1){
            
            // draw backgrond
            background();
            bool checkForCrash = crash(a,b,c,count,x);

            if (checkForCrash) {

                checkForCrash = false;

                // Clear the screen
                clear_screen();
                
                // Print first lose statement
                char* first_lose_string = "Sorry, you lose!";
                int x_pos = 155;
                while (*first_lose_string) {
                    write_char(x_pos, 24, *first_lose_string);
                    x_pos++;
                    first_lose_string++;
                }
                
                // Print second lose statement
                char* second_lose_string = "Thank you for playing";
                int x_pos2 = 155;
                while (*second_lose_string) {
                    write_char(x_pos2, 26, *second_lose_string);
                    x_pos2++;
                    second_lose_string++;
                }
				
				// Print fourth lose statement
                char* fourth_lose_string = "Your score is displayed on the HEX";
                int x_pos4 = 155;
                while (*fourth_lose_string) {
                    write_char(x_pos4, 28, *fourth_lose_string);
                    x_pos4++;
                    fourth_lose_string++;
                }
                
                // Print third lose statement
                char* third_lose_string = "Press KEY0 to restart the game...";
                int x_pos3 = 155;
                while (*third_lose_string) {
                    write_char(x_pos3, 30, *third_lose_string);
                    x_pos3++;
                    third_lose_string++;
                }   
				
				// HEX display for the score
 				volatile int *HEX_ptr = (int *) (0xFF200020); 
				volatile int *HEX_ptr1 = (int *) (0xFF200030);
				
				// Use modulos and division to get each digit from the final score
				int zeroHEX = final_score % 10;
				int firstHEX = final_score % 100 / 10;
				int secondHEX = final_score % 1000 / 100;
				int thirdHEX = final_score % 10000 / 1000;
				int fourthHEX = final_score % 100000 / 10000;
				int fifthHEX = final_score % 1000000 / 100000;
				
				// Create character arrays for HEX3-0 and HEX5-4 display
				unsigned char HEX_segs[] = {0, 0, 0};
				unsigned char HEX_segs1[] = {0, 0};
				
				// Use switch statements to assign the values for HEX display
				switch(zeroHEX){
					case 0 : HEX_segs[0] = 0x3F; break;
        			case 1 : HEX_segs[0] = 0x06; break;
        			case 2 : HEX_segs[0] = 0x5B; break;
       			    case 3 : HEX_segs[0] = 0x4F; break;
        			case 4 : HEX_segs[0] = 0x66; break;
        			case 5 : HEX_segs[0] = 0x6D; break;
        			case 6 : HEX_segs[0] = 0x7D; break;
        			case 7 : HEX_segs[0] = 0x07; break;
        			case 8 : HEX_segs[0] = 0x7F; break;
        			case 9 : HEX_segs[0] = 0x67; break;
				}
				
				switch(firstHEX){
					case 0 : HEX_segs[1] = 0x3F; break;
        			case 1 : HEX_segs[1] = 0x06; break;
        			case 2 : HEX_segs[1] = 0x5B; break;
       			    case 3 : HEX_segs[1] = 0x4F; break;
        			case 4 : HEX_segs[1] = 0x66; break;
        			case 5 : HEX_segs[1] = 0x6D; break;
        			case 6 : HEX_segs[1] = 0x7D; break;
        			case 7 : HEX_segs[1] = 0x07; break;
        			case 8 : HEX_segs[1] = 0x7F; break;
        			case 9 : HEX_segs[1] = 0x67; break;
				}
				
				switch(secondHEX){
					case 0 : HEX_segs[2] = 0x3F; break;
        			case 1 : HEX_segs[2] = 0x06; break;
        			case 2 : HEX_segs[2] = 0x5B; break;
       			    case 3 : HEX_segs[2] = 0x4F; break;
        			case 4 : HEX_segs[2] = 0x66; break;
        			case 5 : HEX_segs[2] = 0x6D; break;
        			case 6 : HEX_segs[2] = 0x7D; break;
        			case 7 : HEX_segs[2] = 0x07; break;
        			case 8 : HEX_segs[2] = 0x7F; break;
        			case 9 : HEX_segs[2] = 0x67; break;
				}
				
				switch(thirdHEX){
					case 0 : HEX_segs[3] = 0x3F; break;
        			case 1 : HEX_segs[3] = 0x06; break;
        			case 2 : HEX_segs[3] = 0x5B; break;
       			    case 3 : HEX_segs[3] = 0x4F; break;
        			case 4 : HEX_segs[3] = 0x66; break;
        			case 5 : HEX_segs[3] = 0x6D; break;
        			case 6 : HEX_segs[3] = 0x7D; break;
        			case 7 : HEX_segs[3] = 0x07; break;
        			case 8 : HEX_segs[3] = 0x7F; break;
        			case 9 : HEX_segs[3] = 0x67; break;
				}
				
				switch(fourthHEX){
					case 0 : HEX_segs1[0] = 0x3F; break;
        			case 1 : HEX_segs1[0] = 0x06; break;
        			case 2 : HEX_segs1[0] = 0x5B; break;
       			    case 3 : HEX_segs1[0] = 0x4F; break;
        			case 4 : HEX_segs1[0] = 0x66; break;
        			case 5 : HEX_segs1[0] = 0x6D; break;
        			case 6 : HEX_segs1[0] = 0x7D; break;
        			case 7 : HEX_segs1[0] = 0x07; break;
        			case 8 : HEX_segs1[0] = 0x7F; break;
        			case 9 : HEX_segs1[0] = 0x67; break;
				}
				
				switch(fifthHEX){
					case 0 : HEX_segs1[1] = 0x3F; break;
        			case 1 : HEX_segs1[1] = 0x06; break;
        			case 2 : HEX_segs1[1] = 0x5B; break;
       			    case 3 : HEX_segs1[1] = 0x4F; break;
        			case 4 : HEX_segs1[1] = 0x66; break;
        			case 5 : HEX_segs1[1] = 0x6D; break;
        			case 6 : HEX_segs1[1] = 0x7D; break;
        			case 7 : HEX_segs1[1] = 0x07; break;
        			case 8 : HEX_segs1[1] = 0x7F; break;
        			case 9 : HEX_segs1[1] = 0x67; break;
				}

				*HEX_ptr =  *(int *)(HEX_segs);
				*HEX_ptr1 = *(int *)(HEX_segs1);
                
                while (!key0Press) {
                    ;
                } //poll
                
                key0Press = false;
        
                // sets initial variables for the lanes
                x1 = 52;
                x2 = 156;
                x3 = 262;
                y1 = 0;
                y2 = 48;
                y3 = 144;

                // sets length for the first line while it moves down
                firstlength = 0;

                // sets inital position for the taxi
                x = 142;
                y = 175;

                // start off with the top coming down
                top = true;
                count = 0;
				
				clear_screen();
				final_score = 0;
                background();
            }   

            // if top == true
            if (top) {

                // draw the top line growing gradually and the bottom two lines
                top_coming(x1, x2, x3, y1, y2, y3, firstlength, WHITE);

                draw_taxi(x,y);

                draw_new_car(a, WHITE, count);
                draw_new_car(b, RED, count);
                draw_new_car(c, ROAD_GREY, count);
                count = count + 1;
				final_score += 1;

                if (count == 290) {

                    // restart cars from top
                    count = 0;

                    // randomize new car positions
                    digit1 = rand()%6 + 1;
                    digit2 = rand()%6 + 1;
                    while (digit2 == digit1) {
                        digit2 = rand()%6 + 1;
                    }
                    digit3 = rand()%6 + 1;
                    while (digit3 == digit1 || digit3 == digit2) {
                        digit3 = rand()%6 + 1;
                    }

                    a = potentialStartingPositions[digit1];
                    b = potentialStartingPositions[digit2];
                    c = potentialStartingPositions[digit3];
                }
            }

            // if top == false
            else {

                // draw the top two full lines and the bottom line getting smaller
                bottom_disappearing(x1, x2, x3, y1, y2, y3, WHITE);

                draw_taxi(x,y);

                draw_new_car(a, WHITE, count);
                draw_new_car(b, RED, count);
                draw_new_car(c, ROAD_GREY, count);
                final_score += 1;
                count = count + 1;

                if (count == 290) {
                    
                    // restart cars from top
                    count = 0;

                    // randomize new car positions
                    digit1 = rand()%6 + 1;
                    digit2 = rand()%6 + 1;
                    while (digit2 == digit1) {
                        digit2 = rand()%6 + 1;
                    }
                    digit3 = rand()%6 + 1;
                    while (digit3 == digit1 || digit3 == digit2) {
                        digit3 = rand()%6 + 1;
                    }

                    a = potentialStartingPositions[digit1];
                    b = potentialStartingPositions[digit2];
                    c = potentialStartingPositions[digit3];
                }
            }

            // wait for screen to refresh
            wait_for_vsync();


            // if top == true
            if (top) {

                // erases the top line growing gradually and the bottom two lines
                top_coming(x1, x2, x3, y1, y2, y3, firstlength, BLACK);

                // increases the starting positions of the bottom two lines
                y2 = y2 + 1;
                y3 = y3 + 1;

                // increases the length of the first line for it to grow
                firstlength = firstlength + 1;

                // once y2 is 96, the top line is fully drawn and switch to bottom dissapearing
                if (y2 == 96) {
                    top = false;
                } 
            }

            // if top == false
            else {

                // erases the top full lines and the bottom line getting smaller
                bottom_disappearing(x1, x2, x3, y1, y2, y3, BLACK);

                // increases the position of the lanes to move down
                y1 = y1 + 1;
                y2 = y2 + 1;
                y3 = y3 + 1;

                // once the bottom line disappears, restart by drawing the top line
                if (y1 == 48) {
                    top = true;
                    y1 = 0;
                    y2 = 48;
                    y3 = 144;
                    firstlength = 0;
                }
            }
            
            if (carMoveRight) {
                
                while(carMoveRight) {
                            
                    // draw backgrond
                    background();

                    // if top == true
                    if (top) {

                        x = x + 1;

                        // draw the top line growing gradually and the bottom two lines
                        top_coming(x1, x2, x3, y1, y2, y3, firstlength, WHITE);
                        
                        draw_taxi(x,y);

                        draw_new_car(a, WHITE, count);
                        draw_new_car(b, RED, count);
                        draw_new_car(c, ROAD_GREY, count);
                        count = count + 1;
						final_score += 1;
						
                        if (count == 290) {

                            // restart cars from top
                            count = 0;

                            // randomize new car positions
                            digit1 = rand()%6 + 1;
                            digit2 = rand()%6 + 1;
                            while (digit2 == digit1) {
                                digit2 = rand()%6 + 1;
                            }
                            digit3 = rand()%6 + 1;
                            while (digit3 == digit1 || digit3 == digit2) {
                                digit3 = rand()%6 + 1;
                            }

                            a = potentialStartingPositions[digit1];
                            b = potentialStartingPositions[digit2];
                            c = potentialStartingPositions[digit3];
                        }
                    }

                    // if top == false
                    else {

                        // draw the top two full lines and the bottom line getting smaller
                        bottom_disappearing(x1, x2, x3, y1, y2, y3, WHITE);
                        
                        x = x + 1;

                        draw_taxi(x,y);

                        draw_new_car(a, WHITE, count);
                        draw_new_car(b, RED, count);
                        draw_new_car(c, ROAD_GREY, count);
                        
                        count = count + 1;
						final_score += 1;
                        if (count == 290) {
                        
                            // restart cars from top
                            count = 0;

                            // randomize new car positions
                            digit1 = rand()%6 + 1;
                            digit2 = rand()%6 + 1;
                            while (digit2 == digit1) {
                                digit2 = rand()%6 + 1;
                            }
                            digit3 = rand()%6 + 1;
                            while (digit3 == digit1 || digit3 == digit2) {
                                digit3 = rand()%6 + 1;
                            }

                            a = potentialStartingPositions[digit1];
                            b = potentialStartingPositions[digit2];
                            c = potentialStartingPositions[digit3];
                        }
                    }

                    // wait for screen to refresh
                    wait_for_vsync();

                    // if top == true
                    if (top) {

                        // erases the top line growing gradually and the bottom two lines
                        top_coming(x1, x2, x3, y1, y2, y3, firstlength, BLACK);


                        // increases the starting positions of the bottom two lines
                        y2 = y2 + 1;
                        y3 = y3 + 1;

                        // increases the length of the first line for it to grow
                        firstlength = firstlength + 1;

                        // once y2 is 96, the top line is fully drawn and switch to bottom dissapearing
                        if (y2 == 96) {
                            top = false;
                        } 
                    }

                    // if top == false
                    else {

                        // erases the top full lines and the bottom line getting smaller
                        bottom_disappearing(x1, x2, x3, y1, y2, y3, BLACK);

                        // increases the position of the lanes to move down
                        y1 = y1 + 1;
                        y2 = y2 + 1;
                        y3 = y3 + 1;


                        // once the bottom line disappears, restart by drawing the top line
                        if (y1 == 48) {
                            top = true;
                            y1 = 0;
                            y2 = 48;
                            y3 = 144;
                            firstlength = 0;
                        }
                    }
                    carMoveRight = false;
                }
            }
            
            else if (carMoveLeft) {
                
                while(carMoveLeft) {
                            
                    // draw backgrond
                    background();

                    // if top == true
                    if (top) {

                        // draw the top line growing gradually and the bottom two lines
                        top_coming(x1, x2, x3, y1, y2, y3, firstlength, WHITE);

                        x = x - 1;
                        draw_taxi(x,y);

                        draw_new_car(a,WHITE, count);
                        draw_new_car(b, RED, count);
                        draw_new_car(c, ROAD_GREY, count);
                        count = count + 1;
						final_score += 1;
						
                        if (count == 290) {
                            
                            // restart cars from top
                            count = 0;

                            // randomize new car positions
                            digit1 = rand()%6 + 1;
                            digit2 = rand()%6 + 1;
                            while (digit2 == digit1) {
                                digit2 = rand()%6 + 1;
                            }
                            digit3 = rand()%6 + 1;
                            while (digit3 == digit1 || digit3 == digit2) {
                                digit3 = rand()%6 + 1;
                            }

                            a = potentialStartingPositions[digit1];
                            b = potentialStartingPositions[digit2];
                            c = potentialStartingPositions[digit3];
                        }
                    }

                    // if top == false
                    else {

                        // draw the top two full lines and the bottom line getting smaller
                        bottom_disappearing(x1, x2, x3, y1, y2, y3, WHITE);

                        x = x - 1;
                        draw_taxi(x,y);

                        draw_new_car(a, WHITE, count);
                        draw_new_car(b, RED, count);
                        draw_new_car(c, ROAD_GREY, count);
                        final_score += 1;
                        count = count + 1;

                        if (count == 290) {
                            
                            // restart cars from top
                            count = 0;

                            // randomize new car positions
                            digit1 = rand()%6 + 1;
                            digit2 = rand()%6 + 1;
                            while (digit2 == digit1) {
                                digit2 = rand()%6 + 1;
                            }
                            digit3 = rand()%6 + 1;
                            while (digit3 == digit1 || digit3 == digit2) {
                                digit3 = rand()%6 + 1;
                            }

                            a = potentialStartingPositions[digit1];
                            b = potentialStartingPositions[digit2];
                            c = potentialStartingPositions[digit3];
                        }
                    }

                    // wait for screen to refresh
                    wait_for_vsync();


                    // if top == true
                    if (top) {

                        // erases the top line growing gradually and the bottom two lines
                        top_coming(x1, x2, x3, y1, y2, y3, firstlength, BLACK);


                        // increases the starting positions of the bottom two lines
                        y2 = y2 + 1;
                        y3 = y3 + 1;

                        // increases the length of the first line for it to grow
                        firstlength = firstlength + 1;

                        // once y2 is 96, the top line is fully drawn and switch to bottom dissapearing
                        if (y2 == 96) {
                            top = false;
                        } 
                    }

                    // if top == false
                    else {

                        // erases the top full lines and the bottom line getting smaller
                        bottom_disappearing(x1, x2, x3, y1, y2, y3, BLACK);

                        // increases the position of the lanes to move down
                        y1 = y1 + 1;
                        y2 = y2 + 1;
                        y3 = y3 + 1;


                        // once the bottom line disappears, restart by drawing the top line
                        if (y1 == 48) {
                            top = true;
                            y1 = 0;
                            y2 = 48;
                            y3 = 144;
                            firstlength = 0;
                        }
                    }

                    carMoveLeft = false;
                }
            }
        } 
    return 0;
}

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
        key0Press = true;
    }
    
    else if (press & 0x2) { //KEY1
        carMoveRight = true;
    }
    
    else if (press & 0x4) { //KEY2
        carMoveLeft = true;
    }
    
    else { //KEY3
        restart = true;
    }
    
    return;
}


void top_coming(int x1, int x2, int x3, int firsty, int secondy, int thirdy, int firstlength, short int colour) {

    // draw lanes on left

    draw_vertical_line(x1, 0, firstlength, colour);
    draw_vertical_line(x1, secondy, secondy+48, colour);
    draw_vertical_line(x1, thirdy, thirdy+48, colour);
        
    // draw lanes in middle
    draw_vertical_line(x2, 0, firstlength, colour);
    draw_vertical_line(x2, secondy, secondy+48, colour);
    draw_vertical_line(x2, thirdy, thirdy+48, colour);

    // draw lanes on left
    draw_vertical_line(x3, 0, firstlength, colour);
    draw_vertical_line(x3, secondy, secondy+48, colour);
    draw_vertical_line(x3, thirdy, thirdy+48, colour);

}

void bottom_disappearing(int x1, int x2, int x3, int firsty, int secondy, int thirdy, short int colour) {

    // draw lanes on left
    draw_vertical_line(x1, firsty, firsty+48, colour);
    draw_vertical_line(x1, secondy, secondy+48, colour);
    draw_vertical_line(x1, thirdy, 240, colour);
    
    // draw lanes in middle
    draw_vertical_line(x2, firsty, firsty+48, colour);
    draw_vertical_line(x2, secondy, secondy+48, colour);
    draw_vertical_line(x2, thirdy, 240, colour);

    // draw lanes on left
    draw_vertical_line(x3, firsty, firsty+48, colour);
    draw_vertical_line(x3, secondy, secondy+48, colour);
    draw_vertical_line(x3, thirdy, 240, colour);

}


// Draws the background
void background(){
    // First pair of dotted lines
    //draw_line(52, 48, 52, 96, 0xFFFF);
    //draw_line(52, 144, 52, 192, 0xFFFF);

    // First big lane
    draw_line(103, 0, 103, 239, 0xFFFF);
    draw_line(104, 0, 104, 239, 0xFFFF);
    
    // Second pair of dotted lines
    //draw_line(156, 48, 156, 96, 0xFFFF);
    //draw_line(156, 144, 156, 192, 0xFFFF);

    // Second big lane
    draw_line(209, 0, 209, 239, 0xFFFF);
    draw_line(210, 0, 210, 239, 0xFFFF);
    
    // Third pair of dotted lines
    //draw_line(262, 48, 262, 96, 0xFFFF);
    //draw_line(262, 144, 262, 192, 0xFFFF);
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
    
   char* fourth_string = "KEY2 = Left, KEY1 = Right";
   x = 153;
   while (*fourth_string) {
     write_char(x, 28, *fourth_string);
     x++;
     fourth_string++;
   }
    
   char* fifth_string = "Avoid the cars for a higher score!";
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
    
   char* seventh_string = "Remember to set breakpoint at EXIT";
   x = 153;
   while (*seventh_string) {
     write_char(x, 34, *seventh_string);
     x++;
     seventh_string++;
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

// checks for a crash
bool crash(int a, int b, int c, int count, int x) {
    
    // if the obstacles are at the height of the car
    if (count > 178 && (count-45) <225) {

        // if the taxi is touching the car from the left or touching the right of the car
        if (((x+26) >= a  && (x) <= (a+26)) || (x >= a && x <= (a+26))){
            return true;
        }
        else if (((x+26) >= b  && (x) <= (b+26)) || (x >= b && x <= (b+26))) {
            return true;
        }
        else if (((x+26) >= c  && (x) <= (c+26)) || (x >= c && x <= (c+26))) {
            return true;
        }
        // if not touching any car, no crash
        else {
            return false;
        }
    }
    // if not at right altitude, no crash
    else {
        return false;
    }
}

void draw_taxi(int x, int y) {


    // draws yellow base
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



void draw_new_car(int x, short int colour, int count) {

    if (count == 0) {

    }

    else if (count == 1) {
        
    }

    else if (count == 2) {
                // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);
    }

    else if (count == 3) {
                // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);
    }

    else if (count == 4) {
                // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);
    }

    else if (count == 5) {
                // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);
    }

    else if (count == 6) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);
    }

    else if (count == 7) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);
    }

    else if (count == 8) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);
    }

    else if (count == 9) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);
    }

    else if (count == 10) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);
    }

    else if (count == 11) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);
    }

    else if (count == 12) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);
    }

    else if (count == 13) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);
    }

    else if (count == 14) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);
    }

    else if (count == 15) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);
    }

    else if (count == 16) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);
    }

    else if (count == 17) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);
    }

    else if (count == 18) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);
    }

    else if (count == 19) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);
    }

    else if (count == 20) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);
    }

    else if (count == 21) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);
    }

    else if (count == 22) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);
    }

    else if (count == 23) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);
    }

    else if (count == 24) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);
    }

    else if (count == 25) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);
    }

    else if (count == 26) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);
    }

    else if (count == 27) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);
    }

    else if (count == 28) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);
    }

    else if (count == 29) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);
    }

    else if (count == 30) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);
    }

    else if (count == 31) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);
    }

    else if (count == 32) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);
    }

    else if (count == 33) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);
    }

    else if (count == 34) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);
    }

    else if (count == 35) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);
    }

    else if (count == 36) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);
    }

    else if (count == 37) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);
    }

    else if (count == 38) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);
    }

    else if (count == 39) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);
    }

    else if (count == 40) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);
    }

    else if (count == 41) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);
    }

    else if (count == 42) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);
    }

    else if (count == 43) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);
    }

    else if (count == 44) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 45) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count > 45 && count < 241) {
        // row 2
        draw_horizontal_line(x+7, x+25, count-1, BLACK);

        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 241) {
        // row 3
        draw_horizontal_line(x+7, x+25, count-2, BLACK);
        draw_horizontal_line(x+8, x+8, count-2, WHITE);
        draw_horizontal_line(x+10, x+12, count-2, colour);
        draw_horizontal_line(x+14, x+18, count-2, BLUE);
        draw_horizontal_line(x+20, x+22, count-2, colour);
        draw_horizontal_line(x+24, x+24, count-2, WHITE);

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 242) {

        // row 4
        draw_horizontal_line(x+5, x+27, count-3, BLACK);
        draw_horizontal_line(x+8, x+8, count-3, WHITE);
        draw_horizontal_line(x+24, x+24, count-3, WHITE);
        draw_horizontal_line(x+10, x+12, count-3, colour);
        draw_horizontal_line(x+20, x+22, count-3, colour);

        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 243) {


        // row 5
        draw_horizontal_line(x+5, x+27, count-4, BLACK);
        draw_horizontal_line(x+6, x+8, count-4, WHITE);
        draw_horizontal_line(x+24, x+26, count-4, WHITE);
        draw_horizontal_line(x+10, x+22, count-4, colour);

        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 244) {
        // row 6
        draw_horizontal_line(x+3, x+29, count-5, BLACK);
        draw_horizontal_line(x+10, x+22, count-5, colour);

        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 245) {
        // row 7
        draw_horizontal_line(x+3, x+29, count-6, BLACK);
        draw_horizontal_line(x+4, x+11, count-6, colour);
        draw_horizontal_line(x+21, x+28, count-6, colour);

        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 246) {
        // row 8
        draw_horizontal_line(x+3, x+29, count-7, BLACK);
        draw_horizontal_line(x+4, x+9, count-7, colour);
        draw_horizontal_line(x+13, x+19, count-7, BLUE);
        draw_horizontal_line(x+23, x+28, count-7, colour);

        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 247) {
        // row 9
        draw_horizontal_line(x+3, x+29, count-8, BLACK);
        draw_horizontal_line(x+4, x+7, count-8, colour);
        draw_horizontal_line(x+11, x+21, count-8, BLUE);
        draw_horizontal_line(x+25, x+28, count-8, colour);

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 248) {

        // row 10
        draw_horizontal_line(x+3, x+29, count-9, BLACK);
        draw_horizontal_line(x+4, x+7, count-9, colour);
        draw_horizontal_line(x+25, x+28, count-9, colour);
        draw_horizontal_line(x+9, x+23, count-9, BLUE);

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 249) {

        // row 11
        draw_horizontal_line(x+2, x+30, count-10, BLACK);
        draw_horizontal_line(x+4, x+7, count-10, colour);
        draw_horizontal_line(x+25, x+28, count-10, colour);
        draw_horizontal_line(x+9, x+23, count-10, BLUE);

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 250) {

        // row 12
        draw_horizontal_line(x+2, x+30, count-11, BLACK);
        draw_horizontal_line(x+4, x+7, count-11, colour);
        draw_horizontal_line(x+25, x+28, count-11, colour);
        draw_horizontal_line(x+9, x+23, count-11, BLUE);

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 251) {

        // row 13
        draw_horizontal_line(x+2, x+30, count-12, BLACK);
        draw_horizontal_line(x+4, x+7, count-12, colour);
        draw_horizontal_line(x+25, x+28, count-12, colour);
        draw_horizontal_line(x+9, x+23, count-12, BLUE);

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 252) {

        // row 14
        draw_horizontal_line(x+2, x+30, count-13, BLACK);
        draw_horizontal_line(x+4, x+7, count-13, colour);
        draw_horizontal_line(x+25, x+28, count-13, colour);

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 253) {

        // row 15
        draw_horizontal_line(x+2, x+30, count-14, BLACK);
        draw_horizontal_line(x+4, x+28, count-14, colour);

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 254) {

        // row 16
        draw_horizontal_line(x+2, x+30, count-15, BLACK);
        draw_horizontal_line(x+4, x+28, count-15, colour);

        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 255) {
        
        // row 17
        draw_horizontal_line(x+2, x+30, count-16, BLACK);
        draw_horizontal_line(x+6, x+26, count-16, colour);

        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 256) {
        
        // row 18
        draw_horizontal_line(x+3, x+29, count-17, BLACK);
        draw_horizontal_line(x+4, x+4, count-17, BLUE);
        draw_horizontal_line(x+28, x+28, count-17, BLUE);
        draw_horizontal_line(x+6, x+26, count-17, colour);

        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 257) {
        
        // row 19
        draw_horizontal_line(x+3, x+29, count-18, BLACK);
        draw_horizontal_line(x+4, x+4, count-18, BLUE);
        draw_horizontal_line(x+28, x+28, count-18, BLUE);
        draw_horizontal_line(x+6, x+26, count-18, colour);

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 258) {

        // row 20
        draw_horizontal_line(x+3, x+29, count-19, BLACK);
        draw_horizontal_line(x+4, x+4, count-19, BLUE);
        draw_horizontal_line(x+28, x+28, count-19, BLUE);
        draw_horizontal_line(x+6, x+26, count-19, colour);

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 259) {

        // row 21
        draw_horizontal_line(x+3, x+29, count-20, BLACK);
        draw_horizontal_line(x+4, x+4, count-20, BLUE);
        draw_horizontal_line(x+28, x+28, count-20, BLUE);
        draw_horizontal_line(x+6, x+26, count-20, colour);

        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 260) {
        
        // row 22
        draw_horizontal_line(x+3, x+29, count-21, BLACK);
        draw_horizontal_line(x+4, x+4, count-21, BLUE);
        draw_horizontal_line(x+28, x+28, count-21, BLUE);
        draw_horizontal_line(x+6, x+26, count-21, colour);

        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 261) {
        
        // row 23
        draw_horizontal_line(x+3, x+29, count-22, BLACK);
        draw_horizontal_line(x+4, x+4, count-22, BLUE);
        draw_horizontal_line(x+28, x+28, count-22, BLUE);
        draw_horizontal_line(x+6, x+26, count-22, colour);

        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 262) {
        
        // row 24
        draw_horizontal_line(x+3, x+29, count-23, BLACK);
        draw_horizontal_line(x+4, x+4, count-23, BLUE);
        draw_horizontal_line(x+28, x+28, count-23, BLUE);
        draw_horizontal_line(x+6, x+26, count-23, colour);

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 263) {

        // row 25
        draw_horizontal_line(x+3, x+29, count-24, BLACK);
        draw_horizontal_line(x+4, x+4, count-24, BLUE);
        draw_horizontal_line(x+28, x+28, count-24, BLUE);
        draw_horizontal_line(x+6, x+26, count-24, colour);

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 264) {

        // row 26
        draw_horizontal_line(x+3, x+29, count-25, BLACK);
        draw_horizontal_line(x+4, x+4, count-25, BLUE);
        draw_horizontal_line(x+28, x+28, count-25, BLUE);
        draw_horizontal_line(x+6, x+26, count-25, colour);

        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 265) {
        
        // row 27
        draw_horizontal_line(x+3, x+29, count-26, BLACK);
        draw_horizontal_line(x+4, x+4, count-26, BLUE);
        draw_horizontal_line(x+28, x+28, count-26, BLUE);
        draw_horizontal_line(x+6, x+26, count-26, colour);

        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 266) {
        
        // row 28
        draw_horizontal_line(x+3, x+29, count-27, BLACK);
        draw_horizontal_line(x+6, x+26, count-27, colour);

        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 267) {
        
        // row 29
        draw_horizontal_line(x+3, x+29, count-28, BLACK);
        draw_horizontal_line(x+4, x+28, count-28, colour);

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 268) {

        // row 30
        draw_horizontal_line(x+2, x+30, count-29, BLACK);
        draw_horizontal_line(x+4, x+28, count-29, colour);

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 269) {

        // row 31
        draw_horizontal_line(x+2, x+30, count-30, BLACK);
        draw_horizontal_line(x+4, x+28, count-30, colour);

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 270) {

        // row 32
        draw_horizontal_line(x+2, x+30, count-31, BLACK);
        draw_horizontal_line(x+4, x+28, count-31, colour);

        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);

        // row 45
    }

    else if (count == 271) {
        
        // row 33
        draw_horizontal_line(x+2, x+30, count-32, BLACK);
        draw_horizontal_line(x+4, x+7, count-32, colour);
        draw_horizontal_line(x+25, x+28, count-32, colour);

        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 272) {
        
        // row 34
        draw_horizontal_line(x+2, x+30, count-33, BLACK);
        draw_horizontal_line(x+4, x+7, count-33, colour);
        draw_horizontal_line(x+25, x+28, count-33, colour);
        draw_horizontal_line(x+9, x+23, count-33, BLUE);

        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 273) {
        
        // row 35
        draw_horizontal_line(x+2, x+30, count-34, BLACK);
        draw_horizontal_line(x+4, x+7, count-34, colour);
        draw_horizontal_line(x+25, x+28, count-34, colour);
        draw_horizontal_line(x+10, x+22, count-34, BLUE);

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 274) {

        // row 36
        draw_horizontal_line(x+2, x+30, count-35, BLACK);
        draw_horizontal_line(x+4, x+8, count-35, colour);
        draw_horizontal_line(x+24, x+28, count-35, colour);
        draw_horizontal_line(x+10, x+22, count-35, BLUE);

        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 275) {
        
        // row 37
        draw_horizontal_line(x+3, x+29, count-36, BLACK);
        draw_horizontal_line(x+4, x+8, count-36, colour);
        draw_horizontal_line(x+24, x+28, count-36, colour);
        draw_horizontal_line(x+11, x+21, count-36, BLUE);

        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 276) {
        
        // row 38
        draw_horizontal_line(x+3, x+29, count-37, BLACK);
        draw_horizontal_line(x+4, x+9, count-37, colour);
        draw_horizontal_line(x+23, x+28, count-37, colour);

        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 277) {
        
        // row 39
        draw_horizontal_line(x+3, x+29, count-38, BLACK);
        draw_horizontal_line(x+4, x+28, count-38, colour);

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 278) {

        // row 40
        draw_horizontal_line(x+3, x+29, count-39, BLACK);
        draw_horizontal_line(x+6, x+26, count-39, colour);

        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 279) {
        
        // row 41
        draw_horizontal_line(x+5, x+27, count-40, BLACK);
        draw_horizontal_line(x+6, x+26, count-40, colour);

        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 280) {
        
        // row 42
        draw_horizontal_line(x+5, x+27, count-41, BLACK);
        draw_horizontal_line(x+8, x+24, count-41, colour);

        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 281) {
    
        // row 43
        draw_horizontal_line(x+7, x+25, count-42, BLACK);
        draw_horizontal_line(x+8, x+24, count-42, colour);

        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }

    else if (count == 282) {
        // row 44
        draw_horizontal_line(x+7, x+25, count-43, BLACK);
    }
}