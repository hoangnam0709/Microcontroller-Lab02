/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 8 - All Processing in Main Loop
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* USER CODE BEGIN PV */

/* =========================================================
 * DISPLAY VARIABLES
 * ========================================================= */

const int MAX_LED = 4;

volatile int index_led = 0;


/*
 * HH:MM buffer
 *
 * led_buffer[0] = hour tens
 * led_buffer[1] = hour units
 * led_buffer[2] = minute tens
 * led_buffer[3] = minute units
 */
volatile int led_buffer[4] = {0, 0, 0, 0};


/* =========================================================
 * CLOCK VARIABLES
 * ========================================================= */

int hour = 15;
int minute = 8;
int second = 50;


/* =========================================================
 * SOFTWARE TIMER SETTINGS
 * =========================================================
 *
 * Hardware TIM2 interrupt period = 10 ms
 */
const int TIMER_CYCLE = 10;


/*
 * TIMER 0
 *
 * Used for:
 * - update second/minute/hour
 * - update clock buffer
 * - blink DOT
 *
 * Period = 1000 ms
 */
volatile int timer0_counter = 0;
volatile int timer0_flag = 0;


/*
 * TIMER 1
 *
 * Used for:
 * - update7SEG()
 * - switch to next display
 *
 * Period = 250 ms
 */
volatile int timer1_counter = 0;
volatile int timer1_flag = 0;

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


/* Software Timer 0 */
void setTimer0(int duration);


/* Software Timer 1 */
void setTimer1(int duration);


/* Run all software timers */
void timer_run(void);

/* USER CODE END PFP */


/* USER CODE BEGIN 0 */


/* =========================================================
 * DISABLE ALL 7SEG
 *
 * PNP:
 *
 * HIGH = OFF
 * LOW  = ON
 * ========================================================= */
void disableAll7SEG(void)
{
    HAL_GPIO_WritePin(
        GPIOA,
        EN0_Pin |
        EN1_Pin |
        EN2_Pin |
        EN3_Pin,
        GPIO_PIN_SET
    );
}


/* =========================================================
 * DISPLAY NUMBER ON COMMON SEGMENT BUS
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
 * COMMON ANODE:
 *
 * LOW  = segment ON
 * HIGH = segment OFF
 * ========================================================= */
void display7SEG(int num)
{
    /*
     * First turn OFF all segments.
     */
    HAL_GPIO_WritePin(
        GPIOB,
        SEG0_Pin |
        SEG1_Pin |
        SEG2_Pin |
        SEG3_Pin |
        SEG4_Pin |
        SEG5_Pin |
        SEG6_Pin,
        GPIO_PIN_SET
    );


    switch(num)
    {
        /* 0 = a b c d e f */
        case 0:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG1_Pin |
                SEG2_Pin |
                SEG3_Pin |
                SEG4_Pin |
                SEG5_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 1 = b c */
        case 1:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG1_Pin |
                SEG2_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 2 = a b d e g */
        case 2:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG1_Pin |
                SEG3_Pin |
                SEG4_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 3 = a b c d g */
        case 3:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG1_Pin |
                SEG2_Pin |
                SEG3_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 4 = b c f g */
        case 4:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG1_Pin |
                SEG2_Pin |
                SEG5_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 5 = a c d f g */
        case 5:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG2_Pin |
                SEG3_Pin |
                SEG5_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 6 = a c d e f g */
        case 6:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG2_Pin |
                SEG3_Pin |
                SEG4_Pin |
                SEG5_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 7 = a b c */
        case 7:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG1_Pin |
                SEG2_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 8 = all segments */
        case 8:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG1_Pin |
                SEG2_Pin |
                SEG3_Pin |
                SEG4_Pin |
                SEG5_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        /* 9 = a b c d f g */
        case 9:

            HAL_GPIO_WritePin(
                GPIOB,
                SEG0_Pin |
                SEG1_Pin |
                SEG2_Pin |
                SEG3_Pin |
                SEG5_Pin |
                SEG6_Pin,
                GPIO_PIN_RESET
            );

            break;


        default:

            break;
    }
}


/* =========================================================
 * UPDATE ONE 7SEG
 *
 * IMPORTANT FOR EX8:
 *
 * This function is now called from main(),
 * NOT from HAL_TIM_PeriodElapsedCallback().
 * ========================================================= */
void update7SEG(int index)
{
    /*
     * Disable all displays before changing
     * shared segment data.
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
 * UPDATE CLOCK BUFFER
 *
 * Example:
 *
 * hour   = 15
 * minute = 8
 *
 * led_buffer =
 *
 * {1,5,0,8}
 *
 * Display:
 *
 * 15:08
 * ========================================================= */
void updateClockBuffer(void)
{
    /*
     * Hour
     */
    led_buffer[0] = hour / 10;

    led_buffer[1] = hour % 10;


    /*
     * Minute
     */
    led_buffer[2] = minute / 10;

    led_buffer[3] = minute % 10;
}


/* =========================================================
 * SOFTWARE TIMER 0
 *
 * Used for clock + DOT.
 * ========================================================= */
void setTimer0(int duration)
{
    timer0_counter = duration / TIMER_CYCLE;

    timer0_flag = 0;
}


/* =========================================================
 * SOFTWARE TIMER 1
 *
 * Used for 7SEG scanning.
 * ========================================================= */
void setTimer1(int duration)
{
    timer1_counter = duration / TIMER_CYCLE;

    timer1_flag = 0;
}


/* =========================================================
 * RUN SOFTWARE TIMERS
 *
 * IMPORTANT:
 *
 * This is the ONLY processing executed
 * by the timer interrupt in Exercise 8.
 * ========================================================= */
void timer_run(void)
{
    /*
     * ============================
     * TIMER 0
     * ============================
     */
    if(timer0_counter > 0)
    {
        timer0_counter--;


        if(timer0_counter == 0)
        {
            timer0_flag = 1;
        }
    }


    /*
     * ============================
     * TIMER 1
     * ============================
     */
    if(timer1_counter > 0)
    {
        timer1_counter--;


        if(timer1_counter == 0)
        {
            timer1_flag = 1;
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
    /* MCU Configuration -----------------------------------------------------*/

    HAL_Init();


    SystemClock_Config();


    MX_GPIO_Init();


    MX_TIM2_Init();


    /* USER CODE BEGIN 2 */


    /* =====================================================
     * INITIAL STATE
     * ===================================================== */


    /*
     * Turn OFF all displays.
     */
    disableAll7SEG();


    /*
     * Turn OFF all segments.
     */
    HAL_GPIO_WritePin(
        GPIOB,
        SEG0_Pin |
        SEG1_Pin |
        SEG2_Pin |
        SEG3_Pin |
        SEG4_Pin |
        SEG5_Pin |
        SEG6_Pin,
        GPIO_PIN_SET
    );


    /*
     * DOT initially OFF.
     *
     * PA4 HIGH -> DOT OFF
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
     *
     * Buffer:
     *
     * {1,5,0,8}
     */
    updateClockBuffer();


    /*
     * Display first digit immediately.
     */
    update7SEG(index_led);


    /*
     * Next scan position.
     */
    index_led++;


    if(index_led >= MAX_LED)
    {
        index_led = 0;
    }


    /*
     * Start TIM2 hardware interrupt.
     *
     * Interrupt every 10 ms.
     */
    HAL_TIM_Base_Start_IT(&htim2);


    /*
     * TIMER 0:
     *
     * clock + DOT every 1 second.
     */
    setTimer0(1000);


    /*
     * TIMER 1:
     *
     * switch 7SEG every 250 ms.
     */
    setTimer1(250);


    /* USER CODE END 2 */


    /* Infinite loop ---------------------------------------------------------*/

    /* USER CODE BEGIN WHILE */

    while(1)
    {
        /*
         * =================================================
         * SOFTWARE TIMER 0
         *
         * CLOCK + DOT
         * =================================================
         */

        if(timer0_flag == 1)
        {
            /*
             * Increase second.
             */
            second++;


            /*
             * 60 seconds -> +1 minute
             */
            if(second >= 60)
            {
                second = 0;

                minute++;
            }


            /*
             * 60 minutes -> +1 hour
             */
            if(minute >= 60)
            {
                minute = 0;

                hour++;
            }


            /*
             * 24 hours -> return to 00
             */
            if(hour >= 24)
            {
                hour = 0;
            }


            /*
             * Update HH:MM data.
             */
            updateClockBuffer();


            /*
             * DOT processing is in MAIN.
             */
            HAL_GPIO_TogglePin(
                DOT_GPIO_Port,
                DOT_Pin
            );


            /*
             * Restart 1-second software timer.
             */
            setTimer0(1000);
        }


        /*
         * =================================================
         * SOFTWARE TIMER 1
         *
         * 7SEG SCANNING
         * =================================================
         */

        if(timer1_flag == 1)
        {
            /*
             * IMPORTANT:
             *
             * update7SEG() is now executed
             * in MAIN, not inside interrupt.
             */
            update7SEG(index_led);


            /*
             * Next display.
             */
            index_led++;


            /*
             * Keep index:
             *
             * 0,1,2,3
             */
            if(index_led >= MAX_LED)
            {
                index_led = 0;
            }


            /*
             * Restart 250-ms scan timer.
             */
            setTimer1(250);
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


    /*
     * HSI = 8 MHz
     */
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


    if(HAL_RCC_ClockConfig(
            &RCC_ClkInitStruct,
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
     * System Clock:
     *
     * 8 MHz
     *
     * Prescaler:
     *
     * 7999
     *
     * 8 MHz / 8000
     * = 1000 Hz
     *
     * Period:
     *
     * 9
     *
     * 0 -> 9
     * = 10 counts
     *
     * 1000 / 10
     * = 100 Hz
     *
     * Interrupt period:
     *
     * 1 / 100
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


    /* GPIO Ports Clock Enable */
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
    HAL_GPIO_WritePin(
        GPIOA,
        DOT_Pin |
        EN0_Pin |
        EN1_Pin |
        EN2_Pin |
        EN3_Pin,
        GPIO_PIN_RESET
    );


    /*
     * GPIOB:
     *
     * PB0 -> PB6
     */
    HAL_GPIO_WritePin(
        GPIOB,
        SEG0_Pin |
        SEG1_Pin |
        SEG2_Pin |
        SEG3_Pin |
        SEG4_Pin |
        SEG5_Pin |
        SEG6_Pin,
        GPIO_PIN_RESET
    );


    /*
     * Configure GPIOA outputs.
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
     * Configure GPIOB outputs.
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
 * EXERCISE 8
 * TIMER INTERRUPT
 * ============================================================
 *
 * IMPORTANT:
 *
 * The interrupt handler ONLY handles
 * the software timers.
 *
 * NO:
 *
 * update7SEG()
 * updateClockBuffer()
 * HAL_GPIO_TogglePin()
 * clock calculation
 *
 * is processed here.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        /*
         * ONLY software timer processing.
         */
        timer_run();
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
