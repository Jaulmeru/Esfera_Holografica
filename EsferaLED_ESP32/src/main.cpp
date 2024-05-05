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
#include <BluetoothSerial.h>

//// Header con las imagenes a usar
#include <images.h>

//// Tamano de las imagenes a usar 
const int img_y_l = 20; //Numero de leds que se pondran en la esfera
const int img_x_l = 40; //Es un giro completo de la esfera, teoricamente 360/img_x_l=grados_p_pixel [360/40=9]

//// Solo se usara un pin para enviar datos
const int data_pin = 27;  //Pin digital de salida de datos (Conectar resistencia de 220 o 330)

//// Variables con las que rabajara el programa
int ms_p_pix = 200;              // 1/(rps de motor*img_x_l)  [1/(5*40) = 5ms] <---Este valor sera cambiado para sincronizar 
unsigned long ms_previos = 0;  // Milisegundos corrientes antes de cambiar los leds
CRGB leds_to_disp[img_y_l];    // Arreglo auxiliar para cargar una secuencia en la tira
int img_x = 0;                 // Contador para identificar que columna de la imagen se mostrara
int brillo =30;                  //divisor del brillo 0-255/brillo
int *pointer = test;                   //Apunta a el vector que contiene la imagen a mostrar


int img_disp[img_y_l*img_x_l*3]; 
int *img_disp_pointer = img_disp; 


///////////////// Bluetooth
BluetoothSerial BT; //Objeto bt


void callback_function(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_START_EVT) {
    Serial.println("Inicializado SPP");
  }
  else if (event == ESP_SPP_SRV_OPEN_EVT ) {
    Serial.println("Cliente conectado");
  }
  else if (event == ESP_SPP_CLOSE_EVT  ) {
    Serial.println("Cliente desconectado");
  }
  else if (event == ESP_SPP_DATA_IND_EVT ) {
    while (BT.available()) { // Mientras haya datos por recibir
        String dato_recibido = BT.readStringUntil('\n');    ///Se recibe la cadena hasta un salto de linea
        dato_recibido.trim();                               ///Se eliminan espacios al inicio y final

        int valor = 0;      //Valor que queremos cambiar en la determinada variable
        String comando = ""; //Variable a la que le queremos cambiar el valor
        int index_delimitador = dato_recibido.indexOf('.'); //Se busca la posicion del punto
        if(index_delimitador !=-1){  //si no se encuentra punto, arroja -1
            comando = dato_recibido.substring(0, index_delimitador); //Se guarda el comando hasta antes del punto
            valor = dato_recibido.substring(index_delimitador + 1).toInt();//Se guarda el numero desde despues del punto    
            Serial.print("Comando recibido: ");
            Serial.println(comando);
            Serial.print("Valor recibido: ");
            Serial.println(valor);

        }else{
            comando = dato_recibido;    
            Serial.print("Comando recibido: ");
        }

        if(comando == "help"){
            BT.println("Envia un comando con la estructura 'cmd.valor'");
            BT.println("Lista de comandos:");
            BT.println("imagen ->");
            BT.println("    0 = Test");
            BT.println("    1 = Zelda");
            BT.println("    2 = Mudkip");
            BT.println("    3 = Estrella");
            BT.println("    4 = Helado");
            BT.println("    5 = Corazon");
            BT.println("    6 = Flor");
            BT.println("velocidad -> En milisec, cuanto tiempo se mostrara cada pixel");
            BT.println("brillo -> Divisor de la intensidad 1=Maximo/1   10=Maximo/10");
        }else if(comando =="imagen"){
            switch (valor)
            {
            case 0:
                pointer = test;
                break;
            case 1:
                pointer = tloz;
                break;
            case 2:
                pointer = mudkip1;
                break;
            case 3:
                pointer = estrella;
                break;
            case 4:
                pointer = helado;
                break;
            case 5:
                pointer = corazon;
                break;
            case 6:
                pointer = flor;
                break;
            default:
            BT.println("Utiliza valores de 0-6");
                break;
            }
        }else if(comando =="velocidad"){
            ms_p_pix=valor;
            BT.println("Velocidad ajustada");
        }else if(comando =="brillo"){
            brillo=valor;
            BT.println("Brillo ajustado");
        }else BT.println("Comando no reconocido. Utiliza help para ver los permitidos");

      }
    }
  }


void setup() {
  Serial.begin(9600);  // Inicio de comunicacion serial para el BT
  FastLED.addLeds <WS2812B,data_pin,GRB> (leds_to_disp,img_y_l); //Llamada de creacion de los leds con atributos para nuestro chip
  BT.begin("Esfera_Uli");
  Serial.println("Bluetooth inicializado");
  BT.register_callback(callback_function);//// Cambio de instruccion por Bluetooth
}

void loop() {
    img_disp_pointer = pointer;
    //// Parpadeo de los LED cada cierto intervalo
    unsigned long ms_actuales = millis();                // Milisegundos desde que arranco el micro
    if(ms_actuales - ms_previos >= ms_p_pix){
        ms_previos = ms_actuales;                       //Guarda el momento donde se empiezan a cambiar los leds
        for (int img_y = 0; img_y < img_y_l; img_y++)   //Se cargaran al vector auxiliar leds[] los valores para el intervalo
        {
            int pixel = (img_x*3)+(img_y*img_x_l*3);
            leds_to_disp[img_y] = CRGB(
                                        round(img_disp_pointer[pixel]/brillo),   //Rojo
                                        round(img_disp_pointer[pixel+1]/brillo), //Verde
                                        round(img_disp_pointer[pixel+2]/brillo)  //Azul
                                        );
        }
        FastLED.show();                                 //Se mandan a visualizar por el data_pin

        if(img_x<img_x_l-1) img_x++;                    //El contador queda confinado entre 0 y 39
        else img_x = 0;
        Serial.println(img_x);
    }
}


