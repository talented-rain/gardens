/*
 * General Function
 *
 * File Name:   generic.c
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.05.18
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

/*!< The includes */
#include <common/generic.h>

/*!< API function */
/*!
 * @brief   unsigned divied: "divied / div"
 * @param   divied, div
 * @retval  none
 * @note    none
 */
kutype_t udiv_integer(kutype_t divied, kutype_t div)
{
	kutype_t count = 0;

	while (divied >= div)
	{
		divied -= div;
		count++;
	}

	return count;
}

/*!
 * @brief   signed divied: "divied / div"
 * @param   divied, div
 * @retval  none
 * @note    none
 */
kstype_t sdiv_integer(kstype_t divied, kstype_t div)
{
	kstype_t count = 0;
	kstype_t number1 = mrt_abs(divied);
	kstype_t number2 = mrt_abs(div);

	while (number1 >= number2)
	{
		number1 -= number2;
		count++;
	}

	return ((divied ^ div) < 0) ? -count : count;
}

/*!
 * @brief   get the remainder: "divied % div"
 * @param   divied, div
 * @retval  none
 * @note    none
 */
kutype_t udiv_remainder(kutype_t divied, kutype_t div)
{
	while (divied >= div)
		divied -= div;

	return divied;
}

/*!
 * @brief   convert number to hex, and save the result to buf
 * @param   buf, number
 * @retval  none
 * @note    none
 */
kutype_t dec_to_hex(kchar_t *buf, kutype_t number, kbool_t mode)
{
	kchar_t temp[(sizeof(kutype_t) << 1) + 4];
	kchar_t result = 0;
	kint16_t count = 0, idx;

	do
	{
		result = number - ((number >> 4) << 4);
		number = number >> 4;

		if (result >= 10)
			temp[count] = result - 10 + (mode ? 'A' : 'a');
		else
			temp[count] = result + '0';

		count++;
		
	} while (number);

	if (!buf)
		goto END;
	
	*buf = '0';
	*(buf + 1) = mode ? 'X' : 'x';

	for (idx = 0; idx < count; idx++)
		*(buf + idx + 2) = temp[count - idx - 1];
	
END:
	return (count + 2);
}

/*!
 * @brief   convert number to binary, and save the result to buf
 * @param   buf, number
 * @retval  none
 * @note    none
 */
kutype_t dec_to_binary(kchar_t *buf, kutype_t number)
{
	kchar_t temp[(sizeof(kutype_t) << 3) + 4];
	kchar_t result = 0;
	kint16_t count = 0, idx;

	do
	{
		result = number - ((number >> 1) << 1);
		number = number >> 1;

		temp[count++] = result + '0';
		
	} while (number);

	if (!buf)
		goto END;
	
	*buf = '0';
	*(buf + 1) = 'b';

	for (idx = 0; idx < count; idx++)
		*(buf + idx + 2) = temp[count - idx - 1];
	
END:
	return (count + 2);
}

/* end of file */
