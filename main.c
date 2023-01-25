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
box_t box_copy[4][4];

struct termios orig_termios;

int up_arrow(box_t b[BOX_SIZE][BOX_SIZE]);
int down_arrow(box_t b[BOX_SIZE][BOX_SIZE]);
int left_arrow(box_t b[BOX_SIZE][BOX_SIZE]);
int right_arrow(box_t b[BOX_SIZE][BOX_SIZE]);


/*
Welcome to the Game :)
*/
void print_intro()
{   
    printf(" __      __       .__                                \n");
    printf("/  \\    /  \\ ____ |  |   ____  ____   _____   ____   \n");
    printf("\\   \\/\\/   // __ \\|  | _/ ___\\/  _ \\ /     \\_/ __ \\  \n");
    printf(" \\        /\\  ___/|  |_\\  \\__(  <_> )  Y Y  \\  ___/  \n");
    printf("  \\__/\\  /  \\___  >____/\\___  >____/|__|_|  /\\___  > \n");
    printf("       \\/       \\/          \\/            \\/     \\/  \n");

    printf("         ___________     \n");
    printf("         \\__    ___/___  \n");
    printf("           |    | /  _ \\ \n");
    printf("           |    |(  <_> )\n");
    printf("           |____| \\____/ \n");

    printf("    _______________      _____   ______  \n");
    printf("    \\_____  \\   _  \\    /  |  | /  __  \\ \n");
    printf("     /  ____/  /_\\  \\  /   |  |_>      < \n");
    printf("    /       \\  \\_/   \\/    ^   /   --   \\ \n");
    printf("    \\_______ \\_____  /\\____   |\\______  /\n");
    printf("            \\/     \\/      |__|       \\/ \n");

    printf("\n\n\nNOTE:\n\n");
    printf("    Move UP     : Up Arrow    / W\n");
    printf("    Move Down   : Down Arrow  / S\n");
    printf("    Move Left   : Left Arrow  / A\n");
    printf("    Move Right  : Right Arrow / D\n");

    printf("\n\nPress Any Key To Start The Game\n\n\n");
}

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

/*
Converting integer to string so that we can print integers and "" in the box box.
"" -> So that the cell can look empty.
*/
void int_to_string(int val, char *str)
{
    if(val)
        sprintf(str, "%d", val);
    else
        sprintf(str, "%s", "");    
}

void print_box()
{   
    int i, j;

    printf("\n");
    for(i = 0; i < BOX_SIZE; ++i)
    {
        char a[5], b[5], c[5], d[5];
        int_to_string(box[i][0].cell, a);
        int_to_string(box[i][1].cell, b);
        int_to_string(box[i][2].cell, c);
        int_to_string(box[i][3].cell, d);

        printf("      __________  __________  __________  __________ \n");
        printf("     |          ||          ||          ||          |\n");	
        printf("     |  %5s   ||  %5s   ||  %5s   ||  %5s   |\n", a, b, c, d);
        printf("     |__________||__________||__________||__________|\n");

    }
    printf("\n");
}

/* 
Creating a copy of the main box, so that we can use this in check_result() 
*/
void copy_box()
{
    int i, j;
    for(i = 0; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {
            box_copy[i][j].cell = box[i][j].cell;
        }
    }
}

/*
Check whether all the cells are filled up. If there are no place to move,
then the game is finished.
*/
int check_cell_filled()
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
Checks the result by seaching the available movement in the box and whether the box is full
*/
int check_result()
{   
    int filled = check_cell_filled();
    int ret = FAILURE;

    copy_box();
    int up = up_arrow(box_copy);
    copy_box();
    int down = down_arrow(box_copy);
    copy_box();
    int left = left_arrow(box_copy);
    copy_box();
    int right = right_arrow(box_copy);

    if(!filled || up || down|| left|| right)
    {
        ret = SUCCESS;
    }
    else
    {
        ret = FAILURE;
    }

    return ret;
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

void reset_merge(box_t b[][BOX_SIZE])
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
int up_arrow(box_t b[][BOX_SIZE])
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(i = 1; i < BOX_SIZE; ++i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {   
            duplicate = i;

            while((b[i][j].cell != 0) && i > 0)
            {
                if(b[i-1][j].cell == 0)
                {
                    move(&b[i][j], &b[i-1][j]);
                    change = UP_ARROW;
                }
                else if(b[i][j].cell == b[i-1][j].cell && b[i-1][j].merge == false && b[i][j].merge == false)
                {
                    b[i][j].cell *= 2;
                    b[i-1][j].merge = true;
                    move(&b[i][j], &b[i-1][j]);
                    change = UP_ARROW;
                    break;
                }
                --i;
            }

            i = duplicate;
        }
    }

    reset_merge(b);

    return change;
}

/* 
Down arrow key pressed
*/
int down_arrow(box_t b[][BOX_SIZE])
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(i = BOX_SIZE - 2; i >= 0; --i)
    {
        for(j = 0; j < BOX_SIZE; ++j)
        {   
            duplicate = i;

            while((b[i][j].cell != 0) && i < BOX_SIZE - 1)
            {
                if(b[i+1][j].cell == 0)
                {
                    move(&b[i][j], &b[i+1][j]);
                    change = DOWN_ARROW;
                }
                else if(b[i][j].cell == b[i+1][j].cell && b[i+1][j].merge == false && b[i][j].merge == false)
                {
                    b[i][j].cell *= 2;
                    b[i+1][j].merge = true;
                    move(&b[i][j], &b[i+1][j]);
                    change = DOWN_ARROW;
                    break;
                }
                ++i;
            }

            i = duplicate;
        }
    }

    reset_merge(b);

    return change;
}

/* 
Right arrow key pressed
*/
int right_arrow(box_t b[][BOX_SIZE])
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(j = BOX_SIZE - 2; j >= 0; --j)
    {
        for(i = 0; i < BOX_SIZE; ++i)
        {   
            duplicate = j;

            while((b[i][j].cell != 0) && j < BOX_SIZE - 1)
            {
                if(b[i][j+1].cell == 0)
                {
                    move(&b[i][j], &b[i][j+1]);
                    change = RIGHT_ARROW;
                }
                else if(b[i][j].cell == b[i][j+1].cell && b[i][j+1].merge == false && b[i][j].merge == false)
                {
                    b[i][j].cell *= 2;
                    b[i][j+1].merge = true;
                    move(&b[i][j], &b[i][j+1]);
                    change = RIGHT_ARROW;
                    break;
                }
                ++j;
            }

            j = duplicate;
        }
    }

    reset_merge(b);

    return change;
}

/* 
Left arrow key pressed
*/
int left_arrow(box_t b[][BOX_SIZE])
{
    int i, j, change = NO_MOVEMENT, duplicate = 0;

    for(j = 1; j < BOX_SIZE; ++j)
    {
        for(i = 0; i < BOX_SIZE; ++i)
        {   
            duplicate = j;

            while((b[i][j].cell != 0) && j > 0)
            {
                if(b[i][j-1].cell == 0)
                {
                    move(&b[i][j], &b[i][j-1]);
                    change = LEFT_ARROW;
                }
                else if(b[i][j].cell == b[i][j-1].cell && b[i][j-1].merge == false && b[i][j].merge == false)
                {
                    b[i][j].cell *= 2;
                    b[i][j-1].merge = true;
                    move(&b[i][j], &b[i][j-1]);
                    change = LEFT_ARROW;
                    break;
                }
                --j;
            }

            j = duplicate;
        }
    }

    reset_merge(b);

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
            ret = up_arrow(box);
        break;

        case 'B':
            ret = down_arrow(box);
        break;

        case 'C':
            ret = right_arrow(box);
        break;

        case 'D':
            ret = left_arrow(box);
        break;

        default:
            ret = 0;
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
            ret = up_arrow(box);
        break;  

        case 'S':
        case 's':
            ret = down_arrow(box);
        break; 

        case 'A':
        case 'a':
            ret = left_arrow(box);
        break; 

        case 'D':
        case 'd':
            ret = right_arrow(box);
        break; 

        default:
            ret = 0;
    } 

    return ret;
}

int main()
{
    int i, j, val, max_point = 0, checking_input = 0;
    bool cntrl_press = false, close = false;
    char user_input;

    system("clear");

    srand(time(0));
    enableRawMode(); 

    print_intro();
    read(STDIN_FILENO, &user_input, 1);

    do{       
        system("clear"); 
        
        insert_new_val_to_box();
        print_box();
        
        if(check_result())
        {
            printf("\nGAME OVER\n");
            max_point = check_max_point();
            break;
        }    

        while (1) 
        {
            read(STDIN_FILENO, &user_input, 1);
            if(iscntrl(user_input)) 
            {
                cntrl_press = true; 
            } 
            
            if(cntrl_press == true && (user_input == 'A' || user_input == 'B' || user_input == 'C' || user_input == 'D'))
            {
                cntrl_press = false;
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
                    close = true;
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

    printf("\nScore : %d\n\n", max_point);
}
