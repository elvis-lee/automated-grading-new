#include "main.h"
#include "uart.h"

#define myUSART 1
#define BufferRec 0
#define BufferSend 1
#define RecPack 0
#define Gen_Samp 1
#define SendPack 2

//=====define number of packets to transfer each time before a check=====
#define M 100
//=====define packet byte size=====
#define N 10
//=====define total packet number
#define Npack 5678
//=====external variables=====
extern __IO uint32_t TimingDelay;
//=====buffer=====
uint8_t buf[QUEUE_SIZE];
uint8_t *tem_buf_ptr;

//=====data array=====
uart_data_t data_array_rec[6000];
uart_data_t data_array_send[6000];
//=====flag for data array=====
__IO uint16_t data_array_rec_length= 0 ; 
__IO uint16_t data_array_send_length= 0 ;
//=====counter slows down buf read=====
uint32_t cnt1 = 0;
//=====state variable=====
uint8_t state = RecPack;
//=====tansfer success flag=====
uint8_t rec_success = 1;
//=====function declare=====
void pack_push(uint8_t *addr, uint8_t IOtype);
uart_data_t* pack_pop(uint8_t IOtype);
void pack_send(const uint8_t *data, uint16_t len);
void State1_RecPack(void);
void State2_Gen_Samp(void);
void State3_SendPack(void);


int main(int argc, char* argv[])
{  
    pins_setup();
	uart_open(myUSART,460800,0);
    //SysTick_Config(SystemCoreClock/200000); 
    NVIC_SetPriority(SysTick_IRQn,0);
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);

	while (1)	
	{  
        switch (state)
        {
            case RecPack:
            State1_RecPack();
            break;

            case Gen_Samp:
            State2_Gen_Samp();
            break;

            case SendPack:
            State3_SendPack();
            break;

            default: 
            break;
        }
	}
}	

//=====send packet=====
void pack_send(const uint8_t *data, uint16_t len)
{
    uint8_t packet[10];
    uint8_t i;
    packet[0] = 'S';
    packet[9] = 'E';
    for (i = 1; i < 9; i++)
        packet[i] = data[i-1];
    Enqueue(&UART1_TXq, packet, len+2);
}


//=====packet push=====
void pack_push(uint8_t *addr,uint8_t IOtype)
{   
	if (!IOtype)
	{
		data_array_rec[data_array_rec_length] = *(uart_data_t*)addr;
        data_array_rec_length++;
	}

	if (IOtype == 1)
	{
		data_array_send[data_array_send_length] = *((uart_data_t*)addr);
        data_array_send_length++;
	}	
	
	
}

//=====packet pop=====
uart_data_t* pack_pop(uint8_t IOtype)
{
	if (!IOtype)
	{   
        data_array_rec_length--;
    	return &(data_array_rec[data_array_rec_length]);
	}

	if (IOtype == 1)
	{
        data_array_send_length--;
    	return &(data_array_send[data_array_send_length]);
	}	
}

//=====State1:Receive Packets=====
void State1_RecPack(void)
{   
    uint32_t ri;

    GPIO_SetBits(GPIOD,LED3_PIN);
    if (data_array_rec_length == Npack)
    {
        GPIO_ResetBits(GPIOD,LED3_PIN);
        state = Gen_Samp;
        return;
    }
    else

    if (cnt1 < 80) 
        cnt1++;
    else 
    {
        cnt1 = 0;
        ri = pack_avail(&UART1_RXq);
        if (ri >= N)
        {
            ri = Dequeue(&UART1_RXq,buf,N);
            for (tem_buf_ptr = buf; tem_buf_ptr - buf < ri ;tem_buf_ptr = tem_buf_ptr + sizeof(uart_frame_t))
            {
                if (*tem_buf_ptr != 'S' || *(tem_buf_ptr+9) != 'E')
                    {
                        rec_success = 0;
                        GPIO_SetBits(GPIOD,LED6_PIN);
                    }
                pack_push(tem_buf_ptr+1,BufferRec);
            }

        }

    }

}
//=====State2:Generate Output and Sample Input=====
void State2_Gen_Samp(void)
{
    uint32_t i;
    GPIO_SetBits(GPIOD,LED4_PIN);
    for (i = 0; i < Npack; i++)
    {
        data_array_send[i] = data_array_rec[i];
    }
    data_array_send_length = Npack;
    //=====end of transfer packet=====
    data_array_send[0].type = 'E'; 
    state = SendPack; 
    cnt1 = 0;
    GPIO_ResetBits(GPIOD,LED4_PIN);
}

//=====State3:Send back Packets=====
void State3_SendPack(void)
{
    int j;
    GPIO_SetBits(GPIOD,LED5_PIN);

    for (j = Npack - M; j > -M; j -= M)
    {
        while (data_array_send_length > ((j>0)?j:0))
        {
            if (pack_avail(&UART1_TXq) < 1)  
            { 
            pack_send((uint8_t*)&data_array_send[--data_array_send_length],sizeof(uart_data_t));
        //Enqueue(&UART1_TXq,(uint8_t*)&data_array_send[--data_array_send_length],sizeof(uart_data_t));
         /*
        if (cnt1 < 800) cnt1++;
        else 
        {
            Enqueue(&UART1_TXq,(uint8_t*)&data_array_send[--data_array_send_length],sizeof(uart_data_t));
            //Enqueue(&UART1_TXq,"12345678",8);
            cnt1 = 0;
        }*/  
            }
        }
        while (pack_avail(&UART1_RXq) < N) 
        {
             __asm__("nop");
        }
        Dequeue(&UART1_RXq,buf,N);
        if (buf[0] == 'S' && buf[9] == 'E' && buf[1] == 'G'){}
            else 
            {
                j += M;
                data_array_send_length = j;
            }
    }
}

//=====initialize pins=====

static void pins_setup(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ALL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ALL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = ALL_PINS;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}



#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    // Infinite loop
    // Use GDB to find out why we're here 
    while (1);
}
#endif