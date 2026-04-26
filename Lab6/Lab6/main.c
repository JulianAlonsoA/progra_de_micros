/*
 * main.c
 *
 * Created: 4/21/2026
 * Author: Julian Alonso
 * Description:
 * Post Lab 6 - UART
 * Muestra un menú en la terminal serial con dos opciones:
 * 1. Leer potenciómetro
 * 2. Enviar ASCII
 * Después de ejecutar una opción, vuelve a mostrar el menú.
 */

/****************************************/
// Encabezado (Libraries y constantes)
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

#include <avr/io.h>
#include <stdint.h>

/****************************************/
// Prototipos de funciones UART
void UART_init(void);
void UART_sendChar(char data);
char UART_receiveChar(void);
void UART_sendText(char *text);
void UART_sendNumber(uint16_t value);

/****************************************/
// Prototipos de funciones ADC
void ADC_init(void);
uint16_t ADC_getValue(void);

/****************************************/
// Prototipos de funciones auxiliares
void delay_simple(void);
void menu_principal(void);
void leds_init(void);
void leds_clear(void);
void mostrar_ascii_en_leds(char dato);
void opcion_leer_pot(void);
void opcion_enviar_ascii(void);

/****************************************/
// Inicialización de UART
void UART_init(void)
{
 // Configurar baud rate
 UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
 UBRR0L = (uint8_t)(UBRR_VALUE);

 // Habilitar transmisión y recepción
 UCSR0B = (1 << TXEN0) | (1 << RXEN0);

 // Configurar formato: 8 bits, sin paridad, 1 stop bit
 UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/****************************************/
// Enviar un carácter por UART
void UART_sendChar(char data)
{
 while (!(UCSR0A & (1 << UDRE0)));
 UDR0 = data;
}

/****************************************/
// Recibir un carácter por UART
char UART_receiveChar(void)
{
 while (!(UCSR0A & (1 << RXC0)));
 return UDR0;
}

/****************************************/
// Enviar una cadena completa por UART
void UART_sendText(char *text)
{
 while (*text != '\0')
 {
  UART_sendChar(*text);
  text++;
 }
}

/****************************************/
// Enviar un número decimal por UART
void UART_sendNumber(uint16_t value)
{
 char digits[5];
 uint8_t index = 0;

 if (value == 0)
 {
  UART_sendChar('0');
  return;
 }

 while (value > 0)
 {
  digits[index] = (value % 10) + '0';
  value = value / 10;
  index++;
 }

 while (index > 0)
 {
  index--;
  UART_sendChar(digits[index]);
 }
}

/****************************************/
// Inicialización de ADC
void ADC_init(void)
{
 // Referencia AVcc y canal ADC0 (A0)
 ADMUX = 0;
 ADMUX |= (1 << REFS0);

 // Habilitar ADC con prescaler 128
 ADCSRA = 0;
 ADCSRA |= (1 << ADEN);
 ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

/****************************************/
// Leer valor del ADC
uint16_t ADC_getValue(void)
{
 ADCSRA |= (1 << ADSC);

 while (ADCSRA & (1 << ADSC));

 return ADC;
}

/****************************************/
// Delay simple por software
void delay_simple(void)
{
 for (volatile long i = 0; i < 300000; i++);
}

/****************************************/
// Inicializar pines de LEDs

void leds_init(void)
{
 DDRB |= (1 << PB0) | (1 << PB1) | (1 << PB2) |
         (1 << PB3) | (1 << PB4) | (1 << PB5);

 DDRD |= (1 << PD6) | (1 << PD7);

 leds_clear();
}

/****************************************/
// Apagar todos los LEDs usados
void leds_clear(void)
{
 PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) |
            (1 << PB3) | (1 << PB4) | (1 << PB5));

 PORTD &= ~((1 << PD6) | (1 << PD7));
}

/****************************************/
// Mostrar un carácter ASCII recibido en 8 LEDs
void mostrar_ascii_en_leds(char dato)
{
 leds_clear();

 if (dato & (1 << 0)) PORTB |= (1 << PB0);
 if (dato & (1 << 1)) PORTB |= (1 << PB1);
 if (dato & (1 << 2)) PORTB |= (1 << PB2);
 if (dato & (1 << 3)) PORTB |= (1 << PB3);
 if (dato & (1 << 4)) PORTB |= (1 << PB4);
 if (dato & (1 << 5)) PORTB |= (1 << PB5);
 if (dato & (1 << 6)) PORTD |= (1 << PD6);
 if (dato & (1 << 7)) PORTD |= (1 << PD7);
}

/****************************************/
// Mostrar menú principal
void menu_principal(void)
{
 UART_sendText("\r\n========================\r\n");
 UART_sendText(" MENU UART \r\n");
 UART_sendText("========================\r\n");
 UART_sendText("1) Leer valor del potenciometro\r\n");
 UART_sendText("2) Enviar ASCII en LEDS\r\n");
 UART_sendText("Seleccione una opcion: ");
}

/****************************************/
// Opción 1: leer potenciómetro y mandar valor
void opcion_leer_pot(void)
{
 uint16_t lectura_adc = ADC_getValue();

 UART_sendText("\r\nValor del potenciometro: ");
 UART_sendNumber(lectura_adc);
 UART_sendText("\r\n");
}

/****************************************/
// Opción 2: recibir un carácter y mostrarlo en LEDs
void opcion_enviar_ascii(void)
{
 char caracter_recibido;

 UART_sendText("\r\nIngrese un caracter: ");

 caracter_recibido = UART_receiveChar();

 UART_sendChar(caracter_recibido);
 UART_sendText("\r\n");

 mostrar_ascii_en_leds(caracter_recibido);

 UART_sendText("ASCII mostrado en LEDs.\r\n");
}

/****************************************/
// Función principal
int main(void)
{
 char opcion_menu;

 UART_init();
 ADC_init();
 leds_init();

 while (1)
 {
  menu_principal();

  opcion_menu = UART_receiveChar();
  UART_sendChar(opcion_menu);

  switch (opcion_menu)
  {
   case '1':
    opcion_leer_pot();
    break;

   case '2':
    opcion_enviar_ascii();
    break;

   default:
    UART_sendText("\r\nOpcion no valida. Ingrese un 1 o un 2.\r\n");
    break;
  }

  delay_simple();
 }
}