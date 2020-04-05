#include <stdbool.h>
volatile int pixel_buffer_start;

// colours - green for points, pink for lines
short int BLACK = 0x00000; 
short int YELLOW = 0xFFF700;
short int WHITE = 0xFFFFFF;
short int BLUE = 0x0055FF;
short int ROAD_GREY = 0x1FA244;


// declaration of functions
void wait_for_vsync();
void plot_pixel(int x, int y, short int line_color);
void draw_horizontal_line(int x0, int x1, int y, short int colour);
void draw_vertical_line(int x0, int x1, int y, short int colour);
void clear_screen();
void draw_box(int x, int y, int width, int length, short int color);
void draw_background();
void draw_taxi(int x, int y);
void draw_car(int x, int y, short int colour);

void draw_line(int x0, int y0, int x1, int y1, short int color);


//Used to swap the values of x and y when requires in draw_line function
void swap(int *x, int *y){
	int temp = *x;
    *x = *y;
    *y = temp;   
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


// function to clear screen
void clear_screen(){
    for (int x=0; x<320; ++x){
        for (int y=0; y<240; ++y){
            plot_pixel(x,y,BLACK);
        }   
    }
}


// function to plot a pixel
void plot_pixel(int x, int y, short int line_color){
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}


// Draw a box at (x,y) location
void draw_box(int x, int y, int width, int length, short int color){
    for(int i = y; i < y+length ; ++i)
        for (int j = x; j < x+width ; ++j)
        plot_pixel(j, i, color);
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




void draw_background() {

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



// declarationof pixel buffer
volatile int pixel_buffer_start;

// main
int main(void){

   // sets buffer register address
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;

    // colours - green for points, pink for lines
    short int GREEN = 0x07E0; 
    short int PINK = 0xF81F;  

    /* set front pixel buffer to start of FPGA On-chip memory */
    *(pixel_ctrl_ptr + 1) = 0xC8000000; // first store the address in the 
                                        // back buffer

    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();

    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;

    clear_screen(); // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to start of SDRAM memory */
    *(pixel_ctrl_ptr + 1) = 0xC0000000;

    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back buffer

    int x = 0;
    int y = 0;
    // infinite loop
    while (1)
    {
        // Clears screen before drawing again
        clear_screen();
        
        //draws background
        draw_background();

        // draws taxi and car
        draw_taxi(x,y);
        draw_car(x+50,y+50, WHITE);

        // moves down and right to see if all pixels are blessed
        x=x+1;
        y=y+1;
        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // new back buffer
    }

}







