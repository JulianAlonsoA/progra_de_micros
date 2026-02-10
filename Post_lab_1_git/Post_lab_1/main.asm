/*
* NombreProgra.asm
*
* Creado: 
* Autor : Julian Alonso
* Descripción: Post_Lab 1, contadores con botones y sumador de contadores
*/
/****************************************/
// Encabezado (Definición de Registros, Variables y Constantes)
.include "M328PDEF.inc"     // Include definitions specific to ATMega328P
.dseg
.org    SRAM_START
//variable_name:     .byte   1   // Memory alocation for variable_name:     .byte   (byte size)

.cseg
.org 0x0000
 /****************************************/
// Configuración de la pila
LDI     R16, LOW(RAMEND)
OUT     SPL, R16
LDI     R16, HIGH(RAMEND)
OUT     SPH, R16
/****************************************/
// Configuracion MCU   
    // Aca configuramos entradas y salidas
SETUP:
	// inputs
	CBI		DDRC, DDC4 // DDRC = 0 -> entrada para boton decremento
	CBI		DDRC, DDC5 // DDRC = 0 -> entrada para boton incremento
	CBI		DDRD, DDD6 // DDRD = 0 -> entrada para boton decremento 2
	CBI		DDRD, DDD7 // DDRD = 0 -> entrada para boton incremento 2
	CBI		DDRB, DDB4 // DDRB = 0 -> entrada para boton de suma

	// outputs
	SBI		DDRD, DDD2 // DDRD = 1 -> led 1 cont 1
	SBI		DDRD, DDD3 // DDRD = 1 -> led 2 cont 1
	SBI		DDRD, DDD4 // DDRD = 1 -> led 3 cont 1
	SBI		DDRD, DDD5 // DDRD = 1 -> led 4 cont 1

	SBI		DDRB, DDB0 // DDRB = 1 -> led 1 cont 2
	SBI		DDRB, DDB1 // DDRB = 1 -> led 2 cont 2
	SBI		DDRB, DDB2 // DDRB = 1 -> led 3 cont 2
	SBI		DDRB, DDB3 // DDRB = 1 -> led 4 cont 2

	SBI		DDRC, DDC0 // DDRC = 1 -> led 1 sumador
	SBI		DDRC, DDC1 // DDRC = 1 -> led 2 sumador
	SBI		DDRC, DDC2 // DDRC = 1 -> led 3 sumador
	SBI		DDRC, DDC3 // DDRC = 1 -> led 4 sumador

	SBI		DDRB, DDB5 // DDRB = 1 -> led overflow
	
	LDI		R16, 0b00000000 // cargamos 0 en todos los bits
	OUT		PORTB, R16 // Definimos leds apagados al inicio
	OUT		PORTC, R16 // Definimos leds apagados al inicio
	OUT		PORTD, R16 // Definimos leds apagados al inicio

	SBI		PORTC, PORTC4 // Activa pullup en bit 4 de C
	SBI		PORTC, PORTC5 // Activa pullup en bit 5 de C
	SBI		PORTD, PORTD6 // Activa pullup en bit 6 de D
	SBI		PORTD, PORTD7 // Activa pullup en bit 7 de D
	SBI		PORTB, PORTB4 // Activa pullup en bit 4 de B

	// prescaler
	LDI		R16, (1<<CLKPCE) // Habilitamos el prescaler
	STS		CLKPR, R16
	LDI		R16, 0b00000100 // Divisor de 16
	STS		CLKPR, R16 // cargamos el valor

	// revision de registros
	CLR		R16
	CLR		R20 // Contador 1
	CLR		R21 // Contador 2
	CLR		R22 // Suma

/****************************************/
// Loop Infinito
MAIN_LOOP:
	CALL	CONT_1
	CALL	CONT_2
	CALL	SUMADOR
	RJMP	MAIN_LOOP

/****************************************/
// NON-Interrupt subroutines

CONT_1:
	IN		R16, PINC // Leemos el estado de los botones
	ANDI	R16, 0b00110000 // Nos quedamos solo con los 4 y 5
	CPI		R16, 0b00110000
	BREQ	ALTOS // Si ambos estan sin presion no pasa nada

	RCALL	DELAY

	IN		R17, PINC // Volvemos a leer el estado de los botones
	ANDI	R17, 0b00110000 // Nos quedamos otra vez solo con el 4 y 5
	CP		R17, R16 // Comparamos para ver si hubo cambio
	BRNE	ALTOS 

	SBRS	R17, 4 // Si se detecta cambio en el bit 4
	RCALL	INC_1 // pasamos a la funcion de incremento

	SBRS	R17, 5 // Si se detecta cambio en el bit 5
	RCALL	DEC_1 // Pasamos a la funcion de decremento

	RJMP	ALTOS


CONT_2:
	IN		R16, PIND // Leemos el estado de los botones
	ANDI	R16, 0b11000000 // Nos quedamos solo con el 6 y 7
	CPI		R16, 0b11000000 
	BREQ	BAJOS // Si no hay nada presionado noo pasa nada

	RCALL	DELAY

	IN		R17, PIND // volvemos a leer
	ANDI	R17, 0b11000000 // volvemos a aislar
	CP		R17, R16
	BRNE	BAJOS // Comparamos viendo si hubo cambio

	SBRS	R17, 6 // Si se detecta cambio en el bit 6
	RCALL	INC_2 // Pasamos a la funcion de incremento

	SBRS	R17, 7 //  Si se detecta cambio en el bit 7
	RCALL	DEC_2 // Pasamos a la funcion de decremento

	RJMP	BAJOS

/****************************************/
// Interrupt routines
ALTOS:
	IN		R18, PORTD // Leemos el puerto
	ANDI	R18, 0b11000011 // Mantemenos lo bits como queremos
	
	MOV		R19, R20 // Lo cambiamos de localidad para no alterear

	ANDI	R19, 0b00001111 // Dejamos solo los 4 bits del final
	LSL		R19	// Corremos los bits para que queden en PD 2...5
	LSL		R19

	OR		R18, R19 //  Unimos los registros
	OUT		PORTD, R18
	RET
BAJOS:
	IN		R18, PORTB // Leemos el puerto
	ANDI	R18, 0b11110000 // mantenemos los bits como queresmos
	
	MOV		R19, R21 // lo cambiamos de localidad para no alterar

	ANDI	R19, 0b00001111 // Dejamos solo los 4 bits del final
	OR		R18, R19 // Unimos los registros
	OUT		PORTB, R18 // Los primeros 4 bits se quedan iguales, los otros muestran contador
	RET

SUMADOR:
	IN		R16, PINB // Leemos el valor de los pines
	SBRC	R16, 4 // Si el boton esta en 0 salta, sino retorna
	RET

	RCALL	DELAY

	IN		R16, PINB // Volvemos a leer
	SBRC	R16, 4 // Comprobamos que siga asi
	RET

	MOV		R22, R20 //	Cargamos el valor del contador 1 al sumador
	ADD		R22, R21 // Sumamos el contador 2

	MOV		R19, R22 // Aseguramos el valor actual
	ANDI	R19, 0b11110000 // Lo limitamos al nibble alto
	CPI		R19, 0b00000000 // COmparamos si todo es 0
	BREQ	NO_OVER

	SBI		PORTB, PORTB5 // Prende led de overflow
	RJMP	LED_SUMA

NO_OVER:
	CBI		PORTB, PORTB5 // Apaga el led de overflow

LED_SUMA:
	IN		R18, PORTC // Leemos el valor del puerto
	ANDI	R18, 0b11110000 // Lo limitamos al nibble alto

	MOV		R19, R22 // Movemos el valor para alterarlo
	ANDI	R19, 0b00001111 // Usamos el nibble bajo

	OR		R18, R19 // Unimos los valores para el display
	OUT		PORTC, R18 // Sacamos el valor

ESPERAR_SUMA:
	SBIS	PINB, 4 // Esperamos a que vuelva a 1
	RJMP	ESPERAR_SUMA

	RCALL DELAY
	RET

DELAY:
	LDI		R23, 0x0F
DELAY_0:
	LDI		R24, 0x0F
DELAY_1:
	LDI		R25, 0x0F
DELAY_2:
	DEC		R25
	BRNE	DELAY_2
	DEC		R24
	BRNE	DELAY_1
	DEC		R23
	BRNE	DELAY_0
	RET

INC_1:
	INC		R20 // incrementamos el contador
	ANDI	R20, 0b00001111 // lo limitamos a 4 bits
	RJMP	ANTI_REB_1
DEC_1:
	DEC		R20 // decrementamos el contador
	ANDI	R20, 0b00001111 // lo limitamos a 4 bits
	RJMP	ANTI_REB_2

ANTI_REB_1:
	SBIS	PINC, 4 // mientras el 4 bit este presionado no salta
	RJMP	ANTI_REB_1
	RCALL	DELAY // llamamos al delay una vez ya se solto
	RET
ANTI_REB_2:
	SBIS	PINC, 5 // mientras el 5 bit este presionado no salta
	RJMP	ANTI_REB_2
	RCALL	DELAY // llaamos al delay una vez ya solto
	RET


INC_2:
	INC		R21 // incrementamos el contador
	ANDI	R21, 0b00001111 // lo limitamos a 4 bits
	RJMP	ANTI_REB_3
DEC_2:
	DEC		R21 // decrementamos el contador
	ANDI	R21, 0b00001111 // lo limitamos a 4 bits
	RJMP	ANTI_REB_4

ANTI_REB_3:
	SBIS	PIND, 6 // mientras el 6 bit este presionado no salta
	RJMP	ANTI_REB_3
	RCALL	DELAY // llamamos al delay una vez ya se solto
	RET
ANTI_REB_4:
	SBIS	PIND, 7 // mientras el 7 bit este presionado no salta
	RJMP	ANTI_REB_4
	RCALL	DELAY // llaamos al delay una vez ya solto
	RET
/****************************************/