/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 10 - LED Matrix Animation
  ******************************************************************************
  */
/* USER CODE END Header */


/* Includes ------------------------------------------------------------------*/
#include "main.h"


/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_TIM2_Init(void);


/* =========================================================
 * EXERCISE 10 CONFIGURATION
 * ========================================================= */

/*
 * Number of rows in LED Matrix
 */
const int MAX_LED_MATRIX = 8;


/*
 * Matrix scan index
 */
int index_led_matrix = 0;


/*
 * Current shift position:
 *
 * 0 -> no shift
 * 1 -> shift 1 position
 * ...
 * 7 -> shift 7 positions
 */
uint8_t shift = 0;


/*
 * Counts how many complete Matrix frames
 * have been displayed.
 */
int matrix_shift_flag = 0;


/*
 * =========================================================
 * MATRIX ANIMATION SPEED
 * =========================================================
 *
 * Timer2 = 10 ms
 *
 * One complete matrix:
 *
 * 8 rows x 10 ms
 * = 80 ms
 *
 * MATRIX_SHIFT_SPEED = 5:
 *
 * 5 x 80 ms
 * = 400 ms
 *
 * Therefore character A moves approximately
 * one position every 400 ms.
 */
#define MATRIX_SHIFT_SPEED 5


/*
 * =========================================================
 * CHARACTER A
 * =========================================================
 *
 * Original pattern from your Exercise 10 code.
 */
uint8_t matrix_buffer[8] =
{
    0xC7,
    0x93,
    0x39,
    0x01,
    0x01,
    0x39,
    0x39,
    0x39
};


/* =========================================================
 * 7-SEGMENT
 * ========================================================= */

int index_led = 0;

int led_buffer[4] =
{
    1, 2, 3, 4
};


/* =========================================================
 * CLOCK
 * ========================================================= */

int hour = 15;

int minute = 8;

int second = 57;


/* =========================================================
 * SOFTWARE TIMERS
 * ========================================================= */

int timer1_counter = 0;

int timer2_counter = 0;

int timer3_counter = 0;


int timer1_flag = 0;

int timer2_flag = 0;

int timer3_flag = 0;


/* =========================================================
 * SOFTWARE TIMER FUNCTIONS
 * ========================================================= */

void setTimer1(int duration)
{
    timer1_counter = duration / 10;

    timer1_flag = 0;
}


void setTimer2(int duration)
{
    timer2_counter = duration / 10;

    timer2_flag = 0;
}


void setTimer3(int duration)
{
    timer3_counter = duration / 10;

    timer3_flag = 0;
}


/* =========================================================
 * SOFTWARE TIMER RUN
 *
 * TIM2 interrupt = 10 ms
 * ========================================================= */

void timerRun(void)
{
    /* TIMER 1 */
    if(timer1_counter > 0)
    {
        timer1_counter--;


        if(timer1_counter <= 0)
        {
            timer1_flag = 1;
        }
    }


    /* TIMER 2 */
    if(timer2_counter > 0)
    {
        timer2_counter--;


        if(timer2_counter <= 0)
        {
            timer2_flag = 1;
        }
    }


    /* TIMER 3 */
    if(timer3_counter > 0)
    {
        timer3_counter--;


        if(timer3_counter <= 0)
        {
            timer3_flag = 1;
        }
    }
}


/* =========================================================
 * TURN OFF ALL 7SEG DISPLAYS
 * ========================================================= */

void clearLed(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_6 |
        GPIO_PIN_7 |
        GPIO_PIN_8 |
        GPIO_PIN_9,
        GPIO_PIN_SET
    );
}


/* =========================================================
 * CLEAR ONE 7SEG ENABLE
 * ========================================================= */

void clearPin(int index)
{
    switch(index)
    {
        case 0:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_6,
                GPIO_PIN_SET
            );

            break;


        case 1:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_7,
                GPIO_PIN_SET
            );

            break;


        case 2:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_8,
                GPIO_PIN_SET
            );

            break;


        case 3:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_9,
                GPIO_PIN_SET
            );

            break;


        default:

            break;
    }
}


/* =========================================================
 * ENABLE ONE 7SEG
 *
 * PNP:
 *
 * LOW = ON
 * ========================================================= */

void enablePin(int index)
{
    switch(index)
    {
        case 0:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_6,
                GPIO_PIN_RESET
            );

            break;


        case 1:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_7,
                GPIO_PIN_RESET
            );

            break;


        case 2:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_8,
                GPIO_PIN_RESET
            );

            break;


        case 3:

            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_9,
                GPIO_PIN_RESET
            );

            break;


        default:

            break;
    }
}


/* =========================================================
 * DISPLAY NUMBER ON 7SEG
 * ========================================================= */

void display7SEG(int num)
{
    const uint8_t segNumber[10] =
    {
        0xC0,   /* 0 */
        0xF9,   /* 1 */
        0xA4,   /* 2 */
        0xB0,   /* 3 */
        0x99,   /* 4 */
        0x92,   /* 5 */
        0x82,   /* 6 */
        0xF8,   /* 7 */
        0x80,   /* 8 */
        0x90    /* 9 */
    };


    /*
     * Safety
     */
    if(num < 0 || num > 9)
    {
        return;
    }


    for(int i = 0; i < 7; i++)
    {
        HAL_GPIO_WritePin(
            GPIOB,
            (GPIO_PIN_0 << i),

            ((segNumber[num] >> i) & 0x01)
            ? GPIO_PIN_SET
            : GPIO_PIN_RESET
        );
    }
}


/* =========================================================
 * CLEAR ALL 7SEG ENABLE
 * ========================================================= */

void clearEnable(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_6 |
        GPIO_PIN_7 |
        GPIO_PIN_8 |
        GPIO_PIN_9,
        GPIO_PIN_SET
    );
}


/* =========================================================
 * UPDATE 7SEG
 * ========================================================= */

void update7SEG(int index)
{
    clearLed();

    clearEnable();


    switch(index)
    {
        case 0:

            display7SEG(
                led_buffer[0]
            );

            enablePin(0);

            break;


        case 1:

            display7SEG(
                led_buffer[1]
            );

            enablePin(1);

            break;


        case 2:

            display7SEG(
                led_buffer[2]
            );

            enablePin(2);

            break;


        case 3:

            display7SEG(
                led_buffer[3]
            );

            enablePin(3);

            break;


        default:

            break;
    }
}


/* =========================================================
 * UPDATE CLOCK BUFFER
 * ========================================================= */

void updateClockBuffer(void)
{
    led_buffer[0] = hour / 10;

    led_buffer[1] = hour % 10;


    led_buffer[2] = minute / 10;

    led_buffer[3] = minute % 10;
}


/* =========================================================
 * SET LED MATRIX COLUMN DATA
 *
 * PA2  -> COL bit7
 * PA3  -> COL bit6
 * PA10 -> COL bit5
 * PA11 -> COL bit4
 * PA12 -> COL bit3
 * PA13 -> COL bit2
 * PA14 -> COL bit1
 * PA15 -> COL bit0
 * ========================================================= */

void setCol(uint8_t val)
{
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_2,
        ((val >> 7) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_3,
        ((val >> 6) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_10,
        ((val >> 5) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_11,
        ((val >> 4) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_12,
        ((val >> 3) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_13,
        ((val >> 2) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_14,
        ((val >> 1) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_15,
        ((val >> 0) & 0x01)
        ? GPIO_PIN_SET
        : GPIO_PIN_RESET
    );
}


/* =========================================================
 * SET ALL MATRIX ROWS HIGH
 *
 * Used before enabling one row.
 * ========================================================= */

void setMatrix(void)
{
    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_8  |
        GPIO_PIN_9  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15,
        GPIO_PIN_SET
    );
}


/* =========================================================
 * UPDATE LED MATRIX
 *
 * index:
 * selected matrix row
 *
 * shift:
 * animation position
 * ========================================================= */

void updateLEDMatrix(
    uint8_t index,
    uint8_t shift_value)
{
    /*
     * Turn OFF every row first
     */
    setMatrix();


    /*
     * Rotate matrix data.
     *
     * Special case shift = 0
     * avoids unnecessary right shift by 8.
     */
    uint8_t matrix_buffer_shift;


    if(shift_value == 0)
    {
        matrix_buffer_shift =
            matrix_buffer[index];
    }
    else
    {
        matrix_buffer_shift =
            (uint8_t)
            (
                (matrix_buffer[index] << shift_value)
                |
                (matrix_buffer[index] >> (8 - shift_value))
            );
    }


    /*
     * Select one matrix ROW.
     *
     * ROW active = LOW.
     */
    switch(index)
    {
        case 0:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_8,
                GPIO_PIN_RESET
            );

            break;


        case 1:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_9,
                GPIO_PIN_RESET
            );

            break;


        case 2:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_10,
                GPIO_PIN_RESET
            );

            break;


        case 3:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_11,
                GPIO_PIN_RESET
            );

            break;


        case 4:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_12,
                GPIO_PIN_RESET
            );

            break;


        case 5:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_13,
                GPIO_PIN_RESET
            );

            break;


        case 6:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_14,
                GPIO_PIN_RESET
            );

            break;


        case 7:

            setCol(
                matrix_buffer_shift
            );

            HAL_GPIO_WritePin(
                GPIOB,
                GPIO_PIN_15,
                GPIO_PIN_RESET
            );

            break;


        default:

            break;
    }
}


/* =========================================================
 * MAIN
 * ========================================================= */

int main(void)
{
    HAL_Init();


    SystemClock_Config();


    /*
     * Disable JTAG so PA13, PA14, PA15
     * can be used as GPIO.
     */
    __HAL_RCC_AFIO_CLK_ENABLE();

    __HAL_AFIO_REMAP_SWJ_DISABLE();


    MX_GPIO_Init();

    MX_TIM2_Init();


    /*
     * Start TIM2 interrupt
     */
    HAL_TIM_Base_Start_IT(
        &htim2
    );


    /*
     * Initial clock value
     */
    updateClockBuffer();


    /*
     * LED/DOT = 1 second
     */
    setTimer1(1000);


    /*
     * 7SEG + Matrix scan = 10 ms
     */
    setTimer2(10);


    /*
     * Clock = 1 second
     */
    setTimer3(1000);


    /* =====================================================
     * MAIN LOOP
     * ===================================================== */

    while(1)
    {
        /* =================================================
         * LED RED + DOT
         *
         * Toggle every 1 second
         * ================================================= */

        if(timer1_flag == 1)
        {
            /*
             * Red LED
             */
            HAL_GPIO_TogglePin(
                GPIOA,
                GPIO_PIN_5
            );


            /*
             * DOT
             */
            HAL_GPIO_TogglePin(
                GPIOA,
                GPIO_PIN_4
            );


            /*
             * Restart timer
             */
            setTimer1(1000);
        }


        /* =================================================
         * TIMER 2
         *
         * 7SEG + MATRIX
         * ================================================= */

        if(timer2_flag == 1)
        {
            /* =============================================
             * 7 SEGMENT
             * ============================================= */

            update7SEG(
                index_led
            );


            index_led++;


            if(index_led >= 4)
            {
                index_led = 0;
            }


            /* =============================================
             * LED MATRIX
             * ============================================= */

            /*
             * Display current Matrix row
             * at current animation shift.
             */
            updateLEDMatrix(
                index_led_matrix,
                shift
            );


            /*
             * Next matrix row
             */
            index_led_matrix++;


            /*
             * Finished one complete 8-row frame
             */
            if(index_led_matrix >= MAX_LED_MATRIX)
            {
                /*
                 * Return to ROW0
                 */
                index_led_matrix = 0;


                /*
                 * Count complete Matrix frames.
                 *
                 * One frame:
                 *
                 * 8 x 10 ms
                 * = 80 ms
                 */
                matrix_shift_flag++;


                /*
                 * Slow animation:
                 *
                 * MATRIX_SHIFT_SPEED = 5
                 *
                 * 5 x 80ms
                 * = 400ms
                 *
                 * A shifts only once every ~400ms.
                 */
                if(matrix_shift_flag
                        >= MATRIX_SHIFT_SPEED)
                {
                    /*
                     * Restart frame counter
                     */
                    matrix_shift_flag = 0;


                    /*
                     * Move A by one position
                     */
                    shift++;


                    /*
                     * shift:
                     *
                     * 0 1 2 3 4 5 6 7
                     */
                    if(shift >= 8)
                    {
                        shift = 0;
                    }
                }
            }


            /*
             * IMPORTANT:
             *
             * Keep scanning at 10 ms.
             *
             * Do NOT increase this value
             * to slow the animation.
             */
            setTimer2(10);
        }


        /* =================================================
         * CLOCK
         * ================================================= */

        if(timer3_flag == 1)
        {
            second++;


            /*
             * 60 seconds
             */
            if(second >= 60)
            {
                second = 0;

                minute++;
            }


            /*
             * 60 minutes
             */
            if(minute >= 60)
            {
                minute = 0;

                hour++;
            }


            /*
             * 24 hours
             */
            if(hour >= 24)
            {
                hour = 0;
            }


            /*
             * Update HH:MM
             */
            updateClockBuffer();


            /*
             * Next second
             */
            setTimer3(1000);
        }
    }
}


/* =========================================================
 * SYSTEM CLOCK CONFIGURATION
 *
 * HSI = 8 MHz
 * ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct =
    {
        0
    };


    RCC_ClkInitTypeDef RCC_ClkInitStruct =
    {
        0
    };


    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;


    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;


    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;


    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if(HAL_RCC_OscConfig(
            &RCC_OscInitStruct)
            != HAL_OK)
    {
        Error_Handler();
    }


    RCC_ClkInitStruct.ClockType =
        RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_SYSCLK |
        RCC_CLOCKTYPE_PCLK1 |
        RCC_CLOCKTYPE_PCLK2;


    RCC_ClkInitStruct.SYSCLKSource =
        RCC_SYSCLKSOURCE_HSI;


    RCC_ClkInitStruct.AHBCLKDivider =
        RCC_SYSCLK_DIV1;


    RCC_ClkInitStruct.APB1CLKDivider =
        RCC_HCLK_DIV1;


    RCC_ClkInitStruct.APB2CLKDivider =
        RCC_HCLK_DIV1;


    if(HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0)
            != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
 * TIM2 INITIALIZATION
 *
 * HSI = 8 MHz
 *
 * Prescaler = 7999
 *
 * 8 MHz / 8000
 * = 1000 Hz
 *
 * Period = 9
 *
 * 1000 / 10
 * = 100 Hz
 *
 * interrupt period = 10 ms
 * ========================================================= */

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef
        sClockSourceConfig =
        {
            0
        };


    TIM_MasterConfigTypeDef
        sMasterConfig =
        {
            0
        };


    htim2.Instance =
        TIM2;


    htim2.Init.Prescaler =
        7999;


    htim2.Init.CounterMode =
        TIM_COUNTERMODE_UP;


    htim2.Init.Period =
        9;


    htim2.Init.ClockDivision =
        TIM_CLOCKDIVISION_DIV1;


    htim2.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;


    if(HAL_TIM_Base_Init(
            &htim2)
            != HAL_OK)
    {
        Error_Handler();
    }


    sClockSourceConfig.ClockSource =
        TIM_CLOCKSOURCE_INTERNAL;


    if(HAL_TIM_ConfigClockSource(
            &htim2,
            &sClockSourceConfig)
            != HAL_OK)
    {
        Error_Handler();
    }


    sMasterConfig.MasterOutputTrigger =
        TIM_TRGO_RESET;


    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;


    if(HAL_TIMEx_MasterConfigSynchronization(
            &htim2,
            &sMasterConfig)
            != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
 * GPIO INITIALIZATION
 * ========================================================= */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct =
    {
        0
    };


    /* GPIO clocks */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();


    /* =====================================================
     * GPIOA INITIAL LEVEL
     * ===================================================== */

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_2  |
        GPIO_PIN_3  |
        GPIO_PIN_4  |
        GPIO_PIN_5  |
        GPIO_PIN_6  |
        GPIO_PIN_7  |
        GPIO_PIN_8  |
        GPIO_PIN_9  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15,
        GPIO_PIN_RESET
    );


    /*
     * PA2,PA3
     * PA4,PA5
     * PA6-PA9
     * PA10-PA15
     */
    GPIO_InitStruct.Pin =
        GPIO_PIN_2  |
        GPIO_PIN_3  |
        GPIO_PIN_4  |
        GPIO_PIN_5  |
        GPIO_PIN_6  |
        GPIO_PIN_7  |
        GPIO_PIN_8  |
        GPIO_PIN_9  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15;


    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;


    GPIO_InitStruct.Pull =
        GPIO_NOPULL;


    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;


    HAL_GPIO_Init(
        GPIOA,
        &GPIO_InitStruct
    );


    /* =====================================================
     * GPIOB INITIAL LEVEL
     * ===================================================== */

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_0  |
        GPIO_PIN_1  |
        GPIO_PIN_2  |
        GPIO_PIN_3  |
        GPIO_PIN_4  |
        GPIO_PIN_5  |
        GPIO_PIN_6  |
        GPIO_PIN_8  |
        GPIO_PIN_9  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15,
        GPIO_PIN_RESET
    );


    GPIO_InitStruct.Pin =
        GPIO_PIN_0  |
        GPIO_PIN_1  |
        GPIO_PIN_2  |
        GPIO_PIN_3  |
        GPIO_PIN_4  |
        GPIO_PIN_5  |
        GPIO_PIN_6  |
        GPIO_PIN_8  |
        GPIO_PIN_9  |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15;


    GPIO_InitStruct.Mode =
        GPIO_MODE_OUTPUT_PP;


    GPIO_InitStruct.Pull =
        GPIO_NOPULL;


    GPIO_InitStruct.Speed =
        GPIO_SPEED_FREQ_LOW;


    HAL_GPIO_Init(
        GPIOB,
        &GPIO_InitStruct
    );
}


/* =========================================================
 * TIM2 CALLBACK
 *
 * Interrupt only runs software timers.
 * ========================================================= */

void HAL_TIM_PeriodElapsedCallback(
    TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        timerRun();
    }
}


/* =========================================================
 * ERROR HANDLER
 * ========================================================= */

void Error_Handler(void)
{
    __disable_irq();


    while(1)
    {

    }
}


#ifdef USE_FULL_ASSERT

void assert_failed(
    uint8_t *file,
    uint32_t line)
{

}

#endif
