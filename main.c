/*
 * -----------------------------------------------------
 * M S P 4 3 0  G 2 5 5 3   -  UART SPI三线 主机模式配置
 * 三线  SCLK P1.5 时钟线
 *      SOMI P1.6 主进从出线
 *      SIMO P1.7 主出从进线
 * (c)-Chen Tianyi
 * --------------------------------------------------------------
 *
 * MSP430G2553的通用串行通信接口配置 ：UART接口（和PC） SPI接口（和另外一台MCU）
 */
#include <msp430.h> 
#include <string.h>
#include "stdint.h"


unsigned int fois = 0;          // fois变量用来表示第几次进的中断 为0表示第一次进入
unsigned char rx[20];           // rx数组用来存放uart模式下接受PC端发送来的字符串


/* ----------------------------------------------------------------------------
 * 初始化函数 ：配置时钟的频率 初始化寄存器
 * 输入: -
 * 输出: -
 */
void init(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    if((CALBC1_1MHZ == 0xFF) || (CALDCO_1MHZ == 0xFF))      // 如果DCO校准值没有被改动（= 0xFF表示被修改）
    {
        __low_power_mode_4();   // 等同于__bis_SR_register(LPM4_bits) 进入低功耗模式4
    }
    BCSCTL1 = CALBC1_1MHZ;      // 配置DCO时钟频率为1MHz
    DCOCTL = CALDCO_1MHZ;
    P1SEL &= ~0x00;             // 将端口功能寄存器复位
    P1SEL2 &= ~0x00;
    P2SEL &= ~0x00;
    P2SEL2 &= ~0x00;
    P1DIR &= ~0x00;             //  将IO端口寄存器复位
    P2DIR &= ~0x00;

    P1DIR |= BIT0;              // 设置P1.0端口（led1）信号为输出
    P1OUT &= ~BIT0;             // 设置led1初始状态下为不亮

    __enable_interrupt();       // 打开全局中断
}


void init_uart(void)
{
    UCA0CTL1 |= UCSWRST;
    UCA0CTL0 &= ~UCSYNC;
    UCA0CTL1 |= UCSSEL_2;
    UCA0CTL0 &= ~(UCPEN | UCPAR | UCMSB | UC7BIT | UCSPB);
    UCA0CTL0 |= UCMODE_3;
    P1SEL = BIT1 + BIT2 ;
    P1SEL2 = BIT1 + BIT2 ;
    UCA0BR0 = 104;              // 1MHz 9600
    UCA0BR1 = 0;
    UCA0MCTL |= UCBRS0;         // modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;
    IE2 |= UCA0RXIE;
    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);
}


void init_spi(void)
{
    UCB0CTL1 |= UCSWRST;
    UCB0CTL1 |= UCSSEL_2;
    P1SEL |= (BIT5 | BIT6 | BIT7);
    P1SEL2 |= (BIT5 | BIT6 | BIT7);
    UCB0CTL0 &= ~(UCCKPH | UCCKPL);     // SPI Mode 1
    UCB0CTL0 |= (UCMODE_0 | UCMST | UCSYNC);
    UCB0CTL0 &= ~(UCMSB | UC7BIT);
    UCB0BR0 = 0x0A;
    UCB0BR1 = 0x00;
    UCB0CTL1 &= ~UCSWRST;
    //IE2 |= UCB0RXIE;
    IFG2 &= ~UCB0RXIFG;
}


void sendstring_uart(unsigned char *chaine)
{
    while(*chaine != '\0')
    {
        while(UCA0STAT & UCBUSY);
        UCA0TXBUF = *chaine;
        chaine ++;
    }
}


int main(void)
{
	init();
	init_uart();
	init_spi();
	sendstring_uart("\rGet ready !\n");
	return 0;
}

#pragma vector = USCIAB0RX_VECTOR
__interrupt void Rx(void)
{
    static unsigned int cmt = 0;
    IFG2 &= ~UCA0RXIFG;
    if(fois == 0)
    {
        sendstring_uart("\rPressez 'h' pour continuer");
    }
    if(fois == 1)
    {
        rx[cmt ++] = UCA0RXBUF;
        //if(rx[cmt - 1] == '\r')
        {
            if((rx[0] == 'h')) //&& (rx[1] == '\r'))
            {
                sendstring_uart("\rPressez 'h' pour afficher les commandes\n");
                sendstring_uart("\rPressez 'a' pour allumer led vert\n");
                sendstring_uart("\rPressez 'b' pour eteindre led vert\n");
                sendstring_uart("\rPressez 'ver' pour voir la version\n\n");
            }
            else if((rx[0] == 'a')) //&& (rx[1] == '\r'))
            {
                P1OUT |= BIT0;
                UCB0TXBUF = 0x31;
            }
            else if((rx[0] == 'b')) //&& (rx[1] == '\r'))
            {
                P1OUT &= ~BIT0;
                UCB0TXBUF = 0x30;
            }
            else if((rx[0] == 'v'))// && (rx[1] == 'e') && (rx[2] == 'r') && (rx[3] == '\r'))
            {
                sendstring_uart("\rversion : 1.0\n");
            }
            else
            {
                sendstring_uart("\rmauvais command\n");
            }
            cmt = 0;
        }
    }
    fois = 1;
}

