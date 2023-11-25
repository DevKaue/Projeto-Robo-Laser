#include <avr/io.h>
#include <avr/interrupt.h>

void setup(){
	
	UCSR0B |= (1<<RXB80) | (1<<TXB80); // desligando a interrupção para a flag  RXC0 e  TXC0
	DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD7); // setando as portas para o motor direito (IN1(D5), IN2(D7) e PWM(D6))
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3); // setando as portas para o motor esquerdo (IN3(B1), IN4(B2) e PWM(B3))
	DDRC |= (1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC1); // setando os LEDs de vida e o laser(PC1)
	PORTC = (1<<PC5) | (1<<PC4) | (1<<PC3); // deixando os LEDs ligados, a principio
	PORTD = (1<<PD0) | (1<<PD1) | (1<<PD2) | (1<<PD3); // definindo como habilitado o pull-up interno do arduino
	sei();
}

void rotina_padrao(){
	
	if(!(PIND & (1<<PD0))){ // verificando se o botao de andar para frente foi precionado
		frente();
	}
	else if(!(PIND & (1<<PD1))){ // verificando se o botao de andar para trás foi precionado
		tras();
	}
	else if(!(PIND & (1<<PD2))){ // verificando se o botao de andar para esquerda foi precionado
		lula();
	}
	else if(!(PIND & (1<<PD3))){ // verificando se o botao de andar para direita foi precionado
		bolsonaro();
	}
	
}

void pwm_init(){

	TCCR0A = (1<<COM0A1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
	TCCR2A = (1<<COM1A1) | (1<<WGM11) | (1<<WGM10) | (1<<WGM12) |  (1<<WGM13);
	
	TCCR0B = (1<<CS00); // sem prescaller
	TCCR2B = (1<<CS10); // sem prescaller

  OCR0A = 180;
}

void frente(){

	// força de tração dos motores

		// rodas funcionando em sentidos opostos, direcionando o carrinho para frente
		PORTD |= (1<<PD5);

}

void tras(){

	// força de tração dos motores


		// rodas funcionando em sentidos opostos, direcionando o carrinho para tras
		PORTD |= (1<<PD7);

}

void lula(){

	// força de tração dos motores


		// rodas funcionando em sentidos iguais, girando o carrinho a esquerda
		PORTD |= (1<<PD7);

}

void bolsonaro(){

	// força de tração dos motores

		// rodas funcionando em sentidos iguais, girando o carrinho a direita
		PORTD |= (1<<PD5);

}

int main(void){
	
	setup(); // estabelece todas as portas
	pwm_init(); // estabelece os parametros para o PWM
	
	while(1){ // verificação que deve ser execultada continuamente
		rotina_padrao();
	}
}