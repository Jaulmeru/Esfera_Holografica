///////////////////////////////////////////////////////////////
//                                                           //
//  Proyecto para Universidad UTEG - SSP Sistemas embebidos  //
//              Codigo creado por Javier Mendoza             //
//              Utilizando VSCode con PlatformIO             // 
//                   Basado en NodeMCU ESP32                 //
//             LEDs WS2812B con lib FastLed 3.6.0            // 
//                                                           //
///////////////////////////////////////////////////////////////

#include <Arduino.h>
#include <FastLED.h>

//// Header con las imagenes a usar
#include <images.h>

//// Tamano de las imagenes a usar 
const int img_y_l = 20; //Numero de leds que se pondran en la esfera
const int img_x_l = 40; //Es un giro completo de la esfera, teoricamente 360/img_x_l=grados_p_pixel [360/40=9]

//// Solo se usara un pin para enviar datos
const int data_pin = 27;  //<-----------------Pin escogido arbitrariamente CAMBIAR-----------------------

//// Variables con las que rabajara el programa
int ms_p_pix = 50;              // 1/(rps de motor*img_x_l)  [1/(5*40) = 5ms] <---Este valor sera cambiado para sincronizar 
unsigned long ms_previos = 0;  // Milisegundos corrientes antes de cambiar los leds
CRGB leds_to_disp[img_y_l];    // Arreglo auxiliar para cargar una secuencia en la tira
int img_x = 0;                 // Contador para identificar que columna de la imagen se mostrara
int brillo =3;                  //divisor del brillo


int img_disp[img_y_l*img_x_l*3]; 
int *img_disp_pointer;
//int img_disp;



void setup() {
  Serial.begin(9600);  // Inicio de comunicacion serial para el BT
  FastLED.addLeds <WS2812B,data_pin,GRB> (leds_to_disp,img_y_l); //Llamada de creacion de los leds con atributos para nuestro chip
}

void loop() {
    

    //// Cambio de instruccion por Bluetooth

    //img_disp_pointer = test;


    //// Parpadeo de los LED cada cierto intervalo
    unsigned long ms_actuales = millis();                // Milisegundos desde que arranco el micro
    if(ms_actuales - ms_previos >= ms_p_pix){
        ms_previos = ms_actuales;                       //Guarda el momento donde se empiezan a cambiar los leds
        for (int img_y = 0; img_y < img_y_l; img_y++)   //Se cargaran al vector auxiliar leds[] los valores para el intervalo
        {
            int pixel = (img_x*3)+(img_y*img_x_l*3);
            leds_to_disp[img_y] = CRGB(
                                        round(tloz[pixel]/brillo),   //Rojo
                                        round(tloz[pixel+1]/brillo), //Verde
                                        round(tloz[pixel+2]/brillo)  //Azul
                                        );
        }
        FastLED.show();                                 //Se mandan a visualizar por el data_pin

        if(img_x<img_x_l-1) img_x++;                    //El contador queda confinado entre 0 y 39
        else img_x = 0;
        Serial.println(img_x);
    }
}


