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
#define FWK_RGB_PIXEL8                              (8)                 /*!< 8 bit color */
#define FWK_RGB_PIXEL16                             (16)                /*!< RGB565 */
#define FWK_RGB_PIXEL24                             (24)                /*!< RGB888 */
#define FWK_RGB_PIXEL32                             (32)                /*!< RGB888 + diaphaneity */

/*!< The number of bits of color actually used: 8/16/24 */
#define FWK_RGB_PIXELBIT                            (CONFIG_LCD_PIXELBIT)

/*!< ------------------------------------------------------------------------------- */
/*!<    English name                                Color pixel         Chinese name */
/*!< ------------------------------------------------------------------------------- */
#if ((FWK_RGB_PIXELBIT == FWK_RGB_PIXEL24) || (FWK_RGB_PIXELBIT == FWK_RGB_PIXEL32))
/*!< RGB888 color chart */
#define RGB_LIGHTPINK                               ( 0x00FFB6C1 )      /*!< 浅粉红 */
#define RGB_PINK                                    ( 0x00FFC0CB )      /*!< 粉红 */
#define RGB_CRIMSON                                 ( 0x00DC143C )      /*!< 深红/猩红 */
#define RGB_LAVENDERBLUSH                           ( 0x00FFF0F5 )      /*!< 淡紫红 */
#define RGB_PALEVIOLETRED                           ( 0x00DB7093 )      /*!< 弱紫罗兰红 */
#define RGB_HOTPINK                                 ( 0x00FF69B4 )      /*!< 热情的粉红 */
#define RGB_DEEPPINK                                ( 0x00FF1493 )      /*!< 深粉红 */
#define RGB_MEDIUMVIOLETRED                         ( 0x00C71585 )      /*!< 中紫罗兰红 */
#define RGB_ORCHID                                  ( 0x00DA70D6 )      /*!< 暗紫色/兰花紫 */
#define RGB_THISTLE                                 ( 0x00D8BFD8 )      /*!< 蓟色 */
#define RGB_PLUM                                    ( 0x00DDA0DD )      /*!< 洋李色/李子紫 */
#define RGB_VIOLET                                  ( 0x00EE82EE )      /*!< 紫罗兰 */
#define RGB_MAGENTA                                 ( 0x00FF00FF )      /*!< 洋红/玫瑰红 */
#define RGB_FUCHSIA                                 ( 0x00FF00FF )      /*!< 紫红/灯笼海棠 */
#define RGB_DARKMAGENTA                             ( 0x008B008B )      /*!< 深洋红 */
#define RGB_PURPLE                                  ( 0x00800080 )      /*!< 紫色 */
#define RGB_MEDIUMORCHID                            ( 0x00BA55D3 )      /*!< 中兰花紫 */
#define RGB_DARKVIOLET                              ( 0x009400D3 )      /*!< 暗紫罗兰 */
#define RGB_DARKORCHID                              ( 0x009932CC )      /*!< 暗兰花紫 */
#define RGB_INDIGO                                  ( 0x004B0082 )      /*!< 靛青/紫兰色 */
#define RGB_BLUEVIOLET                              ( 0x008A2BE2 )      /*!< 蓝紫罗兰 */
#define RGB_MEDIUMPURPLE                            ( 0x009370DB )      /*!< 中紫色 */
#define RGB_MEDIUMSLATEBLUE                         ( 0x007B68EE )      /*!< 中暗蓝色/中板岩蓝 */
#define RGB_SLATEBLUE                               ( 0x006A5ACD )      /*!< 石蓝色/板岩蓝 */
#define RGB_DARKSLATEBLUE                           ( 0x00483D8B )      /*!< 暗灰蓝色/暗板岩蓝 */
#define RGB_LAVENDER                                ( 0x00E6E6FA )      /*!< 淡紫色/熏衣草淡紫 */
#define RGB_GHOSTWHITE                              ( 0x00F8F8FF )      /*!< 幽灵白 */
#define RGB_BLUE                                    ( 0x000000FF )      /*!< 纯蓝 */
#define RGB_MEDIUMBLUE                              ( 0x000000CD )      /*!< 中蓝色 */
#define RGB_MIDNIGHTBLUE                            ( 0x00191970 )      /*!< 午夜蓝 */
#define RGB_DARKBLUE                                ( 0x0000008B )      /*!< 暗蓝色 */
#define RGB_NAVY                                    ( 0x00000080 )      /*!< 海军蓝 */
#define RGB_ROYALBLUE                               ( 0x004169E1 )      /*!< 皇家蓝/宝蓝 */
#define RGB_CORNFLOWERBLUE                          ( 0x006495ED )      /*!< 矢车菊蓝 */
#define RGB_LIGHTSTEELBLUE                          ( 0x00B0C4DE )      /*!< 亮钢蓝 */
#define RGB_LIGHTSLATEGRAY                          ( 0x00778899 )      /*!< 亮蓝灰/亮石板灰 */
#define RGB_SLATEGRAY                               ( 0x00708090 )      /*!< 灰石色/石板灰 */
#define RGB_DODGERBLUE                              ( 0x001E90FF )      /*!< 闪兰色/道奇蓝 */
#define RGB_ALICEBLUE                               ( 0x00F0F8FF )      /*!< 爱丽丝蓝 */
#define RGB_STEELBLUE                               ( 0x004682B4 )      /*!< 钢蓝/铁青 */
#define RGB_LIGHTSKYBLUE                            ( 0x0087CEFA )      /*!< 亮天蓝色 */
#define RGB_SKYBLUE                                 ( 0x0087CEEB )      /*!< 天蓝色 */
#define RGB_DEEPSKYBLUE                             ( 0x0000BFFF )      /*!< 深天蓝 */
#define RGB_LIGHTBLUE                               ( 0x00ADD8E6 )      /*!< 亮蓝 */
#define RGB_POWDERBLUE                              ( 0x00B0E0E6 )      /*!< 粉蓝色/火药青 */
#define RGB_CADETBLUE                               ( 0x005F9EA0 )      /*!< 军兰色/军服蓝 */
#define RGB_AZURE                                   ( 0x00F0FFFF )      /*!< 蔚蓝色 */
#define RGB_LIGHTCYAN                               ( 0x00E0FFFF )      /*!< 淡青色 */
#define RGB_PALETURQUOISE                           ( 0x00AFEEEE )      /*!< 弱绿宝石 */
#define RGB_CYAN                                    ( 0x0000FFFF )      /*!< 青色 */
#define RGB_AQUA                                    ( 0x0000FFFF )      /*!< 浅绿色/水色 */
#define RGB_DARKTURQUOISE                           ( 0x0000CED1 )      /*!< 暗绿宝石 */
#define RGB_DARKSLATEGRAY                           ( 0x002F4F4F )      /*!< 暗瓦灰色/暗石板灰 */
#define RGB_DARKCYAN                                ( 0x00008B8B )      /*!< 暗青色 */
#define RGB_TEAL                                    ( 0x00008080 )      /*!< 水鸭色 */
#define RGB_MEDIUMTURQUOISE                         ( 0x0048D1CC )      /*!< 中绿宝石 */
#define RGB_LIGHTSEAGREEN                           ( 0x0020B2AA )      /*!< 浅海洋绿 */
#define RGB_TURQUOISE                               ( 0x0040E0D0 )      /*!< 绿宝石 */
#define RGB_AQUAMARINE                              ( 0x007FFFD4 )      /*!< 宝石碧绿 */
#define RGB_MEDIUMAQUAMARINE                        ( 0x0066CDAA )      /*!< 中宝石碧绿 */
#define RGB_MEDIUMSPRINGGREEN                       ( 0x0000FA9A )      /*!< 中春绿色 */
#define RGB_MINTCREAM                               ( 0x00F5FFFA )      /*!< 薄荷奶油 */
#define RGB_SPRINGGREEN                             ( 0x0000FF7F )      /*!< 春绿色 */
#define RGB_MEDIUMSEAGREEN                          ( 0x003CB371 )      /*!< 中海洋绿 */
#define RGB_SEAGREEN                                ( 0x002E8B57 )      /*!< 海洋绿 */
#define RGB_HONEYDEW                                ( 0x00F0FFF0 )      /*!< 蜜色/蜜瓜色 */
#define RGB_LIGHTGREEN                              ( 0x0090EE90 )      /*!< 淡绿色 */
#define RGB_PALEGREEN                               ( 0x0098FB98 )      /*!< 弱绿色 */
#define RGB_DARKSEAGREEN                            ( 0x008FBC8F )      /*!< 暗海洋绿 */
#define RGB_LIMEGREEN                               ( 0x0032CD32 )      /*!< 闪光深绿 */
#define RGB_LIME                                    ( 0x0000FF00 )      /*!< 闪光绿 */
#define RGB_FORESTGREEN                             ( 0x00228B22 )      /*!< 森林绿 */
#define RGB_GREEN                                   ( 0x00008000 )      /*!< 纯绿 */
#define RGB_DARKGREEN                               ( 0x00006400 )      /*!< 暗绿色 */
#define RGB_CHARTREUSE                              ( 0x007FFF00 )      /*!< 黄绿色/查特酒绿 */
#define RGB_LAWNGREEN                               ( 0x007CFC00 )      /*!< 草绿色/草坪绿 */
#define RGB_GREENYELLOW                             ( 0x00ADFF2F )      /*!< 绿黄色 */
#define RGB_DARKOLIVEGREEN                          ( 0x00556B2F )      /*!< 暗橄榄绿 */
#define RGB_YELLOWGREEN                             ( 0x009ACD32 )      /*!< 黄绿色 */
#define RGB_OLIVEDRAB                               ( 0x006B8E23 )      /*!< 橄榄褐色 */
#define RGB_BEIGE                                   ( 0x00F5F5DC )      /*!< 米色/灰棕色 */
#define RGB_LIGHTGOLDENRODYELLOW                    ( 0x00FAFAD2 )      /*!< 亮菊黄 */
#define RGB_IVORY                                   ( 0x00FFFFF0 )      /*!< 象牙色 */
#define RGB_LIGHTYELLOW                             ( 0x00FFFFE0 )      /*!< 浅黄色 */
#define RGB_YELLOW                                  ( 0x00FFFF00 )      /*!< 纯黄 */
#define RGB_OLIVE                                   ( 0x00808000 )      /*!< 橄榄 */
#define RGB_DARKKHAKI                               ( 0x00BDB76B )      /*!< 暗黄褐色/深卡叽布 */
#define RGB_LEMONCHIFFON                            ( 0x00FFFACD )      /*!< 柠檬绸 */
#define RGB_PALEGOLDENROD                           ( 0x00EEE8AA )      /*!< 灰菊黄/苍麒麟色 */
#define RGB_KHAKI                                   ( 0x00F0E68C )      /*!< 黄褐色/卡叽布 */
#define RGB_GOLD                                    ( 0x00FFD700 )      /*!< 金色 */
#define RGB_CORNSILK                                ( 0x00FFF8DC )      /*!< 玉米丝色 */
#define RGB_GOLDENROD                               ( 0x00DAA520 )      /*!< 金菊黄 */
#define RGB_DARKGOLDENROD                           ( 0x00B8860B )      /*!< 暗金菊黄 */
#define RGB_FLORALWHITE                             ( 0x00FFFAF0 )      /*!< 花的白色 */
#define RGB_OLDLACE                                 ( 0x00FDF5E6 )      /*!< 老花色/旧蕾丝 */
#define RGB_WHEAT                                   ( 0x00F5DEB3 )      /*!< 浅黄色/小麦色 */
#define RGB_MOCCASIN                                ( 0x00FFE4B5 )      /*!< 鹿皮色/鹿皮靴 */
#define RGB_ORANGE                                  ( 0x00FFA500 )      /*!< 橙色 */
#define RGB_PAPAYAWHIP                              ( 0x00FFEFD5 )      /*!< 番木色/番木瓜 */
#define RGB_BLANCHEDALMOND                          ( 0x00FFEBCD )      /*!< 白杏色 */
#define RGB_NAVAJOWHITE                             ( 0x00FFDEAD )      /*!< 纳瓦白/土著白 */
#define RGB_ANTIQUEWHITE                            ( 0x00FAEBD7 )      /*!< 古董白 */
#define RGB_TAN                                     ( 0x00D2B48C )      /*!< 茶色 */
#define RGB_BURLYWOOD                               ( 0x00DEB887 )      /*!< 硬木色 */
#define RGB_BISQUE                                  ( 0x00FFE4C4 )      /*!< 陶坯黄 */
#define RGB_DARKORANGE                              ( 0x00FF8C00 )      /*!< 深橙色 */
#define RGB_LINEN                                   ( 0x00FAF0E6 )      /*!< 亚麻布 */
#define RGB_PERU                                    ( 0x00CD853F )      /*!< 秘鲁色 */
#define RGB_PEACHPUFF                               ( 0x00FFDAB9 )      /*!< 桃肉色 */
#define RGB_SANDYBROWN                              ( 0x00F4A460 )      /*!< 沙棕色 */
#define RGB_CHOCOLATE                               ( 0x00D2691E )      /*!< 巧克力色 */
#define RGB_SADDLEBROWN                             ( 0x008B4513 )      /*!< 重褐色/马鞍棕色 */
#define RGB_SEASHELL                                ( 0x00FFF5EE )      /*!< 海贝壳 */
#define RGB_SIENNA                                  ( 0x00A0522D )      /*!< 黄土赭色 */
#define RGB_LIGHTSALMON                             ( 0x00FFA07A )      /*!< 浅鲑鱼肉色 */
#define RGB_CORAL                                   ( 0x00FF7F50 )      /*!< 珊瑚 */
#define RGB_ORANGERED                               ( 0x00FF4500 )      /*!< 橙红色 */
#define RGB_DARKSALMON                              ( 0x00E9967A )      /*!< 深鲜肉/鲑鱼色 */
#define RGB_TOMATO                                  ( 0x00FF6347 )      /*!< 番茄红 */
#define RGB_MISTYROSE                               ( 0x00FFE4E1 )      /*!< 浅玫瑰色/薄雾玫瑰 */
#define RGB_SALMON                                  ( 0x00FA8072 )      /*!< 鲜肉/鲑鱼色 */
#define RGB_SNOW                                    ( 0x00FFFAFA )      /*!< 雪白色 */
#define RGB_LIGHTCORAL                              ( 0x00F08080 )      /*!< 淡珊瑚色 */
#define RGB_ROSYBROWN                               ( 0x00BC8F8F )      /*!< 玫瑰棕色 */
#define RGB_INDIANRED                               ( 0x00CD5C5C )      /*!< 印度红 */
#define RGB_RED                                     ( 0x00FF0000 )      /*!< 纯红 */
#define RGB_BROWN                                   ( 0x00A52A2A )      /*!< 棕色 */
#define RGB_FIREBRICK                               ( 0x00B22222 )      /*!< 火砖色/耐火砖 */
#define RGB_DARKRED                                 ( 0x008B0000 )      /*!< 深红色 */
#define RGB_MAROON                                  ( 0x00800000 )      /*!< 栗色 */
#define RGB_WHITE                                   ( 0x00FFFFFF )      /*!< 纯白 */
#define RGB_WHITESMOKE                              ( 0x00F5F5F5 )      /*!< 白烟 */
#define RGB_GAINSBORO                               ( 0x00DCDCDC )      /*!< 淡灰色 */
#define RGB_LIGHTGRAY                               ( 0x00D3D3D3 )      /*!< 浅灰色 */
#define RGB_SILVER                                  ( 0x00C0C0C0 )      /*!< 银灰色 */
#define RGB_DARKGRAY                                ( 0x00A9A9A9 )      /*!< 深灰色 */
#define RGB_GRAY                                    ( 0x00808080 )      /*!< 灰色 */
#define RGB_GRAY0                                   ( 0x00408080 )      /*!< 灰色0 */
#define	RGB_GRAY1                                   ( 0x00808480 )      /*!< 灰色1 */
#define RGB_GRAY2                                   ( 0x00E8ECE8 )      /*!< 灰色2 */
#define RGB_DIMGRAY                                 ( 0x00696969 )      /*!< 暗淡灰 */
#define RGB_BLACK                                   ( 0x00000000 )      /*!< 纯色 */

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
#define RGB_BROWN 			                        ( 0X0000BC40 )      /*!< 棕色 */
#define RGB_FIREBRICK 			                    ( 0X0000FC07 )      /*!< 棕红色、火砖色 */
#define RGB_GRAY  			                        ( 0X00008430 )      /*!< 灰色 */
#define RGB_GRAY0  		                            ( 0x0000EF7D )      /*!< 灰色0 */
#define	RGB_GRAY1   		                        ( 0x00008410 )      /*!< 灰色1 */
#define RGB_GRAY2  		                            ( 0x00004208 )      /*!< 灰色2 */
/*!< PANEL color */
#define RGB_DARKBLUE      	                        ( 0x000001CF )      /*!< 暗蓝色 */
#define RGB_LIGHTBLUE      	                        ( 0x00007D7C )      /*!< 浅蓝色  */
#define RGB_GRAYBLUE       	                        ( 0x00005458 )      /*!< 灰蓝色 */
#define RGB_LIGHTGREEN     	                        ( 0x0000841F )      /*!< 浅绿色 */
#define	RGB_LIGHTGRAY                               ( 0x0000EF5B )      /*!< 浅灰色(PANNEL) */
#define RGB_LGRAY 			                        ( 0x0000C618 )      /*!< 浅灰色(PANNEL), 窗体背景色 */
#define RGB_LGRAYBLUE                               ( 0x0000A651 )      /*!< 浅灰蓝色(中间层颜色) */
#define	RGB_LBBLUE                                  ( 0x00002B12 )      /*!< 浅棕蓝色(选择条目的反色) */
#endif

#endif /*!< __FWK_RGBMAP_H_ */