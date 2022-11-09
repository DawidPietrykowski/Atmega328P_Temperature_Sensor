#include <Arduino.h>

// 2-7 and 11-16
#define G1 13
#define G2 14
#define G3 15
#define G4 2
#define PIN_A 12 
#define PIN_B 16
#define PIN_C 4
#define PIN_D 6
#define PIN_E 5
#define PIN_F 11
#define PIN_G 3
#define PIN_DP 7

#include <OneWire.h> 
#include <DallasTemperature.h>
#define ONE_WIRE_BUS 8
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DeviceAddress tempDeviceAddress;

int step = 0;
float temp = 0.0;

bool digit_array[8] = {0,0,0,0,0,0,0,0};

bool truth_table[77] = {
    1,1,1,1,1,1,0,
    0,1,1,0,0,0,0,
    1,1,0,1,1,0,1,
    1,1,1,1,0,0,1,
    0,1,1,0,0,1,1,
    1,0,1,1,0,1,1,
    1,0,1,1,1,1,1,
    1,1,1,0,0,0,0,
    1,1,1,1,1,1,1,
    1,1,1,1,0,1,1,
    0,0,0,0,0,0,0};
    
// bool truth_table_bin[77] = {
//     1,1,1,1,1,1,0,
//     0,1,1,0,0,0,0,
//     1,1,0,1,1,0,1,
//     1,1,1,1,0,0,1,
//     0,1,1,0,0,1,1,
//     1,0,1,1,0,1,1,
//     1,0,1,1,1,1,1,
//     1,1,1,0,0,0,0,
//     1,1,1,1,1,1,1,
//     1,1,1,1,0,1,1,
//     0,0,0,0,0,0,0};

void set_digit(int disp, int digit, bool dot = false){
    // digitalWrite(G1, HIGH);
    // digitalWrite(G2, HIGH);
    // digitalWrite(G3, HIGH);
    // digitalWrite(G4, HIGH);
    PORTC = PORTC | B00000011;
    PORTD = PORTD | B00000100;
    PORTB = PORTB | B00100000;
    if(digit != -1)
        for(int i = 0; i < 7; i++)
            digit_array[i] = truth_table[i + digit*7];
    else{
        for(int i = 0; i < 7; i++)
            digit_array[i] = 0;
        
        digit_array[6] = 1;
    }
    digit_array[7] = dot;

    //digitalWrite(PIN_A, digit_array[0]);
    if(digit_array[0])
        PORTB |= (1 << 4);
    else
        PORTB &= ~(1 << 4);

    //digitalWrite(PIN_B, digit_array[1]);
    if(digit_array[1])
        PORTC |= (1 << 2);
    else
        PORTC &= ~(1 << 2);

    //digitalWrite(PIN_C, digit_array[2]);
    if(digit_array[2])
        PORTD |= (1 << PIN_C);
    else
        PORTD &= ~(1 << PIN_C);

    //digitalWrite(PIN_D, digit_array[3]);
    if(digit_array[3])
        PORTD |= (1 << PIN_D);
    else
        PORTD &= ~(1 << PIN_D);

    //digitalWrite(PIN_E, digit_array[4]);
    if(digit_array[4])
        PORTD |= (1 << PIN_E);
    else
        PORTD &= ~(1 << PIN_E);

    //digitalWrite(PIN_F, digit_array[5]);
    if(digit_array[5])
        PORTB |= (1 << 3);
    else
        PORTB &= ~(1 << 3);

    //digitalWrite(PIN_G, digit_array[6]);
    if(digit_array[6])
        PORTD |= (1 << PIN_G);
    else
        PORTD &= ~(1 << PIN_G);

    //digitalWrite(PIN_DP,digit_array[7]);
    if(digit_array[7])
        PORTD |= (1 << PIN_DP);
    else
        PORTD &= ~(1 << PIN_DP);
    

    switch(disp){
        case 0: 
            //digitalWrite(G1, LOW);
            PORTB &= ~(1 << 5);
            break;
        case 1: 
            //digitalWrite(G2, LOW);
            PORTC &= ~(1 << 0);
            break;
        case 2: 
            //digitalWrite(G3, LOW);
            PORTC &= ~(1 << 1);
            break;
        case 3: 
            //digitalWrite(G4, LOW);
            PORTD &= ~(1 << 2);
            break;
        default:
            break;
    }
}

// 1kHz timer for changing displayed digit
ISR(TIMER0_COMPA_vect){
    switch(step){
        case 0:
            if(temp<0){
                set_digit(0, -1);
            }
            else
                set_digit(0, 10);
            step++;
            step %= 4;
        break;
        case 1:
            step++;
            set_digit(1, (abs((int)temp) / 10) % 10);
            step %= 4;
        break;
        case 2:
            step++;
            set_digit(2, abs((int)temp) % 10, true);
            step %= 4;
        break;
        case 3:
            step++;
            set_digit(3, (int)(10 * (abs(temp)-abs((int)temp))));
            step %= 4;
        break;
        default:
            step %= 4;
        break;
    }
}

void setup() {
    cli();//stop interrupts

    //set timer1 interrupt at 1kHz
    TCCR0A = 0;// set entire TCCR0A register to 0
    TCCR0B = 0;// same for TCCR0B
    TCNT0  = 0;//initialize counter value to 0
    // set compare match register for 1hz increments
    OCR0A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
    // turn on CTC mode
    TCCR0B |= (1 << WGM01);
    // Set CS10 and CS11 bits for 64 prescaler
    TCCR0B |= (1 << CS11) | (1 << CS10);  
    // enable timer compare interrupt
    TIMSK0 |= (1 << OCIE0A);
    
    sei();//allow interrupts

    //pinMode(PIN_A, OUTPUT);
    //pinMode(PIN_B, OUTPUT);
    //pinMode(PIN_C, OUTPUT);
    //pinMode(PIN_D, OUTPUT);
    //pinMode(PIN_E, OUTPUT);
    //pinMode(PIN_F, OUTPUT);
    //pinMode(PIN_G, OUTPUT);
    //pinMode(PIN_DP, OUTPUT);
    //pinMode(G1, OUTPUT);
    //pinMode(G2, OUTPUT);
    //pinMode(G3, OUTPUT);
    //pinMode(G4, OUTPUT);
    DDRD = DDRD | B11111100;
    DDRB = DDRB | B00111000;
    DDRC = DDRC | B00000111;

    // digitalWrite(G1, HIGH);
    // digitalWrite(G2, HIGH);
    // digitalWrite(G3, HIGH);
    // digitalWrite(G4, HIGH);
    PORTC = PORTC | B00000011;
    PORTD = PORTD | B00000100;
    PORTB = PORTB | B00100000;

    sensors.begin(); 
    sensors.getAddress(tempDeviceAddress, 0);
    sensors.setResolution(tempDeviceAddress, 11);
    sensors.setWaitForConversion(false);
}

uint32_t last_millis = millis();

void loop() {
    if(millis() - last_millis > 200){
        temp = sensors.getTempCByIndex(0);
        sensors.requestTemperatures(); 
        last_millis = millis();
    }
}