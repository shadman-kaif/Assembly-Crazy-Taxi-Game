#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//Initial Function Declarations
volatile int pixel_buffer_start; //Global Variable
void clear_screen();
void draw_line(int x0, int y0, int x1, int y1, short int color);
void plot_pixel(int x, int y, short int line_color);


int main(void){
	volatile int* pixel_ctrl_ptr = (int*) 0xFF203020;
	/* Read location of the pixel buffer from the pixel buffer controller */
	pixel_buffer_start = *pixel_ctrl_ptr;

	clear_screen();
	
	start_message();
	return 0;
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
	
   char* sixth_string = "Press Enter to Continue...";
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

void clear_screen() {
	int x, y;
	for (x = 0; x < 320; x++){
		for (y = 0; y < 240; y++){
			plot_pixel(x, y, 0x0000);
		}
	}
}

void plot_pixel(int x, int y, short int line_color){
	*(short int*)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
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