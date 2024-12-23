/*
 * ascii table
 *
 * File Name:   ascii.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.22
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __ASCII_H
#define __ASCII_H

/*!< The includes */

/*!< The defines */
#define CHAR_ASC_NUL                        0       /*!< NULL */
#define CHAR_ASC_SOH                        1       /*!< Start Of Headling: Ctrl + A */
#define CHAR_ASC_STX                        2       /*!< Start Of Text: Ctrl + B */
#define CHAR_ASC_ETX                        3       /*!< End Of Text: Ctrl + C */
#define CHAR_ASC_EOT                        4       /*!< End Of Transmission: Ctrl + D */
#define CHAR_ASC_ENQ                        5       /*!< Enquiry: Ctrl + E */
#define CHAR_ASC_ACK                        6       /*!< Acknowledge: Ctrl + F */
#define CHAR_ASC_BEL                        7       /*!< Bell: Ctrl + G */
#define CHAR_ASC_BS                         8       /*!< Backspace: Ctrl + H */
#define CHAR_ASC_HT                         9       /*!< Horizontal Tab: Ctrl + I */
#define CHAR_ASC_LF                         10      /*!< Line Feed/New Line: Ctrl + J */
#define CHAR_ASC_VT                         11      /*!< Vertical Tab: Ctrl + K */
#define CHAR_ASC_NP                         12      /*!< Form Feed/New Page: Ctrl + L */
#define CHAR_ASC_CR                         13      /*!< Carriage Return: Ctrl + M */
#define CHAR_ASC_SO                         14      /*!< Shift Out: Ctrl + N */
#define CHAR_ASC_SI                         15      /*!< Shift In: Ctrl + O */
#define CHAR_ASC_DLE                        16      /*!< Data Link Escape: Ctrl + P */
#define CHAR_ASC_DC1_XON                    17      /*!< Device Control 1/Transmission On: Ctrl + Q */
#define CHAR_ASC_DC2                        18      /*!< Device Control 2: Ctrl + R */
#define CHAR_ASC_DC3_XOFF                   19      /*!< Device Control 3/Transmission Off: Ctrl + S */
#define CHAR_ASC_DC4                        20      /*!< Device Control 4: Ctrl + T */
#define CHAR_ASC_NAK                        21      /*!< Negative Acknowledge: Ctrl + U */
#define CHAR_ASC_SYN                        22      /*!< Synchronous Idle: Ctrl + V */
#define CHAR_ASC_ETB                        23      /*!< End of Transmission Block: Ctrl + W */
#define CHAR_ASC_CAN                        24      /*!< Cancel: Ctrl + X */
#define CHAR_ASC_EM                         25      /*!< End of Medium: Ctrl + Y */
#define CHAR_ASC_SUB                        26      /*!< Substitute: Ctrl + Z */
#define CHAR_ASC_ESC                        27      /*!< Escape */
#define CHAR_ASC_FS                         28      /*!< File Separator */
#define CHAR_ASC_GS                         29      /*!< Group Separator */
#define CHAR_ASC_RS                         30      /*!< Record Separator */
#define CHAR_ASC_US                         31      /*!< Unit Separator */

#define CHAR_ASC_SPACE                      32      /*!< ' ' */
#define CHAR_ASC_EXC                        33      /*!< '!' */
#define CHAR_ASC_DQUO                       34      /*!< '"' */
#define CHAR_ASC_HASH                       35      /*!< '#' */
#define CHAR_ASC_DOLLAR                     36      /*!< '$' */
#define CHAR_ASC_PERCENT                    37      /*!< '%' */
#define CHAR_ASC_AND                        38      /*!< '&' */
#define CHAR_ASC_SQUO                       39      /*!< ''' */
#define CHAR_ASC_LBRACKET                   40      /*!< '(' */
#define CHAR_ASC_RBRACKET                   41      /*!< ')' */
#define CHAR_ASC_STAR                       42      /*!< '*' */
#define CHAR_ASC_INC                        43      /*!< '+' */
#define CHAR_ASC_COMMA                      44      /*!< ',' */
#define CHAR_ASC_DEC                        45      /*!< '-' */
#define CHAR_ASC_DOT                        46      /*!< '.' */
#define CHAR_ASC_SLASH                      47      /*!< '/' */

/*!< 0 ~ 9: 48 ~ 57 */

#define CHAR_ASC_COLON                      58      /*!< ':' */
#define CHAR_ASC_SEMICOLON                  59      /*!< ';' */
#define CHAR_ASC_LT                         60      /*!< '<' */
#define CHAR_ASC_EQUAL                      61      /*!< '=' */
#define CHAR_ASC_GT                         62      /*!< '>' */
#define CHAR_ASC_QUERY                      63      /*!< '?' */
#define CHAR_ASC_AT                         64      /*!< '@' */

/*!< A ~ Z: 65 ~ 90 */

#define CHAR_ASC_LBRACE                     91      /*!< '[' */
#define CHAR_ASC_BSLASH                     92      /*!< '\' */
#define CHAR_ASC_RBRACE                     93      /*!< ']' */
#define CHAR_ASC_XOR                        94      /*!< '^' */
#define CHAR_ASC_UNDERSCORE                 95      /*!< '_' */
#define CHAR_ASC_BACKTICK                   96      /*!< '`' */

/*!< a ~ b: 97 ~ 122 */

#define CHAR_ASC_LCURLY                     123     /*!< '{' */
#define CHAR_ASC_PIPE                       124     /*!< '|' */
#define CHAR_ASC_RCURLY                     125     /*!< '}' */
#define CHAR_ASC_TILDE                      126     /*!< '~' */
#define CHAR_ASC_DEL                        127     /*!< Delete */

/*!< The functions */


#endif /* __ASCII_H */
