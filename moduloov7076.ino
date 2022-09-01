#include <stdint.h> 

#include <avr/io.h> 

#include <util/twi.h> 

#include <util/delay.h> 

#include <avr/pgmspace.h>


#define F_CPU 16000000UL 

#define vga 0 

#define qvga 1 

#define qqvga 2 

#define yuv422 0 

#define rgb565 1 

#define bayerRGB 2 

#define camAddr_WR 0x42 

#define camAddr_RD 0x43


/* Registros */ 

#define REG_GAIN 0x00 /* Ganancia de 8 bits inferiores (resto en vref) */ 

#define REG_BLUE 0x01 /* ganancia de azul */ 

#define REG_RED 0x02 /* ganancia de rojo */ 

#define REG_VREF 0x03 /* Piezas de GAIN, VSTART, VSTOP */ 

#define REG_COM1 0x04 /* Control 1 */ 

#define COM1_CCIR656 0x40 /* CCIR656 habilitado */


#define REG_BAVE 0x05 /* U/B Nivel medio */ 

#define REG_GbAVE 0x06 /* Y/Gb Nivel medio */ 

#define REG_AECHH 0x07 /* AEC MS 5 bits */ 

#define REG_RAVE 0x08 /* V/R Nivel medio * / 

#define REG_COM2 0x09 /* Control 2 */ 

#define COM2_SSLEEP 0x10 /* Modo de suspensión suave */ 

#define REG_PID 0x0a /* ID de producto MSB */ 

#define REG_VER 0x0b /* ID de producto LSB */ 

#define REG_COM3 0x0c /* Control 3 */ 

#define COM3_SWAP 0x40 /* Intercambio de bytes */ 

#define COM3_SCALEEN 0x08 /* Habilita escalado */ 

#define COM3_DCWEN 0x04 /* Habilita downsamp/crop/window */ 

#define REG_COM4 0x0d /* Control 4 */

#define REG_COM5 0x0e /* Todo "reservado" */ 

#define REG_COM6 0x0f /* Control 6 */ 

#define REG_AECH 0x10 /* Más bits del valor AEC */ 

#define REG_CLKRC 0x11 /* Clocl control */ 

#define CLK_EXT 0x40 / * Usar reloj externo directamente */ 

#define CLK_SCALE 0x3f /* Máscara para escala de reloj interno */ 

#define REG_COM7 0x12 /* Control 7 */ //REG dirección media. 

#define COM7_RESET 0x80 /* Restablecer registro */ 

#define COM7_FMT_MASK 0x38 

#define COM7_FMT_VGA 0x00 

#define COM7_FMT_CIF 0x20 /* Formato CIF */ 

#define COM7_FMT_QVGA 0x10 /* Formato QVGA */ 

#define COM7_FMT_QCIF 0x08 /* Formato QCIF */

#define COM7_RGB 0x04 /* bits 0 y 2 - formato RGB */ 

#define COM7_YUV 0x00 /* YUV */ 

#define COM7_BAYER 0x01 /* formato Bayer */ 

#define COM7_PBAYER 0x05 /* "Bayer procesado" */ 

#define REG_COM8 0x13 /* Control 8 */ 

#define COM8_FASTAEC 0x80 /* Activar AGC/AEC rápido */ 

#define COM8_AECSTEP 0x40 /* Tamaño de paso de AEC ilimitado */ 

#define COM8_BFILT 0x20 /* Activar filtro de banda */ 

#define COM8_AGC 0x04 /* Ganancia automática habilitar */ 

#define COM8_AWB 0x02 /* Balance de blancos habilitado */ 

#define COM8_AEC 0x01 /* Exposición automática habilitada */ 

#define REG_COM9 0x14 /* Control de 9- techo de ganancia */

#define REG_COM10 0x15 /* Control 10 */ 

#define COM10_HSYNC 0x40 /* HSYNC en lugar de HREF */ 

#define COM10_PCLK_HB 0x20 /* Suprime PCLK en blanco horizontal */ 

#define COM10_HREF_REV 0x08 /* HREF inversa */ 

#define COM10_VS_LEAD 0x04 / * VSYNC en el borde de ataque del reloj */ 

#define COM10_VS_NEG 0x02 /* VSYNC negativo */ 

#define COM10_HS_NEG 0x01 /* HSYNC negativo */ 

#define REG_HSTART 0x17 /* Bits altos de inicio horizontal */ 

#define REG_HSTOP 0x18 /* Bits altos de parada horizontal */ 

#define REG_VSTART 0x19 /* Bits altos de inicio vertical */ 

#define REG_VSTOP 0x1a /* Bits altos de parada vertical */ 

#define REG_PSHFT 0x1b /* Retardo de píxeles después de HREF */

#define REG_MIDH 0x1c /* Manuf. ID alto */ 

#define REG_MIDL 0x1d /* Manuf. ID bajo */ 

#define REG_MVFP 0x1e /* Mirror / vflip */ 

#define MVFP_MIRROR 0x20 /* Mirror image */ 

#define MVFP_FLIP 0x10 /* Vertical flip */


#define REG_AEW 0x24 /* Límite superior de AGC */ 

#define REG_AEB 0x25 /* Límite inferior de AGC */ 

#define REG_VPT 0x26 /* Región de operación de modo rápido de AGC/AEC */ 

#define REG_HSYST 0x30 /* Retardo de flanco ascendente de HSYNC */ 

# define REG_HSYEN 0x31 /* Retardo de flanco descendente de HSYNC */ 

#define REG_HREF 0x32 /* Piezas de HREF */ 

#define REG_TSLB 0x3a /* muchas cosas */ 

#define TSLB_YLAST 0x04 /* UYVY o VYUY - ver com13 */ 

#define REG_COM11 0x3b /* Control 11 */ 

#define COM11_NIGHT 0x80 /* Activar modo nocturno */ 

#define COM11_NMFR 0x60 /* Velocidad de fotogramas NM de dos bits */ 

#define COM11_HZAUTO 0x10 /* Detección automática 50/60 Hz */

#define COM11_50HZ 0x08 /* Selección manual de 50 Hz */ 

#define COM11_EXP 0x02 

#define REG_COM12 0x3c /* Control 12 */ 

#define COM12_HREF 0x80 /* HREF siempre */ 

#define REG_COM13 0x3d /* Control 13 */ 

#define COM13_GAMMA 0x80 / * Activar gamma */ 

#define COM13_UVSAT 0x40 /* Ajuste automático de saturación UV */ 

#define COM13_UVSWAP 0x01 /* V antes de U - con TSLB */ 

#define REG_COM14 0x3e /* Control 14 */ 

#define COM14_DCWEN 0x10 /* DCW/ Habilitar escala PCLK */ 

#define REG_EDGE 0x3f /* Factor de mejora de borde */ 

#define REG_COM15 0x40 /* Control 15 */ 

#define COM15_R10F0 0x00 /* Rango de datos 10 a F0 */

#define COM15_R01FE 0x80 /* 01 a FE */ 

#define COM15_R00FF 0xc0 /* 00 a FF */ 

#define COM15_RGB565 0x10 /* Salida RGB565 */ 

#define COM15_RGB555 0x30 /* Salida RGB555 */ 

#define REG_COM16 0x41 /* Control 16 */ 

#define COM16_AWGBAIN 0x08 /* Activar ganancia AWB */ 

#define REG_COM17 0x42 /* Control 17 */ 

#define COM17_AECWIN 0xc0 /* Ventana AEC: debe coincidir con COM4 */ 

#define COM17_CBAR 0x08 /* Barra de color DSP */ 

/* 

* Esta matriz define cómo se generan los colores, se debe 

* ajustar para ajustar el tono y la saturación. 

* 

* Orden: v-rojo, v-verde, v-azul, u-rojo, u-verde, u-azul

* Son cantidades con signo de nueve bits, con el bit de signo 

* almacenado en 0x58. El signo para v-red es el bit 0, y hacia arriba desde allí. 

*/ 

#define REG_CMATRIX_BASE 0x4f 

#define CMATRIX_LEN 6 

#define REG_CMATRIX_SIGN 0x58 

#define REG_BRIGHT 0x55 /* Brillo */ 

#define REG_CONTRAS 0x56 /* Control de contraste */ 

#define REG_GFIX 0x69 /* Control de ganancia fijo */ 

#define REG_REG76 0x76 /* Nombre de OV */ 

#define R76_BLKPCOR 0x80 /* Activar corrección de píxeles negros */ 

#define R76_WHTPCOR 0x40 /* Activar corrección de píxeles blancos */ 

#define REG_RGB444 0x8c /* Control RGB 444 */ 

#define R444_ENABLE 0x02 /* Activar RGB444, anula 5x5 */

#define R444_RGBX 0x01 /* Nibble vacío al final */ 

#define REG_HAECC1 0x9f /* Hist AEC/AGC control 1 */ 

#define REG_HAECC2 0xa0 /* Hist AEC/AGC control 2 */ 

#define REG_BD50MAX 0xa5 /* Límite de paso de banda de 50 Hz */ 

#define REG_HAECC3 0xa6 /* Hist AEC/AGC control 3 */ 

#define REG_HAECC4 0xa7 /* Hist AEC/AGC control 4 */ 

#define REG_HAECC5 0xa8 /* Hist AEC/AGC control 5 */ 

#define REG_HAECC6 0xa9 /* Hist AEC/AGC control 6 */ 

#define REG_HAECC7 0xaa /* Hist AEC/AGC control 7 */ 

#define REG_BD60MAX 0xab /* Límite de paso de banda de 60 Hz */ 

#define REG_GAIN 0x00 /* Ganancia de 8 bits inferiores (resto en vref) * / 

#define REG_BLUE 0x01 /* ganancia azul */

#define REG_RED 0x02 /* ganancia de rojo */ 

#define REG_VREF 0x03 /* Piezas de GAIN, VSTART, VSTOP */ 

#define REG_COM1 0x04 /* Control 1 */ 

#define COM1_CCIR656 0x40 /* CCIR656 habilitado */ 

#define REG_BAVE 0x05 / * Nivel medio U/B */ 

#define REG_GbAVE 0x06 /* Nivel medio Y/Gb */ 

#define REG_AECHH 0x07 /* AEC MS 5 bits */ 

#define REG_RAVE 0x08 /* Nivel medio V/R */ 

#define REG_COM2 0x09 /* Control 2 */ 

#define COM2_SSLEEP 0x10 /* Modo de suspensión suave */ 

#define REG_PID 0x0a /* ID de producto MSB */ 

#define REG_VER 0x0b /* ID de producto LSB */ 

#define REG_COM3 0x0c /* Control 3 */

#define COM3_SWAP 0x40 /* Intercambio de bytes */ 

#define COM3_SCALEEN 0x08 /* Habilitar escalado */ 

#define COM3_DCWEN 0x04 /* Habilitar downsamp/crop/window */ 

#define REG_COM4 0x0d /* Control 4 */ 

#define REG_COM5 0x0e /* Todo "reservado" */ 

#define REG_COM6 0x0f /* Control 6 */ 

#define REG_AECH 0x10 /* Más bits de valor AEC */ 

#define REG_CLKRC 0x11 /* Control de reloj */ 

#define CLK_EXT 0x40 /* Usar reloj externo directamente * / 

#define CLK_SCALE 0x3f /* Máscara para escala de reloj interna */ 

#define REG_COM7 0x12 /* Control 7 */ 

#define COM7_RESET 0x80 /* Restablecimiento de registro */ 

#define COM7_FMT_MASK 0x38

#define COM7_FMT_VGA 0x00 

#define COM7_FMT_CIF 0x20 /* formato CIF */ 

#define COM7_FMT_QVGA 0x10 /* formato QVGA */ 

#define COM7_FMT_QCIF 0x08 /* formato QCIF */ 

#define COM7_RGB 0x04 /* bits 0 y 2 - formato RGB */ 

# define COM7_YUV 0x00 /* YUV */ 

#define COM7_BAYER 0x01 /* Formato Bayer */ 

#define COM7_PBAYER 0x05 /* "Bayer procesado" */ 

#define REG_COM8 0x13 /* Control 8 */ 

#define COM8_FASTAEC 0x80 /* Habilita AGC rápido/ AEC */ 

#define COM8_AECSTEP 0x40 /* Tamaño de paso AEC ilimitado */ 

#define COM8_BFILT 0x20 /* Habilitación de filtro de banda */ 

#define COM8_AGC 0x04 /* Habilitación de ganancia automática */

#define COM8_AWB 0x02 /* Activar balance de blancos */ 

#define COM8_AEC 0x01 /* Activación de exposición automática */ 

#define REG_COM9 0x14 /* Control 9-ganancia máxima */ 

#define REG_COM10 0x15 /* Control 10 */ 

#define COM10_HSYNC 0x40 / * HSYNC en lugar de HREF */ 

#define COM10_PCLK_HB 0x20 /* Suprime PCLK en horiz en blanco */ 

#define COM10_HREF_REV 0x08 /* Reverse HREF */ 

#define COM10_VS_LEAD 0x04 /* VSYNC en el borde de entrada del reloj */ 

#define COM10_VS_NEG 0x02 /* VSYNC negativo */ 

#define COM10_HS_NEG 0x01 /* HSYNC negativo */ 

#define REG_HSTART 0x17 /* Bits altos de inicio horizontal */ 

#define REG_HSTOP 0x18 /* Bits altos de parada horizontal */

#define REG_VSTART 0x19 /* Bits altos de inicio vertical */ 

#define REG_VSTOP 0x1a /* Bits altos de parada vertical */ 

#define REG_PSHFT 0x1b /* Retardo de píxeles después de HREF */ 

#define REG_MIDH 0x1c /* Manuf. ID alto */ 

#define REG_MIDL 0x1d /* Manuf. ID bajo */ 

#define REG_MVFP 0x1e /* Mirror / vflip */ 

#define MVFP_MIRROR 0x20 /* Mirror image */ 

#define MVFP_FLIP 0x10 /* Vertical flip */ 

#define REG_AEW 0x24 /* Límite superior de AGC */ 

#define REG_AEB 0x25 /* Límite inferior de AGC */ 

#define REG_VPT 0x26 /* Región de operación de modo rápido de AGC/AEC */ 

#define REG_HSYST 0x30 /* Retardo de flanco ascendente de HSYNC */ 

#define REG_HSYEN 0x31 /* Retardo de flanco descendente de HSYNC */

#define REG_HREF 0x32 /* Piezas HREF */ 

#define REG_TSLB 0x3a /* muchas cosas */ 

#define TSLB_YLAST 0x04 /* UYVY o VYUY - ver com13 */ 

#define REG_COM11 0x3b /* Control 11 */ 

#define COM11_NIGHT 0x80 / * Habilitar modo nocturno */ 

#define COM11_NMFR 0x60 /* Velocidad de fotogramas NM de dos bits */ 

#define COM11_HZAUTO 0x10 /* Detección automática 50/60 Hz */ 

#define COM11_50HZ 0x08 /* Selección manual de 50 Hz */ 

#define COM11_EXP 0x02 

#define REG_COM12 0x3c /* Control 12 */ 

#define COM12_HREF 0x80 /* HREF siempre */ 

#define REG_COM13 0x3d /* Control 13 */ 

#define COM13_GAMMA 0x80 /* Gamma habilitado */

#define COM13_UVSAT 0x40 /* Ajuste automático de saturación UV */ 

#define COM13_UVSWAP 0x01 /* V antes de U - con TSLB */ 

#define REG_COM14 0x3e /* Control 14 */ 

#define COM14_DCWEN 0x10 /* Activación de escala DCW/PCLK * / 

#define REG_EDGE 0x3f /* Factor de mejora de borde */ 

#define REG_COM15 0x40 /* Control 15 */ 

#define COM15_R10F0 0x00 /* Rango de datos 10 a F0 */ 

#define COM15_R01FE 0x80 /* 01 a FE */ 

#define COM15_R00FF 0xc0 /* 00 a FF */ 

#define COM15_RGB565 0x10 /* Salida RGB565 */ 

#define COM15_RGB555 0x30 /* Salida RGB555 */ 

#define REG_COM16 0x41 /* Control 16 */

#define COM16_AWGBAIN 0x08 /* Activar ganancia AWB */ 

#define REG_COM17 0x42 /* Control 17 */ 

#define COM17_AECWIN 0xc0 /* Ventana AEC: debe coincidir con COM4 */ 

#define COM17_CBAR 0x08 /* Barra de color DSP */


#define CMATRIX_LEN 6 

#define REG_BRIGHT 0x55 /* Brillo */ 

#define REG_REG76 0x76 /* Nombre del OV */ 

#define R76_BLKPCOR 0x80 /* Activación de corrección de píxeles negros */ 

#define R76_WHTPCOR 0x40 /* Activación de corrección de píxeles blancos */ 

#define REG_RGB444 0x8c /* Control RGB 444 */ 

#define R444_ENABLE 0x02 /* Activar RGB444, anula 5x5 */ 

#define R444_RGBX 0x01 /* Nibble vacío al final */ 

#define REG_HAECC1 0x9f /* Hist AEC/AGC control 1 */ 

#define REG_HAECC2 0xa0 /* Hist AEC/AGC control 2 */ 

#define REG_BD50MAX 0xa5 /* Límite de paso de bandas de 50 Hz */ 

#define REG_HAECC3 0xa6 /* Hist AEC/AGC control 3 */

#define REG_HAECC4 0xa7 /* Hist AEC/AGC control 4 */ 

#define REG_HAECC5 0xa8 /* Hist AEC/AGC control 5 */ 

#define REG_HAECC6 0xa9 /* Hist AEC/AGC control 6 */ 

#define REG_HAECC7 0xaa /* Hist AEC /AGC control 7 */ 

#define REG_BD60MAX 0xab /* Límite de paso de banda de 60 Hz */ 

#define MTX1 0x4f /* Coeficiente de matriz 1 */ 

#define MTX2 0x50 /* Coeficiente de matriz 2 */ 

#define MTX3 0x51 /* Coeficiente de matriz 3 * / 

#define MTX4 0x52 /* Coeficiente de matriz 4 */ 

#define MTX5 0x53 /* Coeficiente de matriz 5 */ 

#define MTX6 0x54 /* Coeficiente de matriz 6 */ 

#define REG_CONTRAS 0x56 /* Control de contraste */

#define MTXS 0x58 /* Signo de coeficiente de matriz */ 

#define AWBC7 0x59 /* Control AWB 7 */ 

#define AWBC8 0x5a /* Control AWB 8 */ 

#define AWBC9 0x5b /* Control AWB 9 */ 

#define AWBC10 0x5c /* Control AWB 10 */ 

#define AWBC11 0x5d /* Control AWB 11 */ 

#define AWBC12 0x5e /* Control AWB 12 */ 

#define REG_GFI 0x69 /* Control de ganancia fijo */ 

#define GGAIN 0x6a /* Ganancia AWB del canal G */ 

#define DBLV 0x6b   

#define AWBCTR3 0x6c /* Control AWB 3 */ 

#define AWBCTR2 0x6d /* Control AWB 2 */ 

#define AWBCTR1 0x6e /* Control AWB 1 */

#define AWBCTR0 0x6f /* Control AWB 0 */


estructura regval_list{ 

  uint8_t reg_num; 

  uint16_t valor; 

};


const estructura regval_list qvga_ov7670[] PROGMEM = { 

  { REG_COM14, 0x19 }, 

  { 0x72, 0x11 }, 

  { 0x73, 0xf1 },


  {REG_HSTART, 0x16}, 

  {REG_HSTOP, 0x04}, 

  {REG_HREF, 0xa4}, 

  {REG_VSTART, 0x02}, 

  {REG_VSTOP, 0x7a}, 

  {REG_VREF, 0x0a},


  { 0xff, 0xff }, /* MARCADOR DE FIN */ 

};


const struct regval_list yuv422_ov7670[] PROGMEM = { 

  { REG_COM7, 0x0 }, /* Selecciona el modo YUV */ 

  { REG_RGB444, 0 }, /* No RGB444 por favor */ 

  { REG_COM1, 0 }, 

  { REG_COM15, COM15_R00FF }, 

  { REG_COM9, 0x6A }, /* techo de ganancia de 128x; 0x8 es un bit reservado */ 

  { 0x4f, 0x80 }, /* "coeficiente de matriz 1" */ 

  { 0x50, 0x80 }, /* "coeficiente de matriz 2" */ 

  { 0x51, 0 }, /* vb */ 

  { 0x52, 0x22 }, /* "coeficiente de matriz 4" */ 

  { 0x53, 0x5e }, /* "coeficiente de matriz 5" */ 

  { 0x54, 0x80 }, /* "coeficiente de matriz 6" */ 

  { REG_COM13, COM13_UVSAT }, 

  { 0xff , 0xff }, /* MARCADOR DE FIN */ 

};


const struct regval_list ov7670_default_regs[] PROGMEM = {//desde el controlador de Linux 

  { REG_COM7, COM7_RESET }, 

  { REG_TSLB, 0x04 }, /* OV */ 

  { REG_COM7, 0 }, /* VGA */ 

  /* 

  * Establecer la ventana de hardware . Estos valores de OV no 

  * tienen sentido del todo - hstop es menor que hstart. Pero funcionan... 

  */ 

  { REG_HSTART, 0x13 }, { REG_HSTOP, 0x01 }, 

  { REG_HREF, 0xb6 }, { REG_VSTART, 0x02 }, 

  { REG_VSTOP, 0x7a }, { REG_VREF, 0x0a },


  { REG_COM3, 0 }, { REG_COM14, 0 }, 

  /* Números de escala misteriosos */ 

  { 0x70, 0x3a }, { 0x71, 0x35 }, 

  { 0x72, 0x11 }, { 0x73, 0xf0 }, 

  { 0xa2,/* 0x02 cambiado a 1*/1 }, { REG_COM10, 0x0 }, 

  /* Valores de la curva gamma */ 

  { 0x7a, 0x20 }, { 0x7b, 0x10 }, 

  { 0x7c, 0x1e }, { 0x7d, 0x35 }, 

  { 0x7e, 0x5a }, { 0x7f, 0x69 }, 

  { 0x80, 0x76 }, { 0x81, 0x80 }, 

  { 0x82, 0x88 }, { 0x83, 0x8f }, 

  { 0x84, 0x96 }, { 0x85, 0xa3 }, 

  { 0x86, 0xaf }, { 0x87 , 0xc4 }, 

  { 0x88, 0xd7 }, { 0x89, 0xe8 }, 

  /* Parámetros AGC y AEC. Tenga en cuenta que comenzamos por deshabilitar esas funciones, 

  luego las activamos solo después de ajustar los valores. */

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP }, 

  { REG_GAIN, 0 }, { REG_AECH, 0 }, 

  { REG_COM4, ​​0x40 }, /* bit mágico reservado */ 

  { REG_COM9, 0x18 }, /* ganancia 4x + bit mágico rsvd */ 

  { REG_BD50MAX, 0x05 }, { REG_BD60MAX, 0x07 }, 

  { REG_AEW, 0x95 }, { REG_AEB, 0x33 }, 

  { REG_VPT, 0xe3 }, { REG_HAECC1, 0x78 }, 

  { REG_HAECC2, 0x68 }, { 0xa1, 0x03 }, /* magia */ 

  { REG_HAECC3, 0xd8 }, { REG_HAECC4, 0xd8 }, 

  { REG_HAECC5, 0xf0 }, { REG_HAECC6, 0x90 }, 

  { REG_HAECC7, 0x94 }, 

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC }, 

  { 0x30, 0 }, { 0x31, 0 },//deshabilita algunos retrasos 

  /* Casi todos estos son valores mágicos "reservados". */

  { REG_COM5, 0x61 }, { REG_COM6, 0x4b }, 

  { 0x16, 0x02 }, { REG_MVFP, 0x07 }, 

  { 0x21, 0x02 }, { 0x22, 0x91 }, 

  { 0x29, 0x07 }, { 0x33, 0x0b }, 

  { 0x35 , 0x0b }, { 0x37, 0x1d }, 

  { 0x38, 0x71 }, { 0x39, 0x2a }, 

  { REG_COM12, 0x78 }, { 0x4d, 0x40 }, 

  { 0x4e, 0x20 }, { REG_GFIX, 0 }, 

  /*{0x6b , 0x4a},*/{ 0x74, 0x10 }, 

  { 0x8d, 0x4f }, { 0x8e, 0 }, 

  { 0x8f, 0 }, { 0x90, 0 }, 

  { 0x91, 0 }, { 0x96, 0 }, 

  { 0x9a , 0 }, { 0xb0, 0x84 }, 

  { 0xb1, 0x0c }, { 0xb2, 0x0e }, 

  { 0xb3, 0x82 }, { 0xb8, 0x0a },


  /* Más magia reservada, parte de la cual ajusta el balance de blancos */ 

  { 0x43, 0x0a }, { 0x44, 0xf0 }, 

  { 0x45, 0x34 }, { 0x46, 0x58 }, 

  { 0x47, 0x28 }, { 0x48, 0x3a }, 

  { 0x59, 0x88 }, { 0x5a, 0x88 }, 

  { 0x5b, 0x44 }, { 0x5c, 0x67 }, 

  { 0x5d, 0x49 }, { 0x5e, 0x0e }, 

  { 0x6c, 0x0a }, { 0x6d, 0x55 }, 

  { 0x6e , 0x11 }, { 0x6f, 0x9e }, /* era 0x9F "9e para AWB avanzado" */ 

  { 0x6a, 0x40 }, { REG_BLUE, 0x40 }, 

  { REG_RED, 0x60 }, 

  { REG_COM8, COM8_FASTAEC | COM8_AECSTEP | COM8_AGC | COM8_AEC | COM8_AWB},


  /* Coeficientes de matriz */ 

  { 0x4f, 0x80 }, { 0x50, 0x80 }, 

  { 0x51, 0 }, { 0x52, 0x22 }, 

  { 0x53, 0x5e }, { 0x54, 0x80 }, 

  { 0x58, 0x9e },


  { REG_COM16, COM16_AWGBAIN }, { REG_EDGE, 0 }, 

  { 0x75, 0x05 }, { REG_REG76, 0xe1 }, 

  { 0x4c, 0 }, { 0x77, 0x01 }, 

  { REG_COM13, /*0xc3*/0x48 }, { 0x4b, 0x09 }, 

  { 0xc9, 0x60 }, /*{REG_COM16, 0x38},*/ 

  { 0x56, 0x40 },


  {0x34, 0x11}, {REG_COM11, COM11_EXP | COM11_HZAUTO }, 

  { 0xa4, 0x82/*Era 0x88*/ }, { 0x96, 0 }, 

  { 0x97, 0x30 }, { 0x98, 0x20 }, 

  { 0x99, 0x30 }, { 0x9a, 0x84 }, 

  { 0x9b, 0x29 } , {0x9c, 0x03}, 

  {0x9d, 0x4c}, {0x9e, 0x3f}, 

  {0x78, 0x04},


  /* Cosas extra raras. Algún tipo de registro multiplexor */ 

  { 0x79, 0x01 }, { 0xc8, 0xf0 }, 

  { 0x79, 0x0f }, { 0xc8, 0x00 }, 

  { 0x79, 0x10 }, { 0xc8, 0x7e }, 

  { 0x79, 0x0a }, { 0xc8, 0x80 }, 

  { 0x79, 0x0b }, { 0xc8, 0x01 }, 

  { 0x79, 0x0c }, { 0xc8, 0x0f }, 

  { 0x79, 0x0d }, { 0xc8, 0x20 }, 

  { 0x79, 0x09 }, { 0xc8, 0x80 }, 

  { 0x79, 0x02 }, { 0xc8, 0xc0 }, 

  { 0x79, 0x03 }, { 0xc8, 0x40 }, 

  { 0x79, 0x05 }, { 0xc8, 0x30 }, 

  { 0x79, 0x26 }, 

  { 0xff, 0xff } , /* MARCADOR DE FIN */ 

};



void error_led(void){ 

  DDRB |= 32;//asegúrese de que el led se emita 

  mientras (1){//espera el reinicio 

    PORTB ^= 32;// alternar led 

    _delay_ms(100); 

  } 

}


void twiStart(void){ 

  TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN);//enviar start 

  while (!(TWCR & (1 << TWINT)));//esperar a que se transmita start 

  if ((TWSR & 0xF8) != TW_START) 

    error_led(); 

}


void twiWriteByte(uint8_t DATOS, uint8_t tipo){ 

  TWDR = DATOS; 

  TWCR = _BV(TWINTO) | _BV(TWEN); 

  while (!(TWCR & (1 << TWINT))) {} 

  if ((TWSR & 0xF8) != tipo) 

    error_led(); 

}


void twiAddr(uint8_t addr, uint8_t typeTWI){ 

  TWDR = addr;//dirección de envío 

  TWCR = _BV(TWINT) | _BV(TWEN); /* borra la interrupción para iniciar la transmisión */ 

  while ((TWCR & _BV(TWINT)) == 0); /* espera la transmisión */ 

  if ((TWSR & 0xF8) != typeTWI) 

    error_led(); 

}


voidwriteReg(uint8_t reg, uint8_t dat){ 

  //enviar condición de inicio 

  twiStart(); 

  twiAddr(camAddr_WR, TW_MT_SLA_ACK); 

  twiWriteByte(reg, TW_MT_DATA_ACK); 

  twiWriteByte(datos, TW_MT_DATA_ACK); 

  TWCR = (1 << TWINT) | (1 << DOS) | (1 << TWSTO);//enviar parada 

  _delay_ms(1); 

}


estático uint8_t twiRd(uint8_t nack){ 

  if (nack){ 

    TWCR = _BV(TWINT) | _BV(TWEN); 

    while ((TWCR & _BV(TWINT)) == 0); /* espera la transmisión */ 

    if ((TWSR & 0xF8) != TW_MR_DATA_NACK) 

      error_led(); 

    devolver TWDR; 

  } 

  más{ 

    TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA); 

    while ((TWCR & _BV(TWINT)) == 0); /* espera la transmisión */ 

    if ((TWSR & 0xF8) != TW_MR_DATA_ACK) 

      error_led(); 

    devolver TWDR; 

  } 

}


uint8_t rdReg(uint8_t registro){ 

  uint8_t datos; 

  twiStart(); 

  twiAddr(camAddr_WR, TW_MT_SLA_ACK); 

  twiWriteByte(reg, TW_MT_DATA_ACK); 

  TWCR = (1 << TWINT) | (1 << DOS) | (1 << TWSTO);//enviar parada 

  _delay_ms(1); 

  twiStart(); 

  twiAddr(camAddr_RD, TW_MR_SLA_ACK); 

  dat = doble(1); 

  TWCR = (1 << TWINT) | (1 << DOS) | (1 << TWSTO);//enviar parada 

  _delay_ms(1); 

  volver dat; 

}


void wrSensorRegs8_8(const struct regval_list reglist[]){ 

  uint8_t reg_addr, reg_val; 

  const struct regval_list *siguiente = lista de registro; 

  while ((reg_addr != 0xff) | (reg_val != 0xff)){ 

    reg_addr = pgm_read_byte(&next->reg_num); 

    reg_val = pgm_read_byte(&siguiente->valor); 

   escribirReg(reg_dir, reg_val); 

    siguiente++; 

  } 

}


void setColor(void){ 

  wrSensorRegs8_8(yuv422_ov7670); 

 // wrSensorRegs8_8(qvga_ov7670); 

}


void setResolution(void){ 

 writeReg(REG_COM3, 4); // REG_COM3 habilitar el escalado 

  wrSensorRegs8_8(qvga_ov7670); 

}


void camInit(void){ 

 writeReg(0x12, 0x80); 

  _retraso_ms(100); 

  wrSensorRegs8_8(ov7670_default_regs); 

 writeReg(REG_COM10, 32);//PCLK no cambia a HBLANK. 

}


void arduinoUnoInut(void) { 

  cli();//deshabilitar interrupciones 

  

    /* Configurar el reloj PWM de 8mhz 

  * Esto estará en el pin 11*/ 

  DDRB |= (1 << 3);//pin 11 

  ASSR &= ~(_BV (EXCLK)|_BV(AS2)); 

  TCCR2A = (1 << COM2A0) | (1 << WGM21) | (1 << WGM20); 

  TCCR2B = (1 << WGM22) | (1 <<CS20); 

  OCR2A = 0;//(F_CPU)/(2*(X+1)) 

  DDRC &= ~15;//cámara baja d0-d3 

  DDRD &= ~252;//d7-d4 y pines de interrupción 

  _delay_ms(3000) ; 

  

    //configurar twi para 100khz 

  TWSR &= ~3;//deshabilitar prescaler para TWI 

  TWBR = 72;//establecer a 100khz 

  

    //habilitar serial 

  UBRR0H = 0; 

  UBRR0L = 1;//0 = velocidad de transmisión de 2M. 1 = 1M baudios. 3 = 0,5 M.

  UCSR0A |= 2;//doble velocidad aysnc 

  UCSR0B = (1 << RXEN0) | (1 << TXEN0);//Habilitar receptor y transmisor 

  UCSR0C = 6;//async 1 stop bit 8bit char no parity bits 

}



void StringPgm(const char * str){ 

  do{ 

      while (!(UCSR0A & (1 << UDRE0)));//esperar byte para transmitir 

      UDR0 = pgm_read_byte_near(str); 

      while (!(UCSR0A & (1 << UDRE0)));//esperar a que se transmita el byte 

  } while (pgm_read_byte_near(++str)); 

}


static void captureImg(uint16_t wg, uint16_t hg){ 

  uint16_t y, x;


  StringPgm(PSTR("*RDY*"));


  while (!(PIND & 8));//esperar alto 

  while ((PIND & 8));//esperar bajo


    y = hectogramo; 

  while (y--){ 

        x = wg; 

      //while (!(PIND & 256));//esperar alto 

    while (x--){ 

      while ((PIND & 4));//esperar bajo 

            UDR0 = (PINC & 15) | (PIN y 240); 

          while (!(UCSR0A & (1 << UDRE0)));//esperar byte para transmitir 

      while (!(PIND & 4));//esperar alto 

      while ((PIND & 4));//esperar low 

      while (!(PIND & 4));//esperar por high 

    } 

    // while ((PIND & 256));//esperar por low 

  } 

    _delay_ms(100); 

}


configuración vacía(){ 

  arduinoUnoInut(); 

  camInit(); 

  establecerResolución(); 

  establecerColor(); 

 escribirRegistro(0x11, 10); //Antes tenía el valor:writeReg(0x11, 12); La nueva versión funciona mejor para mí :) !!!! 

}



bucle vacío(){ 

  captureImg(320, 240); 

}


