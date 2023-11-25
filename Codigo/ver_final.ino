#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t ADC_value;
volatile uint8_t giro_180 = 0;

int dano = 0;
//int a = 1;

ISR(TIMER1_COMPA_vect){
  giro_180 = 1;
	PORTC ^= (1 << PC1);
}

ISR(ADC_vect){
	ADC_value = ADC;
}

void setup(){
	
	UCSR0B &= ~((1 << RXB80) | (1 << TXB80)); // desligando a interrupção para a flag  RXC0 e  TXC0
	DDRD |= (1<<DDD5) | (1<<DDD6) | (1<<DDD7); // setando as portas para o motor direito (IN1(D5), IN2(D7) e PWM(D6))
	DDRB |= (1<<PB1) | (1<<PB2) | (1<<PB3); // setando as portas para o motor esquerdo (IN3(B1), IN4(B2) e PWM(B3))
	DDRC |= (1<<PC5) | (1<<PC4) | (1<<PC3) | (1<<PC1); // setando os LEDs de vida e o laser(PC1)
	PORTC = (1<<PC5) | (1<<PC4) | (1<<PC3); // deixando os LEDs ligados, a principio
	PORTD = (1<<PD4)  | (1<<PD2) | (1<<PD3); // definindo como habilitado o pull-up interno do arduino
	PORTB = (1<<PB0); // definindo como habilitado o pull-up interno do arduino
	sei();
}

void rotina_padrao(){
	//a = 1;
	ADC_read();
	
	if(!(PIND & (1<<PD4))){ // verificando se o botao de andar para frente foi precionado
		frente();
	}
	else if(!(PINB & (1<<PB0))){ // verificando se o botao de andar para trás foi precionado
		tras();
	}
	else if(!(PIND & (1<<PD2))){ // verificando se o botao de andar para esquerda foi precionado
		lula();
	}
	else if(!(PIND & (1<<PD3))){ // verificando se o botao de andar para direita foi precionado
		bolsonaro();
	}
	
}


void ADC_init(){

	ADMUX |= (1<<REFS0); // Referencia externa para o ADC
	
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADEN) | (1<<ADIE); // ADC configurado com prescaller de 128 
															    // também ativa interrupções depois da conversão
															    // ADC
}

void pwm_init(){

	TCCR0A = (1<<COM0A1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
	TCCR2A = (1<<COM2A1) | (1<<WGM21) | (1<<WGM20);
	
	TCCR0B = (1<<CS00); // sem prescaller
	TCCR2B = (1<<CS10); // sem prescaller

  // força de tração dos motores
	OCR0A = 180;
	OCR2A = 180;
}

void atira_timer1(){
	
	TCCR1A |= (1<<WGM12);// toggle CTC
	
	TCCR1B |= (1<<CS12); // prescaller de 256
	
	TIMSK1 |= (1<<OCIE1A); // ativando interrupções para o timer1
	
	OCR1A = 62500; // valor a ser utilizado para a comparação
	
	TCNT1 = 0; // inicializa o timer1
	
}

void vida_verifica(){

	if (dano == 3)
	{
		PORTC ^= (1<<PC5); // apaga a ultima luz
		dano = dano + 1;
	}
	else if (dano == 2)
	{
		PORTC ^= (1<<PC4); // apaga a segunda luz
		dano = dano + 1;
	}
	else if (dano == 1)
	{
		PORTC ^= (1<<PC3); // apaga a primeira luz
		dano = dano + 1;
	}
 	 else if (dano == 0)
	{
    		dano = dano + 1;
  	}
		rotina_dano();
}


void rotina_dano()
{
	/*
	// desliga os motores de forma forçada para evitar que eles fiquem ativos durante o periodo de inatividade
	if(!(PIND & (1<<PD4))){ 
	
		PORTD &= ~(1 << PD5); // Desligar LED em PD5
		PORTB &= ~(1 << PB2); // Desligar LED em PB2
	}
	else if(!(PINB & (1<<PB0))){
	 
		PORTD &= ~(1 << PD7); // Desligar LED em PD7
		PORTB &= ~(1 << PB1); // Desligar LED em PB1
	}
	else if(!(PIND & (1<<PD2))){ 
	
		PORTD &= ~(1 << PD7); // Desligar LED em PD7
		PORTB &= ~(1 << PB2); // Desligar LED em PB2
	}
	else if(!(PIND & (1<<PD3))){ 
	
		PORTD &= ~(1 << PD5); // Desligar LED em PD5
		PORTB &= ~(1 << PB1); // Desligar LED em PB1
	}
	*/
	
	if (!(PIND & (1 << PD4))) {
    PORTD &= ~(1 << PD5); // Desligar LED em PD5
    PORTB &= ~(1 << PB2); // Desligar LED em PB2
  } else if (!(PINB & (1 << PB0))) {
    PORTD &= ~(1 << PD7); // Desligar LED em PD7
    PORTB &= ~(1 << PB1); // Desligar LED em PB1
  } else if (!(PIND & (1 << PD2))) {
    PORTD &= ~(1 << PD7); // Desligar LED em PD7
    PORTB &= ~(1 << PB2); // Desligar LED em PB2
  } else if (!(PIND & (1 << PD3))) {
    PORTD &= ~(1 << PD5); // Desligar LED em PD5
    PORTB &= ~(1 << PB1); // Desligar LED em PB1
  }
	
	//if (PINC & (1<<PC1)){ PORTC ^= (1<<PC1);} // desliga o laser de forma forçada para evitar que ele fique ativo 
	// durante a inatividade
  	if (PINC & (1 << PC1)) 
	{
    		PORTC ^= (1 << PC1);
  	}
 

	while (TCNT1 <= 6000) 
	{
    		// Operações dentro do loop - verificar se estão corretas para o propósito
  	}

	TCNT1 = 0; // zerando o timer1
	
	 while (TCNT1 <= 31250) 
	{
    		PORTD |= (1 << PD7);
    		PORTB |= (1 << PB1);
  	}

	TCNT1 = 0; // zerando o timer1
/*
  TCNT1 = 0; // zerando o timer1

  while(TCNT1 <= 6000);

  TCNT1 = 0; // zerando o timer1

	while(TCNT1 <= 31250){ // tempo em que os motores devem funcionar para girar em 180 graus
		PORTD |= (1<<PD7);
		PORTB |= (1<<PB1);
	}
	
  // TCNT1 = 0; // zerando o timer1
*/
  	PORTD |= ~(1<<PD7);
	PORTB |= ~(1<<PB1);
	
	parado(); // função para deixar o "Gigante de MDF" inativo
}

void parado(){

	TCCR1A = 0;// retirando o toggle CTC (temporario)
	
	TCCR1B |= (1<<CS12) | (1<<CS10); // prescaller de 1024 (temporario)
	
	TIMSK1 |= 0; // desativando interrupções para o timer1 (temporario)

	TCNT1 = 0;
	while(TCNT1 <= 65534);
	TCNT1 = 0;
	while(TCNT1 <= 12589); // ambos while servem para que o "Gigante de MDF" fique inativo por 5 segundos

	while (dano == 4);

	TCCR1A |= (1<<WGM12);// voltando com o toggle CTC
	
	TCCR1B ^= (1<<CS10); 
	
	TIMSK1 |= (1<<OCIE1A); // reativando interrupções para o timer1
}

void ADC_read(){
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADIF));
	ADCSRA |= (1<<ADIF);
	if(ADC_value <= 100){ // simulando o sensor sendo estimulado
		vida_verifica();
	}
}
/*
void frente()
{

  while(!(PIND & (1<<PD4)))
    {
      // rodas funcionando em sentidos opostos, direcionando o carrinho para frente
      PORTD |= (1<<PD5);
      PORTB |= (1<<PB2);
      ADC_read();
	  }

	PORTD &= ~(1<<PD5);
	PORTB &= ~(1<<PB2);
}

void tras(){

	while(!(PINB & (1<<PB0))){
		// rodas funcionando em sentidos opostos, direcionando o carrinho para tras
		 PORTD |= (1<<PD7);
		 PORTB |= (1<<PB1);
		ADC_read();
	}
	PORTD &= ~(1<<PD7);
	PORTB &= ~(1<<PB1);
}

void lula(){
	
	while(!(PIND & (1<<PD2)))
  {
		// rodas funcionando em sentidos iguais, girando o carrinho a esquerda
		PORTD |= (1<<PD7);
		PORTB |= (1<<PB2);
		ADC_read();
	}
	PORTD &= ~(1<<PD7);
	PORTB &= ~(1<<PB2);
}

void bolsonaro(){
	
	while(!(PIND & (1<<PD3)))
  {
		// rodas funcionando em sentidos iguais, girando o carrinho a direita
		PORTD |= (1<<PD5);
		PORTB |= (1<<PB1);
		ADC_read();
	}
	PORTD &= ~(1<<PD5);
	PORTB &= ~(1<<PB1);
}
*/

void frente() {
  while (!(PIND & (1 << PD4))) {
    // rodas funcionando em sentidos opostos, direcionando o carrinho para frente
    PORTD |= (1 << PD5);
    PORTD &= ~(1 << PD7); // Certifique-se de desligar o motor PD7
    PORTB |= (1 << PB2);
    ADC_read();
  }

  PORTD &= ~(1 << PD5);
  PORTB &= ~(1 << PB2);
}

void tras() {
  while (!(PINB & (1 << PB0))) {
    // rodas funcionando em sentidos opostos, direcionando o carrinho para trás
    PORTD |= (1 << PD7);
    PORTD &= ~(1 << PD5); // Certifique-se de desligar o motor PD5
    PORTB |= (1 << PB1);
    ADC_read();
  }
  PORTD &= ~(1 << PD7);
  PORTB &= ~(1 << PB1);
}

void lula() {
  while (!(PIND & (1 << PD2))) {
    // rodas funcionando em sentidos iguais, girando o carrinho à esquerda
    PORTD |= (1 << PD7);
    PORTD &= ~(1 << PD5); // Certifique-se de desligar o motor PD5
    PORTB |= (1 << PB2);
    ADC_read();
  }
  PORTD &= ~(1 << PD7);
  PORTB &= ~(1 << PB2);
}

void bolsonaro() {
  while (!(PIND & (1 << PD3))) {
    // rodas funcionando em sentidos iguais, girando o carrinho à direita
    PORTD |= (1 << PD5);
    PORTD &= ~(1 << PD7); // Certifique-se de desligar o motor PD7
    PORTB |= (1 << PB1);
    ADC_read();
  }
  PORTD &= ~(1 << PD5);
  PORTB &= ~(1 << PB1);
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