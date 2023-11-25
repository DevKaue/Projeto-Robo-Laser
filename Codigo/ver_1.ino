#include <avr/io.h>
#include <avr/interrupt.h>

void frente(){

	// rodas funcionando em sentidos opostos, direcionando o carrinho para frente
	PORTD = (1<<PD5);
	PORTB = (1<<PB2);

}

void tras(){

	// rodas funcionando em sentidos opostos, direcionando o carrinho para tras
	PORTD = (1<<PD7);
	PORTB = (1<<PB1);

}

void lula(){

	// rodas funcionando em sentidos iguais, girando o carrinho a esquerda
	PORTD = (1<<PD7);
	PORTB = (1<<PB2);

}

void bolsonaro(){

	// rodas funcionando em sentidos iguais, girando o carrinho a direita
	PORTD = (1<<PD5);
	PORTB = (1<<PB1);

}

int main(void){
	
	DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD7); // setando as portas para o motor direito (IN1(D5), IN2(D7) e PWM(D6))
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3); // setando as portas para o motor esquerdo (IN3(B1), IN4(B2) e PWM(B3))
	DDRC |= (1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC1); // setando os LEDs de vida e o laser(PC1)
	PORTC = (1<<PC5) | (1<<PC4) | (1<<PC3); // deixando os LEDs ligados, a principio
	
	while(1){ // verificação que deve ser execultada continuamente
	
		if(PIND & (1<<PD0)){ // verificando se o botao de andar para frente foi precionado
			frente();
			PORTD ^= (1 << PD5); // Desligar LED em PD5
			PORTB ^= (1 << PB2); // Desligar LED em PB2
		}
		else if(PIND & (1<<PD1)){ // verificando se o botao de andar para trás foi precionado
			tras();
			PORTD ^= (1 << PD7); // Desligar LED em PD7
			PORTB ^= (1 << PB1); // Desligar LED em PB1
		}
		else if(PIND & (1<<PD2)){ // verificando se o botao de andar para esquerda foi precionado
			lula();
			PORTD ^= (1 << PD7); // Desligar LED em PD7
			PORTB ^= (1 << PB2); // Desligar LED em PB2
		}
		else if(PIND & (1<<PD3)){ // verificando se o botao de andar para direita foi precionado
			bolsonaro();
			PORTD ^= (1 << PD5); // Desligar LED em PD5
			PORTB ^= (1 << PB1); // Desligar LED em PB1
		}
	}
}