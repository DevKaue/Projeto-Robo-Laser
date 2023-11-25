#include <avr/io.h>
#include <avr/interrupt.h>

#define POT 10000

int dano = 0;
int valorADC = 0;
int valorpot = 0;

ISR(TIMER1_COMPA_vect){
	PORTC ^= (1 << PC1);
}

void setup(){

	DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD7); // setando as portas para o motor direito (IN1(D5), IN2(D7) e PWM(D6))
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3); // setando as portas para o motor esquerdo (IN3(B1), IN4(B2) e PWM(B3))
	DDRC |= (1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC1); // setando os LEDs de vida e o laser(PC1)
	PORTC = (1<<PC5) | (1<<PC4) | (1<<PC3); // deixando os LEDs ligados, a principio
	sei();
}

void rotina_padrao(){

	valorADC = ADC_read(0); // executando a conversão ADC na porta A0 e armazeno o resultado em valorADC
	valorpot = (POT/1024)*valorADC; // Valor medido em Ohms
		
	if(valorpot == 9198){ // caso o sensor receba um tiro de laser
		vida_verifica();
	}
				
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

void ADC_init(){

	ADMUX |= (1<<REFS0); // Referencia externa para o ADC
	
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADEN); // ADC configurado com prescaller de 128
}

void pwm_init(){

	TCCR0A = (1<<COM0A1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
	TCCR2A = (1<<COM1A1) | (1<<WGM11) | (1<<WGM10) | (1<<WGM12) |  (1<<WGM13);
	
	TCCR0B = (1<<CS00); // sem prescaller
	TCCR2B = (1<<CS10); // sem prescaller
}

void atira_timer1(){
	
	TCCR1A |= (1<<WGM12);// toggle CTC
	
	TCCR1B |= (1<<CS12); // prescaller de 256
	
	TIMSK1 |= (1<<OCIE1A); // ativando interrupções para o timer1
	
	OCR1A = 62500; // valor a ser utilizado para a comparação
	
	TCNT1 = 0; // inicializa o timer1
	
}

void vida_verifica(){
	if (dano == 2){
		PORTC ^= (1<<PC5); // apaga a ultima luz
		dano = dano + 1;
	}
	else if (dano == 1){
		PORTC ^= (1<<PC4); // apaga a segunda luz
		dano = dano + 1;
	}
	else if (dano == 0){
		PORTC ^= (1<<PC3); // apaga a primeira luz
		dano = dano + 1;
	}
	girar();
}

void girar(){

	if (PINC & (1<<PC1)){ PORTC ^= (1<<PC1); } // desliga o laser de forma forçada para evitar que ele fique ativo 
									   // durante a inatividade

	TCNT1 = 0; // zerando o timer1
	//---------------- WORK IN PROGRESS ----------------//
	//while(TCNT1 <= ){ // tempo em que os motores devem funcionar para girar em 180 graus
	
	//}
	
	parado(); // função para deixar o "Gigante de MDF" inativo
}

void parado(){

	if (PIND & (1<<PD0)){
		if (PIND & (1<<PD1)){ // caso os dois botoes estejam precionados deve resetar o funcionamento
			dano = 0; // reset para a quantidade de dano tomado
			main();   // reset total
		}
	}
	TCNT1 = 0;
	while(TCNT1 <= 65534);
	TCNT1 = 0;
	while(TCNT1 <= 12589); // ambos while servem para que o "Gigante de MDF" fique inativo por 5 segundos
	if (dano == 3){
		parado(); // garante que o "Gigante de MDF" não funcione após levar o ultimo tiro
	}
}

uint16_t ADC_read(uint8_t ADCchannel){

	ADMUX = (ADMUX & 0xF0) | (ADCchannel & 0x0F); // definindo a porta ADC que deve ser lida com mascara de segurança
	
	ADCSRA |= (1<<ADSC); // single mode
	
	while(ADCSRA & (1<<ADSC)); // espera pela finalização da conversão
	return ADC;
}

void frente(){
	// rodas funcionando em sentidos opostos, direcionando o carrinho para frente
	PORTD = (1<<PD5);
	PORTB = (1<<PB2);
	// força de tração dos motores
	OCR0A = 180;
	OCR2A = 180;
}

void tras(){
	// rodas funcionando em sentidos opostos, direcionando o carrinho para tras
	PORTD = (1<<PD7);
	PORTB = (1<<PB1);
	// força de tração dos motores
	OCR0A = 180;
	OCR2A = 180;
}

void lula(){
	// rodas funcionando em sentidos iguais, girando o carrinho a esquerda
	PORTD = (1<<PD7);
	PORTB = (1<<PB2);
	// força de tração dos motores
	OCR0A = 80;
	OCR2A = 80;
}

void bolsonaro(){
	// rodas funcionando em sentidos iguais, girando o carrinho a direita
	PORTD = (1<<PD5);
	PORTB = (1<<PB1);
	// força de tração dos motores
	OCR0A = 80;
	OCR2A = 80;
}

int main(void){
	
	setup(); // estabelece todas as portas
	pwm_init(); // estabelece os parametros para o PWM
	ADC_init(); // estabelece os parametros para o ADC
	atira_timer1(); // rotina para deixar o LED ligado por 1 segundo com intervalos de mesmo tempo
	
	while(1){ // verificação que deve ser execultada continuamente
		rotina_padrao();
	}
}