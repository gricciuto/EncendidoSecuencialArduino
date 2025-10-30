//Constantes
const int T_pulso = 500;                      //Tiempo que va a estar encendida la resistencia
const int tiempo_retardo_inicial = 5000;      //Tiempo entre pulso cuando se enciende el horno
const long tiempo_esperado = 30000;           //Tiempo que va a transcurrir hasta que las 3 resistencias queden encendidas
long tiempo_inicio;                           //Tiempo en el que arranca el programa
long tiempo_ciclo;                            //Contador de tiempo del ciclo
long tiempo_inicio_ultimo_ciclo;              //Marca cuando inicio el ultimo ciclo
boolean encendido_constante;                  //Esto se usa para que una vez que se llego al tiempo deseado, se mantenga encendido fijo
//Pines
const int PIN_r1 = 6;
const int PIN_r2 = 5;
const int PIN_r3 = 4;
const int enable = 12;


// "Objeto" resistencia
struct Resistencia {
  int pin;                                  //Pin asignado a la resistencia
  unsigned long ultimoPulsado;              //Cuanto paso desde que encendio
  bool estado;                              //Si esta encendida o no
  bool encendio_ciclo;                      //Esto indica si en ese ciclo ya encendio la resistencia, para poder apagarla
};

//Configuracion del horno
Resistencia resistencias[3] = {             
  {PIN_r1, 0, false, false},
  {PIN_r2, 0, false, false},
  {PIN_r3, 0, false, false}
};

//Funcion lineal que determina tiempo de retardo entre encendidos
long funcion_tiempo(long tiempo_esperado) {
  long tiempo_programa = millis() - tiempo_inicio;
  long tiempo_retardo = tiempo_retardo_inicial - (tiempo_retardo_inicial * tiempo_programa) / tiempo_esperado;
  return tiempo_retardo;
}

void setup() {
  Serial.begin(9600);
  pinMode(enable, INPUT);
  pinMode(resistencias[0].pin, OUTPUT);
  pinMode(resistencias[1].pin, OUTPUT);
  pinMode(resistencias[2].pin, OUTPUT);
  digitalWrite(resistencias[0].pin, LOW);
  digitalWrite(resistencias[1].pin, LOW);
  digitalWrite(resistencias[2].pin, LOW);
  tiempo_ciclo = 0;
  tiempo_inicio = millis();
  encendido_constante = false;
}

void loop() {
  if (!encendido_constante && digitalRead(enable)){
    long retardo = funcion_tiempo(tiempo_esperado);
    tiempo_ciclo = millis() - tiempo_inicio_ultimo_ciclo;
    if (tiempo_ciclo > retardo){
      //Aca si se entra es porque se termino un ciclo y hay que reiniciar las variables
      tiempo_inicio_ultimo_ciclo = millis();
      resistencias[0].encendio_ciclo = false;
      resistencias[1].encendio_ciclo = false;
      resistencias[2].encendio_ciclo = false;
      tiempo_ciclo = 0; //Esto para evitar problemas de entrar en el if 
    }
      //Por cada resistencia
    for (int i = 0; i < 3; i++) {
      //Si la resistencia esta apagada y en lo que va del ciclo no encendio y ademas, el tiempo que va del ciclo es mayor al tercio correspondiente a la resistencia, enciende
      if (!resistencias[i].estado && !resistencias[i].encendio_ciclo && tiempo_ciclo > i*(retardo/3)) {
          digitalWrite(resistencias[i].pin, HIGH);
          resistencias[i].estado = true;
          resistencias[i].encendio_ciclo = true;
          resistencias[i].ultimoPulsado = millis();
      }
      //Apagar si ya paso su tiempo
      if (resistencias[i].estado && millis() - resistencias[i].ultimoPulsado > T_pulso) {
          digitalWrite(resistencias[i].pin, LOW);
          resistencias[i].estado = false;
      }
    }
    //En el caso de que ya el retardo se haya hecho 0 se quedan las 3 resistencias prendidas.
    if (retardo < 0){
      encendido_constante = true;
    }
    Serial.print("tiempo de ciclo: ");
    Serial.println(tiempo_ciclo);
  }
  else if(digitalRead(enable)){
    digitalWrite(resistencias[0].pin, HIGH);
    digitalWrite(resistencias[1].pin, HIGH);
    digitalWrite(resistencias[2].pin, HIGH);
   }
  else {
    digitalWrite(resistencias[0].pin, LOW);
    digitalWrite(resistencias[1].pin, LOW);
    digitalWrite(resistencias[2].pin, LOW);
  }
}
