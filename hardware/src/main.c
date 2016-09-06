#include "main.h"
#include "uart.h"
#include "ioctrl.h"
#include <string.h>

#define myUSART 1
//=====define packet byte size=====
#define N 9
//=====external variables=====
extern __IO uint32_t TimingDelay;
extern __IO uint8_t analog_reading_enable;
//=====Total number of packets to receive=====
uint32_t Npack = 1; //set it to any number other than 0
//=====Sample Duration=====
uint32_t Sample_Duration;
__IO uint8_t end_of_sample = 0;
//=====variables used in for-loop counting=====
uint32_t i;
//=====function declare=====
void check_for_uart_packets();
void start_output_gen();
void end_output_gen();
void DUT_reset();

uint16_t adc_convert();
void adc_configure();
void DAC_SetValue(uint16_t value); 
void dac_configure();


int main(int argc, char* argv[])
{  
    pins_setup();
    pins_setup_DUT();
    adc_configure();
    dac_configure();
	uart_open(myUSART,460800,0);
    SysTick_Config(SystemCoreClock/5000); //Systick Frequency 
    NVIC_SetPriority(SysTick_IRQn,0);
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);

	while (1)	
	{
        // when there's time, check for complete UART packets
        check_for_uart_packets();
	}
}	

//=====check for complete uart packets=====
void check_for_uart_packets()
{   uint32_t total_size = checksize();
    uint32_t num_bytes = pack_avail(&UART1_RXq);

    if (total_size > 500 || total_size == Npack)
    {
        start_output_gen();
    }

    if( num_bytes >= N)
    {
        GPIO_ToggleBits(GPIOD,LED3_PIN);
        // temporary buffer for parsing packet
        uint8_t raw_bytes[N];
        Dequeue(&UART1_RXq,raw_bytes,N);
        uart_frame_t *frame = (uart_frame_t*)raw_bytes;
        if( frame->start != 'S' || frame->stop != 'E')
        {
            // handle bad packets
            return;
        }
        // if packet is good, create an output event and push to buffer
        switch( frame->data.type )
        {
            case PACKET_TYPE_DIGITAL:
                io_pushDigitalOut( frame->data.time, frame->data.val );
                break;
            case PACKET_TYPE_ANALOG:
                // channel (argument 2) is unused right now.
                io_pushAnalogOut( frame->data.time, 0, frame->data.val );
                break;
            case PACKET_TYPE_PARAM_TOTALPACKET:
                Npack = frame->data.time;
                break;
            case PACKET_TYPE_PARAM_SAMPLELEN:
                // define sample duration here 
                Sample_Duration = frame->data.time;
                break;
            case PACKET_TYPE_RESET_SYSTEM:
                // Reset System
                NVIC_SystemReset();
                break;
            case PACKET_TYPE_ANALOG_READING_ENABLE:
                analog_reading_enable = 1;
                break;
            case PACKET_TYPE_DUT_RESET:
                DUT_reset();
                break;
            default:
                // unsupported type
                break;
        }
    }
    

}

//=====DUT pins setup=====
void pins_setup_DUT()
{
    GPIO_SetBits(GPIOD,GPIO_Pin_14);
}

//=====DUT START=====
void DUT_reset()
{
    //set PD14 low for a while to reset DUT
    GPIO_ResetBits(GPIOD,GPIO_Pin_14);
    for (i = 0; i < 1000000; i++)
        __asm__("nop");
    GPIO_SetBits(GPIOD,GPIO_Pin_14);
    for (i = 0; i < 10000000; i++)
        __asm__("nop");
}

//=====send packet=====
void pack_send(const uint8_t *data)
{
    uint8_t packet[9];
    uart_frame_t *frame = (uart_frame_t*)packet;
    frame->start = 'S';
    frame->stop = 'E';
    memcpy(&(frame->data), data, sizeof(uart_data_t));
    Enqueue(&UART1_TXq, packet, sizeof(uart_frame_t));
}

void start_output_gen()
{
    SysTick->CTRL  =  SysTick->CTRL | 1UL;//enable systick
}

void end_output_gen()
{
    SysTick->CTRL  =  SysTick->CTRL & (~1UL);
    uart_data_t end_packet;
    end_packet.type = 'E';
    end_packet.time = 0;
    end_packet.val = 0;
    pack_send((uint8_t*)&end_packet);  
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

//===== ADC =====
uint16_t adc_convert()
{
    ADC_SoftwareStartConv(ADC1);//Start the conversion
    while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));//Processing the conversion
    return ADC_GetConversionValue(ADC1); //Return the converted data
}

void adc_configure()
{
    ADC_InitTypeDef ADC_init_structure; //Structure for adc confguration
    GPIO_InitTypeDef GPIO_initStructre; //Structure for analog input pin
    //Clock configuration
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//The ADC1 is connected the APB2 peripheral bus thus we will use its clock source
    RCC_AHB1PeriphClockCmd(RCC_AHB1ENR_GPIOAEN,ENABLE);//Clock for the ADC port!! Do not forget about this one ;)
    //Analog pin configuration
    GPIO_initStructre.GPIO_Pin = GPIO_Pin_0;//The channel 10 is connected to PC0
    GPIO_initStructre.GPIO_Mode = GPIO_Mode_AN; //The PC0 pin is configured in analog mode
    GPIO_initStructre.GPIO_PuPd = GPIO_PuPd_NOPULL; //We don't need any pull up or pull down
    GPIO_Init(GPIOA,&GPIO_initStructre);//Affecting the port with the initialization structure configuration
    //ADC structure configuration
    ADC_DeInit();
    ADC_init_structure.ADC_DataAlign = ADC_DataAlign_Right;//data converted will be shifted to right
    ADC_init_structure.ADC_Resolution = ADC_Resolution_12b;//Input voltage is converted into a 12bit number giving a maximum value of 4096
    ADC_init_structure.ADC_ContinuousConvMode = ENABLE; //the conversion is continuous, the input data is converted more than once
    ADC_init_structure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;// conversion is synchronous with TIM1 and CC1 (actually I'm not sure about this one :/)
    ADC_init_structure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;//no trigger for conversion
    ADC_init_structure.ADC_NbrOfConversion = 1;//I think this one is clear :p
    ADC_init_structure.ADC_ScanConvMode = DISABLE;//The scan is configured in one channel
    ADC_Init(ADC1,&ADC_init_structure);//Initialize ADC with the previous configuration
    //Enable ADC conversion
    ADC_Cmd(ADC1,ENABLE);
    //Select the channel to be read from
    ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_3Cycles);
}

//=====DAC=====
void DAC_SetValue(uint16_t value) 
{
    // Check value overflow 
    if (value > 4095) 
    {
        value = 4095;
    }
    
    DAC->DHR12R1 = value;
}

void dac_configure()
{
    DAC_InitTypeDef DAC_InitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;
    // GPIOA clock enable (to be used with DAC) 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    // DAC Periph clock enable 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    DAC_DeInit();
    DAC_InitStruct.DAC_Trigger = DAC_Trigger_None;
    DAC_InitStruct.DAC_WaveGeneration = DAC_WaveGeneration_None;
    DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
    DAC_Init(DAC_Channel_1, &DAC_InitStruct);

    DAC_Cmd(DAC_Channel_1, ENABLE);
}



#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{
    // Infinite loop
    // Use GDB to find out why we're here 
    while (1);
}
#endif