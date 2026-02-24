/*
* Lab3.asm
*
* Creado: 23/02
* Autor : Julian Alonso
* Descripción: Contador de segundos (00-59) con Timer0 y display 7 segmentos (2 dígitos)
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P

.dseg
.org    SRAM_START
d_10ms:      .byte   1      // Contador de 10 ms
unidades:    .byte   1      // Contador para unidades de sgundos
decenas:     .byte   1      // Contador para decenas
mux:         .byte   1      // 0 = mostrar unidades, 1 = mostrar decenas

.cseg
.org 0x0000
    RJMP    RESET           // Vector Reset

.org 0x001C
    RJMP    TMR0_ISR        // Vector TIMER0 compare match A
	// Lo configuramos en modo CTC

/****************************************/
// Configuración de la pila
RESET:
LDI     R16, LOW(RAMEND)    
OUT     SPL, R16            
LDI     R16, HIGH(RAMEND)   
OUT     SPH, R16            

/****************************************/
// Configuracion MCU
SETUP:

    // -------- DISPLAY a-d  --------
    SBI     DDRD, DDD4        // PD4 como salida (segmento a)
    SBI     DDRD, DDD5        // PD5 como salida (segmento b)
    SBI     DDRD, DDD6        // PD6 como salida (segmento c)
    SBI     DDRD, DDD7        // PD7 como salida (segmento d)

    // -------- DISPLAY e-g  --------
    SBI     DDRC, DDC0        // PC0 como salida (segmento e)
    SBI     DDRC, DDC1        // PC1 como salida (segmento f)
    SBI     DDRC, DDC2        // PC2 como salida (segmento g)

    // -------- CONTROL DIGITOS  --------
    SBI     DDRC, DDC3        // PC3 como salida (DIG3)
    SBI     DDRC, DDC4        // PC4 como salida (DIG4)

    // -------- APAGAR SEGMENTOS a-d --------
    CBI     PORTD, PORTD4     // Apagar segmento a
    CBI     PORTD, PORTD5     // Apagar segmento b
    CBI     PORTD, PORTD6     // Apagar segmento c
    CBI     PORTD, PORTD7     // Apagar segmento d

    // -------- APAGAR SEGMENTOS e-g --------
    CBI     PORTC, PORTC0     // Apagar segmento e
    CBI     PORTC, PORTC1     // Apagar segmento f
    CBI     PORTC, PORTC2     // Apagar segmento g

    // -------- APAGAR DIGITOS (activo en 0) --------
    SBI     PORTC, PORTC3     // PC3 = 1 para apagarlo
    SBI     PORTC, PORTC4     // PC4 = 1 para apagarlo

    // Inicializar variables
    CLR     R16                         
    STS     d_10ms, R16                 // iniciamos variable en 0
    STS     unidades, R16               // iniciamos variable en 0
    STS     decenas, R16                // iniciamos variable en 0
    STS     mux, R16                    // iniciamos variable en 0

    // TIMER0 CTC aprox 10 ms
    LDI     R16, (1<<WGM01)             // Modo CTC
    OUT     TCCR0A, R16                 // Cargamos TCCR0A

    LDI     R16, (1<<CS02)|(1<<CS00)    // Prescaler 1024
    OUT     TCCR0B, R16                 // Activamos el prescaler

    LDI     R16, 155                    // Cargamos el valor de comparacion
    OUT     OCR0A, R16                  // Lo guardamos para que se reinicie al llegar

    CLR     R16                         // Cargamos 0
    OUT     TCNT0, R16                  // Reiniciamos timer

    LDI     R16, (1<<OCF0A)             // Cargamos el bit de la bandera de comparacion
    OUT     TIFR0, R16                  // Limpiamos bandera

    LDI     R16, (1<<OCIE0A)            // Habilitar interrupción COMPA
    STS     TIMSK0, R16                 // Activamos la interrupción del timer 0

    SEI                                 // ENABLE GLOBAL INTERRUPTIONS

/****************************************/
// Loop Infinito
MAIN_LOOP:
    RJMP    MAIN_LOOP                   

/****************************************/
// NON-Interrupt subroutines

SEG7_DECODE:
    PUSH    R30                         // Guardamos ZL
    PUSH    R31                         // Guardamos ZH

    LDI     R31, HIGH(TABLA_S7*2)       // Apuntamos a la tabla HIGH
    LDI     R30, LOW(TABLA_S7*2)        // Apuntamos a la tabla LOW

    ADD     R30, R16                    // Sumamos el índice
    CLR     R16                         // R16 = 0 (para carry)
    ADC     R31, R16                    // Ajustamos ZH pasandole el carry del LOW

    LPM     R16, Z                      // Leemos patrón y lo guardamos en R16

    POP     R31                         // Recuperamos ZH
    POP     R30                         // Recuperamos ZL
    RET                                 

SEG7_WRITE:
    PUSH    R18                         // Guardamos R18
    PUSH    R19                         // Guardamos R19

    // Parte a-d -> bits 0-3
    MOV     R18, R16                    // Copiamos patrón
    ANDI    R18, 0x0F                   // Nos quedamos con a-d
    SWAP    R18                         // Subimos a-d al nibble alto
    ANDI    R18, 0xF0                   // Solo PD4..PD7

    IN      R19, PORTD                  // Leemos PORTD
    ANDI    R19, 0x0F                   // Limpiamos PD4..PD7
    OR      R19, R18                    // Pegamos a-d
    OUT     PORTD, R19                  // Sacamos a PORTD

    // Parte e-g -> bits 4-6
    MOV     R18, R16                    // Copiamos patrón
    LSR     R18                         // lo movemos 1
    LSR     R18                         // lo movemos 2
    LSR     R18                         // lo movemos 3 
    LSR     R18                         // lo movemos 4 veces
    ANDI    R18, 0b00000111             // Solo 3 bits

    IN      R19, PORTC                  // Leemos PORTC
    ANDI    R19, 0b11111000             // Limpiamos los ultimos 3 bits
    OR      R19, R18                    // Pegamos e-g
    OUT     PORTC, R19                  // Sacamos a PORTC

    POP     R19                         // Recuperamos R19
    POP     R18                         // Recuperamos R18
    RET                                

DIG_SELECT:
    PUSH    R18                         // Guardamos R18

    LDS     R18, mux                    // Leemos mux
    TST     R18                         // Si es 0 mostramos unidades
    BREQ    SHOW_UNI                    // Saltamos a unidades

    // Mostrar decenas: PC3 = 0, PC4 = 1
    IN      R18, PORTC                  // Leemos PORTC
	SBI		PORTC, PORTC4				// Apagamos el digito de unidades
	CBI		PORTC, PORTC3				// Prendemos el digito de decenas
    POP     R18                         // Recuperamos R18
    RET                                 

SHOW_UNI:
    SBI     PORTC, PORTC3				 // Apagar decenas (PC3=1)
    CBI     PORTC, PORTC4				 // Encender unidades (PC4=0)
    POP     R18
    RET                               

/****************************************/
// Interrupt routines

TMR0_ISR:
    PUSH    R16                         // Guardamos R16
    PUSH    R17                         // Guardamos R17
    PUSH    R18                         // Guardamos R18
    IN      R17, SREG                   // Guardamos SREG
    PUSH    R17                         // Metemos SREG a la pila

    // ----- Contar 10ms -----
    LDS     R16, d_10ms                 // Leemos ticks
    INC     R16                         // Aumentamos 1 tick
    CPI     R16, 100                    // Revisamos si llego a 100
    BRLO    SAVE_TICK                   // Si no, solo guardamos

    // ----- Ya paso 1 segundo -----
    CLR     R16                         // Reiniciamos ticks
    STS     d_10ms, R16                 // Guardamos 0

    // Unidades
    LDS     R16, unidades               // Leemos unidades
    INC     R16                         // Incrementamos
    CPI     R16, 10                     // Si llega a 10, resetea
    BRLO    SAVE_UNI                    // Si no ha llegado a 10, guardamos

    CLR     R16                         // Unidades vuelve a 0
    STS     unidades, R16               // Guardamos unidades=0

    // Decenas
    LDS     R16, decenas                // Leemos decenas
    INC     R16                         // Incrementamos
    CPI     R16, 6                      // Si llega a 6, resetea (60s)
    BRLO    SAVE_DEC                    // Si no ha llegado a 6, guardamos

    CLR     R16                         // Decenas vuelve a 0
    STS     decenas, R16                // Guardamos decenas=0
    RJMP    UPDATE_DISPLAY              // Actualizamos display

SAVE_DEC:
    STS     decenas, R16                // Guardamos decenas actualizadas
    RJMP    UPDATE_DISPLAY              // Actualizamos display

SAVE_UNI:
    STS     unidades, R16               // Guardamos unidades actualizadas
    RJMP    UPDATE_DISPLAY              // Actualizamos display

SAVE_TICK:
    STS     d_10ms, R16                 // Guardamos ticks

UPDATE_DISPLAY:

    // ----- Alternar mux -----
    LDS     R16, mux                    // Leemos mux
	LDI		R18, 1						// Cargamos 1
    EOR     R16, R18                    // Cambiamos 0->1 o 1->0
    ANDI    R16, 0x01                   // Solo dejamos 1 bit
    STS     mux, R16                    // Guardamos mux

    // ----- Elegir numero a mostrar -----
    TST     R16                         // Si es 0, mostramos unidades
    BREQ    LOAD_UNI                    // Saltamos a unidades

    LDS     R16, decenas                // sino cargamos decenas
    RJMP    DECODE_AND_SHOW             // Vamos a mostrar

LOAD_UNI:
    LDS     R16, unidades               // Cargamos unidades

DECODE_AND_SHOW:
    RCALL   SEG7_DECODE                 // Convertimos numero a patrón
    RCALL   SEG7_WRITE                  // Escribimos segmentos
    RCALL   DIG_SELECT                  // Encendemos el dígito correcto

    POP     R17                         // Sacamos SREG
    OUT     SREG, R17                   // Restauramos SREG
    POP     R18                         // Recuperamos R18
    POP     R17                         // Recuperamos R17
    POP     R16                         // Recuperamos R16
    RETI                                // Regresamos de interrupción

/****************************************/
TABLA_S7:
    .DB 0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07
    .DB 0x7F,0x6F,0x00,0x00,0x00,0x00,0x00,0x00
/****************************************/