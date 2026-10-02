/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 3 - update7SEG and LED Buffer
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* USER CODE BEGIN PV */

/*
 * Exercise 3
 */
const int MAX_LED = 4;

/*
 * Current 7SEG index:
 * 0 -> first
 * 1 -> second
 * 2 -> third
 * 3 -> fourth
 */
int index_led = 0;

/*
 * Data displayed on four seven-segment LEDs.
 *
 * LED1 -> 1
 * LED2 -> 2
 * LED3 -> 3
 * LED4 -> 4
 *
 * Change these values to unit test update7SEG().
 */
int led_buffer[4] = {1, 2, 3, 4};


/*
 * TIM2 interrupt = 10 ms
 *
 * 50 x 10 ms = 500 ms
 */
volatile int timer_counter = 0;

/*
 * DOT:
 *
 * 100 x 10 ms = 1000 ms
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

/* USER CODE END PFP */


/* USER CODE BEGIN 0 */


/*
 * ============================================================
 * Disable all four 7SEG displays
 * ============================================================
 *
 * PNP transistor:
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
 * Display number on segment bus
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
 * LOW  -> segment ON
 * HIGH -> segment OFF
 */
void display7SEG(int num)
{
    /*
     * Turn OFF all segments first.
     */
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
        /*
         * Number 0
         *
         * a b c d e f ON
         */
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


        /*
         * Number 1
         *
         * b c ON
         */
        case 1:

            HAL_GPIO_WritePin(GPIOB,
                              SEG1_Pin |
                              SEG2_Pin,
                              GPIO_PIN_RESET);

            break;


        /*
         * Number 2
         *
         * a b d e g ON
         */
        case 2:

            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG3_Pin |
                              SEG4_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);

            break;


        /*
         * Number 3
         *
         * a b c d g ON
         */
        case 3:

            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);

            break;


        /*
         * Number 4
         *
         * b c f g ON
         */
        case 4:

            HAL_GPIO_WritePin(GPIOB,
                              SEG1_Pin |
                              SEG2_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);

            break;


        /*
         * Number 5
         *
         * a c d f g ON
         */
        case 5:

            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG2_Pin |
                              SEG3_Pin |
                              SEG5_Pin |
                              SEG6_Pin,
                              GPIO_PIN_RESET);

            break;


        /*
         * Number 6
         *
         * a c d e f g ON
         */
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


        /*
         * Number 7
         *
         * a b c ON
         */
        case 7:

            HAL_GPIO_WritePin(GPIOB,
                              SEG0_Pin |
                              SEG1_Pin |
                              SEG2_Pin,
                              GPIO_PIN_RESET);

            break;


        /*
         * Number 8
         *
         * all segments ON
         */
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


        /*
         * Number 9
         *
         * a b c d f g ON
         */
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

            /*
             * Invalid value:
             * all segments remain OFF
             */
            break;
    }
}


/*
 * ============================================================
 * Exercise 3:
 * update7SEG(int index)
 * ============================================================
 *
 * index = 0 -> first LED
 * index = 1 -> second LED
 * index = 2 -> third LED
 * index = 3 -> fourth LED
 *
 * The number displayed comes from led_buffer[].
 */
void update7SEG(int index)
{
    /*
     * Turn OFF all displays first.
     *
     * Prevent ghosting when segment data changes.
     */
    disableAll7SEG();


    switch(index)
    {
        /*
         * First seven-segment
         */
        case 0:

            display7SEG(led_buffer[0]);

            HAL_GPIO_WritePin(
                EN0_GPIO_Port,
                EN0_Pin,
                GPIO_PIN_RESET
            );

            break;


        /*
         * Second seven-segment
         */
        case 1:

            display7SEG(led_buffer[1]);

            HAL_GPIO_WritePin(
                EN1_GPIO_Port,
                EN1_Pin,
                GPIO_PIN_RESET
            );

            break;


        /*
         * Third seven-segment
         */
        case 2:

            display7SEG(led_buffer[2]);

            HAL_GPIO_WritePin(
                EN2_GPIO_Port,
                EN2_Pin,
                GPIO_PIN_RESET
            );

            break;


        /*
         * Fourth seven-segment
         */
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

/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* MCU Configuration------------------------------------------------------*/

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

    /*
     * Turn OFF all displays.
     */
    disableAll7SEG();


    /*
     * Turn OFF all segments.
     */
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
     * Show first LED immediately.
     *
     * led_buffer[0] = 1
     */
    update7SEG(index_led);


    /*
     * Next interrupt scanning operation
     * will use display #2.
     */
    index_led++;

    if(index_led >= MAX_LED)
    {
        index_led = 0;
    }


    /*
     * Reset software counters.
     */
    timer_counter = 0;
    dot_counter = 0;


    /*
     * Start TIM2 interrupt.
     */
    HAL_TIM_Base_Start_IT(&htim2);


    /* USER CODE END 2 */


    /* Infinite loop ---------------------------------------------------------*/
    while (1)
    {

    }
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
     * System clock = 8 MHz
     *
     * Prescaler:
     *
     * 8 MHz / (7999 + 1)
     * = 1000 Hz
     *
     * Period:
     *
     * Counter 0 -> 9
     * = 10 counts
     *
     * 1000 Hz / 10
     * = 100 Hz
     *
     * Interrupt:
     *
     * 1 / 100Hz
     * = 10ms
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
     * GPIOA initial output state
     */
    HAL_GPIO_WritePin(GPIOA,
                      DOT_Pin |
                      EN0_Pin |
                      EN1_Pin |
                      EN2_Pin |
                      EN3_Pin,
                      GPIO_PIN_RESET);


    /*
     * GPIOB initial output state
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
     * GPIOA
     *
     * PA4 = DOT
     * PA6 = EN0
     * PA7 = EN1
     * PA8 = EN2
     * PA9 = EN3
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
     * GPIOB
     *
     * PB0 = SEG0
     * PB1 = SEG1
     * PB2 = SEG2
     * PB3 = SEG3
     * PB4 = SEG4
     * PB5 = SEG5
     * PB6 = SEG6
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
         * ======================================================
         * 7SEG scanning
         * ======================================================
         */

        timer_counter++;


        /*
         * 50 x 10ms = 500ms
         */
        if(timer_counter >= 50)
        {
            timer_counter = 0;


            /*
             * Exercise 3:
             *
             * update current 7SEG using led_buffer[].
             */
            update7SEG(index_led);


            /*
             * Move to next LED.
             */
            index_led++;


            /*
             * index_led must stay:
             *
             * 0, 1, 2, 3
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
         * 100 x 10ms
         * = 1000ms
         * = 1 second
         */
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
