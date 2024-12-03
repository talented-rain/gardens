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
#include <common/io_stream.h>

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

/*!
 * @brief   convert string to number
 * @param   str
 * @retval  number
 * @note    such as: "160"/"0xA0"/"0xa0"/"0b10100000" ===> 160
 */
kutype_t ascii_to_dec(const kchar_t *str)
{
    kchar_t *p;
    kutype_t val = 0;
    kchar_t type = -1;

    for (p = (kchar_t *)str; p && (*p != '\0'); p++) 
	{
        if (p == str) 
		{
            if ((*(p) == '0') && 
                ((*(p + 1) == 'X') || (*(p + 1) == 'x')))
                type = 16;
            else if ((*(p) == '0') && 
                ((*(p + 1) == 'B') || (*(p + 1) == 'b')))
                type = 2;
            else
                type = 10;

            if (type != 10)
                p += 2;
        }

        switch (type) 
		{
            case 2:
                if ((*p != '0') && (*p != '1'))
                    goto fail;

                val = (val << 1) + (*p) - '0';
                break;

            case 10:
                if ((*p < '0') || (*p > '9'))
                    goto fail;

                val = (val << 1) + (val << 3) + (*p) - '0';
                break;

            case 16:
                if ((*p >= '0') && (*p <= '9'))
                    val = (val << 4) + (*p) - '0';
                else if ((*p >= 'a') && (*p <= 'z'))
                    val = (val << 4) + (*p) - 'a' + 10;
                else if ((*p >= 'A') && (*p <= 'Z'))
                    val = (val << 4) + (*p) - 'A' + 10;
                else
                    goto fail;
                break;

            default: goto fail;
        }
    }

    return val;

fail:
    print_err("%s: input argument error!\n", __FUNCTION__);
    return 0;
}

/* end of file */
