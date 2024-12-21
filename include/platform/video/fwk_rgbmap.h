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
#define RGB_LIGHTPINK                               ( 0x00FFB6C1 )      /*!< Ç³·Ûºì */
#define RGB_PINK                                    ( 0x00FFC0CB )      /*!< ·Ûºì */
#define RGB_CRIMSON                                 ( 0x00DC143C )      /*!< Éîºì/ÐÉºì */
#define RGB_LAVENDERBLUSH                           ( 0x00FFF0F5 )      /*!< µ­×Ïºì */
#define RGB_PALEVIOLETRED                           ( 0x00DB7093 )      /*!< Èõ×ÏÂÞÀ¼ºì */
#define RGB_HOTPINK                                 ( 0x00FF69B4 )      /*!< ÈÈÇéµÄ·Ûºì */
#define RGB_DEEPPINK                                ( 0x00FF1493 )      /*!< Éî·Ûºì */
#define RGB_MEDIUMVIOLETRED                         ( 0x00C71585 )      /*!< ÖÐ×ÏÂÞÀ¼ºì */
#define RGB_ORCHID                                  ( 0x00DA70D6 )      /*!< °µ×ÏÉ«/À¼»¨×Ï */
#define RGB_THISTLE                                 ( 0x00D8BFD8 )      /*!< ¼»É« */
#define RGB_PLUM                                    ( 0x00DDA0DD )      /*!< ÑóÀîÉ«/Àî×Ó×Ï */
#define RGB_VIOLET                                  ( 0x00EE82EE )      /*!< ×ÏÂÞÀ¼ */
#define RGB_MAGENTA                                 ( 0x00FF00FF )      /*!< Ñóºì/Ãµ¹åºì */
#define RGB_FUCHSIA                                 ( 0x00FF00FF )      /*!< ×Ïºì/µÆÁýº£ÌÄ */
#define RGB_DARKMAGENTA                             ( 0x008B008B )      /*!< ÉîÑóºì */
#define RGB_PURPLE                                  ( 0x00800080 )      /*!< ×ÏÉ« */
#define RGB_MEDIUMORCHID                            ( 0x00BA55D3 )      /*!< ÖÐÀ¼»¨×Ï */
#define RGB_DARKVIOLET                              ( 0x009400D3 )      /*!< °µ×ÏÂÞÀ¼ */
#define RGB_DARKORCHID                              ( 0x009932CC )      /*!< °µÀ¼»¨×Ï */
#define RGB_INDIGO                                  ( 0x004B0082 )      /*!< µåÇà/×ÏÀ¼É« */
#define RGB_BLUEVIOLET                              ( 0x008A2BE2 )      /*!< À¶×ÏÂÞÀ¼ */
#define RGB_MEDIUMPURPLE                            ( 0x009370DB )      /*!< ÖÐ×ÏÉ« */
#define RGB_MEDIUMSLATEBLUE                         ( 0x007B68EE )      /*!< ÖÐ°µÀ¶É«/ÖÐ°åÑÒÀ¶ */
#define RGB_SLATEBLUE                               ( 0x006A5ACD )      /*!< Ê¯À¶É«/°åÑÒÀ¶ */
#define RGB_DARKSLATEBLUE                           ( 0x00483D8B )      /*!< °µ»ÒÀ¶É«/°µ°åÑÒÀ¶ */
#define RGB_LAVENDER                                ( 0x00E6E6FA )      /*!< µ­×ÏÉ«/Ñ¬ÒÂ²Ýµ­×Ï */
#define RGB_GHOSTWHITE                              ( 0x00F8F8FF )      /*!< ÓÄÁé°× */
#define RGB_BLUE                                    ( 0x000000FF )      /*!< ´¿À¶ */
#define RGB_MEDIUMBLUE                              ( 0x000000CD )      /*!< ÖÐÀ¶É« */
#define RGB_MIDNIGHTBLUE                            ( 0x00191970 )      /*!< ÎçÒ¹À¶ */
#define RGB_DARKBLUE                                ( 0x0000008B )      /*!< °µÀ¶É« */
#define RGB_NAVY                                    ( 0x00000080 )      /*!< º£¾üÀ¶ */
#define RGB_ROYALBLUE                               ( 0x004169E1 )      /*!< »Ê¼ÒÀ¶/±¦À¶ */
#define RGB_CORNFLOWERBLUE                          ( 0x006495ED )      /*!< Ê¸³µ¾ÕÀ¶ */
#define RGB_LIGHTSTEELBLUE                          ( 0x00B0C4DE )      /*!< ÁÁ¸ÖÀ¶ */
#define RGB_LIGHTSLATEGRAY                          ( 0x00778899 )      /*!< ÁÁÀ¶»Ò/ÁÁÊ¯°å»Ò */
#define RGB_SLATEGRAY                               ( 0x00708090 )      /*!< »ÒÊ¯É«/Ê¯°å»Ò */
#define RGB_DODGERBLUE                              ( 0x001E90FF )      /*!< ÉÁÀ¼É«/µÀÆæÀ¶ */
#define RGB_ALICEBLUE                               ( 0x00F0F8FF )      /*!< °®ÀöË¿À¶ */
#define RGB_STEELBLUE                               ( 0x004682B4 )      /*!< ¸ÖÀ¶/ÌúÇà */
#define RGB_LIGHTSKYBLUE                            ( 0x0087CEFA )      /*!< ÁÁÌìÀ¶É« */
#define RGB_SKYBLUE                                 ( 0x0087CEEB )      /*!< ÌìÀ¶É« */
#define RGB_DEEPSKYBLUE                             ( 0x0000BFFF )      /*!< ÉîÌìÀ¶ */
#define RGB_LIGHTBLUE                               ( 0x00ADD8E6 )      /*!< ÁÁÀ¶ */
#define RGB_POWDERBLUE                              ( 0x00B0E0E6 )      /*!< ·ÛÀ¶É«/»ðÒ©Çà */
#define RGB_CADETBLUE                               ( 0x005F9EA0 )      /*!< ¾üÀ¼É«/¾ü·þÀ¶ */
#define RGB_AZURE                                   ( 0x00F0FFFF )      /*!< ÎµÀ¶É« */
#define RGB_LIGHTCYAN                               ( 0x00E0FFFF )      /*!< µ­ÇàÉ« */
#define RGB_PALETURQUOISE                           ( 0x00AFEEEE )      /*!< ÈõÂÌ±¦Ê¯ */
#define RGB_CYAN                                    ( 0x0000FFFF )      /*!< ÇàÉ« */
#define RGB_AQUA                                    ( 0x0000FFFF )      /*!< Ç³ÂÌÉ«/Ë®É« */
#define RGB_DARKTURQUOISE                           ( 0x0000CED1 )      /*!< °µÂÌ±¦Ê¯ */
#define RGB_DARKSLATEGRAY                           ( 0x002F4F4F )      /*!< °µÍß»ÒÉ«/°µÊ¯°å»Ò */
#define RGB_DARKCYAN                                ( 0x00008B8B )      /*!< °µÇàÉ« */
#define RGB_TEAL                                    ( 0x00008080 )      /*!< Ë®Ñ¼É« */
#define RGB_MEDIUMTURQUOISE                         ( 0x0048D1CC )      /*!< ÖÐÂÌ±¦Ê¯ */
#define RGB_LIGHTSEAGREEN                           ( 0x0020B2AA )      /*!< Ç³º£ÑóÂÌ */
#define RGB_TURQUOISE                               ( 0x0040E0D0 )      /*!< ÂÌ±¦Ê¯ */
#define RGB_AQUAMARINE                              ( 0x007FFFD4 )      /*!< ±¦Ê¯±ÌÂÌ */
#define RGB_MEDIUMAQUAMARINE                        ( 0x0066CDAA )      /*!< ÖÐ±¦Ê¯±ÌÂÌ */
#define RGB_MEDIUMSPRINGGREEN                       ( 0x0000FA9A )      /*!< ÖÐ´ºÂÌÉ« */
#define RGB_MINTCREAM                               ( 0x00F5FFFA )      /*!< ±¡ºÉÄÌÓÍ */
#define RGB_SPRINGGREEN                             ( 0x0000FF7F )      /*!< ´ºÂÌÉ« */
#define RGB_MEDIUMSEAGREEN                          ( 0x003CB371 )      /*!< ÖÐº£ÑóÂÌ */
#define RGB_SEAGREEN                                ( 0x002E8B57 )      /*!< º£ÑóÂÌ */
#define RGB_HONEYDEW                                ( 0x00F0FFF0 )      /*!< ÃÛÉ«/ÃÛ¹ÏÉ« */
#define RGB_LIGHTGREEN                              ( 0x0090EE90 )      /*!< µ­ÂÌÉ« */
#define RGB_PALEGREEN                               ( 0x0098FB98 )      /*!< ÈõÂÌÉ« */
#define RGB_DARKSEAGREEN                            ( 0x008FBC8F )      /*!< °µº£ÑóÂÌ */
#define RGB_LIMEGREEN                               ( 0x0032CD32 )      /*!< ÉÁ¹âÉîÂÌ */
#define RGB_LIME                                    ( 0x0000FF00 )      /*!< ÉÁ¹âÂÌ */
#define RGB_FORESTGREEN                             ( 0x00228B22 )      /*!< É­ÁÖÂÌ */
#define RGB_GREEN                                   ( 0x00008000 )      /*!< ´¿ÂÌ */
#define RGB_DARKGREEN                               ( 0x00006400 )      /*!< °µÂÌÉ« */
#define RGB_CHARTREUSE                              ( 0x007FFF00 )      /*!< »ÆÂÌÉ«/²éÌØ¾ÆÂÌ */
#define RGB_LAWNGREEN                               ( 0x007CFC00 )      /*!< ²ÝÂÌÉ«/²ÝÆºÂÌ */
#define RGB_GREENYELLOW                             ( 0x00ADFF2F )      /*!< ÂÌ»ÆÉ« */
#define RGB_DARKOLIVEGREEN                          ( 0x00556B2F )      /*!< °µéÏé­ÂÌ */
#define RGB_YELLOWGREEN                             ( 0x009ACD32 )      /*!< »ÆÂÌÉ« */
#define RGB_OLIVEDRAB                               ( 0x006B8E23 )      /*!< éÏé­ºÖÉ« */
#define RGB_BEIGE                                   ( 0x00F5F5DC )      /*!< Ã×É«/»Ò×ØÉ« */
#define RGB_LIGHTGOLDENRODYELLOW                    ( 0x00FAFAD2 )      /*!< ÁÁ¾Õ»Æ */
#define RGB_IVORY                                   ( 0x00FFFFF0 )      /*!< ÏóÑÀÉ« */
#define RGB_LIGHTYELLOW                             ( 0x00FFFFE0 )      /*!< Ç³»ÆÉ« */
#define RGB_YELLOW                                  ( 0x00FFFF00 )      /*!< ´¿»Æ */
#define RGB_OLIVE                                   ( 0x00808000 )      /*!< éÏé­ */
#define RGB_DARKKHAKI                               ( 0x00BDB76B )      /*!< °µ»ÆºÖÉ«/Éî¿¨ß´²¼ */
#define RGB_LEMONCHIFFON                            ( 0x00FFFACD )      /*!< ÄûÃÊ³ñ */
#define RGB_PALEGOLDENROD                           ( 0x00EEE8AA )      /*!< »Ò¾Õ»Æ/²Ô÷è÷ëÉ« */
#define RGB_KHAKI                                   ( 0x00F0E68C )      /*!< »ÆºÖÉ«/¿¨ß´²¼ */
#define RGB_GOLD                                    ( 0x00FFD700 )      /*!< ½ðÉ« */
#define RGB_CORNSILK                                ( 0x00FFF8DC )      /*!< ÓñÃ×Ë¿É« */
#define RGB_GOLDENROD                               ( 0x00DAA520 )      /*!< ½ð¾Õ»Æ */
#define RGB_DARKGOLDENROD                           ( 0x00B8860B )      /*!< °µ½ð¾Õ»Æ */
#define RGB_FLORALWHITE                             ( 0x00FFFAF0 )      /*!< »¨µÄ°×É« */
#define RGB_OLDLACE                                 ( 0x00FDF5E6 )      /*!< ÀÏ»¨É«/¾ÉÀÙË¿ */
#define RGB_WHEAT                                   ( 0x00F5DEB3 )      /*!< Ç³»ÆÉ«/Ð¡ÂóÉ« */
#define RGB_MOCCASIN                                ( 0x00FFE4B5 )      /*!< Â¹Æ¤É«/Â¹Æ¤Ñ¥ */
#define RGB_ORANGE                                  ( 0x00FFA500 )      /*!< ³ÈÉ« */
#define RGB_PAPAYAWHIP                              ( 0x00FFEFD5 )      /*!< ·¬Ä¾É«/·¬Ä¾¹Ï */
#define RGB_BLANCHEDALMOND                          ( 0x00FFEBCD )      /*!< °×ÐÓÉ« */
#define RGB_NAVAJOWHITE                             ( 0x00FFDEAD )      /*!< ÄÉÍß°×/ÍÁÖø°× */
#define RGB_ANTIQUEWHITE                            ( 0x00FAEBD7 )      /*!< ¹Å¶­°× */
#define RGB_TAN                                     ( 0x00D2B48C )      /*!< ²èÉ« */
#define RGB_BURLYWOOD                               ( 0x00DEB887 )      /*!< Ó²Ä¾É« */
#define RGB_BISQUE                                  ( 0x00FFE4C4 )      /*!< ÌÕÅ÷»Æ */
#define RGB_DARKORANGE                              ( 0x00FF8C00 )      /*!< Éî³ÈÉ« */
#define RGB_LINEN                                   ( 0x00FAF0E6 )      /*!< ÑÇÂé²¼ */
#define RGB_PERU                                    ( 0x00CD853F )      /*!< ÃØÂ³É« */
#define RGB_PEACHPUFF                               ( 0x00FFDAB9 )      /*!< ÌÒÈâÉ« */
#define RGB_SANDYBROWN                              ( 0x00F4A460 )      /*!< É³×ØÉ« */
#define RGB_CHOCOLATE                               ( 0x00D2691E )      /*!< ÇÉ¿ËÁ¦É« */
#define RGB_SADDLEBROWN                             ( 0x008B4513 )      /*!< ÖØºÖÉ«/Âí°°×ØÉ« */
#define RGB_SEASHELL                                ( 0x00FFF5EE )      /*!< º£±´¿Ç */
#define RGB_SIENNA                                  ( 0x00A0522D )      /*!< »ÆÍÁô÷É« */
#define RGB_LIGHTSALMON                             ( 0x00FFA07A )      /*!< Ç³öÙÓãÈâÉ« */
#define RGB_CORAL                                   ( 0x00FF7F50 )      /*!< Éºº÷ */
#define RGB_ORANGERED                               ( 0x00FF4500 )      /*!< ³ÈºìÉ« */
#define RGB_DARKSALMON                              ( 0x00E9967A )      /*!< ÉîÏÊÈâ/öÙÓãÉ« */
#define RGB_TOMATO                                  ( 0x00FF6347 )      /*!< ·¬ÇÑºì */
#define RGB_MISTYROSE                               ( 0x00FFE4E1 )      /*!< Ç³Ãµ¹åÉ«/±¡ÎíÃµ¹å */
#define RGB_SALMON                                  ( 0x00FA8072 )      /*!< ÏÊÈâ/öÙÓãÉ« */
#define RGB_SNOW                                    ( 0x00FFFAFA )      /*!< Ñ©°×É« */
#define RGB_LIGHTCORAL                              ( 0x00F08080 )      /*!< µ­Éºº÷É« */
#define RGB_ROSYBROWN                               ( 0x00BC8F8F )      /*!< Ãµ¹å×ØÉ« */
#define RGB_INDIANRED                               ( 0x00CD5C5C )      /*!< Ó¡¶Èºì */
#define RGB_RED                                     ( 0x00FF0000 )      /*!< ´¿ºì */
#define RGB_BROWN                                   ( 0x00A52A2A )      /*!< ×ØÉ« */
#define RGB_FIREBRICK                               ( 0x00B22222 )      /*!< »ð×©É«/ÄÍ»ð×© */
#define RGB_DARKRED                                 ( 0x008B0000 )      /*!< ÉîºìÉ« */
#define RGB_MAROON                                  ( 0x00800000 )      /*!< ÀõÉ« */
#define RGB_WHITE                                   ( 0x00FFFFFF )      /*!< ´¿°× */
#define RGB_WHITESMOKE                              ( 0x00F5F5F5 )      /*!< °×ÑÌ */
#define RGB_GAINSBORO                               ( 0x00DCDCDC )      /*!< µ­»ÒÉ« */
#define RGB_LIGHTGRAY                               ( 0x00D3D3D3 )      /*!< Ç³»ÒÉ« */
#define RGB_SILVER                                  ( 0x00C0C0C0 )      /*!< Òø»ÒÉ« */
#define RGB_DARKGRAY                                ( 0x00A9A9A9 )      /*!< Éî»ÒÉ« */
#define RGB_GRAY                                    ( 0x00808080 )      /*!< »ÒÉ« */
#define RGB_GRAY0                                   ( 0x00408080 )      /*!< »ÒÉ«0 */
#define	RGB_GRAY1                                   ( 0x00808480 )      /*!< »ÒÉ«1 */
#define RGB_GRAY2                                   ( 0x00E8ECE8 )      /*!< »ÒÉ«2 */
#define RGB_DIMGRAY                                 ( 0x00696969 )      /*!< °µµ­»Ò */
#define RGB_BLACK                                   ( 0x00000000 )      /*!< ´¿É« */

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
#define RGB_BROWN 			                        ( 0X0000BC40 )      /*!< ×ØÉ« */
#define RGB_FIREBRICK 			                    ( 0X0000FC07 )      /*!< ×ØºìÉ«¡¢»ð×©É« */
#define RGB_GRAY  			                        ( 0X00008430 )      /*!< »ÒÉ« */
#define RGB_GRAY0  		                            ( 0x0000EF7D )      /*!< »ÒÉ«0 */
#define	RGB_GRAY1   		                        ( 0x00008410 )      /*!< »ÒÉ«1 */
#define RGB_GRAY2  		                            ( 0x00004208 )      /*!< »ÒÉ«2 */
/*!< PANEL color */
#define RGB_DARKBLUE      	                        ( 0x000001CF )      /*!< °µÀ¶É« */
#define RGB_LIGHTBLUE      	                        ( 0x00007D7C )      /*!< Ç³À¶É«  */
#define RGB_GRAYBLUE       	                        ( 0x00005458 )      /*!< »ÒÀ¶É« */
#define RGB_LIGHTGREEN     	                        ( 0x0000841F )      /*!< Ç³ÂÌÉ« */
#define	RGB_LIGHTGRAY                               ( 0x0000EF5B )      /*!< Ç³»ÒÉ«(PANNEL) */
#define RGB_LGRAY 			                        ( 0x0000C618 )      /*!< Ç³»ÒÉ«(PANNEL), ´°Ìå±³¾°É« */
#define RGB_LGRAYBLUE                               ( 0x0000A651 )      /*!< Ç³»ÒÀ¶É«(ÖÐ¼ä²ãÑÕÉ«) */
#define	RGB_LBBLUE                                  ( 0x00002B12 )      /*!< Ç³×ØÀ¶É«(Ñ¡ÔñÌõÄ¿µÄ·´É«) */
#endif

#endif /*!< __FWK_RGBMAP_H_ */
