#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h> 


volatile uint8_t adc_counter, voltage, value;

ISR(SIG_ADC)
{
	value = value + (ADCH*7)/81; //insert 4xtrot. For me U. 1.1V = 22
	adc_counter++;
}

int main(void)
{
DDRB = 0b00011011;
PORTB = 0b00000000; //all (excluded PB4 (ADC)) as out

MCUCR |= _BV(SM1); //power-down setup

ADMUX |= /*(1 << MUX1)|*/(1 << MUX0);  //select ADC1 (PB2) input
ADMUX = ADMUX | (1<<REFS0); //internal refefrence 1.1V
ADMUX = ADMUX|(1<<ADLAR); //left put conversion result
ADCSRA |= (1 << ADEN) //enable
       |(1 << ADPS2)|(1 << ADPS1) //prescaller?
	   |(1 << ADIE); //allow interupt
ACSR |= (1 << ACD); //disable comparator
DIDR0 |= (1 << ADC1D); //input analog
ADCSRA = ADCSRA|(1<<ADSC); //start conversion

sei(); 


//turn on voltage as default
PORTB |= _BV(PB4); 
value++; //only one time

while(1)
{
	ADCSRA |= (1 << ADSC); 
    _delay_ms(50);

if(adc_counter > 10)  //adc_counter links to value voltage
{
	voltage = value/adc_counter;
	adc_counter = 0;
	value = 0;
}


//if(voltage > 4 /*125*/) //2 = ~0.2V on input 
//{
//    PORTB |= _BV(PB4); //turn on voltage
//}
//else
//{
//	PORTB &= ~_BV(PB4); 
//	MCUCR |= _BV(SE); //power-down
//}

//or

if(voltage <= 4 && value == 0 /*for first pass (value=1)*/){
    /*if(counter_down++ > 5)*/{
		PORTB &= ~_BV(PB4);	//turn off power
		MCUCR |= _BV(SE); //power-down
	}
}

}
}


