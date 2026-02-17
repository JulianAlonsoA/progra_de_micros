/*
* main.asm
*
* Creado:
* Autor : Julian Alonso
* Descripción: Lab 2 - Contador hexadecimal 
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P

.dseg
.org    SRAM_START
//variable_name:     .byte   1   // Memory alocation for variable_name:     .byte   (byte size)

.cseg
.org 0x0000
    RJMP    SETUP

/****************************************/
// Tabla de display de 7 segmentos (HEX 0-F)
table7seg:    .db 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71
// 0 - 1 - 2 - 3 - 4 - 5 - 6 - 7 - 8 - 9 - A - b - C - d - E - F
/****************************************/
// Configuración de la pila
SETUP:
    LDI     R16, LOW(RAMEND) 
    OUT     SPL, R16
    LDI     R16, HIGH(RAMEND)
    OUT     SPH, R16

/****************************************/
// Configuracion MCU
    CLI              

    //CLK
    LDI     R16, (1<<CLKPCE)    // Habilitamos el prescaler
    STS     CLKPR, R16
    LDI     R16, (1<<CLKPS2)    // Se carga el valor de division
    STS     CLKPR, R16			// Divide por 16 para tener 1MHz

    //Disable UART
    LDI     R16, 0x00 // Desabilitamos PD0 y PD1 
    STS     UCSR0B, R16

    //DISPLAY:
    SBI		DDRC, DDC0 // Bit de salida
	SBI		DDRC, DDC1 // Bit de salida
	SBI		DDRC, DDC2 // Bit de salida
	SBI		DDRC, DDC3 // Bit de salida
	SBI		DDRC, DDC4 // Bit de salida
	SBI		DDRC, DDC5 // Bit de salida

	SBI		DDRB, DDB0 // Bit de salida F en otro puerto

	CBI		PORTC, PC0 // Inicio en 0
	CBI		PORTC, PC1 // Inicio en 0
	CBI		PORTC, PC2 // Inicio en 0
	CBI		PORTC, PC3 // Inicio en 0
	CBI		PORTC, PC4 // Inicio en 0
	CBI		PORTC, PC5 // Inicio en 0
	CBI     PORTB, PB0

    //PB1/PB2 entradas con pull-up (botones)

    CBI		DDRB, DDB1 // Bit de entrada	
	CBI		DDRB, DDB2 // Bit de entrada	
	SBI     PORTB, PB1 // pull-up PB1
    SBI     PORTB, PB2 // pull-up PB2
        
    CLR     R20  // contador hexadecimal
    CLR     R21                 
    CLR     R22                 

    // -------- POSTLAB --------
    SBI     DDRD, DDD2// Bit de salida
    SBI     DDRD, DDD3// Bit de salida
    SBI     DDRD, DDD4// Bit de salida
    SBI     DDRD, DDD5// Bit de salida
    SBI     DDRD, DDD6// Bit de salida

    CBI     PORTD, PD2// inicio en 0
    CBI     PORTD, PD3// inicio en 0
    CBI     PORTD, PD4// inicio en 0
    CBI     PORTD, PD5// inicio en 0
    CBI     PORTD, PD6// inicio en 0

    CLR     R19        ; contador binario
    CLR     R26        ; acumulador de ticks

    RCALL   INIT_T0

    // Mostramos inicial
    RCALL   DISPLAY

/****************************************/
// Loop Infinito
MAIN_LOOP:
    RCALL   BOT_INC
    RCALL   BOT_RES
    RCALL   TICK_10MS
    RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

BOT_INC:
    SBIC    PINB, PB1 // Leemos el boton de incremento
    RET // Saltamos si no pasa nada
    RCALL   ANTI_REB_1 // Sino pasamos al antirebote
    RCALL   SUMA // Incrementamos
    RCALL   DISPLAY // Mostramos
    RET

BOT_RES:
    SBIC    PINB, PB2 // Leemos el boton de resta
    RET // Saltamos si no pasa nada
    RCALL   ANTI_REB_2 // sino pasamos al antirebote
    RCALL   RESTA // restamos
    RCALL   DISPLAY // mostramos
    RET

SUMA:
    INC     R20 // Incrementamos el contador
    ANDI    R20, 0x0F // Lo limitamos al nibble bajo
    RET

RESTA:
    DEC     R20 // Restamos al contador
    ANDI    R20, 0x0F // Lo limitamos al nibble bajo
    RET
 
DISPLAY:
    LDI     ZH, HIGH(table7seg<<1) // Cargamos la parte alta de la direccion en la flash
    LDI     ZL, LOW(table7seg<<1) // Cargamos la parte baja de la direccion en la flash

    ADD     ZL, R20 // Suma el contador al puntero en Z
    ADC     ZH, R22 // Suma en la parte alta si hubo carry 

    LPM     R18, Z    // Leemos el valor de la flash          

    MOV     R21, R18 // Copiamos r18 para alterar
    ANDI    R21, 0b0011_1111 // Dejamos solo los bits que queremos
    OUT     PORTC, R21 // Lo mostramos

    CBI		PORTB, PB0 // Apaga G
	SBRC	R18,6 // Si el sexto bit es 1, G se enciende
	SBI		PORTB, PB0 // Lo mostramos
	RET

ANTI_REB_1:
    RCALL   DELAY
    SBIC    PINB, PB1 // Si ya no esta presionado no hace nada
    RET
SOLTAR_1:
    SBIS    PINB, PB1
    RJMP    SOLTAR_1 // Espera a que se suelte el boton
    RCALL   DELAY // Delay para estabilizar
    RET

ANTI_REB_2:
    RCALL   DELAY
    SBIC    PINB, PB2 // Si ya no esta presionado no hace nada
    RET
SOLTAR_2:
    SBIS    PINB, PB2
    RJMP    SOLTAR_2 // Espera a que se suelte el boton
    RCALL   DELAY // Delay para estabilizar
    RET

/****************************************/
//Delay

DELAY:
    LDI     R23, 0x20
DELAY_0:
    LDI     R24, 0x0F
DELAY_1:
    LDI     R25, 0x0F 
DELAY_2:
    DEC     R25
    BRNE    DELAY_2
    DEC     R24
    BRNE    DELAY_1
    DEC     R23
    BRNE    DELAY_0
    RET


//* ---------------- POSTLAB ---------------- */

INIT_T0:
    LDI     R16, 0 // cargamos 0
    OUT     TCNT0, R16 // Ponemos en 0 el timer 0

    LDI     R16, 155 // valor final de la cuenta
    OUT     OCR0A, R16 // el timer cuenta de 0 - 155

    LDI     R16, (1<<WGM01) // Lo configuramos en CTC
    OUT     TCCR0A, R16 // Configura el modo del timer0

    LDI     R16, (1<<CS01)|(1<<CS00) // Activamos un prescaler de 64
    OUT     TCCR0B, R16 // Cargamos el valor

    LDI     R16, (1<<OCF0A) // Se escribe el 1 para limpiar la bandera OCF0A
    OUT     TIFR0, R16 // Se reinicia el programa
    RET

TICK_10MS:
    IN      R16, TIFR0 // Leemos las banderas
    SBRS    R16, OCF0A // Si hubo conmpare match salta
    RET

    LDI     R16, (1<<OCF0A) // Carga mascara en 1
    OUT     TIFR0, R16 // Limpiamos la bandera

    INC     R26 // Incrementa el contador de 10 ms
    CPI     R26, 100 // COmpara si ya llego a 100 o 10 repeticiones
    BRNE    TICK_END // Si no ha llegado salta al final

    CLR     R26 // Si llego se limpia el registro

    INC     R19 // Incrementamos el contador binario
    ANDI    R19, 0x0F // Lo limitamos al nibble bajo
    RCALL   LEDS_BIN // Actualizamos la salida de los leds
    RCALL   OVERFLOW_CHECK // LLamamos para verificar overflow

TICK_END:
    RET // Regresamos al main_loop

LEDS_BIN:
    MOV     R16, R19 // COpiamos el valor para alterarlo
    ANDI    R16, 0x0F // Lo limitamos
    LSL     R16 // Lo corremos
    LSL     R16 // Lo corremos

    IN      R17, PORTD // Leemos el puerto d
    ANDI    R17, 0b11000011 // Limpiamos los bits PD2 - PD5
    OR      R17, R16 // Insetramos el valor correcto
    OUT     PORTD, R17 // Sacamos ese resultado
    RET

OVERFLOW_CHECK:
    MOV     R16, R19 // Copiamos el valor del contador
    ANDI    R16, 0x0F // Lo limitamos al nibble bajo

    MOV     R17, R20 // COpiamos el valor objetivo del display
    ANDI    R17, 0x0F // Lo limitamos

    CP      R16, R17 //Comparamos el contador con el display
    BRNE    FIN_OVERFLOW // Si no son iguales sigue sin hacer nada

    SBI     PIND, PD6 // Cambiamos el estado 
    CLR     R19 // Reiniciamos el contador binario a 0
    RCALL   LEDS_BIN // Actualizamos los les a 0

FIN_OVERFLOW:
    RET
