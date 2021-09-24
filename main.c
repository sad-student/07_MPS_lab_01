#include <msp430.h> 

// L5:1.2 L4:1.1
// S1:1.7 S2:2.2

//#define S1_INV_BEH // enable if button S1 active level is high
//#define S2_INV_BEH // enable if button S2 active level is high
#define INT_CONTROL_IMPL // enable for interrupt implementation switching

#pragma vector = PORT1_VECTOR
__interrupt void S1_interrupt_handler(void)
{
	if(P1IFG & BIT7){
		P1IFG &= ~BIT7;
		P1IES ^= BIT7;
#ifdef S2_INV_BEH
	    if(P2IN & BIT2){
#else
	    if(~P2IN & BIT2){
#endif
#ifdef S1_INV_BEH
	    	if(~P1IES & BIT7){
			//if(~P1IN & BIT7){
#else
	    	if(P1IES & BIT7){
			//if(P1IN & BIT7){
#endif
				// LED5 off
				P1OUT &= ~BIT2;

			} else {
				// LED5 on
				P1OUT |= BIT2;
			}
	    }else{
#ifdef S1_INV_BEH
	    	if(~P1IES & BIT7){
			//if(~P1IN & BIT7){
#else
	    	if(P1IES & BIT7){
	    	//if(P1IN & BIT7){
#endif
				// LED4 off
				P1OUT &= ~BIT1;
			}
	    }
	}
}

#pragma vector = PORT2_VECTOR
__interrupt void S2_interrupt_handler(void)
{
	if(P2IFG & BIT2){
		P2IFG &= ~BIT2;
		// LED4 on
		P1OUT |= BIT1;
	}
}

void control_loop();

int main(void) {
    WDTCTL = WDTPW | WDTHOLD;	// Stop watchdog timer
	
    // LED initialization
    P1SEL &= ~(BIT1 | BIT2);
    P1DIR |= (BIT1 | BIT2);
    P1OUT &= ~(BIT1 | BIT2);

#ifdef INT_CONTROL_IMPL
    __bis_SR_register(GIE);
#endif

    // Button S1: pushed - 0, free - 1
    P1SEL &= ~BIT7;
    P1DIR &= ~BIT7;
    P1REN |= BIT7;
#ifdef INT_CONTROL_IMPL
    // no need to switch depending on button behavior, actually works well on both scenarios
    P1IES = (P1IES & ~BIT7) | (P1IN & BIT7);
    P1IFG &= ~BIT7;
    P1IE |= BIT7;
#endif

    // Button S2: pushed - 1, free - 0
    P2SEL &= ~BIT2;
    P2DIR &= ~BIT2;
    P2REN |= BIT2;

#ifdef INT_CONTROL_IMPL
#ifdef S2_INV_BEH
    P2IES &= ~BIT2;
#else
    P2IES |= BIT2;
#endif
    P2IFG &= ~BIT2;
    P2IE |= BIT2;
#endif

#ifndef INT_CONTROL_IMPL
    control_loop();
#endif
    __no_operation();
	return 0;
}

void control_loop(){
	short int s_state = (P1IN & BIT7) | (P2IN & BIT2);
	while(1){
		if((s_state & BIT7) ^ (P1IN & BIT7)){
			// button 1 event
#ifdef S2_INV_BEH
			if(P2IN & BIT2){
#else
			if(~P2IN & BIT2){
#endif
#ifdef S1_INV_BEH
				if(~P1IN & BIT7){
#else
				if(P1IN & BIT7){
#endif
					// LED5 off
					P1OUT &= ~BIT2;

				} else {
					// LED5 on
					P1OUT |= BIT2;
				}
			}else{
#ifdef S1_INV_BEH
				if(~P1IN & BIT7){
#else
				if(P1IN & BIT7){
#endif
					// LED4 off
					P1OUT &= ~BIT1;
				}
			}
			s_state = (s_state & ~BIT7) | (P1IN & BIT7);
		}
		if((s_state & BIT2) ^ (P2IN & BIT2)){
			// button 2 event
#ifdef S2_INV_BEH
			if(P2IN & BIT2){
#else
			if(~P2IN & BIT2){
#endif
				// LED4 on
				P1OUT |= BIT1;
			}
			s_state = (s_state & ~BIT2) | (P2IN & BIT2);
		}
	}
}
