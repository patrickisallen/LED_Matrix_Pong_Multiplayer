#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/gpio.h> // Required for the GPIO functions
#include <asm/uaccess.h>

// #error Are we building this?

#define MY_DEVICE_FILE "matrix"

/* GPIO Pins Definition */
#define RED1_PIN 8 // UPPER
#define GREEN1_PIN 80
#define BLUE1_PIN 78
#define RED2_PIN 76 // LOWER
#define GREEN2_PIN 79
#define BLUE2_PIN 74
#define CLK_PIN 73   // Arrival of each data
#define LATCH_PIN 75 // End of a row of data
#define A_PIN 72     // Row select
#define B_PIN 77
#define C_PIN 70

/*Available colours */
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define PURPLE 5
#define TEAL 6

#define DELAY_IN_MS 5
/* LED Screen Values */
static int screen[32][16];

//look up datasheet
static void exportAndOut(int pinNum)
{
    // Export the gpio pins
    gpio_request(pinNum, "sysfs");
    gpio_direction_output(pinNum, true);
    gpio_export(pinNum, false);

    return;
}

static void unexport_gpio_pins(void)
{
    gpio_unexport(RED1_PIN);
    gpio_unexport(GREEN1_PIN);
    gpio_unexport(BLUE1_PIN);

    gpio_unexport(RED2_PIN);
    gpio_unexport(GREEN2_PIN);
    gpio_unexport(BLUE2_PIN);

    gpio_unexport(CLK_PIN);
    gpio_unexport(LATCH_PIN);

    gpio_unexport(A_PIN);
    gpio_unexport(B_PIN);
    gpio_unexport(C_PIN);
}

static void unregister_gpio_pins(void)
{

    unexport_gpio_pins();

    gpio_free(RED1_PIN);
    gpio_free(GREEN1_PIN);
    gpio_free(BLUE1_PIN);

    // Lower led
    gpio_free(RED2_PIN);
    gpio_free(GREEN2_PIN);
    gpio_free(BLUE2_PIN);

    // Timing
    gpio_free(CLK_PIN);
    gpio_free(LATCH_PIN);

    // Row Select
    gpio_free(A_PIN);
    gpio_free(B_PIN);
    gpio_free(C_PIN);
}

/**
 * ledMatrix_setupPins
 * Setup the pins used by the led matrix, by exporting and set the direction to out
 */
static void ledMatrix_setupPins(void)
{
    // !Upper led
    exportAndOut(RED1_PIN);
    exportAndOut(GREEN1_PIN);
    exportAndOut(BLUE1_PIN);

    // Lower led
    exportAndOut(RED2_PIN);
    exportAndOut(GREEN2_PIN);
    exportAndOut(BLUE2_PIN);

    // Timing
    exportAndOut(CLK_PIN);
    exportAndOut(LATCH_PIN);

    // Row Select
    exportAndOut(A_PIN);
    exportAndOut(B_PIN);
    exportAndOut(C_PIN);

    return;
}
/** 
 *  ledMatrix_clock
 *  Generate the clock pins
 */
static void ledMatrix_clock(void)
{
    // Bit-bang the clock gpio
    // Notes: Before program writes, must make sure it's on the 0 index
    gpio_set_value(CLK_PIN, 1);
    gpio_set_value(CLK_PIN, 0);

    return;
}

/**
 *  ledMatrix_latch
 *  Generate the latch pins
 */
static void ledMatrix_latch(void)
{
    gpio_set_value(LATCH_PIN, 1);
    gpio_set_value(LATCH_PIN, 0);

    return;
}

/**
 *  ledMatrix_bitsFromInt
 *  Convert integer passed on into bits and put in array
 *  @params:
 *      int * arr: pointer to array to be filled with bits
 *      int input: integer to be converted to bits
 */
static void ledMatrix_bitsFromInt(int *arr, int input)
{
    arr[0] = input & 1;

    arr[1] = input & 2;
    arr[1] = arr[1] >> 1;

    arr[2] = input & 4;
    arr[2] = arr[2] >> 2;

    return;
}

/**
 *  ledMatrix_setRow
 *  Set LED Matrix row
 *  @params:
 *      int rowNum: the rowNumber to be inputted to row pins
 */
static void ledMatrix_setRow(int rowNum)
{
    // Convert rowNum single bits from int
    int arr[3] = {0, 0, 0};
    ledMatrix_bitsFromInt(arr, rowNum);

    // Write on the row pins
    gpio_set_value(A_PIN, arr[0]);
    gpio_set_value(A_PIN, arr[1]);
    gpio_set_value(A_PIN, arr[2]);

    return;
}

/**
 *  ledMatrix_setColourTop
 *  Set the colour of the top part of the LED
 *  @params:
 *      int colour: colour to be set
 */
static void ledMatrix_setColourTop(int colour)
{
    int arr[3] = {0, 0, 0};
    ledMatrix_bitsFromInt(arr, colour);

    gpio_set_value(RED1_PIN, arr[0]);
    gpio_set_value(GREEN1_PIN, arr[1]);
    gpio_set_value(BLUE1_PIN, arr[2]);

    return;
}

/**
 *  ledMatrix_setColourBottom
 *  Set the colour of the bottom part of the LED
 *  @params:
 *      int colour: colour to be set
 */
static void ledMatrix_setColourBottom(int colour)
{
    int arr[3] = {0, 0, 0};
    ledMatrix_bitsFromInt(arr, colour);

    gpio_set_value(RED2_PIN, arr[0]);
    gpio_set_value(GREEN2_PIN, arr[1]);
    gpio_set_value(BLUE2_PIN, arr[2]);

    return;
}
/**
 *  ledMatrix_refresh
 *  Fill the LED Matrix with the respective pixel colour
 */
static void ledMatrix_refresh(void)
{
    int rowNum;
    for (rowNum = 0; rowNum < 8; rowNum++)
    {
        int colNum;
        ledMatrix_setRow(rowNum);
        for (colNum = 0; colNum < 32; colNum++)
        {
            ledMatrix_setColourTop(screen[colNum][rowNum]);
            ledMatrix_setColourBottom(screen[colNum][rowNum + 8]);
            ledMatrix_clock();
        }
        ledMatrix_latch();
        // udelay(100);
        msleep(DELAY_IN_MS); // Sleep for delay
    }

    return;
}

/**
 *  ledMatrix_setPixel
 *  Set the pixel selected on LED MAtrix with the colour selected
 *  @params:
 *      int x: x-axis
 *      int y: y-axis
 *      int colour: colour selected
 */
static void ledMatrix_setPixel(int x, int y, int colour)
{
    screen[y][x] = colour;

    return;
}

static void drive_matrix(void)
{
    // Reset the screen
    int i;
    memset(screen, 0, sizeof(screen));

    // Setup pins
    ledMatrix_setupPins();

    for (i = 0; i < 16; i++)
    {
        ledMatrix_setPixel(i, i, 1);
        ledMatrix_setPixel(i, 32 - 1 - i, 2);
    }
    i = 0;
    printk(KERN_INFO "Starting the program\n");
    while (i < 1000)
    {
        ledMatrix_refresh();
        i++;
    }
}

/******************************************************
 * Callbacks
 ******************************************************/
static ssize_t write(struct file *file,
                     const char *buff, size_t count, loff_t *ppos)
{

    drive_matrix();
    return count;
}

/******************************************************
 * Misc support
 ******************************************************/
struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = write,
};

static struct miscdevice morsecode_driver = {
    .minor = MISC_DYNAMIC_MINOR, // Let the system assign one.
    .name = MY_DEVICE_FILE,      // /dev/.... file.
    .fops = &fops                // Callback functions.
};

static int __init morsecode_init(void)
{
    int ret;
    printk(KERN_INFO "----> matrix driver init(): file /dev/%s.\n", MY_DEVICE_FILE);

    ret = misc_register(&morsecode_driver);
    return ret;
}

static void __exit morsecode_exit(void)
{
    printk(KERN_INFO "<---- matrix driver exit().\n");
    unregister_gpio();
    misc_deregister(&morsecode_driver);
}

module_init(morsecode_init);
module_exit(morsecode_exit);

MODULE_AUTHOR("Scott Plummer");
MODULE_DESCRIPTION("An LED driver");
MODULE_LICENSE("GPL"); // Important to leave as GPL.
