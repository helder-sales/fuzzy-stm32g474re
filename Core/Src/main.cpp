/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Fuzzy.h"
#include "stdio.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Fuzzy *fuzzy = new Fuzzy();

// FuzzyInput 1
FuzzySet *lowMag = new FuzzySet(0, 0, 2482, 2792);
FuzzySet *mediumMag = new FuzzySet(2482, 3102, 3475, 3723);
FuzzySet *highMag = new FuzzySet(3475, 3909, 4095, 4095);

// FuzzyInput 2
FuzzySet *lowGain = new FuzzySet(0, 0, 175, 300);
FuzzySet *mediumGain = new FuzzySet(200, 400, 600, 767);
FuzzySet *highGain = new FuzzySet(650, 850, 1023, 1023);

// FuzzyOutput 1
FuzzySet *speedCtrlLow = new FuzzySet(0, 0, 11000, 19000);
FuzzySet *speedCtrlMedium = new FuzzySet(12850, 26000, 38500, 49000);
FuzzySet *speedCtrlHigh = new FuzzySet(42000, 55000, 65535, 65535);

// FuzzyOutput 2
FuzzySet *gainCtrlLow = new FuzzySet(0, 0, 175, 300);
FuzzySet *gainCtrlMedium = new FuzzySet(200, 400, 600, 767);
FuzzySet *gainCtrlHigh = new FuzzySet(650, 765, 1023, 1023);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void Fuzzy_Init(void);
void Fuzzy_GetPertinence(void);
void Fuzzy_WhichRuleWasFired(void);
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
	Fuzzy_Init();
	/* USER CODE END 1 */

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
	MX_LPUART1_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\033[2J\033[0;0H", sizeof "\033[2J\033[0;0H", 1000);

	// Uncomment for debug
	//	volatile uint32_t cycles1 = 0;
	//	volatile uint32_t cycles2 = 0;
	//	volatile float totalTime1 = 0;
	//	volatile float totalTime2 = 0;
	//	char bufTotalTime[16] = {0};
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		uint8_t bufMagnitude[16] = {0};
		uint8_t bufGain[16] = {0};
		uint8_t i = 0;

		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rInsert magnitude: ", sizeof "\n\n\rInsert magnitude: ", 1000);

		do
			HAL_UART_Receive(&hlpuart1, &bufMagnitude[i], 1, 0xFFFFFFFF);
		while(bufMagnitude[i++] != '\r' && i < 16);

		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "Insert gain: ", sizeof "\n\rInsert gain: ", 1000);

		i = 0;

		do
			HAL_UART_Receive(&hlpuart1, &bufGain[i], 1, 0xFFFFFFFF);
		while(bufGain[i++] != '\r' && i < 16);

		// Uncomment for debug and comment above block until while(1)
		uint8_t bufMagnitude[16] = "4095";
		uint8_t bufGain[16] = "1023";

		uint32_t input1 = strtoul((const char *) bufMagnitude, NULL, 10);
		uint32_t input2 = strtoul((const char *) bufGain, NULL, 10);

		fuzzy->setInput(1, input1);
		fuzzy->setInput(2, input2);

		// Uncomment for debug
		//		ITM->PORT[0].u32 = 0;
		//		cycles1 = DWT->CYCCNT;

		fuzzy->fuzzify();

		// Uncomment for debug
		//		cycles2 = DWT->CYCCNT;
		//		ITM->PORT[0].u32 = 1;
		//		totalTime1 = (cycles2 - cycles1) / 170e6f;
		//		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufTotalTime,
		//				snprintf(bufTotalTime, sizeof bufTotalTime, "%.3e\n\r", totalTime1), 1000);
		//		cycles1 = DWT->CYCCNT;
		//		ITM->PORT[0].u32 = 2;

		float output1 = fuzzy->defuzzify(1);
		float output2 = fuzzy->defuzzify(2);

		// Uncomment for debug
		//		cycles2 = DWT->CYCCNT;
		//		ITM->PORT[0].u32 = 3;
		//		totalTime2 = (cycles2 - cycles1) / 170e6f;
		//		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufTotalTime,
		//				snprintf(bufTotalTime, sizeof bufTotalTime, "%.3e\n\r", totalTime2), 1000);
		//		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufTotalTime,
		//				snprintf(bufTotalTime, sizeof bufTotalTime, "%.3e\n\r", totalTime1 + totalTime2), 1000);
		//		asm volatile("nop");

		char bufSpeedCtrl[16] = {0};
		char bufGainCtrl[16] = {0};

		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\rSpeed Ctrl is: ", sizeof "\n\n\rSpeed Ctrl is: ", 1000);
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufSpeedCtrl,
				snprintf(bufSpeedCtrl, sizeof bufSpeedCtrl, "%.2f", output1), 1000);
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufSpeedCtrl,
				snprintf(bufSpeedCtrl, sizeof bufSpeedCtrl, " (%.1f%%)", output1 * 0.0015259f), 1000);

		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\rGain Ctrl is: ", sizeof "\n\rGain Ctrl is: ", 1000);
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufGainCtrl,
				snprintf(bufGainCtrl, sizeof bufGainCtrl, "%.2f", output2), 1000);
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufGainCtrl,
				snprintf(bufGainCtrl, sizeof bufGainCtrl, " (%.1f%%)", output2 * 0.09776f), 1000);

		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSummary: ", sizeof "\n\rSummary: ", 1000);

		Fuzzy_GetPertinence();
		Fuzzy_WhichRuleWasFired();
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
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
	RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV6;
	RCC_OscInitStruct.PLL.PLLN = 85;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
	{
		Error_Handler();
	}
	/** Initializes the peripherals clocks
	 */
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_LPUART1;
	PeriphClkInit.Lpuart1ClockSelection = RCC_LPUART1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
		Error_Handler();
	}
	/** Enables the Clock Security System
	 */
	HAL_RCC_EnableCSS();
}

/* USER CODE BEGIN 4 */
void Fuzzy_Init(void)
{
	// FuzzyInput 1
	FuzzyInput *magnitude = new FuzzyInput(1);

	magnitude->addFuzzySet(lowMag);
	magnitude->addFuzzySet(mediumMag);
	magnitude->addFuzzySet(highMag);
	fuzzy->addFuzzyInput(magnitude);

	// FuzzyInput 2
	FuzzyInput *gain = new FuzzyInput(2);

	gain->addFuzzySet(lowGain);
	gain->addFuzzySet(mediumGain);
	gain->addFuzzySet(highGain);
	fuzzy->addFuzzyInput(gain);

	// FuzzyOutput 1
	FuzzyOutput *speedCtrl = new FuzzyOutput(1);

	speedCtrl->addFuzzySet(speedCtrlLow);
	speedCtrl->addFuzzySet(speedCtrlMedium);
	speedCtrl->addFuzzySet(speedCtrlHigh);
	fuzzy->addFuzzyOutput(speedCtrl);

	// FuzzyOutput 2
	FuzzyOutput *gainCtrl = new FuzzyOutput(2);

	gainCtrl->addFuzzySet(gainCtrlLow);
	gainCtrl->addFuzzySet(gainCtrlMedium);
	gainCtrl->addFuzzySet(gainCtrlHigh);
	fuzzy->addFuzzyOutput(gainCtrl);

	// Building FuzzyRule 1
	FuzzyRuleAntecedent *if_LowMag_AND_LowGain = new FuzzyRuleAntecedent();
	if_LowMag_AND_LowGain->joinWithAND(lowMag, lowGain);

	FuzzyRuleConsequent *then_SpeedCtrlIsLow_AND_GainCtrlIsMedium = new FuzzyRuleConsequent();
	then_SpeedCtrlIsLow_AND_GainCtrlIsMedium->addOutput(speedCtrlLow);
	then_SpeedCtrlIsLow_AND_GainCtrlIsMedium->addOutput(gainCtrlMedium);

	FuzzyRule *fuzzyRule1 = new FuzzyRule(1, if_LowMag_AND_LowGain, then_SpeedCtrlIsLow_AND_GainCtrlIsMedium);
	fuzzy->addFuzzyRule(fuzzyRule1);

	// Building FuzzyRule 2
	FuzzyRuleAntecedent *if_LowMag_AND_MediumGain = new FuzzyRuleAntecedent();
	if_LowMag_AND_MediumGain->joinWithAND(lowMag, mediumGain);

	FuzzyRuleConsequent *then_SpeedCtrlIsLow_AND_GainCtrlIsHigh = new FuzzyRuleConsequent();
	then_SpeedCtrlIsLow_AND_GainCtrlIsHigh->addOutput(speedCtrlLow);
	then_SpeedCtrlIsLow_AND_GainCtrlIsHigh->addOutput(gainCtrlHigh);

	FuzzyRule *fuzzyRule2 = new FuzzyRule(2, if_LowMag_AND_MediumGain, then_SpeedCtrlIsLow_AND_GainCtrlIsHigh);
	fuzzy->addFuzzyRule(fuzzyRule2);

	// Building FuzzyRule 3
	FuzzyRuleAntecedent *if_LowMag_AND_HighGain = new FuzzyRuleAntecedent();
	if_LowMag_AND_HighGain->joinWithAND(lowMag, highGain);

	FuzzyRule *fuzzyRule3 = new FuzzyRule(3, if_LowMag_AND_HighGain, then_SpeedCtrlIsLow_AND_GainCtrlIsHigh);
	fuzzy->addFuzzyRule(fuzzyRule3);

	// Building FuzzyRule 4
	FuzzyRuleAntecedent *if_MediumMag_AND_LowGain = new FuzzyRuleAntecedent();
	if_MediumMag_AND_LowGain->joinWithAND(mediumMag, lowGain);

	FuzzyRuleConsequent *then_SpeedCtrlIsMedium_AND_GainCtrlIsMedium = new FuzzyRuleConsequent();
	then_SpeedCtrlIsMedium_AND_GainCtrlIsMedium->addOutput(speedCtrlMedium);
	then_SpeedCtrlIsMedium_AND_GainCtrlIsMedium->addOutput(gainCtrlMedium);

	FuzzyRule *fuzzyRule4 = new FuzzyRule(4, if_MediumMag_AND_LowGain, then_SpeedCtrlIsMedium_AND_GainCtrlIsMedium);
	fuzzy->addFuzzyRule(fuzzyRule4);

	// Building FuzzyRule 5
	FuzzyRuleAntecedent *if_MediumMag_AND_MediumGain = new FuzzyRuleAntecedent();
	if_MediumMag_AND_MediumGain->joinWithAND(mediumMag, mediumGain);

	FuzzyRuleConsequent *then_SpeedCtrlIsMedium_AND_GainCtrlIsHigh = new FuzzyRuleConsequent();
	then_SpeedCtrlIsMedium_AND_GainCtrlIsHigh->addOutput(speedCtrlMedium);
	then_SpeedCtrlIsMedium_AND_GainCtrlIsHigh->addOutput(gainCtrlHigh);

	FuzzyRule *fuzzyRule5 = new FuzzyRule(5, if_MediumMag_AND_MediumGain, then_SpeedCtrlIsMedium_AND_GainCtrlIsHigh);
	fuzzy->addFuzzyRule(fuzzyRule5);

	// Building FuzzyRule 6
	FuzzyRuleAntecedent *if_MediumMag_AND_HighGain = new FuzzyRuleAntecedent();
	if_MediumMag_AND_HighGain->joinWithAND(mediumMag, highGain);

	FuzzyRule *fuzzyRule6 = new FuzzyRule(6, if_MediumMag_AND_HighGain, then_SpeedCtrlIsLow_AND_GainCtrlIsHigh);
	fuzzy->addFuzzyRule(fuzzyRule6);

	// Building FuzzyRule 7
	FuzzyRuleAntecedent *if_HighMag_AND_LowGain = new FuzzyRuleAntecedent();
	if_HighMag_AND_LowGain->joinWithAND(highMag, lowGain);

	FuzzyRuleConsequent *then_SpeedCtrlIsHigh_AND_GainCtrlIsLow = new FuzzyRuleConsequent();
	then_SpeedCtrlIsHigh_AND_GainCtrlIsLow->addOutput(speedCtrlHigh);
	then_SpeedCtrlIsHigh_AND_GainCtrlIsLow->addOutput(gainCtrlLow);

	FuzzyRule *fuzzyRule7 = new FuzzyRule(7, if_HighMag_AND_LowGain, then_SpeedCtrlIsHigh_AND_GainCtrlIsLow);
	fuzzy->addFuzzyRule(fuzzyRule7);

	// Building FuzzyRule 8
	FuzzyRuleAntecedent *if_HighMag_AND_MediumGain = new FuzzyRuleAntecedent();
	if_HighMag_AND_MediumGain->joinWithAND(highMag, mediumGain);

	FuzzyRule *fuzzyRule8 = new FuzzyRule(8, if_HighMag_AND_MediumGain, then_SpeedCtrlIsHigh_AND_GainCtrlIsLow);
	fuzzy->addFuzzyRule(fuzzyRule8);

	// Building FuzzyRule 9
	FuzzyRuleAntecedent *if_HighMag_AND_HighGain = new FuzzyRuleAntecedent();
	if_HighMag_AND_HighGain->joinWithAND(highMag, highGain);

	FuzzyRuleConsequent *then_SpeedCtrlIsHigh_AND_GainCtrlIsMedium = new FuzzyRuleConsequent();
	then_SpeedCtrlIsHigh_AND_GainCtrlIsMedium->addOutput(speedCtrlHigh);
	then_SpeedCtrlIsHigh_AND_GainCtrlIsMedium->addOutput(gainCtrlMedium);

	FuzzyRule *fuzzyRule9 = new FuzzyRule(9, if_HighMag_AND_HighGain, then_SpeedCtrlIsHigh_AND_GainCtrlIsMedium);
	fuzzy->addFuzzyRule(fuzzyRule9);
}

void Fuzzy_GetPertinence(void)
{
	char bufPertinence[16] = {0};

	float pertinenceOfhighMag = highMag->getPertinence();
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rhighMag Pertinence: ", sizeof "\n\n\rhighMag Pertinence: ", 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufPertinence,
			snprintf(bufPertinence, sizeof bufPertinence, "%.4f", pertinenceOfhighMag), 1000);

	float pertinenceOfmediumMag = mediumMag->getPertinence();
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\rmediumMag Pertinence: ", sizeof "\n\rmediumMag Pertinence: ", 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufPertinence,
			snprintf(bufPertinence, sizeof bufPertinence, "%.4f", pertinenceOfmediumMag), 1000);

	float pertinenceOflowMag = lowMag->getPertinence();
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\rlowMag Pertinence: ", sizeof "\n\rlowMag Pertinence: ", 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufPertinence,
			snprintf(bufPertinence, sizeof bufPertinence, "%.4f", pertinenceOflowMag), 1000);

	float pertinenceOfhighGain = highGain->getPertinence();
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rhighGain Pertinence: ", sizeof "\n\n\rhighGain Pertinence: ", 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufPertinence,
			snprintf(bufPertinence, sizeof bufPertinence, "%.4f", pertinenceOfhighGain), 1000);

	float pertinenceOfmediumGain = mediumGain->getPertinence();
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\rmediumGain Pertinence: ", sizeof "\n\rmediumGain Pertinence: ", 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufPertinence,
			snprintf(bufPertinence, sizeof bufPertinence, "%.4f", pertinenceOfmediumGain), 1000);

	float pertinenceOflowGain = lowGain->getPertinence();
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\rlowGain Pertinence: ", sizeof "\n\rlowGain Pertinence: ", 1000);
	HAL_UART_Transmit(&hlpuart1, (uint8_t *) bufPertinence,
			snprintf(bufPertinence, sizeof bufPertinence, "%.4f", pertinenceOflowGain), 1000);
}

void Fuzzy_WhichRuleWasFired(void)
{
	uint8_t i = 0;

	while(!fuzzy->isFiredRule(++i));

	// Adapt to ASCII numbers
	i += 48;

	HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rRule Fired: ", sizeof "\n\n\rRule Fired: ", 1000);
	HAL_UART_Transmit(&hlpuart1, &i, 1, 1000);

	switch(i)
	{
	case '1':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsLow_AND_GainCtrlIsMedium",
				sizeof "\n\n\rSpeedCtrlIsLow_AND_GainCtrlIsMedium", 1000);
		break;

	case '2':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsLow_AND_GainCtrlIsHigh",
				sizeof "\n\n\rSpeedCtrlIsLow_AND_GainCtrlIsHigh", 1000);
		break;

	case '3':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsLow_AND_GainCtrlIsHigh",
				sizeof "\n\n\rSpeedCtrlIsLow_AND_GainCtrlIsHigh", 1000);
		break;

	case '4':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsMedium_AND_GainCtrlIsMedium",
				sizeof "\n\n\rSpeedCtrlIsMedium_AND_GainCtrlIsMedium", 1000);
		break;

	case '5':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsMedium_AND_GainCtrlIsHigh",
				sizeof "\n\n\rSpeedCtrlIsMedium_AND_GainCtrlIsHigh", 1000);
		break;

	case '6':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsMedium_AND_GainCtrlIsHigh",
				sizeof "\n\n\rSpeedCtrlIsMedium_AND_GainCtrlIsHigh", 1000);
		break;

	case '7':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsHigh_AND_GainCtrlIsLow",
				sizeof "\n\n\rSpeedCtrlIsHigh_AND_GainCtrlIsLow", 1000);
		break;

	case '8':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsHigh_AND_GainCtrlIsLow",
				sizeof "\n\n\rSpeedCtrlIsHigh_AND_GainCtrlIsLow", 1000);
		break;

	case '9':
		HAL_UART_Transmit(&hlpuart1, (uint8_t *) "\n\n\rSpeedCtrlIsHigh_AND_GainCtrlIsMedium",
				sizeof "\n\n\rSpeedCtrlIsHigh_AND_GainCtrlIsMedium", 1000);
		break;

	default:
		Error_Handler();
	}
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
	//  __disable_irq();
	while (1)
	{
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		HAL_Delay(500);
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
