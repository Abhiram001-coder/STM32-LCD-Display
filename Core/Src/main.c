//PINOUT---------------------------------------

//SDA of PCF8574t IC----> PA10 or D2 PIN of stm32
//SCL of PCF8574t IC----> PA9 or D8 PIN of stm32
//Vcc of PCF8574t IC----> 3v3 of stm32
//gnd of PCF8574t IC----> gnd of stm32
//CODE APPROACH-------------------------------------------
//Steps to proceed the implementation of the code
//Step 1: Change the slave address according to the I2C slave device you are using
//Step 2: STM32 I2C1 module HAL driver Initialization {"static void MX_I2C1_Init(void)" locate to the function definition and change the MCU parameters for I2C1 peripheral according to your application}
//Step 3: PCF8574t Driver Initialization
//Step 4: Make sure you need to perform read operation or write operation, as we are interfacing LCD with STM32 we prefer write operation, we use "Master_Transfer_Config" function of STM32 inside our user defined function "stm_i2c_write1"
//        Routines or arguments that are used in the STM32_Master_transfer_Config is commented above the function call
//Step 5: If you want to display the string for a specific duration of time use delay or if you want to make the string sustain on the display without getting off use while(1)
//
#include "main.h"
//Header file for driver
#include "PCF8574T_LCD_Driver.h"

//In order to access the variables of the STM32_HAL_DRIVER for the peripheral I2C
I2C_HandleTypeDef hi2c1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
void stm_delay(uint16_t delay_ms);
//User defined functions for writing strings at LCD via I2C BUS
void stm_i2c_write1(uint8_t addr, uint8_t *pData, uint8_t send_data_len1);
void stm_i2c_write2(uint8_t addr, uint8_t *pData, uint8_t send_data_len2);
//Step 1: slave address i.e. pcf8574t IC's
uint8_t addr_ = 0x27;
uint8_t send_data_len1;
uint8_t send_data_len2;


int main(void)
{
	char outString1[] = "Hello Gotham!!!";
	char outString2[]= "I'm Batman";
	send_data_len1=sizeof(outString1);
	send_data_len2=sizeof(outString2);


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  // For configuring the system clock
  SystemClock_Config();

  // Initialize all configured peripherals
  MX_GPIO_Init();
  //Step 2: Initialize the HAL_DRIVER for I2C1 peripheral (Which initializes all the parameters such as clock speed, duty cycle, addressing mode of MCU maybe 7 bit or 10 bit addressing)
  MX_I2C1_Init();

  //Step 3: Function call to the pcf8674t driver for initialization --> Follow 3rd argument stm_i2c_write for step 4
  PCF8574T_Init(0x27,stm_delay, stm_i2c_write1);
  PCF8574T_Init(0x27,stm_delay, stm_i2c_write2);

 //After initializing the slave address and making it to operate in 8-bit mode and configuring it in write operation
  while (1)
  {
     /** Step 5: Displaying the string
      * first argument indicates the data we need to pass
      * second argument indicates the line we are going to print on the LCD display
      */
	  PCF8574T_displayString(outString1,1);
	  HAL_Delay(1000);
	  PCF8574T_displayString(outString2,2);
	  HAL_Delay(1000);


  }

}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);


  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_I2C1_Init(void)
{


  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x00303D5B;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }


  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }


  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }


}


static void MX_GPIO_Init(void)
{

  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();


}

/**
 * Step 4: 1st argument as slave address
 *         2nd argument as pointer which points the data buffer that we use to send the data
 *         3rd argument length of data
 */
void stm_i2c_write1(uint8_t addr, uint8_t *pData, uint8_t send_data_len1)
{
	/**
	 * @brief  Transmits in master mode an amount of data in blocking mode.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @param  DevAddress Target device address: The device 7 bits address value
  *         in datasheet must be shifted to the left before calling the interface
  * @param  pData Pointer to data buffer
  * @param  Size Amount of data to be sent
  * @param  Timeout Timeout duration
  * @retval HAL status
	 */
    HAL_I2C_Master_Transmit(&hi2c1, addr<<1,pData, send_data_len1, 1000);
}
void stm_i2c_write2(uint8_t addr, uint8_t *pData, uint8_t send_data_len2)
{
    HAL_I2C_Master_Transmit(&hi2c1, addr<<1,pData, send_data_len2, 1000);
}
void stm_delay(uint16_t delay_ms)
{
    HAL_Delay(delay_ms);
}


void Error_Handler(void)
{

  __disable_irq();
  while (1)
  {
  }

}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{

}
#endif
