#include "main.h"

/* =========================================================
   TIM2
   ========================================================= */
TIM_HandleTypeDef htim2;



volatile int timer1_counter = 0;
volatile int timer1_flag = 0;

volatile int timer2_counter = 0;
volatile int timer2_flag = 0;

volatile int timer3_counter = 0;
volatile int timer3_flag = 0;

const int TIMER_CYCLE = 10;




int index_led = 0;

uint8_t led_buffer[4] = {1, 5, 0, 8};



const uint8_t seg7_code[10] =
{
    0xC0,   // 0
    0xF9,   // 1
    0xA4,   // 2
    0xB0,   // 3
    0x99,   // 4
    0x92,   // 5
    0x82,   // 6
    0xF8,   // 7
    0x80,   // 8
    0x90    // 9
};


/* =========================================================
   CLOCK
   ========================================================= */

int hour = 15;
int minute = 8;
int second = 50;


/* =========================================================
   LED MATRIX 8x8
   ========================================================= */

#define MAX_LED_MATRIX 8

int index_led_matrix = 0;




uint8_t matrix_buffer[8] =
{
    0x00,
    0xFC,
    0x12,
    0x11,
    0x11,
    0x12,
    0xFC,
    0x00
};


/* =========================================================
   PROTOTYPE
   ========================================================= */

void SystemClock_Config(void);

static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);


/* 7 SEG */
void display7SEG(int num);
void clearLed(void);
void update7SEG(int index);
void updateClockBuffer(void);


/* SOFTWARE TIMER */
void setTimer1(int duration);
void setTimer2(int duration);
void setTimer3(int duration);
void timerRun(void);


/* MATRIX */
void clearAllMatrixCOL(void);
void setMatrixROW(uint8_t val);
void updateLEDMatrix(int index);


/* =========================================================
   DISPLAY 7 SEGMENT
   ========================================================= */

void display7SEG(int num)
{
    uint8_t code;

    if (num < 0 || num > 9)
    {
        code = 0xFF;
    }
    else
    {
        code = seg7_code[num];
    }

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_0,
        (code & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_1,
        (code & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_2,
        (code & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_3,
        (code & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_4,
        (code & 0x10) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_5,
        (code & 0x20) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_6,
        (code & 0x40) ? GPIO_PIN_SET : GPIO_PIN_RESET
    );
}


/* =========================================================
   7 SEG ALL OFF
   ========================================================= */

void clearLed(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_6,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_7,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_8,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_9,
        GPIO_PIN_SET
    );
}



void update7SEG(int index)
{
    clearLed();

    if (index < 0 || index >= 4)
    {
        return;
    }

    display7SEG(led_buffer[index]);

    switch (index)
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
   CLOCK BUFFER
   ========================================================= */

void updateClockBuffer(void)
{
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;
    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}




void clearAllMatrixCOL(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_2,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_3,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_10,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_11,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_12,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_13,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_14,
        GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_15,
        GPIO_PIN_SET
    );
}



void setMatrixROW(uint8_t val)
{
    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_8,
        (val & 0x01)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_9,
        (val & 0x02)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_10,
        (val & 0x04)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_11,
        (val & 0x08)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_12,
        (val & 0x10)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_13,
        (val & 0x20)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_14,
        (val & 0x40)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );

    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_15,
        (val & 0x80)
        ? GPIO_PIN_RESET
        : GPIO_PIN_SET
    );
}



void updateLEDMatrix(int index)
{
    clearAllMatrixCOL();


    setMatrixROW(matrix_buffer[index]);


    switch (index)
    {
        case 0:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_2,
                GPIO_PIN_RESET
            );
            break;

        case 1:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_3,
                GPIO_PIN_RESET
            );
            break;

        case 2:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_10,
                GPIO_PIN_RESET
            );
            break;

        case 3:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_11,
                GPIO_PIN_RESET
            );
            break;

        case 4:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_12,
                GPIO_PIN_RESET
            );
            break;

        case 5:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_13,
                GPIO_PIN_RESET
            );
            break;

        case 6:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_14,
                GPIO_PIN_RESET
            );
            break;

        case 7:
            HAL_GPIO_WritePin(
                GPIOA,
                GPIO_PIN_15,
                GPIO_PIN_RESET
            );
            break;

        default:
            break;
    }
}


/* =========================================================
   SOFTWARE TIMER 1
   ========================================================= */

void setTimer1(int duration)
{
    timer1_counter = duration / TIMER_CYCLE;
    timer1_flag = 0;
}


/* =========================================================
   SOFTWARE TIMER 2
   ========================================================= */

void setTimer2(int duration)
{
    timer2_counter = duration / TIMER_CYCLE;
    timer2_flag = 0;
}


/* =========================================================
   SOFTWARE TIMER 3
   ========================================================= */

void setTimer3(int duration)
{
    timer3_counter = duration / TIMER_CYCLE;
    timer3_flag = 0;
}


/* =========================================================
   TIMER RUN
   ========================================================= */

void timerRun(void)
{
    if (timer1_counter > 0)
    {
        timer1_counter--;

        if (timer1_counter == 0)
        {
            timer1_flag = 1;
        }
    }


    if (timer2_counter > 0)
    {
        timer2_counter--;

        if (timer2_counter == 0)
        {
            timer2_flag = 1;
        }
    }


    if (timer3_counter > 0)
    {
        timer3_counter--;

        if (timer3_counter == 0)
        {
            timer3_flag = 1;
        }
    }
}


/* =========================================================
   MAIN
   ========================================================= */

int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_TIM2_Init();


    HAL_TIM_Base_Start_IT(&htim2);


    updateClockBuffer();


    setTimer1(1000);


    setTimer2(10);


    setTimer3(1000);


    while (1)
    {
        /* =================================================
           LED RED + DOT
           ================================================= */

        if (timer1_flag == 1)
        {
            timer1_flag = 0;


            HAL_GPIO_TogglePin(
                GPIOA,
                GPIO_PIN_5
            );


            HAL_GPIO_TogglePin(
                GPIOA,
                GPIO_PIN_4
            );


            setTimer1(1000);
        }


        /* =================================================
           7SEG + MATRIX
           ================================================= */

        if (timer2_flag == 1)
        {
            timer2_flag = 0;


            /* 7SEG */
            update7SEG(index_led);

            index_led++;

            if (index_led >= 4)
            {
                index_led = 0;
            }


            /* MATRIX */
            updateLEDMatrix(index_led_matrix);

            index_led_matrix++;

            if (index_led_matrix >= MAX_LED_MATRIX)
            {
                index_led_matrix = 0;
            }


            setTimer2(10);
        }


        /* =================================================
           CLOCK
           ================================================= */

        if (timer3_flag == 1)
        {
            timer3_flag = 0;


            second++;


            if (second >= 60)
            {
                second = 0;
                minute++;
            }


            if (minute >= 60)
            {
                minute = 0;
                hour++;
            }


            if (hour >= 24)
            {
                hour = 0;
            }


            updateClockBuffer();


            setTimer3(1000);
        }
    }
}


/* =========================================================
   SYSTEM CLOCK
   HSI = 8MHz
   ========================================================= */

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if (HAL_RCC_OscConfig(&RCC_OscInitStruct)
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


    if (HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
            FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
   TIM2
   8MHz / (7999 + 1) = 1kHz
   1kHz / (9 + 1) = 100Hz
   => 10ms
   ========================================================= */

static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};


    htim2.Instance = TIM2;


    htim2.Init.Prescaler = 7999;

    htim2.Init.CounterMode =
        TIM_COUNTERMODE_UP;

    htim2.Init.Period = 9;

    htim2.Init.ClockDivision =
        TIM_CLOCKDIVISION_DIV1;

    htim2.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;


    if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }


    sClockSourceConfig.ClockSource =
        TIM_CLOCKSOURCE_INTERNAL;


    if (HAL_TIM_ConfigClockSource(
            &htim2,
            &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }


    sMasterConfig.MasterOutputTrigger =
        TIM_TRGO_RESET;

    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;


    if (HAL_TIMEx_MasterConfigSynchronization(
            &htim2,
            &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


/* =========================================================
   GPIO INIT
   ========================================================= */

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();



    __HAL_AFIO_REMAP_SWJ_DISABLE();




    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_4 |
        GPIO_PIN_5 |
        GPIO_PIN_6 |
        GPIO_PIN_7 |
        GPIO_PIN_8 |
        GPIO_PIN_9,
        GPIO_PIN_SET
    );


    HAL_GPIO_WritePin(
        GPIOA,
        GPIO_PIN_2 |
        GPIO_PIN_3 |
        GPIO_PIN_10 |
        GPIO_PIN_11 |
        GPIO_PIN_12 |
        GPIO_PIN_13 |
        GPIO_PIN_14 |
        GPIO_PIN_15,
        GPIO_PIN_SET
    );


    /* =====================================================
       GPIOA
       ===================================================== */

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



    HAL_GPIO_WritePin(
        GPIOB,
        GPIO_PIN_0 |
        GPIO_PIN_1 |
        GPIO_PIN_2 |
        GPIO_PIN_3 |
        GPIO_PIN_4 |
        GPIO_PIN_5 |
        GPIO_PIN_6,
        GPIO_PIN_SET
    );


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


    HAL_GPIO_Init(
        GPIOB,
        &GPIO_InitStruct
    );
}



void HAL_TIM_PeriodElapsedCallback(
    TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2)
    {
        timerRun();
    }
}


/* =========================================================
   ERROR HANDLER
   ========================================================= */

void Error_Handler(void)
{
    __disable_irq();

    while (1)
    {
    }
}
