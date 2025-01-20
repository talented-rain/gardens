/*
 * keyboard table
 *
 * File Name:   keyboard.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2024.12.22
 *
 * Copyright (c) 2024   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#ifdef __cplusplus
    extern "C" {
#endif

/*!< The includes */
#include <common/ascii.h>

/*!< The defines */
#define KBD_KEY_NUL                         CHAR_ASC_NUL        /*!< NULL */
#define KBD_KEY_SCROLL_LOCK                 CHAR_ASC_ETX        /*!< End Of Text */
#define KBD_KEY_BS                          CHAR_ASC_BS         /*!< Backspace */
#define KBD_KEY_TAB                         CHAR_ASC_HT         /*!< Horizontal Tab */
#define KBD_KEY_CLEAR                       CHAR_ASC_NP         /*!< Form Feed/New Page */
#define KBD_KEY_ENTER                       CHAR_ASC_CR         /*!< Carriage Return */
#define KBD_KEY_SHIFT                       CHAR_ASC_DLE        /*!< Data Link Escape */
#define KBD_KEY_CTRL                        CHAR_ASC_DC1_XON    /*!< Device Control 1/Transmission On */
#define KBD_KEY_ALT                         CHAR_ASC_DC2        /*!< Device Control 2 */
#define KBD_KEY_CAPSLK                      CHAR_ASC_DC4        /*!< Device Control 4 */
#define KBD_KEY_ESC                         CHAR_ASC_ESC        /*!< Escape */

#define KBD_KEY_SPACE                       CHAR_ASC_SPACE      /*!< space */
#define KBD_KEY_PAGEUP                      CHAR_ASC_EXC        /*!< page up */
#define KBD_KEY_PAGEDOWN                    CHAR_ASC_DQUO       /*!< page down */
#define KBD_KEY_END                         CHAR_ASC_HASH       /*!< End */
#define KBD_KEY_HOME                        CHAR_ASC_DOLLAR     /*!< Home */
#define KBD_KEY_LEFT                        CHAR_ASC_PERCENT    /*!< to left */
#define KBD_KEY_UP                          CHAR_ASC_PERCENT    /*!< to up */
#define KBD_KEY_RIGHT                       CHAR_ASC_SQUO       /*!< to right */
#define KBD_KEY_DOWN                        CHAR_ASC_LBRACKET   /*!< to down */
#define KBD_KEY_INSERT                      CHAR_ASC_DEC        /*!< insert */
#define KBD_KEY_DELETE                      CHAR_ASC_DOT        /*!< delete */

#define KBD_KEY_0                           48
#define KBD_KEY_1                           49
#define KBD_KEY_2                           50
#define KBD_KEY_3                           51
#define KBD_KEY_4                           52
#define KBD_KEY_5                           53
#define KBD_KEY_6                           54
#define KBD_KEY_7                           55
#define KBD_KEY_8                           56
#define KBD_KEY_9                           57

#define KBD_KEY_A                           65
#define KBD_KEY_B                           66
#define KBD_KEY_C                           67
#define KBD_KEY_D                           68
#define KBD_KEY_E                           69
#define KBD_KEY_F                           70
#define KBD_KEY_G                           71
#define KBD_KEY_H                           72
#define KBD_KEY_I                           73
#define KBD_KEY_J                           74
#define KBD_KEY_K                           75
#define KBD_KEY_L                           76
#define KBD_KEY_M                           77
#define KBD_KEY_N                           78
#define KBD_KEY_O                           79
#define KBD_KEY_P                           80
#define KBD_KEY_Q                           81
#define KBD_KEY_R                           82
#define KBD_KEY_S                           83
#define KBD_KEY_T                           84
#define KBD_KEY_U                           85
#define KBD_KEY_V                           86
#define KBD_KEY_W                           87
#define KBD_KEY_X                           88
#define KBD_KEY_Y                           89
#define KBD_KEY_Z                           90

/*!< Number Keyboard */
#define KBD_KEY_NUM_0                       96
#define KBD_KEY_NUM_1                       97
#define KBD_KEY_NUM_2                       98
#define KBD_KEY_NUM_3                       99
#define KBD_KEY_NUM_4                       100
#define KBD_KEY_NUM_5                       101
#define KBD_KEY_NUM_6                       102
#define KBD_KEY_NUM_7                       103
#define KBD_KEY_NUM_8                       104
#define KBD_KEY_NUM_9                       105
#define KBD_KEY_NUM_MUL                     106
#define KBD_KEY_NUM_INC                     107
#define KBD_KEY_NUM_ENTER                   108
#define KBD_KEY_NUM_DEC                     109
#define KBD_KEY_NUM_DOT                     110
#define KBD_KEY_NUM_DEL                     KBD_KEY_NUM_DOT
#define KBD_KEY_NUM_DIV                     111

#define KBD_KEY_F1                          112
#define KBD_KEY_F2                          113
#define KBD_KEY_F3                          114
#define KBD_KEY_F4                          115
#define KBD_KEY_F5                          116
#define KBD_KEY_F6                          117
#define KBD_KEY_F7                          118
#define KBD_KEY_F8                          119
#define KBD_KEY_F9                          120
#define KBD_KEY_F10                         121
#define KBD_KEY_F11                         122
#define KBD_KEY_F12                         123
/*!< F13 ~ F24: 124 ~ 135 */

#define KBD_KEY_NUM_LOCK                    144
#define KBD_KEY_RSHIFT                      161

/*!< shortcut */
#define KBD_KEY_SEARCH                      170
#define KBD_KEY_FAVORITE                    171

#define KBD_KEY_SND_OFF                     173                 /*!< sound off */
#define KBD_KEY_SND_WEAKEN                  174                 /*!< sound- */
#define KBD_KEY_SND_ENHANCE                 175                 /*!< sound+ */

#define KBD_KEY_MAIL                        180

/*!< sign */
#define KBD_KEY_COLON                       186                 /*!< ";", ":" */
#define KBD_KEY_SEMICOLON                   KBD_KEY_COLON
#define KBD_KEY_EQUAL                       187                 /*!< "=", "+" */
#define KBD_KEY_INC                         KBD_KEY_EQUAL
#define KBD_KEY_COMMA                       188                 /*!< ",", "<" */
#define KBD_KEY_LT                          KBD_KEY_COMMA
#define KBD_KEY_DEC                         189                 /*!< "-", "——" */
#define KBD_KEY_DASH                        KBD_KEY_DEC
#define KBD_KEY_DOT                         190                 /*!< ".", ">" */
#define KBD_KEY_GT                          KBD_KEY_DOT
#define KBD_KEY_HASH                        191                 /*!< "/", "?" */
#define KBD_KEY_QUERY                       KBD_KEY_HASH
#define KBD_KEY_BACKTICK                    192                 /*!< "`", "~" */
#define KBD_KEY_TILDE                       KBD_KEY_BACKTICK

#define KBD_KEY_LBRACE                      219                 /*!< "[", "{" */
#define KBD_KEY_LCURLY                      KBD_KEY_LBRACE
#define KBD_KEY_BSLASH                      220                 /*!< "\", "|" */
#define KBD_KEY_PIPE                        KBD_KEY_BSLASH
#define KBD_KEY_RBRACE                      221                 /*!< "]", "}" */
#define KBD_KEY_RCURLY                      KBD_KEY_RBRACE
#define KBD_KEY_SQUO                        222                 /*!< "'", """ */
#define KBD_KEY_DQUO                        KBD_KEY_SQUO

/*!< SHIFT */
#define KBD_KEY_SHIFT_A                     8257                /*!< 'a' */
#define KBD_KEY_SHIFT_B                     8258
#define KBD_KEY_SHIFT_C                     8259
#define KBD_KEY_SHIFT_D                     8260
#define KBD_KEY_SHIFT_E                     8261
#define KBD_KEY_SHIFT_F                     8262
#define KBD_KEY_SHIFT_G                     8263
#define KBD_KEY_SHIFT_H                     8264
#define KBD_KEY_SHIFT_I                     8265
#define KBD_KEY_SHIFT_J                     8266
#define KBD_KEY_SHIFT_K                     8267
#define KBD_KEY_SHIFT_L                     8268
#define KBD_KEY_SHIFT_M                     8269
#define KBD_KEY_SHIFT_N                     8270
#define KBD_KEY_SHIFT_O                     8271
#define KBD_KEY_SHIFT_P                     8272
#define KBD_KEY_SHIFT_Q                     8273
#define KBD_KEY_SHIFT_R                     8274
#define KBD_KEY_SHIFT_S                     8275
#define KBD_KEY_SHIFT_T                     8276
#define KBD_KEY_SHIFT_U                     8277
#define KBD_KEY_SHIFT_V                     8278
#define KBD_KEY_SHIFT_W                     8279
#define KBD_KEY_SHIFT_X                     8280
#define KBD_KEY_SHIFT_Y                     8281
#define KBD_KEY_SHIFT_Z                     8282

#define KBD_KEY_SHIFT_HASH                  8303                /*!< shift + "/" */
#define KBD_KEY_SHIFT_COLON                 8378                /*!< shift + ";" */
#define KBD_KEY_SHIFT_INC                   8379                /*!< shift + "+" */
#define KBD_KEY_SHIFT_COMMA                 8380                /*!< shift + "," */
#define KBD_KEY_SHIFT_DEC                   8381                /*!< shift + "-" */
#define KBD_KEY_SHIFT_DOT                   8382                /*!< shift + "." */
#define KBD_KEY_SHIFT_BACKTICK              8384                /*!< shift + "`" */
#define KBD_KEY_SHIFT_LBRACE                8411                /*!< shift + "[" */
#define KBD_KEY_SHIFT_BSLASH                8412                /*!< shift + "\" */
#define KBD_KEY_SHIFT_RBRACE                8413                /*!< shift + "]" */
#define KBD_KEY_SHIFT_SQUO                  8414                /*!< shift + "'" */

/*!< CTRL */
#define KBD_KEY_CTRL_A                      16449
#define KBD_KEY_CTRL_B                      16450
#define KBD_KEY_CTRL_C                      16451
#define KBD_KEY_CTRL_D                      16452
#define KBD_KEY_CTRL_E                      16453
#define KBD_KEY_CTRL_F                      16454
#define KBD_KEY_CTRL_G                      16455
#define KBD_KEY_CTRL_H                      16456
#define KBD_KEY_CTRL_I                      16457
#define KBD_KEY_CTRL_J                      16458
#define KBD_KEY_CTRL_K                      16459
#define KBD_KEY_CTRL_L                      16460
#define KBD_KEY_CTRL_M                      16461
#define KBD_KEY_CTRL_N                      16462
#define KBD_KEY_CTRL_O                      16463
#define KBD_KEY_CTRL_P                      16464
#define KBD_KEY_CTRL_Q                      16465
#define KBD_KEY_CTRL_R                      16466
#define KBD_KEY_CTRL_S                      16467
#define KBD_KEY_CTRL_T                      16468
#define KBD_KEY_CTRL_U                      16469
#define KBD_KEY_CTRL_V                      16470
#define KBD_KEY_CTRL_W                      16471
#define KBD_KEY_CTRL_X                      16472
#define KBD_KEY_CTRL_Y                      16473
#define KBD_KEY_CTRL_Z                      16474

#define KBD_KEY_CTRL_HASH                   16495               /*!< ctrl + "/" */
#define KBD_KEY_CTRL_F1                     16496
#define KBD_KEY_CTRL_F2                     16497
#define KBD_KEY_CTRL_F3                     16498
#define KBD_KEY_CTRL_F4                     16499
#define KBD_KEY_CTRL_F5                     16500
#define KBD_KEY_CTRL_F6                     16501
#define KBD_KEY_CTRL_F7                     16502
#define KBD_KEY_CTRL_F8                     16503
#define KBD_KEY_CTRL_F9                     16504
#define KBD_KEY_CTRL_F10                    16505
#define KBD_KEY_CTRL_F11                    16506
#define KBD_KEY_CTRL_F12                    16507

#define KBD_KEY_CTRL_RSHIFT                 16545               /*!< ctrl + shift */

#define KBD_KEY_CTRL_COLON                  16570               /*!< ctrl + ";" */
#define KBD_KEY_CTRL_INC                    16571               /*!< ctrl + "+" */
#define KBD_KEY_CTRL_COMMA                  16572               /*!< ctrl + "," */
#define KBD_KEY_CTRL_DEC                    16573               /*!< ctrl + "-" */
#define KBD_KEY_CTRL_DOT                    16574               /*!< ctrl + "." */
#define KBD_KEY_CTRL_BACKTICK               16576               /*!< ctrl + "`" */
#define KBD_KEY_CTRL_LBRACE                 16603               /*!< ctrl + "[" */
#define KBD_KEY_CTRL_BSLASH                 16604               /*!< ctrl + "\" */
#define KBD_KEY_CTRL_RBRACE                 16605               /*!< ctrl + "]" */
#define KBD_KEY_CTRL_SQUO                   16606               /*!< ctrl + "'" */

/*!< SHIFT + CTRL */
#define KBD_KEY_SHIFT_CTRL_A                24641
#define KBD_KEY_SHIFT_CTRL_B                24642
#define KBD_KEY_SHIFT_CTRL_C                24643
#define KBD_KEY_SHIFT_CTRL_D                24644
#define KBD_KEY_SHIFT_CTRL_E                24645
#define KBD_KEY_SHIFT_CTRL_F                24646
#define KBD_KEY_SHIFT_CTRL_G                24647
#define KBD_KEY_SHIFT_CTRL_H                24648
#define KBD_KEY_SHIFT_CTRL_I                24649
#define KBD_KEY_SHIFT_CTRL_J                24650
#define KBD_KEY_SHIFT_CTRL_K                24651
#define KBD_KEY_SHIFT_CTRL_L                24652
#define KBD_KEY_SHIFT_CTRL_M                24653
#define KBD_KEY_SHIFT_CTRL_N                24654
#define KBD_KEY_SHIFT_CTRL_O                24655
#define KBD_KEY_SHIFT_CTRL_P                24656
#define KBD_KEY_SHIFT_CTRL_Q                24657
#define KBD_KEY_SHIFT_CTRL_R                24658
#define KBD_KEY_SHIFT_CTRL_S                24659
#define KBD_KEY_SHIFT_CTRL_T                24660
#define KBD_KEY_SHIFT_CTRL_U                24661
#define KBD_KEY_SHIFT_CTRL_V                24662
#define KBD_KEY_SHIFT_CTRL_W                24663
#define KBD_KEY_SHIFT_CTRL_X                24664
#define KBD_KEY_SHIFT_CTRL_Y                24665
#define KBD_KEY_SHIFT_CTRL_Z                24666

#define KBD_KEY_SHIFT_CTRL_HASH             24687               /*!< shift + ctrl + "/" */
#define KBD_KEY_SHIFT_CTRL_F1               24688
#define KBD_KEY_SHIFT_CTRL_F2               24689
#define KBD_KEY_SHIFT_CTRL_F3               24690
#define KBD_KEY_SHIFT_CTRL_F4               24691
#define KBD_KEY_SHIFT_CTRL_F5               24692
#define KBD_KEY_SHIFT_CTRL_F6               24693
#define KBD_KEY_SHIFT_CTRL_F7               24694
#define KBD_KEY_SHIFT_CTRL_F8               24695
#define KBD_KEY_SHIFT_CTRL_F9               24696
#define KBD_KEY_SHIFT_CTRL_F10              24697
#define KBD_KEY_SHIFT_CTRL_F11              24698
#define KBD_KEY_SHIFT_CTRL_F12              24699

#define KBD_KEY_SHIFT_CTRL_COLON            24762               /*!< shift + ctrl + ";" */
#define KBD_KEY_SHIFT_CTRL_INC              24763               /*!< shift + ctrl + "+" */
#define KBD_KEY_SHIFT_CTRL_COMMA            24764               /*!< shift + ctrl + "," */
#define KBD_KEY_SHIFT_CTRL_DEC              24765               /*!< shift + ctrl + "-" */
#define KBD_KEY_SHIFT_CTRL_DOT              24766               /*!< shift + ctrl + "." */
#define KBD_KEY_SHIFT_CTRL_BACKTICK         24768               /*!< shift + ctrl + "`" */
#define KBD_KEY_SHIFT_CTRL_LBRACE           24795               /*!< shift + ctrl + "[" */
#define KBD_KEY_SHIFT_CTRL_BSLASH           24796               /*!< shift + ctrl + "\" */
#define KBD_KEY_SHIFT_CTRL_RBRACE           24797               /*!< shift + ctrl + "]" */
#define KBD_KEY_SHIFT_CTRL_SQUO             24798               /*!< shift + ctrl + "'" */

/*!< ALT */
#define KBD_KEY_ALT_TAB                     32777
#define KBD_KEY_ALT_ENTER                   32781

#define KBD_KEY_ALT_A                       32833
#define KBD_KEY_ALT_B                       32834
#define KBD_KEY_ALT_C                       32835
#define KBD_KEY_ALT_D                       32836
#define KBD_KEY_ALT_E                       32837
#define KBD_KEY_ALT_F                       32838
#define KBD_KEY_ALT_G                       32839
#define KBD_KEY_ALT_H                       32840
#define KBD_KEY_ALT_I                       32841
#define KBD_KEY_ALT_J                       32842
#define KBD_KEY_ALT_K                       32843
#define KBD_KEY_ALT_L                       32844
#define KBD_KEY_ALT_M                       32845
#define KBD_KEY_ALT_N                       32846
#define KBD_KEY_ALT_O                       32847
#define KBD_KEY_ALT_P                       32848
#define KBD_KEY_ALT_Q                       32849
#define KBD_KEY_ALT_R                       32850
#define KBD_KEY_ALT_S                       32851
#define KBD_KEY_ALT_T                       32852
#define KBD_KEY_ALT_U                       32853
#define KBD_KEY_ALT_V                       32854
#define KBD_KEY_ALT_W                       32855
#define KBD_KEY_ALT_X                       32856
#define KBD_KEY_ALT_Y                       32857
#define KBD_KEY_ALT_Z                       32858

#define KBD_KEY_ALT_HASH                    32879               /*!< alt + "/" */
#define KBD_KEY_ALT_F1                      32880
#define KBD_KEY_ALT_F2                      32881
#define KBD_KEY_ALT_F3                      32882
#define KBD_KEY_ALT_F4                      32883
#define KBD_KEY_ALT_F5                      32884
#define KBD_KEY_ALT_F6                      32885
#define KBD_KEY_ALT_F7                      32886
#define KBD_KEY_ALT_F8                      32887
#define KBD_KEY_ALT_F9                      32888
#define KBD_KEY_ALT_F10                     32889
#define KBD_KEY_ALT_F11                     32890
#define KBD_KEY_ALT_F12                     32891

#define KBD_KEY_ALT_RSHIFT                  32929               /*!< alt + shift */

#define KBD_KEY_ALT_COLON                   32954               /*!< alt + ";" */
#define KBD_KEY_ALT_INC                     32955               /*!< alt + "+" */
#define KBD_KEY_ALT_COMMA                   32956               /*!< alt + "," */
#define KBD_KEY_ALT_DEC                     32957               /*!< alt + "-" */
#define KBD_KEY_ALT_DOT                     32958               /*!< alt + "." */
#define KBD_KEY_ALT_BACKTICK                32960               /*!< alt + "`" */
#define KBD_KEY_ALT_LBRACE                  32987               /*!< alt + "[" */
#define KBD_KEY_ALT_BSLASH                  32988               /*!< alt + "\" */
#define KBD_KEY_ALT_RBRACE                  32989               /*!< alt + "]" */
#define KBD_KEY_ALT_SQUO                    32990               /*!< alt + "'" */

/*!< SHIFT + ALT */
#define KBD_KEY_SHIFT_ALT_A                 41025
#define KBD_KEY_SHIFT_ALT_B                 41026
#define KBD_KEY_SHIFT_ALT_C                 41027
#define KBD_KEY_SHIFT_ALT_D                 41028
#define KBD_KEY_SHIFT_ALT_E                 41029
#define KBD_KEY_SHIFT_ALT_F                 41030
#define KBD_KEY_SHIFT_ALT_G                 41031
#define KBD_KEY_SHIFT_ALT_H                 41032
#define KBD_KEY_SHIFT_ALT_I                 41033
#define KBD_KEY_SHIFT_ALT_J                 41034
#define KBD_KEY_SHIFT_ALT_K                 41035
#define KBD_KEY_SHIFT_ALT_L                 41036
#define KBD_KEY_SHIFT_ALT_M                 41037
#define KBD_KEY_SHIFT_ALT_N                 41038
#define KBD_KEY_SHIFT_ALT_O                 41039
#define KBD_KEY_SHIFT_ALT_P                 41040
#define KBD_KEY_SHIFT_ALT_Q                 41041
#define KBD_KEY_SHIFT_ALT_R                 41042
#define KBD_KEY_SHIFT_ALT_S                 41043
#define KBD_KEY_SHIFT_ALT_T                 41044
#define KBD_KEY_SHIFT_ALT_U                 41045
#define KBD_KEY_SHIFT_ALT_V                 41046
#define KBD_KEY_SHIFT_ALT_W                 41047
#define KBD_KEY_SHIFT_ALT_X                 41048
#define KBD_KEY_SHIFT_ALT_Y                 41049
#define KBD_KEY_SHIFT_ALT_Z                 41050

#define KBD_KEY_SHIFT_ALT_HASH              41071               /*!< shift + alt + "/" */
#define KBD_KEY_SHIFT_ALT_F1                41072
#define KBD_KEY_SHIFT_ALT_F2                41073
#define KBD_KEY_SHIFT_ALT_F3                41074
#define KBD_KEY_SHIFT_ALT_F4                41075
#define KBD_KEY_SHIFT_ALT_F5                41076
#define KBD_KEY_SHIFT_ALT_F6                41077
#define KBD_KEY_SHIFT_ALT_F7                41078
#define KBD_KEY_SHIFT_ALT_F8                41079
#define KBD_KEY_SHIFT_ALT_F9                41080
#define KBD_KEY_SHIFT_ALT_F10               41081
#define KBD_KEY_SHIFT_ALT_F11               41082
#define KBD_KEY_SHIFT_ALT_F12               41083

#define KBD_KEY_SHIFT_ALT_RSHIFT            41121               /*!< shift + alt + right shift */

#define KBD_KEY_SHIFT_ALT_COLON             41146               /*!< shift + alt + ";" */
#define KBD_KEY_SHIFT_ALT_INC               41147               /*!< shift + alt + "+" */
#define KBD_KEY_SHIFT_ALT_COMMA             41148               /*!< shift + alt + "," */
#define KBD_KEY_SHIFT_ALT_DEC               41149               /*!< shift + alt + "-" */
#define KBD_KEY_SHIFT_ALT_DOT               41150               /*!< shift + alt + "." */
#define KBD_KEY_SHIFT_ALT_BACKTICK          41152               /*!< shift + alt + "`" */
#define KBD_KEY_SHIFT_ALT_LBRACE            41179               /*!< shift + alt + "[" */
#define KBD_KEY_SHIFT_ALT_BSLASH            41180               /*!< shift + alt + "\" */
#define KBD_KEY_SHIFT_ALT_RBRACE            41181               /*!< shift + alt + "]" */
#define KBD_KEY_SHIFT_ALT_SQUO              41182               /*!< shift + alt + "'" */

/*!< CTRL + ALT */
#define KBD_KEY_CTRL_ALT_A                  49217
#define KBD_KEY_CTRL_ALT_B                  49218
#define KBD_KEY_CTRL_ALT_C                  49219
#define KBD_KEY_CTRL_ALT_D                  49220
#define KBD_KEY_CTRL_ALT_E                  49221
#define KBD_KEY_CTRL_ALT_F                  49222
#define KBD_KEY_CTRL_ALT_G                  49223
#define KBD_KEY_CTRL_ALT_H                  49224
#define KBD_KEY_CTRL_ALT_I                  49225
#define KBD_KEY_CTRL_ALT_J                  49226
#define KBD_KEY_CTRL_ALT_K                  49227
#define KBD_KEY_CTRL_ALT_L                  49228
#define KBD_KEY_CTRL_ALT_M                  49229
#define KBD_KEY_CTRL_ALT_N                  49230
#define KBD_KEY_CTRL_ALT_O                  49231
#define KBD_KEY_CTRL_ALT_P                  49232
#define KBD_KEY_CTRL_ALT_Q                  49233
#define KBD_KEY_CTRL_ALT_R                  49234
#define KBD_KEY_CTRL_ALT_S                  49235
#define KBD_KEY_CTRL_ALT_T                  49236
#define KBD_KEY_CTRL_ALT_U                  49237
#define KBD_KEY_CTRL_ALT_V                  49238
#define KBD_KEY_CTRL_ALT_W                  49239
#define KBD_KEY_CTRL_ALT_X                  49240
#define KBD_KEY_CTRL_ALT_Y                  49241
#define KBD_KEY_CTRL_ALT_Z                  49242

#define KBD_KEY_CTRL_ALT_HASH               49263               /*!< ctrl + alt + "/" */
#define KBD_KEY_CTRL_ALT_F1                 49264
#define KBD_KEY_CTRL_ALT_F2                 49265
#define KBD_KEY_CTRL_ALT_F3                 49266
#define KBD_KEY_CTRL_ALT_F4                 49267
#define KBD_KEY_CTRL_ALT_F5                 49268
#define KBD_KEY_CTRL_ALT_F6                 49269
#define KBD_KEY_CTRL_ALT_F7                 49270
#define KBD_KEY_CTRL_ALT_F8                 49271
#define KBD_KEY_CTRL_ALT_F9                 49272
#define KBD_KEY_CTRL_ALT_F10                49273
#define KBD_KEY_CTRL_ALT_F11                49274
#define KBD_KEY_CTRL_ALT_F12                49275

#define KBD_KEY_CTRL_ALT_RSHIFT             49313               /*!< ctrl + alt + right shift */

#define KBD_KEY_CTRL_ALT_COLON              49338               /*!< ctrl + alt + ";" */
#define KBD_KEY_CTRL_ALT_INC                49339               /*!< ctrl + alt + "+" */
#define KBD_KEY_CTRL_ALT_COMMA              49340               /*!< ctrl + alt + "," */
#define KBD_KEY_CTRL_ALT_DEC                49341               /*!< ctrl + alt + "-" */
#define KBD_KEY_CTRL_ALT_DOT                49342               /*!< ctrl + alt + "." */
#define KBD_KEY_CTRL_ALT_BACKTICK           49344               /*!< ctrl + alt + "`" */
#define KBD_KEY_CTRL_ALT_LBRACE             49371               /*!< ctrl + alt + "[" */
#define KBD_KEY_CTRL_ALT_BSLASH             49372               /*!< ctrl + alt + "\" */
#define KBD_KEY_CTRL_ALT_RBRACE             49373               /*!< ctrl + alt + "]" */
#define KBD_KEY_CTRL_ALT_SQUO               49374               /*!< ctrl + alt + "'" */

/*!< SHIFT + CTRL + ALT */
#define KBD_KEY_SHIFT_CTRL_ALT_A            57409
#define KBD_KEY_SHIFT_CTRL_ALT_B            57409
#define KBD_KEY_SHIFT_CTRL_ALT_C            57409
#define KBD_KEY_SHIFT_CTRL_ALT_D            57409
#define KBD_KEY_SHIFT_CTRL_ALT_E            57409
#define KBD_KEY_SHIFT_CTRL_ALT_F            57409
#define KBD_KEY_SHIFT_CTRL_ALT_G            57409
#define KBD_KEY_SHIFT_CTRL_ALT_H            57409
#define KBD_KEY_SHIFT_CTRL_ALT_I            57409
#define KBD_KEY_SHIFT_CTRL_ALT_J            57409
#define KBD_KEY_SHIFT_CTRL_ALT_K            57409
#define KBD_KEY_SHIFT_CTRL_ALT_L            57409
#define KBD_KEY_SHIFT_CTRL_ALT_M            57409
#define KBD_KEY_SHIFT_CTRL_ALT_N            57409
#define KBD_KEY_SHIFT_CTRL_ALT_O            57409
#define KBD_KEY_SHIFT_CTRL_ALT_P            57409
#define KBD_KEY_SHIFT_CTRL_ALT_Q            57409
#define KBD_KEY_SHIFT_CTRL_ALT_R            57409
#define KBD_KEY_SHIFT_CTRL_ALT_S            57409
#define KBD_KEY_SHIFT_CTRL_ALT_T            57409
#define KBD_KEY_SHIFT_CTRL_ALT_U            57409
#define KBD_KEY_SHIFT_CTRL_ALT_V            57409
#define KBD_KEY_SHIFT_CTRL_ALT_W            57409
#define KBD_KEY_SHIFT_CTRL_ALT_X            57409
#define KBD_KEY_SHIFT_CTRL_ALT_Y            57409
#define KBD_KEY_SHIFT_CTRL_ALT_Z            57409

#define KBD_KEY_SHIFT_CTRL_ALT_HASH         57455               /*!< shift + ctrl + alt + "/" */
#define KBD_KEY_SHIFT_CTRL_ALT_F1           57456
#define KBD_KEY_SHIFT_CTRL_ALT_F2           57457
#define KBD_KEY_SHIFT_CTRL_ALT_F3           57458
#define KBD_KEY_SHIFT_CTRL_ALT_F4           57459
#define KBD_KEY_SHIFT_CTRL_ALT_F5           57460
#define KBD_KEY_SHIFT_CTRL_ALT_F6           57461
#define KBD_KEY_SHIFT_CTRL_ALT_F7           57462
#define KBD_KEY_SHIFT_CTRL_ALT_F8           57463
#define KBD_KEY_SHIFT_CTRL_ALT_F9           57464
#define KBD_KEY_SHIFT_CTRL_ALT_F10          57465
#define KBD_KEY_SHIFT_CTRL_ALT_F11          57466
#define KBD_KEY_SHIFT_CTRL_ALT_F12          57467

#define KBD_KEY_SHIFT_CTRL_ALT_RSHIFT       57505               /*!< shift + ctrl + alt + right shift */

#define KBD_KEY_SHIFT_CTRL_ALT_COLON        57530               /*!< shift + ctrl + alt + ";" */
#define KBD_KEY_SHIFT_CTRL_ALT_INC          57531               /*!< shift + ctrl + alt + "+" */
#define KBD_KEY_SHIFT_CTRL_ALT_COMMA        57532               /*!< shift + ctrl + alt + "," */
#define KBD_KEY_SHIFT_CTRL_ALT_DEC          57533               /*!< shift + ctrl + alt + "-" */
#define KBD_KEY_SHIFT_CTRL_ALT_DOT          57534               /*!< shift + ctrl + alt + "." */
#define KBD_KEY_SHIFT_CTRL_ALT_BACKTICK     57536               /*!< shift + ctrl + alt + "`" */
#define KBD_KEY_SHIFT_CTRL_ALT_LBRACE       57563               /*!< shift + ctrl + alt + "[" */
#define KBD_KEY_SHIFT_CTRL_ALT_BSLASH       57564               /*!< shift + ctrl + alt + "\" */
#define KBD_KEY_SHIFT_CTRL_ALT_RBRACE       57565               /*!< shift + ctrl + alt + "]" */
#define KBD_KEY_SHIFT_CTRL_ALT_SQUO         57566               /*!< shift + ctrl + alt + "'" */

/*!< The functions */

#ifdef __cplusplus
    }
#endif

#endif /* __KEYBOARD_H */
