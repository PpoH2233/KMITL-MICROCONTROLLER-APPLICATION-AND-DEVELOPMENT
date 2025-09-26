/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "eth.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"
#include "stdio.h"
#include "string.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ILI9341_GFX.h"
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "image.h"
#include "conan.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

int MAX_X = ILI9341_SCREEN_WIDTH;
int MAX_Y = ILI9341_SCREEN_HEIGHT;

float sensor_value = 27.1;
char sensor_value_text[100];

int isUpdate = 1;
int isUpdateSecondPage = 1;

int red_value = 0;
int green_value = 0;
int blue_value = 0;
int color_red = RED;
int color_green = GREEN;
int color_blue = BLUE;

int count_time = 0;

int next_screen = 0;
uint16_t color_big_circle;

int increase_progress_bar_rate = 10;

uint8_t step = 0 ;
float h=30.0, t= 40.0 ;


char str[50];
uint8_t cmdBuffer[3];
uint8_t dataBuffer[8];
HAL_StatusTypeDef status;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
char* isDetectCircle(uint16_t pos_x, uint16_t pos_y);
uint16_t mixedColor(int red_value, int green_value, int blue_value);
int is_detect_picture(uint16_t pos_x, uint16_t pos_y);

uint16_t CRC16_2(uint8_t *ptr, uint8_t length);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */


  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_SPI5_Init();
  MX_TIM2_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
  ILI9341_Init();

  cmdBuffer[0]= 0x03;
  cmdBuffer[1]= 0x00;
  cmdBuffer[2]= 0x04;



  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  ILI9341_Fill_Screen(WHITE);
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (next_screen == 1){
		  second_screen();
		  isUpdate = 1;
	  }
	  else{
//		  HAL_TIM_Base_Stop_IT(&htim2);
		  main_screen();
		  isUpdateSecondPage = 1;
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void draw_sensor_value(){

	//Wake up sensor
	HAL_I2C_Master_Transmit(&hi2c1, 0x5c << 1 , cmdBuffer, 3,  200);
	//Send reading cmd
	HAL_I2C_Master_Transmit(&hi2c1, 0x5c << 1 , cmdBuffer, 3,  200);

	HAL_Delay(1);

	//Receive sensor data
	HAL_I2C_Master_Receive(&hi2c1, 0x5c << 1, dataBuffer, 8, 200);

	uint16_t Rcrc = dataBuffer[7] << 8;
	Rcrc += dataBuffer[6];

	if (Rcrc == CRC16_2(dataBuffer, 6)) {
		uint16_t temperature = ((dataBuffer[4] & 0x7F) << 8 ) + dataBuffer[5];
		t = temperature / 10.0;
		t = (((dataBuffer[4] & 0x80) >> 7 ) == 1) ? (t *(-1)) : t;

		uint16_t humidity = (dataBuffer[2] << 8 ) + dataBuffer[3];
		h = humidity / 10.0;
	}

	ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
	char hum[100];
	char temp[100];

//	*hum = "55.6%%RH";
//	*temp = "27.1C";

	sprintf(hum, "%.1f%%RH", h);
	sprintf(temp, "%.1fC", t);

	int font_size = 2;

	int text_pos_y = 38;
	int temp_pos_x = 22-4;
	int mois_pos_x = 160-4;

	ILI9341_Draw_Text(temp, temp_pos_x, text_pos_y, BLACK, font_size, WHITE);
	ILI9341_Draw_Text(hum, mois_pos_x, text_pos_y, BLACK, font_size, WHITE);

	ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
}
void second_screen(){

	if(isUpdateSecondPage == 1){
		ILI9341_Draw_Image((const char*)image_lcd, SCREEN_HORIZONTAL_1);
		draw_text_character_info();

		HAL_TIM_Base_Start_IT(&htim2);

		isUpdateSecondPage = 0;
	}

	if(count_time == 5){
		ILI9341_Fill_Screen(WHITE);
		ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
		next_screen = 0;
		count_time = 0;

		HAL_TIM_Base_Stop_IT(&htim2);
	}

//	ILI9341_Draw_Filled_Circle(8, 50, 3, RED);
//	ILI9341_Draw_Filled_Circle(121, 50, 3, RED);
//	ILI9341_Draw_Filled_Circle(8, 190, 3, RED);
//	ILI9341_Draw_Filled_Circle(121, 190, 3, RED);

	HAL_Delay(20);
	if(TP_Touchpad_Pressed()){
	  uint16_t x_pos = 0 ;
	  uint16_t y_pos = 0 ;

	  uint16_t pos_array[2];

	  if(TP_Read_Coordinates(pos_array) == TOUCHPAD_DATA_OK){
		  x_pos = pos_array[0];
		  y_pos = pos_array[1];

//		  back to first screen
		  if(is_detect_picture(x_pos, y_pos) == 1){
			  ILI9341_Fill_Screen(WHITE);
			  ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
			  next_screen = 0;
			  count_time = 0;

			  HAL_TIM_Base_Stop_IT(&htim2);
		  }

	  }
	}


}

int is_detect_picture(uint16_t pos_x, uint16_t pos_y ){
	int picture_x1 = 8;
	int picture_x2 = 121;
	int picture_y1 = 50;
	int picture_y2 = 190;

//	ILI9341_Draw_Filled_Circle(picture_x1, picture_y1, 2, RED);
//	ILI9341_Draw_Filled_Circle(picture_x1, picture_y2, 2, RED);
//	ILI9341_Draw_Filled_Circle(picture_x2, picture_y1, 2, RED);
//	ILI9341_Draw_Filled_Circle(picture_x2, picture_y2, 2, RED);

	uint8_t in_x = (pos_x > picture_x1) && (pos_x < picture_x2);
	uint8_t in_y = (pos_y > picture_y1) && (pos_y < picture_y2);

	if(in_x && in_y){
		return 1;
	}
	return 0;
}

void draw_text_character_info(){
	char group[]      = "Group No.41";
	char first_name[] = "   Jimmy  ";
	char last_name[]  = "   Kudo   ";
	char student_id[] = " 66010572 ";

	int div_text_x = 130;
	int shift_y = 32;
	int div_text_start_y = 62;
	int font_size = 2;
	int text_color = color_big_circle;
	int backgorund_color = WHITE;

	int group_pos_x = div_text_x;
	int group_pos_y = div_text_start_y;

	int first_pos_x = div_text_x;
	int first_pos_y = div_text_start_y + shift_y;

	int last_pos_x = div_text_x;
	int last_pos_y = first_pos_y + shift_y;

	int id_pos_x = div_text_x;
	int id_pos_y = last_pos_y + shift_y;



//	ILI9341_Draw_Filled_Circle(20, 80, 10, RED);

	ILI9341_Draw_Text(group, group_pos_x, group_pos_y, text_color , font_size, backgorund_color);
	ILI9341_Draw_Text(first_name, first_pos_x, first_pos_y, text_color , font_size, backgorund_color);
	ILI9341_Draw_Text(last_name, last_pos_x, last_pos_y, text_color , font_size, backgorund_color);
	ILI9341_Draw_Text(student_id, id_pos_x, id_pos_y, text_color , font_size, backgorund_color);


}

void main_screen(){

	if (isUpdate == 1){
		  ILI9341_Draw_Image((const char*)conan, SCREEN_HORIZONTAL_1);
		  ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
		  draw_base_progress_bar();
		  draw_div_circle();
		  isUpdate = 0;
		  color_big_circle = mixedColor(red_value, green_value, blue_value);
	  }

	  draw_big_circle();
	  draw_sensor_value();

	  HAL_Delay(20);
	  if(TP_Touchpad_Pressed()){
		  uint16_t x_pos = 0 ;
		  uint16_t y_pos = 0 ;

		  uint16_t pos_array[2];

		  if(TP_Read_Coordinates(pos_array) == TOUCHPAD_DATA_OK){
			  x_pos = pos_array[0];
			  y_pos = pos_array[1];

			  char *circle_detected = isDetectCircle(x_pos, y_pos);

			  if(circle_detected == "RED"){
				  if (red_value == 100){
					  red_value = 0;
					  draw_base_progress_bar();
//					  draw_div_circle();
				  }else{
					  red_value += increase_progress_bar_rate;
				  }

			  }
			  else if(circle_detected == "GREEN"){
				  if(green_value == 100){
					  green_value = 0;
					  draw_base_progress_bar();
//					  draw_div_circle();
				  }else{
					  green_value += increase_progress_bar_rate;
				  }

			  }
			  else if(circle_detected == "BLUE"){
				  if (blue_value == 100){
					  blue_value = 0;
					  draw_base_progress_bar();
//					  draw_div_circle();
				  }else{
					  blue_value += increase_progress_bar_rate;
				  }
			  }
			  else if(circle_detected == "BIG"){
				  next_screen = 1;
			  }
			  else if(circle_detected == "NONE"){

			  }
		  }
		  color_big_circle = mixedColor(red_value, green_value, blue_value);
	  }

	  draw_value_prog_bar(red_value, green_value, blue_value);

}


char* isDetectCircle(uint16_t pos_x, uint16_t pos_y){
	int red_circle_pos_x = 130;
	int green_circle_pos_x = 80;
	int blue_circle_pos_x = 30;

	int circle_pos_y = 40;

	int circle_radius = 20;

	uint8_t in_red_circle_x = pos_x > red_circle_pos_x - circle_radius &&  pos_x < red_circle_pos_x + circle_radius;
	uint8_t in_red_circle_y =  pos_y > circle_pos_y - circle_radius && pos_y < circle_pos_y + circle_radius;

	uint8_t in_green_circle_x = pos_x > green_circle_pos_x - circle_radius &&  pos_x < green_circle_pos_x + circle_radius;
	uint8_t in_green_circle_y =  pos_y > circle_pos_y - circle_radius && pos_y < circle_pos_y + circle_radius;

	uint8_t in_blue_circle_x = pos_x > blue_circle_pos_x - circle_radius &&  pos_x < blue_circle_pos_x + circle_radius;
	uint8_t in_blue_circle_y =  pos_y > circle_pos_y - circle_radius && pos_y < circle_pos_y + circle_radius;

	int big_circle_pos_x = 196;
	int big_circle_pos_y = 125;
	int big_radius = 25;

	uint8_t in_big_circle_x = pos_x > big_circle_pos_x - big_radius &&  pos_x < big_circle_pos_x + big_radius;
	uint8_t in_big_circle_y =  pos_y > big_circle_pos_y - big_radius && pos_y < big_circle_pos_y + big_radius;

	if (in_red_circle_x && in_red_circle_y){
		return "RED";
	}
	else if(in_green_circle_x && in_green_circle_y){
		return "GREEN";
	}
	else if (in_blue_circle_x && in_blue_circle_y){
		return "BLUE";
	}
	else if(in_big_circle_x && in_big_circle_y){
		return "BIG";
	}
	else{
		return"NONE";
	}
}

void draw_div_circle(void){
	  int div_circle_position_y = 46;
	  int circle_red_position_x = 130;
	  int circle_green_position_x = 80;
	  int circle_blue_position_x = 30;
	  int div_circle_radius = 20;

	  int shift_circle_x = 10;

	  ILI9341_Draw_Filled_Circle(circle_red_position_x + shift_circle_x, div_circle_position_y, div_circle_radius, RED);
	  ILI9341_Draw_Filled_Circle(circle_green_position_x +shift_circle_x,  div_circle_position_y, div_circle_radius, GREEN);
	  ILI9341_Draw_Filled_Circle(circle_blue_position_x +shift_circle_x,  div_circle_position_y, div_circle_radius, BLUE);
}

void draw_big_circle(void){
	int big_circle_position_x = 196;
	int big_circle_position_y = 116;
	int big_radius = 25;
	ILI9341_Draw_Filled_Circle(big_circle_position_x , big_circle_position_y, big_radius, color_big_circle);
}

void draw_base_progress_bar(void){
	int div_prog_bar_position_y = 70;
	int div_prog_bar_position_x = 153;

	int shift_x = 50;
	int prog_bar_base_red_x = div_prog_bar_position_x;
	int prog_bar_base_green_x = div_prog_bar_position_x - shift_x;
	int prog_bar_base_blue_x = prog_bar_base_green_x - shift_x;

	int w_size = 130;
	int h_size = 28;

	const uint16_t prog_bar_base_color_red = 62870;
	const uint16_t prog_bar_base_color_green = 47030;
	const uint16_t prog_bar_base_color_blue = 46590;

	ILI9341_Draw_Filled_Rectangle_Coord(prog_bar_base_red_x, div_prog_bar_position_y, prog_bar_base_red_x -h_size , div_prog_bar_position_y + w_size, prog_bar_base_color_red );
	ILI9341_Draw_Filled_Rectangle_Coord(prog_bar_base_green_x, div_prog_bar_position_y, prog_bar_base_green_x -h_size , div_prog_bar_position_y + w_size, prog_bar_base_color_green );
	ILI9341_Draw_Filled_Rectangle_Coord(prog_bar_base_blue_x, div_prog_bar_position_y, prog_bar_base_blue_x -h_size , div_prog_bar_position_y + w_size, prog_bar_base_color_blue );
}

uint16_t mixedColor(int red_value, int green_value, int blue_value){
	int r = (red_value * 31) / 100;
	int g = (green_value * 63) / 100;
	int b = (blue_value * 31) /100;

	return (r << 11) | (g << 5) | b;
}


uint16_t CRC16_2(uint8_t *ptr, uint8_t length ){
	uint16_t crc = 0xFFFF;
	uint8_t s = 0x00;

	while(length--){
		crc ^= *ptr++;
		for (s=0;s < 8; s++){
			if((crc & 0x01) != 0){
				crc >>= 1;
				crc ^= 0xA001;
			}
			else{
				crc >>=1 ;
			}
		}
	}

	return crc;
}
void draw_value_prog_bar(int red_value, int green_value, int blue_value){
// fill prog bar
	int w_prog_bar_size = 130;
	int h_prog_bar_size = 28;

	int div_prog_bar_position_y = 70;
	int div_prog_bar_position_x = 153;

	int shift_x = 50;
	int prog_bar_base_red_x = div_prog_bar_position_x;
	int prog_bar_base_green_x = prog_bar_base_red_x - shift_x;
	int prog_bar_base_blue_x = prog_bar_base_green_x - shift_x;

	int w_size = 130;
	int h_size = 28;

	int w_size_red = (red_value/100.0)*130;
	int w_size_green = (green_value/100.0)*130;
	int w_size_blue = (blue_value/100.0)*130;

	const uint16_t prog_bar_base_color_red = RED;
	const uint16_t prog_bar_base_color_green = GREEN;
	const uint16_t prog_bar_base_color_blue = BLUE;

	ILI9341_Draw_Filled_Rectangle_Coord(prog_bar_base_red_x, div_prog_bar_position_y, prog_bar_base_red_x -h_size , div_prog_bar_position_y + w_size_red, prog_bar_base_color_red );
	ILI9341_Draw_Filled_Rectangle_Coord(prog_bar_base_green_x, div_prog_bar_position_y, prog_bar_base_green_x -h_size , div_prog_bar_position_y + w_size_green, prog_bar_base_color_green );
	ILI9341_Draw_Filled_Rectangle_Coord(prog_bar_base_blue_x, div_prog_bar_position_y, prog_bar_base_blue_x -h_size , div_prog_bar_position_y + w_size_blue, prog_bar_base_color_blue );


//  print text
	char buf_red[20];
	char buf_green[20];
	char buf_blue[20];

	sprintf(buf_red, "%d%%  ", red_value);
	sprintf(buf_green, "%d%%  ", green_value);
	sprintf(buf_blue, "%d%%  ", blue_value);

	int text_red_position_y = 91;
	int text_green_position_y = text_red_position_y + 50 ;
	int text_blue_position_y = text_green_position_y + 50;

	int size_font = 2;
	int div_position_x = 205;
	ILI9341_Set_Rotation(SCREEN_HORIZONTAL_1);
	ILI9341_Draw_Text(buf_red, div_position_x , text_red_position_y , BLACK , size_font, WHITE);
	ILI9341_Draw_Text(buf_green, div_position_x , text_green_position_y , BLACK , size_font, WHITE);
	ILI9341_Draw_Text(buf_blue, div_position_x , text_blue_position_y , BLACK , size_font, WHITE);
	ILI9341_Set_Rotation(SCREEN_VERTICAL_1);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
