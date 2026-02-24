/*
* Lab3.asm
*
* Creado: 17/02/26
* Autor : Julian Alonso
* Descripción: Contador de 4 bits con botones usando PCINT y antirebote
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P

.dseg
.org    SRAM_START
flag_inc:      .byte   1
flag_dec:      .byte   1

.cseg
.org 0x0000
    RJMP    RESET            // Vector Reset

.org 0x000A
    RJMP    INTERRUPCION_1       // Vector PCINT2

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

    // OUTPUTS
	SBI		DDRB, DDB0 // Led salida
	SBI		DDRB, DDB1 // Led salida
	SBI		DDRB, DDB2 // Led salida
	SBI		DDRB, DDB3 // Led salida

    // Inicia apagado (poner PB0..PB3 en 0)
    CBI     PORTB, PORTB0 // Apaga LED0
    CBI     PORTB, PORTB1 // Apaga LED1
    CBI     PORTB, PORTB2 // Apaga LED2
    CBI     PORTB, PORTB3 // Apaga LED3

    CLR     R17

    // INPUTS
    CBI     DDRD, DDD2 // Boton incremento
    CBI     DDRD, DDD3 // Boton resta
    SBI     PORTD, PORTD2 // Activo pullup
    SBI     PORTD, PORTD3 // Activo pullup

    // INICIO BANDERAS
    CLR     R16 // Cargamos 0
    STS     flag_inc, R16 // Banderas apagadas
    STS     flag_dec, R16 // Banderas apagadas

    // TIMER0
    LDI     R16, 0 // Hablilitamos la edicion modo normal
    OUT     TCCR0A, R16 // Cargamos el valor
    LDI     R16, (1<<CS01)|(1<<CS00) // Divisor de 64
    OUT     TCCR0B, R16 // Lo cargamos

    // INTERRUPCIONES
    LDI     R16, (1<<PCINT18)|(1<<PCINT19) // Pines que van a tener interrupciones
    STS     PCMSK2, R16 // Cargamos el valor

    // LIMPIEZA
    LDI     R16, (1<<PCIF2) // Cargamos 1
    OUT     PCIFR, R16 // Limpiamos la bandera

    LDI     R16, (1<<PCIE2) // Habilitamos el gurpo 2
    STS     PCICR, R16 // Lo cargamos

    SEI // ENABLE GLOBAL INTERRUPTS 

/****************************************/
// Loop Infinito
MAIN_LOOP:

    // -------- INC --------
    LDS     R16, flag_inc // Leemos desde la sram
    TST     R16 // Comparamos si el valor es 0 o negativo
    BREQ    CHECK_DEC_MAIN // Saltamos si no hubo incremento

    CLR     R16 // Limpiamos el valor
    STS     flag_inc, R16 // Apagamos la bandera

    INC     R17 // Incrementamos el contador
    ANDI    R17, 0x0F // Lo limitamos al nibble bajo
    OUT     PORTB, R17 // Lo mostramos en las LEDS

    RCALL   ESPERA_INC // Aplicamos un delay corto

CHECK_DEC_MAIN:

    // -------- DEC --------
    LDS     R16, flag_dec // Leemos desde la sram
    TST     R16 // Comparamos si el valor es 0 o negativo
    BREQ    MAIN_LOOP // Saltamos si no hubo resta

    CLR     R16 // Limiamos el contador
    STS     flag_dec, R16 // Apagamos la bandera

    DEC     R17 // Restamos el contador
    ANDI    R17, 0x0F // Lo limitamos al nibble bajo
    OUT     PORTB, R17 // Lo mostramos en las LEDS

    RCALL   ESPERA_DEC // Aplicamos un delay corto

    RJMP    MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

ESPERA_INC:

WAIT1:
    SBIS    PIND,2 // Si ya se solto, saltamos
    RJMP    WAIT1 
    RCALL   DELAY_5MS // Estabilizamos el boton

    LDS     R16, PCMSK2 // Leemos la mascara de interrupciones
    ORI     R16, (1<<PCINT18) // Volvemos a habilitar PCINT18 (PD2)
    STS     PCMSK2, R16 // Guardamos la mascara actualizada
    RET // Regresamos al loop principal

ESPERA_DEC:
WAIT2:
    SBIS    PIND,3 // Si ya se solto, saltamos
    RJMP    WAIT2 // Si sigue presionado, seguimos esperando
    RCALL   DELAY_5MS // Estabilizamos el boton (antirebote)

    // Rehabilitar PCINT19
    LDS     R16, PCMSK2 // Leemos la mascara de interrupciones
    ORI     R16, (1<<PCINT19) // Volvemos a habilitar PCINT19 (PD3)
    STS     PCMSK2, R16 // Guardamos la mascara actualizada
    RET // Regresamos al loop principal


DELAY_5MS:
    LDI     R20, 5 // Cargamos la cantidad de overflows que queremos esperar
MINI_DELAY:
    IN      R16, TIFR0 // Leemos banderas del Timer0
    SBRS    R16, TOV0 // Si no hay overflow, no saltamos
    RJMP    MINI_DELAY // Seguimos esperando el overflow

    LDI     R16, (1<<TOV0) // Cargamos 1 para limpiar 
    OUT     TIFR0, R16 // Limpiamos la bandera de overflow

    DEC     R20 // Restamos 1 al contador
    BRNE    MINI_DELAY // Si no llego a 0, repetimos
    RET                       

/****************************************/
// Interrupt routines

INTERRUPCION_1:

    PUSH    R16 // Guardamos R16 EN LA PILA
    IN      R16, SREG  // Guardamos el estado actual del procesador
    PUSH    R16 // Metemos SREG a la pila

    // Si PD2 presionado hacemos INC
    SBIC    PIND,2 // Si no esta presionado, saltamos a revisar resta
    RJMP    CHECK_DEC_ISR // Revisamos el otro boton

    LDI     R16, 1 // Cargamos 1
    STS     flag_inc, R16 // Encendemos bandera de incremento

    LDS     R16, PCMSK2 // Leemos la mascara
    CBR     R16, (1<<PCINT18) // Quitamos PCINT18 de la mascara
    STS     PCMSK2, R16 // Guardamos la mascara
    RJMP    END_ISR // Salimos de la interrupcion

CHECK_DEC_ISR:

    // Si PD3 presionado hacemos DEC
    SBIC    PIND,3 // Si no esta presionado, seguimos
    RJMP    END_ISR // Salimos de la interrupcion

    LDI     R16, 1 // Cargamos 1
    STS     flag_dec, R16 // Encendemos bandera de decremento

    LDS     R16, PCMSK2 // Leemos mascara
    CBR     R16, (1<<PCINT19) // Quitamos PCINT19 de la mascara
    STS     PCMSK2, R16 // Guardamos la mascara

END_ISR:
    POP     R16 // Sacamos SREG guardado
    OUT     SREG, R16 // Restauramos SREG a como estaba antes
    POP     R16 // Recuperamos R16 original
    RETI // Regresamos de interrupcion

/****************************************/
