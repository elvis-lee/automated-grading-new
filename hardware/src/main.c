#include "main.h"
#include "uart.h"

#define myUSART 1
#define BufferRec 0
#define BufferSend 1
#define SetParam 0
#define RecPack 1
#define Gen_Samp 2
#define SendPack 3

//=====define number of packets to transfer each time before a check=====
#define M 100
//=====define packet byte size=====
#define N 9
//=====external variables=====
extern __IO uint32_t TimingDelay;
//=====buffer=====
uint8_t buf[QUEUE_SIZE];
uint8_t *tem_buf_ptr;
//=====data array=====
uart_data_t data_array_rec[6000];
uart_data_t data_array_send[6000];
//=====data array length=====
__IO uint16_t data_array_rec_length= 0 ; 
__IO uint16_t data_array_send_length= 0 ;
//=====counter slows down buf read=====
uint32_t cnt1 = 0;
//=====state variable=====
uint8_t state = SetParam;
//=====tansfer success flag=====
uint8_t rec_success = 1;
//=====Total number of packets to send=====
uint16_t Npack;
//=====Sample Duration=====
uint32_t Sample_Duration;
__IO uint8_t end_of_sample = 0;
//=====function declare=====
void pack_push(uint8_t *addr, uint8_t IOtype);
uart_data_t* pack_pop(uint8_t IOtype);
void pack_send(const uint8_t *data, uint16_t len);
void State0_SetParm(void);
void State1_RecPack(void);
void State2_Gen_Samp(void);
void State3_SendPack(void);


int main(int argc, char* argv[])
{  
    pins_setup();
	uart_open(myUSART,460800,0);
    SysTick_Config(SystemCoreClock/100000); 
    NVIC_SetPriority(SysTick_IRQn,0);
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);

	while (1)	
	{  
        switch (state)
        {
            case SetParam:
            State0_SetParm();
            break;

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
    uint8_t packet[9];
    uint8_t i;
    packet[0] = 'S';
    packet[8] = 'E';
    for (i = 1; i < 8; i++)
        packet[i] = data[i-1];
    Enqueue(&UART1_TXq, packet, len+2);
}


//=====packet push=====
void pack_push(uint8_t *addr,uint8_t IOtype)
{   
	if (IOtype == BufferRec)
	{
		data_array_rec[data_array_rec_length] = *(uart_data_t*)addr;
        data_array_rec_length++;
	}

	if (IOtype == BufferSend)
	{
		data_array_send[data_array_send_length] = *((uart_data_t*)addr);
        data_array_send_length++;
	}	
	
	
}

//=====packet pop=====
uart_data_t* pack_pop(uint8_t IOtype)
{
	if (IOtype == BufferRec)
	{   
        data_array_rec_length--;
    	return &(data_array_rec[data_array_rec_length]);
	}

	if (IOtype == BufferSend)
	{
        data_array_send_length--;
    	return &(data_array_send[data_array_send_length]);
	}	
}

//=====State0:Set Parameters=====
void State0_SetParm(void)
{ 
    uint32_t ri;
    uart_data_t temp;

    GPIO_SetBits(GPIOD,LED6_PIN);

    while (pack_avail(&UART1_RXq) < 2*N)
    {
        __asm__("nop");
    }
    
    ri = Dequeue(&UART1_RXq,buf,N);
    temp = *(uart_data_t*)(buf + 1);
    if (temp.type == 'S')
        Npack = temp.val;

    ri = Dequeue(&UART1_RXq,buf,N);
    temp = *(uart_data_t*)(buf + 1);
    if (temp.type == 'L')
        Sample_Duration = temp.time;

    state = RecPack;

    GPIO_ResetBits(GPIOD,LED6_PIN);
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
    {
        ri = pack_avail(&UART1_RXq);
        if (ri >= N)
        {
            ri = Dequeue(&UART1_RXq,buf,N);
            for (tem_buf_ptr = buf; tem_buf_ptr - buf < ri ;tem_buf_ptr = tem_buf_ptr + sizeof(uart_frame_t))
            {
                if (*tem_buf_ptr != 'S' || *(tem_buf_ptr+8) != 'E')
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
    SysTick->CTRL  =  SysTick->CTRL | 1UL;//enable systick
    while(!end_of_sample)
    {
        __asm__("nop");
    }
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);
    //=====end of transfer packet=====
    //data_array_send[0].type = 'E'; 
    state = SendPack; 
    GPIO_ResetBits(GPIOD,LED4_PIN);
}

//=====State3:Send back Packets=====
void State3_SendPack(void)
{
    int j;
    uint16_t total_packet_to_send = data_array_send_length;
    GPIO_SetBits(GPIOD,LED5_PIN);

    for (j = total_packet_to_send - M; j > -M; j -= M)
    {
        while (data_array_send_length > ((j>0)?j:0))
        {
            if (pack_avail(&UART1_TXq) < 200)  
            { 
            pack_send((uint8_t*)&data_array_send[--data_array_send_length],sizeof(uart_data_t));
            }
        }

        if (!data_array_send_length)
        {
          uart_data_t end_packet;
          end_packet.type = 'E';
          end_packet.time = 0;
          end_packet.val = 0;
          pack_send((uint8_t*)&end_packet,sizeof(uart_data_t));  
        }

        while (pack_avail(&UART1_RXq) < N) 
        {
             __asm__("nop");
        }
        
        Dequeue(&UART1_RXq,buf,N);
        if (buf[0] == 'S' && buf[8] == 'E' && buf[1] == 'G'){}
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