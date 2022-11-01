 //FileName:        system_functions.c
 // Dependencies:    system.h
 // Processor:       MSP432
 // Board:           MSP43P401R
 // Program Version: CCS V8.3 TI
 // Company:         Texas Instruments
 // Authors:         Jesus Adrian Guerra Delgado
 //                  Jaime Alejandro Herrera Armendariz
 //                  Andrew Joshua Barrientos Lonzo
 // Updated:         10/2022

#include "system.h"
#include <stdio.h>
#include <stdlib.h>
bool delay = false;
void Timer32_INT1 (void); // Función de interrupción.
void Delay_ms (uint32_t time); // Función de delay.
/* EXPLICACION DEL PROGRAMA
 *  El LED de Color inicia apagado, Led rojo parpadea:
 *  a). Si se presiona una vez  el botón 1, el led de color se pone amarillo.
 *  b). Si se presiona dos veces el botón 1, el led de color se pone verde.
 *  c). Si se presiona dos veces el botón 2, el led de color se pone azul.
 * Si se presiona cualquiera de los dos botones por tercera vez, el led se apaga y el algoritmo vuelve a empezar (se reinician cuentas).
 */


/*FUNCTION*********************************************************************************
*
* Function Name    : System_InicialiceIO
* Returned Value   : None.
* Comments         :
*
*END****************************************************************************************/

void System_InicialiceIO(void){
    GPIO_init_board();
}

/*FUNCTION******************************************************************************
*
* Function Name    : System_InicialiceUART
* Returned Value   : None.
* Comments         :
*
*END***********************************************************************************/
void System_InicialiceUART (void){
    UART_init();
}

/*FUNCTION******************************************************************************
*
* Function Name    : funcion_inicial
* Returned Value   : None.
* Comments         :
*
*END***********************************************************************************/

void funcion_inicial (void){
    GPIO_setOutput(BSP_LED1_PORT,  BSP_LED1,  0);
    GPIO_setOutput(BSP_LED2_PORT,  BSP_LED2,  0);
    GPIO_setOutput(BSP_LED3_PORT,  BSP_LED3,  0);
    GPIO_setOutput(BSP_LED4_PORT,  BSP_LED4,  0);
}
/*FUNCTION******************************************************************************
*
* Function Name    : Delay_ms
* Returned Value   : None.
* Comments         :
*
*END***********************************************************************************/
void Delay_ms(uint32_t time){
 // Habilita timer, luego la interrupción y carga valor de conteo.
 BITBAND_PERI(TIMER32_1->CONTROL,TIMER32_CONTROL_ENABLE_OFS)= 1;
 BITBAND_PERI(TIMER32_1->CONTROL,TIMER32_CONTROL_IE_OFS ) = 1;
 TIMER32_1->LOAD = time*(__SYSTEM_CLOCK/1000); // mS.
 delay = true;
 while(delay);
}

/*FUNCTION******************************************************************************
*
* Function Name    : Timer32_INT1
* Returned Value   : None.
* Comments         :
*
*END***********************************************************************************/
void Timer32_INT1(void){
 TIMER32_1->INTCLR = 0x00000000;
 delay = false;
}
/*FUNCTION******************************************************************************
*
* Function Name    : process_events
* Returned Value   : None.
* Comments         :
*
*END***********************************************************************************/
uint32_t x=0, i=0, j=0, t=0, k=0;
void process_events(void){
    // Modo de disparo único, tamaño de 32 bits, prescaler de 1.
    T32_Configure1(TIMER32_OneShotMode, TIMER32_PrescaleDiv_1,
    TIMER32_32BitSize);
    // Registra y habilita la interrupción deseada.
    Int_registerInterrupt(INT_T32_INT1, Timer32_INT1);
    Int_enableInterrupt(INT_T32_INT1);

    if(i==0){
        GPIO_setOutput(BSP_LED1_PORT,  BSP_LED1,  0);
        GPIO_setOutput(BSP_LED2_PORT,  BSP_LED2,  0);
        GPIO_setOutput(BSP_LED3_PORT,  BSP_LED3,  0);
        GPIO_setOutput(BSP_LED4_PORT,  BSP_LED4,  0);
        UART_putsf(MAIN_UART, "Presione un boton para iniciar.\r\n");
        i=1;
    }
    // Entra cuando se presiona el botón 1 o 2 y la variable x es igual a cero, iniciando la aplicacion.
    if(((GPIO_getInputPinValue(BSP_BUTTON1_PORT, BSP_BUTTON1) != BOARD_BUTTON_NORMAL_STATE)||
        (GPIO_getInputPinValue(BSP_BUTTON2_PORT, BSP_BUTTON2) != BOARD_BUTTON_NORMAL_STATE))&&x==0){
        x=1;
        j=1;
        UART_putsf(MAIN_UART, "Espere...\r\n");
        Delay_ms(500);
    }
    if(x==1){
        GPIO_setOutput(BSP_LED2_PORT,  BSP_LED2,  0);//RED OFF
        GPIO_setOutput(BSP_LED3_PORT,  BSP_LED3,  1);//GREEN ON
        GPIO_setOutput(BSP_LED4_PORT,  BSP_LED4,  0);//BLUE OFF
        UART_putsf(MAIN_UART, "APP Ejecutandose\r\n");
        x++;
    }
    if((GPIO_getInputPinValue(BSP_BUTTON1_PORT, BSP_BUTTON1) != BOARD_BUTTON_NORMAL_STATE)&&x==2){
        GPIO_setOutput(BSP_LED2_PORT,  BSP_LED2,  1);//RED On
        GPIO_setOutput(BSP_LED3_PORT,  BSP_LED3,  0);//GREEN OFF
        GPIO_setOutput(BSP_LED4_PORT,  BSP_LED4,  0);//BLUE OFF
        UART_putsf(MAIN_UART, "APP Pausada\r\n");
        k++;
        Delay_ms(4000);
        GPIO_setOutput(BSP_LED2_PORT,  BSP_LED2,  0);//RED OFF
        GPIO_setOutput(BSP_LED3_PORT,  BSP_LED3,  1);//GREEN ON
        GPIO_setOutput(BSP_LED4_PORT,  BSP_LED4,  0);//BLUE OFF
        UART_putsf(MAIN_UART, "APP Ejecutandose\r\n");
    }
    if((GPIO_getInputPinValue(BSP_BUTTON2_PORT, BSP_BUTTON2) != BOARD_BUTTON_NORMAL_STATE)&&j==1){
        j++;
        UART_putsf(MAIN_UART, "SI DESEA TERMINAR LA APLICACIÓN,\r\nENTONCES VUELVA APRESIONAR EL SWITCH\r\n");
        Delay_ms(500);
    }
    if(j==2){
        Delay_ms(10);
        t++;
        if(GPIO_getInputPinValue(BSP_BUTTON2_PORT, BSP_BUTTON2) != BOARD_BUTTON_NORMAL_STATE){
            UART_putsf(MAIN_UART, "SE ACABO EL PROGRAMA,\r\nBOTESE A LA CHINGADA O INICIE DE NUEVO\r\n");
            Delay_ms(500);
            i=0;
            j=0;
            x=0;
            t=0;
        }
        if(t==500){
            j=1;
            t=0;
        }
    }
    if(k==5){
        UART_putsf(MAIN_UART, "SE ACABO EL PROGRAMA,\r\nBOTESE A LA CHINGADA O INICIE DE NUEVO\r\n");
        Delay_ms(500);
        i=0;
        j=0;
        x=0;
        t=0;
        k=0;
    }

}
