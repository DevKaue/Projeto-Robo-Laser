/**
 *@file 	 ver_final.ino
 *@brief	 Projeto da matéria Programação de Hardware do 6º ciclo de engenharia de computação 
 *@author | Kaue Wendt Sabino | Alec Gomez Yagi Rodrigues | Leonardo Raposo Rocha | Leonardo Montagner de Ramos Vallim |
 *@see https://github.com/DevKaue/Projeto-Robo-Laser
 *@date 25-11-2023
*/

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint16_t ADC_value; ///< Variável para armazenar o valor do ADC
volatile uint8_t giro_180 = 0; ///< Variável para auxiliar no giro de 180graus

int dano = 0; ///< Variável para auxiliar no dano, do sistema de vida
//int a = 1;


/**
* @brief Interrupção do Timer1
*/
ISR(TIMER1_COMPA_vect){
  giro_180 = 1;
	PORTC ^= (1 << PC1);
}

/**
* @brief Interrupção do ADC
*/
ISR(ADC_vect){
	ADC_value = ADC;
}

/**
* @brief Void Setup
*/
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

/**
* @brief Rotina padrão, para movimentação
*/
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

/**
* @brief Configuração do ADC
*/
void ADC_init(){

	ADMUX |= (1<<REFS0); // Referencia externa para o ADC
	
	ADCSRA |= (1<<ADPS2) | (1<<ADPS1) | (1<<ADEN) | (1<<ADIE); // ADC configurado com prescaller de 128 
															    // também ativa interrupções depois da conversão
															    // ADC
}

/**
* @brief Configuração dos PWM
*/
void pwm_init(){

	TCCR0A = (1<<COM0A1) | (1<<WGM02) | (1<<WGM01) | (1<<WGM00);
	TCCR2A = (1<<COM2A1) | (1<<WGM21) | (1<<WGM20);
	
	TCCR0B = (1<<CS00); // sem prescaller
	TCCR2B = (1<<CS10); // sem prescaller

  // força de tração dos motores
	OCR0A = 180;
	OCR2A = 180;
}

/**
* @brief Função do Acionamento do Laser
*/
void atira_timer1(){
	
	TCCR1A |= (1<<WGM12);// toggle CTC
	
	TCCR1B |= (1<<CS12); // prescaller de 256
	
	TIMSK1 |= (1<<OCIE1A); // ativando interrupções para o timer1
	
	OCR1A = 62500; // valor a ser utilizado para a comparação
	
	TCNT1 = 0; // inicializa o timer1
	
}


/**
* @brief Função de Verificação do sistema de Vida
*/
void vida_verifica(){

	if (dano == 3){
		PORTC ^= (1<<PC5); // apaga a ultima luz
		dano = dano + 1;
	}
	else if (dano == 2){
		PORTC ^= (1<<PC4); // apaga a segunda luz
		dano = dano + 1;
	}
	else if (dano == 1){
		PORTC ^= (1<<PC3); // apaga a primeira luz
		dano = dano + 1;
	}
  else if (dano == 0){
    dano = dano + 1;
  }
	rotina_dano();
}

/**
* @brief Rotina do sistema de recebimento de Dano
*/
void rotina_dano(){
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

	if (PINC & (1<<PC1)){ PORTC ^= (1<<PC1);} // desliga o laser de forma forçada para evitar que ele fique ativo 
	// durante a inatividade
  
  // _delay_ms(2000);

  TCNT1 = 0; // zerando o timer1

  while(TCNT1 <= 1000);

  TCNT1 = 0; // zerando o timer1

	while(TCNT1 <= 31250){ // tempo em que os motores devem funcionar para girar em 180 graus
		PORTD |= (1<<PD7);
		PORTB |= (1<<PB1);
	}
	
  // TCNT1 = 0; // zerando o timer1

  PORTD &= ~(1<<PD7);
	PORTB &= ~(1<<PB1);
	
	parado(); // função para deixar o "Gigante de MDF" inativo
}

//Teste de função com giro de 180graus
/*
void rotina_dano() {
    // desliga os motores de forma forçada para evitar que eles fiquem ativos durante o período de inatividade
    if (!(PIND & (1 << PD4))) {

        PORTD &= ~(1 << PD5); // Desligar LED em PD5
        PORTB &= ~(1 << PB2); // Desligar LED em PB2
    }
    else if (!(PINB & (1 << PB0))) {

        PORTD &= ~(1 << PD7); // Desligar LED em PD7
        PORTB &= ~(1 << PB1); // Desligar LED em PB1
    }
    else if (!(PIND & (1 << PD2))) {

        PORTD &= ~(1 << PD7); // Desligar LED em PD7
        PORTB &= ~(1 << PB2); // Desligar LED em PB2
    }
    else if (!(PIND & (1 << PD3))) {

        PORTD &= ~(1 << PD5); // Desligar LED em PD5
        PORTB &= ~(1 << PB1); // Desligar LED em PB1
    }

    if (PINC & (1 << PC1)) { PORTC ^= (1 << PC1); } // desliga o laser de forma forçada para evitar que ele fique ativo 
    // durante a inatividade

    // Girar 180 graus
    // giro_180 = 1; // Sinaliza o giro de 180 graus
    // TCNT1 = 0; // Reinicia o timer
    // while (TCNT1 <= 31250) {
    //     if (giro_180) {
    //         PORTD |= (1 << PD7); // Ativa o motor esquerdo para girar no sentido horário
    //         PORTB |= (1 << PB1); // Ativa o motor direito para girar no sentido anti-horário
    //     } else {
    //         PORTD &= ~(1 << PD7); // Desliga o motor esquerdo
    //         PORTB &= ~(1 << PB1); // Desliga o motor direito
    //         break; // Sai do loop quando o giro é concluído
    //     }
    // }

    // if (dano > 0) {
    //     // Ativar temporizador para girar 180 graus
    //     giro_180 = 0; // Reseta a flag
    //     TCNT1 = 0; // Reinicia o timer
    //     OCR1A = 31250; // Defina um valor apropriado para o tempo de giro (ajuste conforme necessário)
    //     TIMSK1 |= (1 << OCIE1A); // Ativa interrupção por comparação
    // }

    // giro_180 = 0; // Reseta a flag após o giro

    TCNT1 = 0; // Zera o timer1

    PORTD &= ~(1 << PD7);
    PORTB &= ~(1 << PB1);

    parado(); // função para deixar o "Gigante de MDF" inativo
}
*/

/**
* @brief Função para parar sistema do Gigante de MDF
*/
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

/**
* @brief Leitura do ADC
*/
void ADC_read(){
	ADCSRA |= (1<<ADSC);
	while(ADCSRA & (1<<ADIF));
	ADCSRA |= (1<<ADIF);
	if(ADC_value <= 100){ // simulando o sensor sendo estimulado
		vida_verifica();
	}
}

/**
* @brief Movimentação para frente
*/
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

/**
* @brief Movimentação para tras
*/
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

/**
* @brief Movimentação para esquerda
*/
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

/**
* @brief Movimentação para direita
*/
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


/**
* @brief Função Principal
*/
int main(void){
	setup(); // estabelece todas as portas
	pwm_init(); // estabelece os parametros para o PWM
	ADC_init(); // estabelece os parametros para o ADC
	atira_timer1(); // rotina para deixar o LED ligado por 1 segundo com intervalos de mesmo tempo
	
	while(1){ // verificação que deve ser execultada continuamente
		rotina_padrao();
	}
}