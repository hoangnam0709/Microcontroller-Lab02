/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 5 - Digital Clock HH:MM
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* USER CODE BEGIN PV */

/* Number of seven-segment displays */
const int MAX_LED = 4;

/* Current display index: 0 -> 3 */
int index_led = 0;

/*
 * Four digits displayed:
 *
 * led_buffer[0] = hour tens
 * led_buffer[1] = hour units
 * led_buffer[2] = minute tens
 * led_buffer[3] = minute units
 */
int led_buffer[4] = {0, 0, 0, 0};


/*
 * Exercise 5 clock
 *
 * Initial time from the lab:
 * 15:08:50
 */
int hour = 15;
int minute = 8;
int second = 50;


/*
 * TIM2 interrupt = 10 ms
 *
 * Exercise 4/5 scanning:
 * 25 x 10 ms = 250 ms/display
 *
 * 4 x 250 ms = 1 second
 * => scanning frequency = 1 Hz
 */
volatile int timer_counter = 0;


/*
 * DOT:
 *
 * 100 x 10 ms = 1000 ms = 1 second
 */
volatile int dot_counter = 0;

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

/* USER CODE END PFP */


/* USER CODE BEGIN 0 */


/*
 * ============================================================
 * Disable all four seven-segment displays
 * ============================================================
 *
 * PNP:
 *
 * EN HIGH -> OFF
 * EN LOW  -> ON
 */
void disableAll7SEG(void)
{
    HAL_GPIO_WritePin(GPIOA,
                      EN0_Pin |
                      EN1_Pin |
                      EN2_Pin |
                      EN3_Pin,
                      GPIO_PIN_SET);
}


/*
 * ============================================================
 * display7SEG()
 * ============================================================
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
 */
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


        /* 8 = all segments */
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


/*
 * ============================================================
 * update7SEG()
 * ============================================================
 *
 * Select one display and show corresponding led_buffer value.
 */
void update7SEG(int index)
{
    /* Disable all displays before changing segment data */
    disableAll7SEG();

    switch(index)
    {
        case 0:

            display7SEG(led_buffer[0]);

            HAL_GPIO_WritePin(EN0_GPIO_Port,
                              EN0_Pin,
                              GPIO_PIN_RESET);
            break;


        case 1:

            display7SEG(led_buffer[1]);

            HAL_GPIO_WritePin(EN1_GPIO_Port,
                              EN1_Pin,
                              GPIO_PIN_RESET);
            break;


        case 2:

            display7SEG(led_buffer[2]);

            HAL_GPIO_WritePin(EN2_GPIO_Port,
                              EN2_Pin,
                              GPIO_PIN_RESET);
            break;


        case 3:

            display7SEG(led_buffer[3]);

            HAL_GPIO_WritePin(EN3_GPIO_Port,
                              EN3_Pin,
                              GPIO_PIN_RESET);
            break;


        default:
            break;
    }
}


/*
 * ============================================================
 * Exercise 5
 * updateClockBuffer()
 * ============================================================
 *
 * Example:
 *
 * hour   = 15
 * minute = 8
 *
 * led_buffer:
 *
 * [1] [5] [0] [8]
 *
 * Display:
 *
 * 15:08
 *
 * If hour/minute has only one digit,
 * a leading zero is automatically added.
 */
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


    /*
     * ============================================================
     * INITIAL STATE
     * ============================================================
     */

    /* Turn OFF all seven-segment displays */
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
     * DOT initially OFF
     *
     * PA4 HIGH -> DOT OFF
     */
    HAL_GPIO_WritePin(DOT_GPIO_Port,
                      DOT_Pin,
                      GPIO_PIN_SET);


    /*
     * Generate initial HH:MM
     *
     * hour   = 15
     * minute = 8
     *
     * => led_buffer = {1,5,0,8}
     */
    updateClockBuffer();


    /*
     * Display first digit immediately
     */
    update7SEG(index_led);


    /*
     * Next scanning position
     */
    index_led++;

    if(index_led >= MAX_LED)
    {
        index_led = 0;
    }


    /* Reset counters */
    timer_counter = 0;
    dot_counter = 0;


    /*
     * Start TIM2 interrupt
     */
    HAL_TIM_Base_Start_IT(&htim2);


    /* USER CODE END 2 */


    /* Infinite loop ---------------------------------------------------------*/
    /* USER CODE BEGIN WHILE */

    while(1)
    {
        /*
         * ========================================================
         * Exercise 5 clock
         * ========================================================
         *
         * This follows Program 1.5 from the lab.
         */


        /*
         * Increase second
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
         * 24 hours -> return to 00
         */
        if(hour >= 24)
        {
            hour = 0;
        }


        /*
         * Convert hour/minute into four digits
         */
        updateClockBuffer();


        /*
         * Wait one second
         */
        HAL_Delay(1000);
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
     * Clock = 8 MHz
     *
     * Prescaler = 7999
     *
     * 8 MHz / 8000
     * = 1000 Hz
     *
     * Period = 9
     *
     * counter = 0 -> 9
     * = 10 counts
     *
     * 1000 / 10
     * = 100 Hz
     *
     * Timer period:
     *
     * 1/100
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


    if(HAL_TIM_ConfigClockSource(&htim2,
                                 &sClockSourceConfig) != HAL_OK)
    {
        Error_Handler();
    }


    sMasterConfig.MasterOutputTrigger =
        TIM_TRGO_RESET;


    sMasterConfig.MasterSlaveMode =
        TIM_MASTERSLAVEMODE_DISABLE;


    if(HAL_TIMEx_MasterConfigSynchronization(&htim2,
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
     * GPIOA initial state
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
     * GPIOB initial state
     *
     * PB0 -> PB6 = SEG0 -> SEG6
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
     * GPIOA Outputs
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


    HAL_GPIO_Init(GPIOA,
                  &GPIO_InitStruct);


    /*
     * GPIOB Outputs
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


    HAL_GPIO_Init(GPIOB,
                  &GPIO_InitStruct);
}


/* USER CODE BEGIN 4 */


/**
 * ============================================================
 * TIM2 CALLBACK
 * ============================================================
 *
 * Interrupt every 10 ms
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        /*
         * ======================================================
         * 7SEG scanning
         * ======================================================
         *
         * Exercise 4/5:
         *
         * 25 x 10 ms
         * = 250 ms/display
         *
         * Four displays:
         *
         * 4 x 250 ms
         * = 1 second
         *
         * Scanning frequency = 1 Hz
         */

        timer_counter++;


        if(timer_counter >= 25)
        {
            timer_counter = 0;


            /*
             * Show current digit from led_buffer
             */
            update7SEG(index_led);


            /*
             * Next digit
             */
            index_led++;


            /*
             * Keep valid range 0 -> 3
             */
            if(index_led >= MAX_LED)
            {
                index_led = 0;
            }
        }


        /*
         * ======================================================
         * DOT blinking
         * ======================================================
         */

        dot_counter++;


        /*
         * 100 x 10 ms = 1 second
         */
        if(dot_counter >= 100)
        {
            dot_counter = 0;


            HAL_GPIO_TogglePin(DOT_GPIO_Port,
                               DOT_Pin);
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
