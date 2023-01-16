#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<ctype.h>
#include<string.h>
#include<termios.h>
#include<stdbool.h>

#define PERCENTAGE_FOR_TWO          8
#define BOX_SIZE                    4
#define SUCCESS                     0
#define FAILURE                     1
#define CLOSE_PROG                  -1
#define NO_MOVEMENT                 0
#define UP_ARROW                    1
#define DOWN_ARROW                  2
#define RIGHT_ARROW                 3
#define LEFT_ARROW                  4
#define CHECK_RETURN_VAL(val)       (val == UP_ARROW || val == DOWN_ARROW || val == LEFT_ARROW || val == RIGHT_ARROW)

typedef struct boxes{
    int cell;
    bool merge;
}box_t;

box_t box[4][4];

struct termios orig_termios;

/*
Generating random number between 0-9.
80% chance of returning a value of 2.
*/
int random_num_2_4()
{
    int ret;

    ret = rand() % 10;

    /* we should get 2 max number of times */
    if(ret < PERCENTAGE_FOR_TWO)
        ret = 2;
    else 
        ret = 4;

    return ret;
}

/*
Generate a random number between 0-15, because we have 15 cell(array of 4x4).
Finding the right cell to place the new_val and placing it only when it is zero.
If it is not zero then try to find a new cell.
*/
void insert_new_val_to_box()
{
    int new_val = random_num_2_4();
    int pos, div, rem;

    do
    {
        pos = rand() % 16;
        div = pos/4;
        rem = pos % 4;

        if(!box[div][rem].cell)
            break;
    }while(1); 

    box[div][rem].cell = new_val; 
}

void print_box()
{   
    int i, j;

    printf("\n");
    for(i = 0; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {
            if(box[i][j].cell == 0)
                printf("%8c ", '-');
            else
                printf("%8d ", box[i][j].cell);
        }
        printf("\n\n");
    }
    printf("\n");
}

/*
Check whether all the cells are filled up. If there are no place to move,
then the game is finished.
*/
int check_result()
{
    int i, j, found = FAILURE;

    for(i = 0; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {
            if(box[i][j].cell == 0)
            {
                found = SUCCESS;
                break;
            }
        }
        if(found == SUCCESS)
            break;
    }

    return found;
}

/*
Check the max number in the matrix
*/
int check_max_point()
{
    int i, j, max = -1;

    for(i = 0; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {
            if(box[i][j].cell > max)
            {
                max = box[i][j].cell;
            }
        }
    }

    return max;
}

/*
Disabling the canonical mode when the program terminates
*/
void disableRawMode() 
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

/*
Keeping the terminal in canonical mode, so that we dont have to hit
enter every time we hit the arrow keys.
*/
void enableRawMode() 
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

/*
Moving data from one cell to another
*/
void move(box_t *src, box_t *dst)
{
    dst->cell = src->cell;
    dst->merge = src->merge;
    src->cell = 0;
    src->merge = false;
}

void reset_merge()
{
    int i, j;
    for(i = 0; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {
            box[i][j].merge = 0;
        }
    }
}

/*
return 0 if unchanged and UP_ARROW if changes are made when moving up...
if no changes are there, then no need to add a new value in the box

return value: 
    0           - changes are there in box
    UP_ARROW    - no changes to the box
*/
int up_arrow()
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(i = 1; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {   
            duplicate = i;

            while((box[i][j].cell != 0) && i > 0)
            {
                if(box[i-1][j].cell == 0)
                {
                    move(&box[i][j], &box[i-1][j]);
                    change = UP_ARROW;
                }
                else if(box[i][j].cell == box[i-1][j].cell && box[i-1][j].merge == false && box[i][j].merge == false)
                {
                    box[i][j].cell *= 2;
                    box[i-1][j].merge = true;
                    move(&box[i][j], &box[i-1][j]);
                    change = UP_ARROW;
                    break;
                }
                --i;
            }

            i = duplicate;
        }
    }

    reset_merge();

    return change;
}

/* 
Down arrow key pressed
*/
int down_arrow()
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(i = BOX_SIZE - 2; i >= 0; --i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {   
            duplicate = i;

            while((box[i][j].cell != 0) && i < BOX_SIZE - 1)
            {
                if(box[i+1][j].cell == 0)
                {
                    move(&box[i][j], &box[i+1][j]);
                    change = DOWN_ARROW;
                }
                else if(box[i][j].cell == box[i+1][j].cell && box[i+1][j].merge == false && box[i][j].merge == false)
                {
                    box[i][j].cell *= 2;
                    box[i+1][j].merge = true;
                    move(&box[i][j], &box[i+1][j]);
                    change = DOWN_ARROW;
                    break;
                }
                ++i;
            }

            i = duplicate;
        }
    }

    reset_merge();

    return change;
}

/* 
Right arrow key pressed
*/
int right_arrow()
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(j = BOX_SIZE - 2; j >= 0; --j)
    {
        for(i = 0; i < BOX_SIZE; ++i)
        {   
            duplicate = j;

            while((box[i][j].cell != 0) && j < BOX_SIZE - 1)
            {
                if(box[i][j+1].cell == 0)
                {
                    move(&box[i][j], &box[i][j+1]);
                    change = RIGHT_ARROW;
                }
                else if(box[i][j].cell == box[i][j+1].cell && box[i][j+1].merge == false && box[i][j].merge == false)
                {
                    box[i][j].cell *= 2;
                    box[i][j+1].merge = true;
                    move(&box[i][j], &box[i][j+1]);
                    change = RIGHT_ARROW;
                    break;
                }
                ++j;
            }

            j = duplicate;
        }
    }

    reset_merge();

    return change;
}

/* 
Left arrow key pressed
*/
int left_arrow()
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(j = 1; j < BOX_SIZE; ++j)
    {
        for(i = 0; i < BOX_SIZE; ++i)
        {   
            duplicate = j;

            while((box[i][j].cell != 0) && j > 0)
            {
                if(box[i][j-1].cell == 0)
                {
                    move(&box[i][j], &box[i][j-1]);
                    change = LEFT_ARROW;
                }
                else if(box[i][j].cell == box[i][j-1].cell && box[i][j-1].merge == false && box[i][j].merge == false)
                {
                    box[i][j].cell *= 2;
                    box[i][j-1].merge = true;
                    move(&box[i][j], &box[i][j-1]);
                    change = LEFT_ARROW;
                    break;
                }
                --j;
            }

            j = duplicate;
        }
    }

    reset_merge();

    return change;
}

/*
Arrow keys sends 3 bytes command when pressed.
1. control (0x27)
2. '['
3. based on the arrow key pressed:
    (A - Up)
    (B - Down)
    (C - Right)
    (D - Left)
*/
int arrows_detected(char user_input)
{
    int ret = 0;

    switch(user_input)
    {
        case 'A':
            ret = up_arrow();

        break;

        case 'B':
            ret = down_arrow();
        break;

        case 'C':
            ret = right_arrow();
        break;

        case 'D':
            ret = left_arrow();
        break;

        default:
            ret = -0;
    } 

    return ret;
}

int check_letters(char user_input)
{   
    int ret = 0;

    switch(user_input)
    {
        case 'Q':
        case 'q':
            ret = CLOSE_PROG;
        break;

        case 'W':
        case 'w':
            ret = up_arrow();
        break;  

        case 'S':
        case 's':
            ret = down_arrow();
        break; 

        case 'A':
        case 'a':
            ret = left_arrow();
        break; 

        case 'D':
        case 'd':
            ret = right_arrow();
        break; 

        default:
            ret = 0;
    } 

    return ret;
}

int main()
{
    int i, j, val, max_point = 0, close = 0, checking_input = 0, cntrl_press = 0;
    char user_input;

    srand(time(0));
    enableRawMode(); 

    do{       
        if(check_result())
        {
            max_point = check_max_point();
            break;
        }

        insert_new_val_to_box();
        print_box();

        while (1) 
        {
            read(STDIN_FILENO, &user_input, 1);
            if(iscntrl(user_input)) 
            {
                cntrl_press = 1; 
            } 
            
            if(cntrl_press == 1 && (user_input == 'A' || user_input == 'B' || user_input == 'C' || user_input == 'D'))
            {
                cntrl_press = 0;
                checking_input = arrows_detected(user_input);
                if(CHECK_RETURN_VAL(checking_input))
                {
                    break;
                }
                else if(checking_input == NO_MOVEMENT)
                {
                    /* As there is no change in the box */
                    continue;
                }
            }
            else
            {
                checking_input = check_letters(user_input);
                if(checking_input == CLOSE_PROG)
                {
                    close = 1;
                    break;
                }
                else if(CHECK_RETURN_VAL(checking_input))
                {
                    break;
                }
                else
                {
                    /* As there is no change in the box */
                    continue;
                }
            }
        }

    }while(!close);

    printf("\nScore : %d\n", max_point);
}