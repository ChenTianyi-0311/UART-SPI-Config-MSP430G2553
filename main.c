#include <msp430.h> 

char *string1="bonjour\n";
char string2[]="tout le monde\n";


void putchar(unsigned char tx_data) //发送字符函数
{
while (!(IFG2&UCA0TXIFG)); // USCI_A0 TX buffer ready? 等待TX buffer为空
UCA0TXBUF = tx_data;// TX -> RXed character? 发送字符c
}

void putstr(char *s)//发送字符串函数
{
IE2 &= ~UCA0RXIE;//发送时先关闭接收中断，不接收
//如果没有发完，就继续循环发送
while((*s)!='\0')
{
putchar(*s);
s++;
}
IE2 |= UCA0RXIE; //发送完了打开接收中断
}


/*void USCI_A0_init(void){
      DCOCTL = 0;                               // Select lowest DCOx and MODx settings
      BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
      DCOCTL = CALDCO_1MHZ;

      P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
      P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD

      P1DIR |=BIT0;
      P1DIR |=BIT6;

      UCA0CTL1 |= UCSSEL_2;                     // SMCLK
      UCA0BR0 = 104;                            // 1MHz 9600
      UCA0BR1 = 0;                              // 1MHz 9600
      UCA0MCTL =UCBRS_0;                      // Modulation UCBRSx = 1
      UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

      IE2 |= UCA0RXIE+UCA0TXIE;
      _enable_interrupts();

      _bis_SR_register(LPM0_bits);
 }*/

#pragma vector=USCIAB0TX_VECTOR
__interrupt void USCI0TX_ISR(void)
{
   //while(!(IFG2 & UCA0TXIFG));
    IFG2&=~UCA0TXIFG;
    //UART_OnTx();

    P1OUT^=BIT6;
}


#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
  //while (!(IFG2&UCA0TXIFG));
   IFG2&=~UCA0RXIFG;
   //UCA0TXBUF = UCA0RXBUF+1 ;
   UCA0TXBUF = 0x00;
   putstr(string1);
   //putstr(string1);



   //UART_OnRx();

  P1OUT^=BIT0;
}

/*void UART_OnRx()
{
    unsigned char Temp=0;
    Temp=UCA0RXBUF;
    if(Temp==0x0d)
    {
        if(Rx_FIFO_DataNum>0)
        {
            Command_match();
          Rx_FIFO_Clear();
        }
        else{
            UART_SendString(String1);
        }
    }
    else
    {
        Rx_FIFO_WriteChar(Temp);
    }
}

void UART_OnTx(void)
{
 unsigned char Temp;
 if(Tx_FIFO_DataNum>0)
 {
  Tx_FIFO_ReadChar(&Temp);
  UCA0TXBUF= Temp;
 }
}*/


/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW + WDTHOLD;                 // Stop WDT
    DCOCTL = 0;                               // Select lowest DCOx and MODx settings
    BCSCTL1 = CALBC1_1MHZ;                    // Set DCO
    DCOCTL = CALDCO_1MHZ;
    P1SEL = BIT1 + BIT2 ;                     // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2 ;                    // P1.1 = RXD, P1.2=TXD
    P1DIR = BIT0;                             //p1.0外接LED为输出状态
    P1OUT &= ~BIT0;
    UCA0CTL1 |= UCSSEL_2;                     // SMCLK
    UCA0BR0 = 104;                            // 1MHz 9600
    UCA0BR1 = 0;                              // 1MHz 9600
    UCA0MCTL = UCBRS0;                        // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**
    IE2 |= UCA0RXIE+UCA0TXIE;                     // Enable USCI_A0 RX interrupt
    putstr(string1);
    putstr(string2);

    _enable_interrupts();
    __bis_SR_register(LPM0_bits);       // Enter LPM0, interrupts enabled
    return 0;
}

