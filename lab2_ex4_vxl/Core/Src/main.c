/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 4 - 1Hz Seven Segment Scanning
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;


/* USER CODE BEGIN PV */

/*
 * Number of seven-segment displays
 */
const int MAX_LED = 4;


/*
 * Current display index
 *
 * 0 -> first 7SEG
 * 1 -> second 7SEG
 * 2 -> third 7SEG
 * 3 -> fourth 7SEG
 */
int index_led = 0;


/*
 * Data displayed on the four seven-segment displays
 *
 * LED1 -> 1
 * LED2 -> 2
 * LED3 -> 3
 * LED4 -> 4
 */
int led_buffer[4] = {1, 2, 3, 4};


/*
 * TIM2 interrupt = 10 ms
 *
 * Exercise 4:
 *
 * 4 displays must complete one scan in 1 second.
 *
 * 1 second / 4 = 250 ms per display
 *
 * 250 ms / 10 ms = 25 interrupts
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

/* USER CODE END PFP */


/* USER CODE BEGIN 0 */


/*
 * ============================================================
 * Disable all four seven-segment displays
 * ============================================================
 *
 * PNP transistor:
 *
 * EN = HIGH -> OFF
 * EN = LOW  -> ON
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
 * Display number on seven-segment bus
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
     * Turn OFF all segments first
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
         * ON = a b c d e f
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
         * ON = b c
         */
        case 1:

            HAL_GPIO_WritePin(GPIOB,
                              SEG1_Pin |
                              SEG2_Pin,
                              GPIO_PIN_RESET);
            break;


        /*
         * Number 2
         * ON = a b d e g
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
         * ON = a b c d g
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
         * ON = b c f g
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
         * ON = a c d f g
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
         * ON = a c d e f g
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
         * ON = a b c
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
         * ON = all segments
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
         * ON = a b c d f g
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
             * keep all segments OFF
             */
            break;
    }
}


/*
 * ============================================================
 * update7SEG()
 * ============================================================
 *
 * index 0 -> first 7SEG  -> led_buffer[0]
 * index 1 -> second 7SEG -> led_buffer[1]
 * index 2 -> third 7SEG  -> led_buffer[2]
 * index 3 -> fourth 7SEG -> led_buffer[3]
 */
void update7SEG(int index)
{
    /*
     * Disable all displays before changing segment data.
     */
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

/* USER CODE END 0 */


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /*
     * MCU Configuration
     */

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
     * Turn OFF all seven-segment displays
     */
    disableAll7SEG();


    /*
     * Turn OFF all segments
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
     * DOT initially OFF
     */
    HAL_GPIO_WritePin(DOT_GPIO_Port,
                      DOT_Pin,
                      GPIO_PIN_SET);


    /*
     * Display first seven-segment immediately
     */
    update7SEG(index_led);


    /*
     * Next display will be display #2
     */
    index_led++;

    if(index_led >= MAX_LED)
    {
        index_led = 0;
    }


    /*
     * Reset counters
     */
    timer_counter = 0;
    dot_counter = 0;


    /*
     * Start TIM2 interrupt
     */
    HAL_TIM_Base_Start_IT(&htim2);


    /* USER CODE END 2 */


    /*
     * Infinite loop
     */
    while(1)
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
     * 8 MHz / (7999 + 1)
     * = 1000 Hz
     *
     * Period = 9
     *
     * Counter:
     * 0 -> 9
     * = 10 counts
     *
     * 1000 Hz / 10
     * = 100 Hz
     *
     * Timer interrupt:
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


    /*
     * GPIO Ports Clock Enable
     */
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
     * GPIOA:
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


    HAL_GPIO_Init(GPIOA,
                  &GPIO_InitStruct);


    /*
     * GPIOB:
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


    HAL_GPIO_Init(GPIOB,
                  &GPIO_InitStruct);
}


/* USER CODE BEGIN 4 */


/**
 * ============================================================
 * Exercise 4
 * TIM2 interrupt callback
 * ============================================================
 *
 * Timer interrupt = 10 ms
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        /*
         * ======================================================
         * 7SEG SCANNING
         * ======================================================
         *
         * Desired scanning frequency = 1 Hz
         *
         * One complete scan:
         *
         * LED1 -> LED2 -> LED3 -> LED4
         *
         * must take:
         *
         * T = 1 second
         *
         * Therefore each display:
         *
         * 1000 ms / 4
         * = 250 ms
         *
         * TIM2 interrupt = 10 ms
         *
         * 250 / 10
         * = 25 interrupts
         */

        timer_counter++;


        /*
         * Exercise 4:
         *
         * 25 x 10 ms = 250 ms
         */
        if(timer_counter >= 25)
        {
            timer_counter = 0;


            /*
             * Display current 7SEG
             */
            update7SEG(index_led);


            /*
             * Go to next display
             */
            index_led++;


            /*
             * Keep index in range:
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
         * DOT BLINKING
         * ======================================================
         *
         * DOT still changes state every 1 second
         */

        dot_counter++;


        /*
         * 100 x 10 ms
         * = 1000 ms
         * = 1 second
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
