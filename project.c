#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include "image.h"
#include "interrupt.h"

// global variables
volatile int pixel_buffer_start;
int block_size = 16;
int blocks[16] = {0};

// retunrn a randomly generatd block location for 2's
int generate_new_block();
// change row or column data of blocks
void change_block_data(int block1, int block2, int block3, int block4);
// return true if the game board if full
bool gameboard_full();
// return true if a move can be made
bool available_move();
// return true if any block value reaches 2048
bool win_f();
// check if change made
bool change = false;
// PS2 keyboard set up
bool KEYBOARD_UP = false;
bool KEYBOARD_DOWN = false;
bool KEYBOARD_LEFT = false;
bool KEYBOARD_RIGHT = false;
bool KEYBOARD_RESTART = false;
volatile int key_dir = 0;

// draw one pixel with given color
void plot_pixel(int x, int y, short int line_color);
// draw one block with given block number and value
void draw_block(int blockNum, int blockValue);
// draw full game board
void draw_all();
// clear the screen
void clear_screen();
// draw gameover interface
void draw_game_over();
// draw win interface
void draw_win();
// for debug only
void print_all_block();

void wait_for_vsync();

int main(void){
    set_A9_IRQ_stack();
    config_GIC();
    config_KEYs();
    config_PS2();
    enable_A9_interrupts();
    // pointer to the Onchip memory
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    // pointer to the pushbutton KEYs edge capture
    volatile int * KEY_EDGE_ptr = (int *) 0xFF20005C;
    // pointer to the LEDR
    volatile int * LEDR_ptr = (int *) 0xFF200000;
    // pointer to the SW
    volatile int * SW_ptr = (int *) 0xFF200040;
    // read the value in pushbutton KEYs edge capture
    int KEY_PRESS;
    // read the value in SW
    int SW_value = * SW_ptr;
    // store the location of newly generated 2s
    int location_inGame;

    // win flag
    bool win = false;
    // game over flag
    bool gameOver = false;
    // Read location of the pixel buffer from the pixel buffer controller
    pixel_buffer_start = *pixel_ctrl_ptr;

    new_game:
    {

    };
    clear_screen();
    // store the location of initial game 2s
    int location_init1 = generate_new_block();
    blocks[location_init1] = 2;
    int location_init2 = generate_new_block();
    blocks[location_init2] = 2;


    // clear the value in pushbutton KEYs edge capture
    *KEY_EDGE_ptr = 0xF;

    draw_all();
    SW_value = * SW_ptr;
    while(SW_value != 0b0){
        SW_value = * SW_ptr;
    }
    while(!win && !gameOver && SW_value == 0b0){

        KEY_PRESS = *KEY_EDGE_ptr;



        // KEY0 pressed, move down
        if(KEY_PRESS == 0b0001 || KEYBOARD_RIGHT){
            change_block_data(0,1,2,3);
            change_block_data(4,5,6,7);
            change_block_data(8,9,10,11);
            change_block_data(12,13,14,15);
            win = win_f();
			if(gameboard_full()){
				if(!available_move()){
					gameOver = true;
				}
				else{
					if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
				}
			}
			else{
				if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
			}
		

            * LEDR_ptr = 0b0001;
            * KEY_EDGE_ptr = 0xF;
			KEYBOARD_RIGHT = false;
            change = false;
        }
            // KEY1 pressed, move up
        else if(KEY_PRESS == 0b0010 || KEYBOARD_LEFT){
            change_block_data(3,2,1,0);
            change_block_data(7,6,5,4);
            change_block_data(11,10,9,8);
            change_block_data(15,14,13,12);
            win = win_f();
            if(gameboard_full()){
				if(!available_move()){
					gameOver = true;
				}
				else{
					if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
				}
			}
			else{
				if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
			}
            * LEDR_ptr = 0b0010;
            * KEY_EDGE_ptr = 0xF;
			KEYBOARD_LEFT = false;
            change = false;
        }
            // KEY2 pressed, move right
        else if(KEY_PRESS == 0b0100 || KEYBOARD_DOWN){
            change_block_data(0,4,8,12);
            change_block_data(1,5,9,13);
            change_block_data(2,6,10,14);
            change_block_data(3,7,11,15);
            win = win_f();
            if(gameboard_full()){
				if(!available_move()){
					gameOver = true;
				}
				else{
					if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
				}
			}
			else{
				if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
			}
            * LEDR_ptr = 0b0100;
            * KEY_EDGE_ptr = 0xF;
			KEYBOARD_DOWN = false;
            change = false;
        }
            // KEY1 pressed, move left
        else if(KEY_PRESS == 0b1000 || KEYBOARD_UP){
            change_block_data(12,8,4,0);
            change_block_data(13,9,5,1);
            change_block_data(14,10,6,2);
            change_block_data(15,11,7,3);
            win = win_f();
            if(gameboard_full()){
				if(!available_move()){
					gameOver = true;
				}
				else{
					if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
				}
			}
			else{
				if(change) {
                    location_inGame = generate_new_block();
                    blocks[location_inGame] = 2;
                }
                draw_all();
			}
            * LEDR_ptr = 0b1000;
            * KEY_EDGE_ptr = 0xF;
			KEYBOARD_UP = false;
            change = false;
        }
        SW_value = * SW_ptr;

    }

	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	wait_for_vsync();
	
    if(win){
		draw_win();
    }

    if(gameOver){
		clear_screen();
		draw_game_over();
    }

    // read SW value
    SW_value = * SW_ptr;
    if(SW_value ==0b0){

    }

    while(SW_value == 0b0){
        SW_value = * SW_ptr;
    }
    win = false;
    gameOver = false;
    goto new_game;

    // for debug only
    print_all_block();

}

// return a randomly generate block location for 2's
int generate_new_block(){
    int location = rand()%16;

    while(blocks[location] != 0){
        location = rand()%16;
    }
    return location;
}

// change row or column data of blocks
void change_block_data(int block1, int block2, int block3, int block4){
    // case 1: block1 empty, block2 empty, block3 empty, block4 empty
    if(blocks[block1] == 0 && blocks[block2] == 0 && blocks[block3] == 0 && blocks[block4] == 0){
        ;
    }

        // case 2: block1 empty, block2 empty, block3 empty, block4 occupied
    else if(blocks[block1] == 0 && blocks[block2] == 0 && blocks[block3] == 0 && blocks[block4] != 0){
        ;
    }

        // case 3: block1 empty, block2 empty, block3 occupied, block4 empty
    else if(blocks[block1] == 0 && blocks[block2] == 0 && blocks[block3] != 0 && blocks[block4] == 0){
        blocks[block4] = blocks[block3];
        blocks[block3] = 0;
        blocks[block2] = 0;
        blocks[block1] = 0;
        change = true;
    }

        // case 4: block1 empty, block2 empty, block3 occupied, block4 occupied
    else if(blocks[block1] == 0 && blocks[block2] == 0 && blocks[block3] != 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block3]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = 0;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            ;
        }
    }

        // case 5: block1 empty, block2 occupied, block3 empty, block4 empty
    else if(blocks[block1] == 0 && blocks[block2] != 0 && blocks[block3] == 0 && blocks[block4] == 0){
        blocks[block4] = blocks[block2];
        blocks[block3] = 0;
        blocks[block2] = 0;
        blocks[block1] = 0;
        change = true;
    }

        // case 6: block1 empty, block2 occupied, block3 empty, block4 occupied
    else if(blocks[block1] == 0 && blocks[block2] != 0 && blocks[block3] == 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block2]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = 0;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block2];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 7: block1 empty, block2 occupied, block3 occupied, block4 empty
    else if(blocks[block1] == 0 && blocks[block2] != 0 && blocks[block3] != 0 && blocks[block4] == 0){
        if(blocks[block3] == blocks[block2]){
            blocks[block4] = blocks[block3] * 2;
            blocks[block3] = 0;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block3];
            blocks[block3] = blocks[block2];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 8: block1 empty, block2 occupied, block3 occupied, block4 occupied
    else if(blocks[block1] == 0 && blocks[block2] != 0 && blocks[block3] != 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block3]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = blocks[block2];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block4] != blocks[block3] && blocks[block3] == blocks[block2]){
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block3] * 2;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            ;
        }
    }

        // case 9: block1 occupied, block2 empty, block3 empty, block4 empty
    else if(blocks[block1] != 0 && blocks[block2] == 0 && blocks[block3] == 0 && blocks[block4] == 0){
        blocks[block4] = blocks[block1];
        blocks[block3] = 0;
        blocks[block2] = 0;
        blocks[block1] = 0;
        change = true;
    }

        // case 10: block1 occupied, block2 empty, block3 empty, block4 occupied
    else if(blocks[block1] != 0 && blocks[block2] == 0 && blocks[block3] == 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block1]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = 0;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block1];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 11: block1 occupied, block2 empty, block3 occupied, block4 empty
    else if(blocks[block1] != 0 && blocks[block2] == 0 && blocks[block3] != 0 && blocks[block4] == 0){
        if(blocks[block3] == blocks[block1]){
            blocks[block4] = blocks[block3] * 2;
            blocks[block3] = 0;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block3];
            blocks[block3] = blocks[block1];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 12: block1 occupied, block2 empty, block3 occupied, block4 occupied
    else if(blocks[block1] != 0 && blocks[block2] == 0 && blocks[block3] != 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block3]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = blocks[block1];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block4] != blocks[block3] && blocks[block3] == blocks[block1]){
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block3] * 2;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block3];
            blocks[block2] = blocks[block1];
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 13: block1 occupied, block2 occupied, block3 empty, block4 empty
    else if(blocks[block1] != 0 && blocks[block2] != 0 && blocks[block3] == 0 && blocks[block4] == 0){
        if(blocks[block2] == blocks[block1]){
            blocks[block4] = blocks[block2] * 2;
            blocks[block3] = 0;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block2];
            blocks[block3] = blocks[block1];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 14: block1 occupied, block2 occupied, block3 empty, block4 occupied
    else if(blocks[block1] != 0 && blocks[block2] != 0 && blocks[block3] == 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block2]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = blocks[block1];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block4] != blocks[block2] && blocks[block2] == blocks[block1]){
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block2] * 2;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block2];
            blocks[block2] = blocks[block1];
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 15: block1 occupied, block2 occupied, block3 occupied, block4 empty
    else if(blocks[block1] != 0 && blocks[block2] != 0 && blocks[block3] != 0 && blocks[block4] == 0){
        if(blocks[block3] == blocks[block2]){
            blocks[block4] = blocks[block3] * 2;
            blocks[block3] = blocks[block1];
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block3] != blocks[block2] && blocks[block2] == blocks[block1]){
            blocks[block4] = blocks[block3];
            blocks[block3] = blocks[block2] * 2;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else{
            blocks[block4] = blocks[block3];
            blocks[block3] = blocks[block2];
            blocks[block2] = blocks[block1];
            blocks[block1] = 0;
            change = true;
        }
    }

        // case 16: block1 occupied, block2 occupied, block3 occupied, block4 occupied
    else if(blocks[block1] != 0 && blocks[block2] != 0 && blocks[block3] != 0 && blocks[block4] != 0){
        if(blocks[block4] == blocks[block3] && blocks[block2] == blocks[block1]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = blocks[block2] * 2;
            blocks[block2] = 0;
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block4] == blocks[block3] && blocks[block2] != blocks[block1]){
            blocks[block4] = blocks[block4] * 2;
            blocks[block3] = blocks[block2];
            blocks[block2] = blocks[block1];
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block4] != blocks[block3] && blocks[block3] == blocks[block2]){
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block3] * 2;
            blocks[block2] = blocks[block1];
            blocks[block1] = 0;
            change = true;
        }
        else if(blocks[block4] != blocks[block3] && blocks[block3] != blocks[block2] && blocks[block2] == blocks[block1] ){
            blocks[block4] = blocks[block4];
            blocks[block3] = blocks[block3];
            blocks[block2] = blocks[block2] * 2;
            blocks[block1] = 0;
            change = true;
        }
        else{
            ;
        }
    }




}

// return true if the game board if full
bool gameboard_full(){
    int i;
    for(i = 0; i < block_size; i++){
        if(blocks[i] == 0){
            return true;
        }
    }
    return false;
}

// return true if a move can be made
bool available_move(){
    // case 1: check term 0
    if(blocks[0] == blocks[1] || blocks[0] == blocks[4] ){
        return true;
    }

    // case 2: check term 1
    if(blocks[1] == blocks[0] || blocks[1] == blocks[5] || blocks[1] == blocks[2] ){
        return true;
    }

    // case 3: check term 2
    if(blocks[2] == blocks[1] || blocks[2] == blocks[6] || blocks[2] == blocks[3] ){
        return true;
    }

    // case 4: check term 3
    if(blocks[3] == blocks[2] || blocks[3] == blocks[7] || blocks[3] == blocks[6] ){
        return true;
    }

    // case 5: check term 4
    if(blocks[4] == blocks[0] || blocks[4] == blocks[5] || blocks[4] == blocks[8] ){
        return true;
    }

    // case 6: check term 5
    if(blocks[5] == blocks[1] || blocks[5] == blocks[4] || blocks[5] == blocks[9] || blocks[5] == blocks[6] ){
        return true;
    }

    // case 7: check term 6
    if(blocks[6] == blocks[2] || blocks[6] == blocks[5] || blocks[6] == blocks[10] || blocks[6] == blocks[7] ){
        return true;
    }

    // case 8: check term 7
    if(blocks[7] == blocks[3] || blocks[7] == blocks[6] || blocks[7] == blocks[11] ){
        return true;
    }

    // case 9: check term 8
    if(blocks[8] == blocks[4] || blocks[8] == blocks[9] || blocks[8] == blocks[12] ){
        return true;
    }

    // case 10: check term 9
    if(blocks[9] == blocks[5] || blocks[9] == blocks[8] || blocks[9] == blocks[13] || blocks[9] == blocks[10] ){
        return true;
    }

    // case 11: check term 10
    if(blocks[10] == blocks[6] || blocks[10] == blocks[9] || blocks[10] == blocks[14] || blocks[10] == blocks[11] ){
        return true;
    }

    // case 12: check term 11
    if(blocks[11] == blocks[7] || blocks[11] == blocks[10] || blocks[11] == blocks[15] ){
        return true;
    }

    // case 13: check term 12
    if(blocks[12] == blocks[8] || blocks[12] == blocks[13] ){
        return true;
    }

    // case 14: check term 13
    if(blocks[13] == blocks[9] || blocks[13] == blocks[12] || blocks[13] == blocks[14] ){
        return true;
    }

    // case 15: check term 14
    if(blocks[14] == blocks[10] || blocks[14] == blocks[13] || blocks[14] == blocks[15] ){
        return true;
    }

    // case 16: check term 15
    if(blocks[15] == blocks[11] || blocks[15] == blocks[14] ){
        return true;
    }

    return false;
}

// return true if any block value reaches 2048
bool win_f(){
    int i;
    for(i = 0; i < block_size; i++){
        if(blocks[i] == 2048 ){
            return true;
        }
    }
    return false;
}

// fill the pixel with given color
void plot_pixel(int x, int y, short int line_color)
{
    *(short int *)(pixel_buffer_start + (y << 10) + (x << 1)) = line_color;
}

// clear the screen
void clear_screen(){
    int x,y;
    for(x = 0; x < 320 ; x++){
        for(y = 0; y < 240; y++){
            plot_pixel(x,y,0);
        }
    }
    int z;
    for(z=0; z < 16; z++){
        blocks[z] = 0;
    }
}


// for debug only
void print_all_block(){
    int i;
    for (i = 0; i < block_size; i++){
        printf("%d \n", blocks[i]);
    }
}

void wait_for_vsync(){
    volatile int * pixel_ctrl_ptr = (int *)0xFF203020; // pixel controller
    register int status;

    *pixel_ctrl_ptr =1;
    status = *(pixel_ctrl_ptr+3);
    while ((status & 0x01)!= 0){
        status = *(pixel_ctrl_ptr+3);
    }
    //*pixel_ctrl_ptr =1;
    return  true;
}


// plot single image
void plot_image(int initialX, int initialY, int imageArray[], unsigned width, unsigned height) {

    int i = 0; // index for pixel colours in the image array

    int y;
    for (y = 0; y < height; y++) {
        int x;
        for (x = 0; x < width; x++) {
            int plotX = initialX + x;
            int plotY = initialY + y;

            // check for magenta, which is selected as a substitute of the alpha(transparent) colour
            // when the pixel is out of bound, ignore it
            if (imageArray[i] != 0b1111100000011111 && plotX >= initialX && plotY >= initialY && x < width && y < height)
                plot_pixel(plotX, plotY, imageArray[i]);

            i++; // switch to the next pixel colour
        }
    }
}

// draw any block on screen
void draw_block(int blockNum, int blockValue){
    int x = 0;
    int y = 0;
    if (blockNum == 0 || blockNum == 4 || blockNum == 8 || blockNum == 12){
        x = 44;
        if (blockNum == 0)
            y = 4;
        else if (blockNum == 4)
            y = 63;
        else if (blockNum == 8)
            y = 122;
        else
            y = 181;
    }
    else if (blockNum == 1 || blockNum == 5 || blockNum == 9 || blockNum == 13){
        x = 103;
        if (blockNum == 1)
            y = 4;
        else if (blockNum == 5)
            y = 63;
        else if (blockNum == 9)
            y = 122;
        else
            y = 181;
    }
    else if (blockNum == 2 || blockNum == 6 || blockNum == 10 || blockNum == 14){
        x = 162;
        if (blockNum == 2)
            y = 4;
        else if (blockNum == 6)
            y = 63;
        else if (blockNum == 10)
            y = 122;
        else
            y = 181;
    }
    else {
        x = 221;
        if (blockNum == 3)
            y = 4;
        else if (blockNum == 7)
            y = 63;
        else if (blockNum == 11)
            y = 122;
        else
            y = 181;
    }

    if (blockValue == 2)
        plot_image(x, y, image_2_55x55, 55, 55);
    else if (blockValue == 4)
        plot_image(x, y, image_4_55x55, 55, 55);
    else if (blockValue == 8)
        plot_image(x, y, image_8_55x55, 55, 55);
    else if (blockValue == 16)
        plot_image(x, y, image_16_55x55, 55, 55);
    else if (blockValue == 32)
        plot_image(x, y, image_32_55x55, 55, 55);
    else if (blockValue == 64)
        plot_image(x, y, image_64_55x55, 55, 55);
    else if (blockValue == 128)
        plot_image(x, y, image_128_55x55, 55, 55);
    else if (blockValue == 256)
        plot_image(x, y, image_256_55x55, 55, 55);
    else if (blockValue == 512)
        plot_image(x, y, image_512_55x55, 55, 55);
    else if (blockValue == 1024)
        plot_image(x, y, image_1024_55x55, 55, 55);
    else
        plot_image(x, y, image_2048_55x55, 55, 55);
}

// draw all blocks
void draw_all(){
    //clear_screen();
    int x;
    // drae main canvas
    for(x = 44; x < 276 ; x++){
        int y;
        for(y = 4; y < 236; y++){
            plot_pixel(x,y,0xD637);
        }
    }
    // draw lines
    for(x = 40; x < 280 ; x++){
        int y;
        for(y = 0; y < 4; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 40; x < 280 ; x++){
        int y;
        for(y = 59; y < 63; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 40; x < 280 ; x++){
        int y;
        for(y = 118; y < 122; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 40; x < 280 ; x++){
        int y;
        for(y = 177; y < 181; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 40; x < 280 ; x++){
        int y;
        for(y = 236; y < 240; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 40; x < 44 ; x++){
        int y;
        for(y = 4; y < 236; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 99; x < 103 ; x++){
        int y;
        for(y = 4; y < 236; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 158; x < 162 ; x++){
        int y;
        for(y = 4; y < 236; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 217; x < 221 ; x++){
        int y;
        for(y = 4; y < 236; y++){
            plot_pixel(x,y,0xAD13);
        }
    }
    for(x = 276; x < 280 ; x++){
        int y;
        for(y = 4; y < 236; y++){
            plot_pixel(x,y,0xAD13);
        }
    }


    int blockNum;
    int blockValue;
    for (blockNum = 0; blockNum < 16; blockNum++){
        blockValue = blocks[blockNum];
        if (blockValue != 0)
            draw_block(blockNum, blockValue);
    }
}

// draw gameover interface
void draw_game_over(){
	plot_image(0, 80, image_game_over_320x80, 320, 160);
}
// draw win interface
void draw_win(){
	plot_image(0, 0, image_win_320x240, 320, 240);
}

