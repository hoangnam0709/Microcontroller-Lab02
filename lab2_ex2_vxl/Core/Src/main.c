/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Exercise 2 - Timer Interrupt and LED Scanning
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/*
 * TIM2 interrupt = 10 ms
 *
 * timer_counter:
 * 50 x 10 ms = 500 ms
 *
 * dot_counter:
 * 100 x 10 ms = 1000 ms = 1 second
 */
volatile int timer_counter = 0;
volatile int dot_counter = 0;

/*
 * 0 -> first 7SEG
 * 1 -> second 7SEG
 * 2 -> third 7SEG
 * 3 -> fourth 7SEG
 */
volatile int led_index = 0;

/* USER CODE END PV */


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);

/* USER CODE BEGIN PFP */

void display7SEG(int num);
void disableAll7SEG(void);

/* USER CODE END PFP */


/* USER CODE BEGIN 0 */

/*
 * Disable all four seven-segment displays.
 *
 * Because Q1-Q4 are PNP:
 *
 * EN = HIGH -> transistor OFF
 * EN = LOW  -> transistor ON
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
 * Display a number on the shared segment bus.
 *
 * Mapping:
 *
 * SEG0 / PB0 -> a
 * SEG1 / PB1 -> b
 * SEG2 / PB2 -> c
 * SEG3 / PB3 -> d
 * SEG4 / PB4 -> e
 * SEG5 / PB5 -> f
 * SEG6 / PB6 -> g
 *
 * The seven-segment displays are COMMON ANODE.
 *
 * GPIO LOW  -> segment ON
 * GPIO HIGH -> segment OFF
 */
void display7SEG(int num)
{
    /*
     * First turn OFF all seven segments.
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
         *  a
         * ---
         * f   b
         * |   |
         *
         * e   c
         * |   |
         * ---
         *  d
         *
         * ON = a,b,c,d,e,f
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
         * ON = b,c
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
         * ON = a,b,d,e,g
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
         * ON = a,b,c,d,g
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


        default:

            /*
             * Invalid number:
             * all segments remain OFF.
             */
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
     * Disable all 7-segment displays first.
     */
    disableAll7SEG();


    /*
     * Turn OFF all segments initially.
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
     *
     * In Proteus:
     *
     * +3.3V
     *   |
     * D2 / D3
     *   |
     * DOT
     *   |
     * PA4
     *
     * PA4 HIGH -> LEDs OFF
     * PA4 LOW  -> LEDs ON
     */
    HAL_GPIO_WritePin(DOT_GPIO_Port,
                      DOT_Pin,
                      GPIO_PIN_SET);


    /*
     * Immediately show the first digit = 1.
     *
     * This avoids waiting 500 ms after startup.
     */
    display7SEG(1);

    HAL_GPIO_WritePin(EN0_GPIO_Port,
                      EN0_Pin,
                      GPIO_PIN_RESET);


    /*
     * The next display after 500 ms will be display #2.
     */
    led_index = 1;

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
        /*
         * Exercise 2 performs the requested processing
         * in the timer interrupt callback.
         */
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


    RCC_OscInitStruct.OscillatorType =
        RCC_OSCILLATORTYPE_HSI;

    RCC_OscInitStruct.HSIState =
        RCC_HSI_ON;

    RCC_OscInitStruct.HSICalibrationValue =
        RCC_HSICALIBRATION_DEFAULT;

    RCC_OscInitStruct.PLL.PLLState =
        RCC_PLL_NONE;


    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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


    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct,
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
     * Timer clock:
     *
     * 8 MHz / (7999 + 1)
     * = 1000 Hz
     *
     * Counter:
     *
     * 0 -> 9 = 10 counts
     *
     * 1000 Hz / 10
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
     * Initial GPIOA state.
     */
    HAL_GPIO_WritePin(GPIOA,
                      DOT_Pin |
                      EN0_Pin |
                      EN1_Pin |
                      EN2_Pin |
                      EN3_Pin,
                      GPIO_PIN_RESET);


    /*
     * Initial GPIOB state.
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
     * PA4 -> DOT
     * PA6 -> EN0
     * PA7 -> EN1
     * PA8 -> EN2
     * PA9 -> EN3
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
     * PB0 -> SEG0
     * PB1 -> SEG1
     * PB2 -> SEG2
     * PB3 -> SEG3
     * PB4 -> SEG4
     * PB5 -> SEG5
     * PB6 -> SEG6
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
 * @brief Timer interrupt callback
 *
 * TIM2 calls this function every 10 ms.
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    /*
     * Make sure the interrupt came from TIM2.
     */
    if(htim->Instance == TIM2)
    {
        /*
         * ========================================================
         * PART 1:
         * SEVEN SEGMENT SCANNING
         * ========================================================
         */

        timer_counter++;


        /*
         * 50 interrupts x 10 ms
         * = 500 ms
         */
        if(timer_counter >= 50)
        {
            timer_counter = 0;


            /*
             * Disable all displays before changing
             * segment data.
             *
             * This also helps avoid ghosting.
             */
            disableAll7SEG();


            /*
             * Select one of four displays.
             */
            switch(led_index)
            {
                /*
                 * Display #1:
                 *
                 * show number 1
                 */
                case 0:

                    display7SEG(1);

                    HAL_GPIO_WritePin(
                        EN0_GPIO_Port,
                        EN0_Pin,
                        GPIO_PIN_RESET);

                    break;


                /*
                 * Display #2:
                 *
                 * show number 2
                 */
                case 1:

                    display7SEG(2);

                    HAL_GPIO_WritePin(
                        EN1_GPIO_Port,
                        EN1_Pin,
                        GPIO_PIN_RESET);

                    break;


                /*
                 * Display #3:
                 *
                 * show number 3
                 */
                case 2:

                    display7SEG(3);

                    HAL_GPIO_WritePin(
                        EN2_GPIO_Port,
                        EN2_Pin,
                        GPIO_PIN_RESET);

                    break;


                /*
                 * Display #4:
                 *
                 * show number 0
                 */
                case 3:

                    display7SEG(0);

                    HAL_GPIO_WritePin(
                        EN3_GPIO_Port,
                        EN3_Pin,
                        GPIO_PIN_RESET);

                    break;


                default:

                    led_index = 0;

                    break;
            }


            /*
             * Move to next display.
             */
            led_index++;


            /*
             * After display #4,
             * return to display #1.
             */
            if(led_index >= 4)
            {
                led_index = 0;
            }
        }


        /*
         * ========================================================
         * PART 2:
         * DOT BLINKING
         * ========================================================
         */

        dot_counter++;


        /*
         * 100 interrupts x 10 ms
         * = 1000 ms
         * = 1 second
         */
        if(dot_counter >= 100)
        {
            dot_counter = 0;


            /*
             * Toggle both DOT LEDs.
             *
             * D2 and D3 are connected in parallel
             * to the same DOT signal at PA4,
             * therefore they blink together.
             */
            HAL_GPIO_TogglePin(
                DOT_GPIO_Port,
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

    while (1)
    {
    }
}


#ifdef USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
    /*
     * User can add implementation here.
     */
}

#endif
