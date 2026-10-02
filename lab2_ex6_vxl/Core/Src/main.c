/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 6 - Software Timer / Non-blocking Clock
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* USER CODE BEGIN PV */

/* =========================================================
 * 7-SEGMENT VARIABLES
 * ========================================================= */

const int MAX_LED = 4;

volatile int index_led = 0;

/*
 * led_buffer:
 *
 * [hour tens]
 * [hour units]
 * [minute tens]
 * [minute units]
 */
volatile int led_buffer[4] = {0, 0, 0, 0};


/* =========================================================
 * CLOCK VARIABLES
 * ========================================================= */

int hour = 15;
int minute = 8;
int second = 50;


/* =========================================================
 * 7SEG SCANNING TIMER
 *
 * TIM2 interrupt = 10 ms
 *
 * 25 x 10 ms = 250 ms/display
 * 4 displays = 1000 ms
 * scanning frequency = 1 Hz
 * ========================================================= */

volatile int timer_counter = 0;


/* =========================================================
 * DOT TIMER
 *
 * 100 x 10 ms = 1000 ms
 * ========================================================= */

volatile int dot_counter = 0;


/* =========================================================
 * EXERCISE 6 - SOFTWARE TIMER
 * =========================================================
 *
 * timer0_counter:
 * decreases once every TIM2 interrupt.
 *
 * timer0_flag:
 * = 1 when software timer expires.
 *
 * TIMER_CYCLE:
 * TIM2 interrupt period = 10 ms.
 */

volatile int timer0_counter = 0;
volatile int timer0_flag = 0;

const int TIMER_CYCLE = 10;

/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);


/* USER CODE BEGIN PFP */

void display7SEG(int num);
void disableAll7SEG(void);

void update7SEG(int index);
void updateClockBuffer(void);

/* Exercise 6 */
void setTimer0(int duration);
void timer_run(void);

/* USER CODE END PFP */


/* USER CODE BEGIN 0 */


/* =========================================================
 * Disable all four seven-segment displays
 *
 * PNP:
 * HIGH -> OFF
 * LOW  -> ON
 * ========================================================= */
void disableAll7SEG(void)
{
    HAL_GPIO_WritePin(GPIOA,
                      EN0_Pin |
                      EN1_Pin |
                      EN2_Pin |
                      EN3_Pin,
                      GPIO_PIN_SET);
}


/* =========================================================
 * DISPLAY ONE NUMBER
 *
 * Mapping:
 *
 * SEG0 = a
 * SEG1 = b
 * SEG2 = c
 * SEG3 = d
 * SEG4 = e
 * SEG5 = f
 * SEG6 = g
 *
 * Common Anode:
 *
 * RESET = segment ON
 * SET   = segment OFF
 * ========================================================= */
void display7SEG(int num)
{
    /* Turn OFF all segments first */
    HAL_GPIO_WritePin(GPIOB,
                      SEG0_Pin |
                      SEG1_Pin |
                      SEG2_Pin |
                      SEG3_Pin |
                      SEG4_Pin |
                      SEG5_Pin |
                      SEG6_Pin,
                      GPIO_PIN_SET);


    switch(num)
    {
        /* 0 = a b c d e f */
        case 0:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG4_Pin |
                              SEG5_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 1 = b c */
        case 1:
            HAL_GPIO_WritePin(GPIOB,
                              SEG1_Pin |
                              SEG2_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 2 = a b d e g */
        case 2:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG3_Pin |
                              SEG4_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 3 = a b c d g */
        case 3:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 4 = b c f g */
        case 4:
            HAL_GPIO_WritePin(GPIOB,
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 5 = a c d f g */
        case 5:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 6 = a c d e f g */
        case 6:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG4_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 7 = a b c */
        case 7:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 8 = all */
        case 8:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG4_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        /* 9 = a b c d f g */
        case 9:
            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);
            break;


        default:
            break;
    }
}


/* =========================================================
 * UPDATE ONE 7SEG
 * ========================================================= */
void update7SEG(int index)
{
    /*
     * Disable all displays before changing the
     * shared segment bus.
     */
    disableAll7SEG();


    switch(index)
    {
        case 0:

            display7SEG(led_buffer[0]);

            HAL_GPIO_WritePin(
                EN0_GPIO_Port,
                EN0_Pin,
                GPIO_PIN_RESET
            );

            break;


        case 1:

            display7SEG(led_buffer[1]);

            HAL_GPIO_WritePin(
                EN1_GPIO_Port,
                EN1_Pin,
                GPIO_PIN_RESET
            );

            break;


        case 2:

            display7SEG(led_buffer[2]);

            HAL_GPIO_WritePin(
                EN2_GPIO_Port,
                EN2_Pin,
                GPIO_PIN_RESET
            );

            break;


        case 3:

            display7SEG(led_buffer[3]);

            HAL_GPIO_WritePin(
                EN3_GPIO_Port,
                EN3_Pin,
                GPIO_PIN_RESET
            );

            break;


        default:
            break;
    }
}


/* =========================================================
 * EXERCISE 5
 * UPDATE CLOCK BUFFER
 *
 * Example:
 *
 * hour   = 15
 * minute = 8
 *
 * led_buffer = {1,5,0,8}
 *
 * Display = 15:08
 * ========================================================= */
void updateClockBuffer(void)
{
    led_buffer[0] = hour / 10;
    led_buffer[1] = hour % 10;

    led_buffer[2] = minute / 10;
    led_buffer[3] = minute % 10;
}


/* =========================================================
 * EXERCISE 6
 * SET SOFTWARE TIMER
 *
 * duration is in milliseconds.
 *
 * Example:
 *
 * setTimer0(1000)
 *
 * timer0_counter = 1000 / 10
 *                = 100
 * ========================================================= */
void setTimer0(int duration)
{
    timer0_counter = duration / TIMER_CYCLE;

    timer0_flag = 0;
}


/* =========================================================
 * EXERCISE 6
 * SOFTWARE TIMER RUN
 *
 * Called every TIM2 interrupt = every 10 ms.
 * ========================================================= */
void timer_run(void)
{
    if(timer0_counter > 0)
    {
        timer0_counter--;

        if(timer0_counter == 0)
        {
            timer0_flag = 1;
        }
    }
}

/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_TIM2_Init();


    /* USER CODE BEGIN 2 */


    /* =====================================================
     * INITIAL STATE
     * ===================================================== */

    /* Disable all displays */
    disableAll7SEG();


    /* Turn OFF all segments */
    HAL_GPIO_WritePin(GPIOB,
                      SEG0_Pin |
                      SEG1_Pin |
                      SEG2_Pin |
                      SEG3_Pin |
                      SEG4_Pin |
                      SEG5_Pin |
                      SEG6_Pin,
                      GPIO_PIN_SET);


    /*
     * DOT initially OFF.
     */
    HAL_GPIO_WritePin(
        DOT_GPIO_Port,
        DOT_Pin,
        GPIO_PIN_SET
    );


    /*
     * Initial clock:
     *
     * 15:08:50
     */
    updateClockBuffer();


    /*
     * Immediately display first digit.
     */
    update7SEG(index_led);


    index_led++;

    if(index_led >= MAX_LED)
    {
        index_led = 0;
    }


    timer_counter = 0;
    dot_counter = 0;


    /*
     * Start hardware TIM2 interrupt.
     */
    HAL_TIM_Base_Start_IT(&htim2);


    /*
     * =====================================================
     * EXERCISE 6
     *
     * Start software timer for 1000 ms.
     *
     * This replaces HAL_Delay(1000).
     * =====================================================
     */
    setTimer0(1000);


    /* USER CODE END 2 */


    /* Infinite loop ---------------------------------------------------------*/

    /* USER CODE BEGIN WHILE */

    while(1)
    {
        /*
         * =================================================
         * NON-BLOCKING CLOCK UPDATE
         * =================================================
         *
         * No HAL_Delay() here.
         *
         * Main keeps running continuously.
         */

        if(timer0_flag == 1)
        {
            /*
             * The software timer has reached 1 second.
             */

            second++;


            /*
             * 60 seconds -> next minute
             */
            if(second >= 60)
            {
                second = 0;

                minute++;
            }


            /*
             * 60 minutes -> next hour
             */
            if(minute >= 60)
            {
                minute = 0;

                hour++;
            }


            /*
             * 24 hours -> 00
             */
            if(hour >= 24)
            {
                hour = 0;
            }


            /*
             * Update HH:MM on led_buffer.
             */
            updateClockBuffer();


            /*
             * Restart software timer:
             *
             * next clock update occurs after
             * another 1000 ms.
             */
            setTimer0(1000);
        }
    }

    /* USER CODE END WHILE */
}


/**
  * @brief System Clock Configuration
  * @retval None
  */
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


    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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


    if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
                           FLASH_LATENCY_0) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};

    TIM_MasterConfigTypeDef sMasterConfig = {0};


    /*
     * System Clock = 8 MHz
     *
     * Prescaler = 7999
     *
     * 8 MHz / 8000
     * = 1000 Hz
     *
     * Period = 9
     *
     * 0 -> 9 = 10 counts
     *
     * 1000 / 10
     * = 100 Hz
     *
     * Interrupt period:
     *
     * 1 / 100 Hz
     * = 10 ms
     */

    htim2.Instance = TIM2;


    htim2.Init.Prescaler = 7999;


    htim2.Init.CounterMode =
        TIM_COUNTERMODE_UP;


    htim2.Init.Period = 9;


    htim2.Init.ClockDivision =
        TIM_CLOCKDIVISION_DIV1;


    htim2.Init.AutoReloadPreload =
        TIM_AUTORELOAD_PRELOAD_DISABLE;


    if(HAL_TIM_Base_Init(&htim2) != HAL_OK)
    {
        Error_Handler();
    }


    sClockSourceConfig.ClockSource =
        TIM_CLOCKSOURCE_INTERNAL;


    if(HAL_TIM_ConfigClockSource(
            &htim2,
            &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }


    sMasterConfig.MasterOutputTrigger =
        TIM_TRGO_RESET;


    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;


    if(HAL_TIMEx_MasterConfigSynchronization(
            &htim2,
            &sMasterConfig) != HAL_OK)
    {
        Error_Handler();
    }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    /*
     * GPIO clock
     */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();


    /*
     * GPIOA:
     *
     * PA4 = DOT
     * PA6 = EN0
     * PA7 = EN1
     * PA8 = EN2
     * PA9 = EN3
     */
    HAL_GPIO_WritePin(GPIOA,
                      DOT_Pin |
                      EN0_Pin |
                      EN1_Pin |
                      EN2_Pin |
                      EN3_Pin,
                      GPIO_PIN_RESET);


    /*
     * GPIOB:
     *
     * PB0 -> PB6
     */
    HAL_GPIO_WritePin(GPIOB,
                      SEG0_Pin |
                      SEG1_Pin |
                      SEG2_Pin |
                      SEG3_Pin |
                      SEG4_Pin |
                      SEG5_Pin |
                      SEG6_Pin,
                      GPIO_PIN_RESET);


    /*
     * Configure GPIOA outputs
     */
    GPIO_InitStruct.Pin =
        DOT_Pin |
        EN0_Pin |
        EN1_Pin |
        EN2_Pin |
        EN3_Pin;


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


    /*
     * Configure GPIOB outputs
     */
    GPIO_InitStruct.Pin =
        SEG0_Pin |
        SEG1_Pin |
        SEG2_Pin |
        SEG3_Pin |
        SEG4_Pin |
        SEG5_Pin |
        SEG6_Pin;


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


/* USER CODE BEGIN 4 */


/**
 * ============================================================
 * TIM2 CALLBACK
 * ============================================================
 *
 * Called every 10 ms.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        /*
         * ==================================================
         * EXERCISE 6 SOFTWARE TIMER
         * ==================================================
         *
         * Decrease timer0_counter every 10ms.
         */
        timer_run();


        /*
         * ==================================================
         * 7SEG SCANNING
         * ==================================================
         *
         * 25 x 10ms = 250ms/display
         *
         * 4 displays:
         *
         * 4 x 250ms = 1 second
         *
         * Scan frequency = 1 Hz
         */
        timer_counter++;


        if(timer_counter >= 25)
        {
            timer_counter = 0;


            update7SEG(index_led);


            index_led++;


            if(index_led >= MAX_LED)
            {
                index_led = 0;
            }
        }


        /*
         * ==================================================
         * DOT
         * ==================================================
         *
         * Toggle every 1 second.
         */
        dot_counter++;


        if(dot_counter >= 100)
        {
            dot_counter = 0;


            HAL_GPIO_TogglePin(
                DOT_GPIO_Port,
                DOT_Pin
            );
        }
    }
}

/* USER CODE END 4 */


/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    __disable_irq();

    while(1)
    {

    }
}


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}

#endif
