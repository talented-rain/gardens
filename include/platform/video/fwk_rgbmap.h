/*
 * Display RGB Color Code
 *
 * File Name:   fwk_rgbmap.h
 * Author:      Yang Yujun
 * E-mail:      <yujiantianhu@163.com>
 * Created on:  2023.06.29
 *
 * Copyright (c) 2023   Yang Yujun <yujiantianhu@163.com>
 *
 */

#ifndef __FWK_RGBMAP_H_
#define __FWK_RGBMAP_H_

/*!< The includes */
#include <platform/fwk_basic.h>

/*!< The defines */
/*!< ------------------------------------------------------------------------------- */   
#define FWK_RGB_PIXEL8                              (8  >> 3)           /*!< 8 bit color */
#define FWK_RGB_PIXEL16                             (16 >> 3)           /*!< RGB565 */
#define FWK_RGB_PIXEL24                             (24 >> 3)           /*!< RGB888 */
#define FWK_RGB_PIXEL32                             (32 >> 3)           /*!< RGB888 + diaphaneity */

/*!< The number of bits of color actually used: 8/16/24 */
#define FWK_RGB_PIXELBIT                            ((CONFIG_LCD_PIXELBIT) >> 3)

/*!< ------------------------------------------------------------------------------- */
/*!<    English name                                Color pixel         Chinese name */
/*!< ------------------------------------------------------------------------------- */
#if (FWK_RGB_PIXELBIT == FWK_RGB_PIXEL24)
/*!< RGB888 color chart */
#define RGB_LIGHTPINK                               ( 0x00FFB6C1 )      /*!< ǳ�ۺ� */
#define RGB_PINK                                    ( 0x00FFC0CB )      /*!< �ۺ� */
#define RGB_CRIMSON                                 ( 0x00DC143C )      /*!< ���/�ɺ� */
#define RGB_LAVENDERBLUSH                           ( 0x00FFF0F5 )      /*!< ���Ϻ� */
#define RGB_PALEVIOLETRED                           ( 0x00DB7093 )      /*!< ���������� */
#define RGB_HOTPINK                                 ( 0x00FF69B4 )      /*!< ����ķۺ� */
#define RGB_DEEPPINK                                ( 0x00FF1493 )      /*!< ��ۺ� */
#define RGB_MEDIUMVIOLETRED                         ( 0x00C71585 )      /*!< ���������� */
#define RGB_ORCHID                                  ( 0x00DA70D6 )      /*!< ����ɫ/������ */
#define RGB_THISTLE                                 ( 0x00D8BFD8 )      /*!< ��ɫ */
#define RGB_PLUM                                    ( 0x00DDA0DD )      /*!< ����ɫ/������ */
#define RGB_VIOLET                                  ( 0x00EE82EE )      /*!< ������ */
#define RGB_MAGENTA                                 ( 0x00FF00FF )      /*!< ���/õ��� */
#define RGB_FUCHSIA                                 ( 0x00FF00FF )      /*!< �Ϻ�/�������� */
#define RGB_DARKMAGENTA                             ( 0x008B008B )      /*!< ����� */
#define RGB_PURPLE                                  ( 0x00800080 )      /*!< ��ɫ */
#define RGB_MEDIUMORCHID                            ( 0x00BA55D3 )      /*!< �������� */
#define RGB_DARKVIOLET                              ( 0x009400D3 )      /*!< �������� */
#define RGB_DARKORCHID                              ( 0x009932CC )      /*!< �������� */
#define RGB_INDIGO                                  ( 0x004B0082 )      /*!< ����/����ɫ */
#define RGB_BLUEVIOLET                              ( 0x008A2BE2 )      /*!< �������� */
#define RGB_MEDIUMPURPLE                            ( 0x009370DB )      /*!< ����ɫ */
#define RGB_MEDIUMSLATEBLUE                         ( 0x007B68EE )      /*!< �а���ɫ/�а����� */
#define RGB_SLATEBLUE                               ( 0x006A5ACD )      /*!< ʯ��ɫ/������ */
#define RGB_DARKSLATEBLUE                           ( 0x00483D8B )      /*!< ������ɫ/�������� */
#define RGB_LAVENDER                                ( 0x00E6E6FA )      /*!< ����ɫ/Ѭ�²ݵ��� */
#define RGB_GHOSTWHITE                              ( 0x00F8F8FF )      /*!< ����� */
#define RGB_BLUE                                    ( 0x000000FF )      /*!< ���� */
#define RGB_MEDIUMBLUE                              ( 0x000000CD )      /*!< ����ɫ */
#define RGB_MIDNIGHTBLUE                            ( 0x00191970 )      /*!< ��ҹ�� */
#define RGB_DARKBLUE                                ( 0x0000008B )      /*!< ����ɫ */
#define RGB_NAVY                                    ( 0x00000080 )      /*!< ������ */
#define RGB_ROYALBLUE                               ( 0x004169E1 )      /*!< �ʼ���/���� */
#define RGB_CORNFLOWERBLUE                          ( 0x006495ED )      /*!< ʸ������ */
#define RGB_LIGHTSTEELBLUE                          ( 0x00B0C4DE )      /*!< ������ */
#define RGB_LIGHTSLATEGRAY                          ( 0x00778899 )      /*!< ������/��ʯ��� */
#define RGB_SLATEGRAY                               ( 0x00708090 )      /*!< ��ʯɫ/ʯ��� */
#define RGB_DODGERBLUE                              ( 0x001E90FF )      /*!< ����ɫ/������ */
#define RGB_ALICEBLUE                               ( 0x00F0F8FF )      /*!< ����˿�� */
#define RGB_STEELBLUE                               ( 0x004682B4 )      /*!< ����/���� */
#define RGB_LIGHTSKYBLUE                            ( 0x0087CEFA )      /*!< ������ɫ */
#define RGB_SKYBLUE                                 ( 0x0087CEEB )      /*!< ����ɫ */
#define RGB_DEEPSKYBLUE                             ( 0x0000BFFF )      /*!< ������ */
#define RGB_LIGHTBLUE                               ( 0x00ADD8E6 )      /*!< ���� */
#define RGB_POWDERBLUE                              ( 0x00B0E0E6 )      /*!< ����ɫ/��ҩ�� */
#define RGB_CADETBLUE                               ( 0x005F9EA0 )      /*!< ����ɫ/������ */
#define RGB_AZURE                                   ( 0x00F0FFFF )      /*!< ε��ɫ */
#define RGB_LIGHTCYAN                               ( 0x00E0FFFF )      /*!< ����ɫ */
#define RGB_PALETURQUOISE                           ( 0x00AFEEEE )      /*!< ���̱�ʯ */
#define RGB_CYAN                                    ( 0x0000FFFF )      /*!< ��ɫ */
#define RGB_AQUA                                    ( 0x0000FFFF )      /*!< ǳ��ɫ/ˮɫ */
#define RGB_DARKTURQUOISE                           ( 0x0000CED1 )      /*!< ���̱�ʯ */
#define RGB_DARKSLATEGRAY                           ( 0x002F4F4F )      /*!< ���߻�ɫ/��ʯ��� */
#define RGB_DARKCYAN                                ( 0x00008B8B )      /*!< ����ɫ */
#define RGB_TEAL                                    ( 0x00008080 )      /*!< ˮѼɫ */
#define RGB_MEDIUMTURQUOISE                         ( 0x0048D1CC )      /*!< ���̱�ʯ */
#define RGB_LIGHTSEAGREEN                           ( 0x0020B2AA )      /*!< ǳ������ */
#define RGB_TURQUOISE                               ( 0x0040E0D0 )      /*!< �̱�ʯ */
#define RGB_AQUAMARINE                              ( 0x007FFFD4 )      /*!< ��ʯ���� */
#define RGB_MEDIUMAQUAMARINE                        ( 0x0066CDAA )      /*!< �б�ʯ���� */
#define RGB_MEDIUMSPRINGGREEN                       ( 0x0000FA9A )      /*!< �д���ɫ */
#define RGB_MINTCREAM                               ( 0x00F5FFFA )      /*!< �������� */
#define RGB_SPRINGGREEN                             ( 0x0000FF7F )      /*!< ����ɫ */
#define RGB_MEDIUMSEAGREEN                          ( 0x003CB371 )      /*!< �к����� */
#define RGB_SEAGREEN                                ( 0x002E8B57 )      /*!< ������ */
#define RGB_HONEYDEW                                ( 0x00F0FFF0 )      /*!< ��ɫ/�۹�ɫ */
#define RGB_LIGHTGREEN                              ( 0x0090EE90 )      /*!< ����ɫ */
#define RGB_PALEGREEN                               ( 0x0098FB98 )      /*!< ����ɫ */
#define RGB_DARKSEAGREEN                            ( 0x008FBC8F )      /*!< �������� */
#define RGB_LIMEGREEN                               ( 0x0032CD32 )      /*!< �������� */
#define RGB_LIME                                    ( 0x0000FF00 )      /*!< ������ */
#define RGB_FORESTGREEN                             ( 0x00228B22 )      /*!< ɭ���� */
#define RGB_GREEN                                   ( 0x00008000 )      /*!< ���� */
#define RGB_DARKGREEN                               ( 0x00006400 )      /*!< ����ɫ */
#define RGB_CHARTREUSE                              ( 0x007FFF00 )      /*!< ����ɫ/���ؾ��� */
#define RGB_LAWNGREEN                               ( 0x007CFC00 )      /*!< ����ɫ/��ƺ�� */
#define RGB_GREENYELLOW                             ( 0x00ADFF2F )      /*!< �̻�ɫ */
#define RGB_DARKOLIVEGREEN                          ( 0x00556B2F )      /*!< ������� */
#define RGB_YELLOWGREEN                             ( 0x009ACD32 )      /*!< ����ɫ */
#define RGB_OLIVEDRAB                               ( 0x006B8E23 )      /*!< ��魺�ɫ */
#define RGB_BEIGE                                   ( 0x00F5F5DC )      /*!< ��ɫ/����ɫ */
#define RGB_LIGHTGOLDENRODYELLOW                    ( 0x00FAFAD2 )      /*!< ���ջ� */
#define RGB_IVORY                                   ( 0x00FFFFF0 )      /*!< ����ɫ */
#define RGB_LIGHTYELLOW                             ( 0x00FFFFE0 )      /*!< ǳ��ɫ */
#define RGB_YELLOW                                  ( 0x00FFFF00 )      /*!< ���� */
#define RGB_OLIVE                                   ( 0x00808000 )      /*!< ��� */
#define RGB_DARKKHAKI                               ( 0x00BDB76B )      /*!< ���ƺ�ɫ/�ߴ�� */
#define RGB_LEMONCHIFFON                            ( 0x00FFFACD )      /*!< ���ʳ� */
#define RGB_PALEGOLDENROD                           ( 0x00EEE8AA )      /*!< �Ҿջ�/������ɫ */
#define RGB_KHAKI                                   ( 0x00F0E68C )      /*!< �ƺ�ɫ/��ߴ�� */
#define RGB_GOLD                                    ( 0x00FFD700 )      /*!< ��ɫ */
#define RGB_CORNSILK                                ( 0x00FFF8DC )      /*!< ����˿ɫ */
#define RGB_GOLDENROD                               ( 0x00DAA520 )      /*!< ��ջ� */
#define RGB_DARKGOLDENROD                           ( 0x00B8860B )      /*!< ����ջ� */
#define RGB_FLORALWHITE                             ( 0x00FFFAF0 )      /*!< ���İ�ɫ */
#define RGB_OLDLACE                                 ( 0x00FDF5E6 )      /*!< �ϻ�ɫ/����˿ */
#define RGB_WHEAT                                   ( 0x00F5DEB3 )      /*!< ǳ��ɫ/С��ɫ */
#define RGB_MOCCASIN                                ( 0x00FFE4B5 )      /*!< ¹Ƥɫ/¹Ƥѥ */
#define RGB_ORANGE                                  ( 0x00FFA500 )      /*!< ��ɫ */
#define RGB_PAPAYAWHIP                              ( 0x00FFEFD5 )      /*!< ��ľɫ/��ľ�� */
#define RGB_BLANCHEDALMOND                          ( 0x00FFEBCD )      /*!< ����ɫ */
#define RGB_NAVAJOWHITE                             ( 0x00FFDEAD )      /*!< ���߰�/������ */
#define RGB_ANTIQUEWHITE                            ( 0x00FAEBD7 )      /*!< �Ŷ��� */
#define RGB_TAN                                     ( 0x00D2B48C )      /*!< ��ɫ */
#define RGB_BURLYWOOD                               ( 0x00DEB887 )      /*!< Ӳľɫ */
#define RGB_BISQUE                                  ( 0x00FFE4C4 )      /*!< ������ */
#define RGB_DARKORANGE                              ( 0x00FF8C00 )      /*!< ���ɫ */
#define RGB_LINEN                                   ( 0x00FAF0E6 )      /*!< ���鲼 */
#define RGB_PERU                                    ( 0x00CD853F )      /*!< ��³ɫ */
#define RGB_PEACHPUFF                               ( 0x00FFDAB9 )      /*!< ����ɫ */
#define RGB_SANDYBROWN                              ( 0x00F4A460 )      /*!< ɳ��ɫ */
#define RGB_CHOCOLATE                               ( 0x00D2691E )      /*!< �ɿ���ɫ */
#define RGB_SADDLEBROWN                             ( 0x008B4513 )      /*!< �غ�ɫ/����ɫ */
#define RGB_SEASHELL                                ( 0x00FFF5EE )      /*!< ������ */
#define RGB_SIENNA                                  ( 0x00A0522D )      /*!< ������ɫ */
#define RGB_LIGHTSALMON                             ( 0x00FFA07A )      /*!< ǳ������ɫ */
#define RGB_CORAL                                   ( 0x00FF7F50 )      /*!< ɺ�� */
#define RGB_ORANGERED                               ( 0x00FF4500 )      /*!< �Ⱥ�ɫ */
#define RGB_DARKSALMON                              ( 0x00E9967A )      /*!< ������/����ɫ */
#define RGB_TOMATO                                  ( 0x00FF6347 )      /*!< ���Ѻ� */
#define RGB_MISTYROSE                               ( 0x00FFE4E1 )      /*!< ǳõ��ɫ/����õ�� */
#define RGB_SALMON                                  ( 0x00FA8072 )      /*!< ����/����ɫ */
#define RGB_SNOW                                    ( 0x00FFFAFA )      /*!< ѩ��ɫ */
#define RGB_LIGHTCORAL                              ( 0x00F08080 )      /*!< ��ɺ��ɫ */
#define RGB_ROSYBROWN                               ( 0x00BC8F8F )      /*!< õ����ɫ */
#define RGB_INDIANRED                               ( 0x00CD5C5C )      /*!< ӡ�Ⱥ� */
#define RGB_RED                                     ( 0x00FF0000 )      /*!< ���� */
#define RGB_BROWN                                   ( 0x00A52A2A )      /*!< ��ɫ */
#define RGB_FIREBRICK                               ( 0x00B22222 )      /*!< ��שɫ/�ͻ�ש */
#define RGB_DARKRED                                 ( 0x008B0000 )      /*!< ���ɫ */
#define RGB_MAROON                                  ( 0x00800000 )      /*!< ��ɫ */
#define RGB_WHITE                                   ( 0x00FFFFFF )      /*!< ���� */
#define RGB_WHITESMOKE                              ( 0x00F5F5F5 )      /*!< ���� */
#define RGB_GAINSBORO                               ( 0x00DCDCDC )      /*!< ����ɫ */
#define RGB_LIGHTGRAY                               ( 0x00D3D3D3 )      /*!< ǳ��ɫ */
#define RGB_SILVER                                  ( 0x00C0C0C0 )      /*!< ����ɫ */
#define RGB_DARKGRAY                                ( 0x00A9A9A9 )      /*!< ���ɫ */
#define RGB_GRAY                                    ( 0x00808080 )      /*!< ��ɫ */
#define RGB_GRAY0                                   ( 0x00408080 )      /*!< ��ɫ0 */
#define	RGB_GRAY1                                   ( 0x00808480 )      /*!< ��ɫ1 */
#define RGB_GRAY2                                   ( 0x00E8ECE8 )      /*!< ��ɫ2 */
#define RGB_DIMGRAY                                 ( 0x00696969 )      /*!< ������ */
#define RGB_BLACK                                   ( 0x00000000 )      /*!< ��ɫ */

#elif (FWK_RGB_PIXELBIT == FWK_RGB_PIXEL16)
/*!< RGB565 color chart */
#define RGB_WHITE                                   ( 0x0000FFFF )
#define RGB_BLACK                                   ( 0x00000000 )
#define RGB_BLUE         	                        ( 0x0000001F )
#define RGB_RED           	                        ( 0x0000F800 )
#define RGB_MAGENTA       	                        ( 0x0000F81F )
#define RGB_GREEN         	                        ( 0x000007E0 )
#define RGB_CYAN          	                        ( 0x00007FFF )
#define RGB_YELLOW        	                        ( 0x0000FFE0 )
#define RGB_BROWN 			                        ( 0X0000BC40 )      /*!< ��ɫ */
#define RGB_FIREBRICK 			                    ( 0X0000FC07 )      /*!< �غ�ɫ����שɫ */
#define RGB_GRAY  			                        ( 0X00008430 )      /*!< ��ɫ */
#define RGB_GRAY0  		                            ( 0x0000EF7D )      /*!< ��ɫ0 */
#define	RGB_GRAY1   		                        ( 0x00008410 )      /*!< ��ɫ1 */
#define RGB_GRAY2  		                            ( 0x00004208 )      /*!< ��ɫ2 */
/*!< PANEL color */
#define RGB_DARKBLUE      	                        ( 0x000001CF )      /*!< ����ɫ */
#define RGB_LIGHTBLUE      	                        ( 0x00007D7C )      /*!< ǳ��ɫ  */
#define RGB_GRAYBLUE       	                        ( 0x00005458 )      /*!< ����ɫ */
#define RGB_LIGHTGREEN     	                        ( 0x0000841F )      /*!< ǳ��ɫ */
#define	RGB_LIGHTGRAY                               ( 0x0000EF5B )      /*!< ǳ��ɫ(PANNEL) */
#define RGB_LGRAY 			                        ( 0x0000C618 )      /*!< ǳ��ɫ(PANNEL), ���屳��ɫ */
#define RGB_LGRAYBLUE                               ( 0x0000A651 )      /*!< ǳ����ɫ(�м����ɫ) */
#define	RGB_LBBLUE                                  ( 0x00002B12 )      /*!< ǳ����ɫ(ѡ����Ŀ�ķ�ɫ) */
#endif

#endif /*!< __FWK_RGBMAP_H_ */
