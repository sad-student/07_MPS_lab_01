#include <msp430.h> 

// 7.1 2.2

#pragma vector = PORT1_VECTOR
__interrupt void S1_interrupt_handler(void)
{
	if(P1IFG & BIT7){
		P1IFG &= ~BIT7;

	    if(~P2IN & BIT2){
			P1IES ^= BIT7;

			P1SEL &= ~BIT2;
			P1DIR |= BIT2;
			if(P1IN & BIT7){
				// LED off
				P1OUT &= ~BIT2;

			} else {
				// LED on
				P1OUT |= BIT2;
			}
	    }
	}
}

#pragma vector = PORT2_VECTOR
__interrupt void S2_interrupt_handler(void)
{
	if(P2IFG & BIT2){
		P2IFG &= ~BIT2;

	    if(~P2IN & BIT2){

			P1SEL &= ~BIT1;
			P1DIR |= BIT1;

			if(P2IN & BIT2){
				// LED on
				P1OUT |= BIT1;
			}
	    }
	}
}

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    // LED initialization
    P1SEL &= ~(BIT1 | BIT2 | BIT7);
    P1DIR |= (BIT1 | BIT2);
    P1OUT &= ~(BIT1 | BIT2);

    __bis_SR_register(GIE);


    // Button S1: pushed - 0, free - 1
    P1DIR &= ~BIT7;
    P1REN |= BIT7;
    P1IES = (P1IES & ~BIT7) | (P1IN & BIT7);
    P1IFG &= ~BIT7;
    P1IE |= BIT7;

    // Button S2: pushed - 1, free - 0
    P2SEL &= ~BIT2;
    P2DIR &= ~BIT2;
    P2REN |= BIT2;
    P2IES &= ~BIT2;
    // P2IES = (P2IES & ~BIT2) | (P2IN & ~BIT2);
    P2IFG &= ~BIT2;
    P2IE |= BIT2;


    while(1){
    	__no_operation();
    }
	return 0;
}
