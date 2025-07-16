/*
 *  DISPENSADOR AUTOMÁTICO - VERSIÓN CORREGIDA
 *  VERSIÓN AUTÓNOMA (SIN MONITOR SERIAL) - CON DEBOUNCE MEJORADO
 *
 *  CORRECCIONES APLICADAS:
 *  - Lógica del sensor corregida para INPUT_PULLUP
 *  - Variables con tipos adecuados para evitar desbordamiento
 *  - LED indicador para diagnóstico
 *  - Debounce ajustable
 *  - Mejor manejo de estados iniciales
 *
 *  ADVERTENCIA: Este circuito maneja ALTO VOLTAJE. Proceda con MÁXIMA PRECAUCIÓN.
 */

// --- Pines ---
const int pinSensor = 2;
const int pinControlMotor = 3;
const int pinLEDIndicador = 13; // LED incorporado para diagnóstico

// --- Parámetros de Configuración ---
const int PULSOS_OBJETIVO = 30;
const int NUMERO_DE_PULSOS_MOTOR = 6;
const int TIEMPO_ENCENDIDO_MS = 120;
const int TIEMPO_APAGADO_MS = 900; 
const long RETARDO_INICIAL_MS = 10000;
const unsigned long TIEMPO_DEBOUNCE_MS = 50; // Aumentado para mayor estabilidad

// --- Variables de Estado y Banderas de Control ---
bool sistemaActivo = false;
bool secuenciaEnMarcha = false;

// --- Variables para el Sensor (tipos corregidos) ---
unsigned long pasosDetectados = 0;  // Cambio: unsigned long para evitar desbordamiento
int conteoPrincipal = 0;

// Variables para el Debounce del sensor
int estadoSensorEstable = HIGH;     // Estado estable y confirmado del sensor
int ultimoEstadoLeido = HIGH;       // Última lectura instantánea del sensor
unsigned long tiempoUltimoCambio = 0; // Momento en que se detectó un cambio por última vez

// --- Variables para el Motor ---
unsigned long tiempoAnteriorMotor = 0;
int pulsoMotorActual = 0;

// --- Variables para indicador LED ---
unsigned long tiempoAnteriorLED = 0;
bool estadoLED = false;

// --- FUNCIÓN DE CONFIGURACIÓN ---
void setup() {
  pinMode(pinSensor, INPUT_PULLUP);
  pinMode(pinControlMotor, OUTPUT);
  pinMode(pinLEDIndicador, OUTPUT);
  
  digitalWrite(pinControlMotor, LOW);
  digitalWrite(pinLEDIndicador, LOW);
  
  // Leemos el estado inicial para evitar un falso trigger al arrancar
  ultimoEstadoLeido = digitalRead(pinSensor);
  estadoSensorEstable = ultimoEstadoLeido;
  
  // Parpadeo inicial para indicar que el sistema está iniciando
  for(int i = 0; i < 3; i++) {
    digitalWrite(pinLEDIndicador, HIGH);
    delay(200);
    digitalWrite(pinLEDIndicador, LOW);
    delay(200);
  }
}

// --- BUCLE PRINCIPAL (ACTÚA COMO DESPACHADOR) ---
void loop() {
  // --- TAREA 0: GESTIÓN DEL ARRANQUE INICIAL ---
  if (!sistemaActivo) {
    // Parpadeo lento durante espera inicial
    if (millis() - tiempoAnteriorLED >= 500) {
      estadoLED = !estadoLED;
      digitalWrite(pinLEDIndicador, estadoLED);
      tiempoAnteriorLED = millis();
    }
    
    if (millis() >= RETARDO_INICIAL_MS) {
      sistemaActivo = true;
      secuenciaEnMarcha = true;
      digitalWrite(pinLEDIndicador, HIGH); // LED encendido = sistema activo
      iniciarSecuenciaMotor();
    }
    return; // No hacer nada más hasta que termine el retardo
  }

  // --- TAREAS CONCURRENTES ---
  gestionarSensor();
  gestionarSecuenciaMotor();
  gestionarIndicadorLED();
}

// --- FUNCIONES AUXILIARES ---

void gestionarSensor() {
  // 1. Leer el estado actual del pin del sensor
  int lecturaActual = digitalRead(pinSensor);

  // 2. Comprobar si la lectura ha cambiado respecto a la última vez
  if (lecturaActual != ultimoEstadoLeido) {
    // Si cambió, reiniciar el contador de tiempo para el debounce
    tiempoUltimoCambio = millis();
  }
  
  // Guardamos la lectura actual para la próxima iteración del loop
  ultimoEstadoLeido = lecturaActual;

  // 3. Comprobar si ha pasado suficiente tiempo desde el último cambio
  if ((millis() - tiempoUltimoCambio) > TIEMPO_DEBOUNCE_MS) {
    // Si el tiempo ha pasado, la señal es estable.
    // Comprobamos si el estado estable es diferente del que teníamos guardado.
    if (lecturaActual != estadoSensorEstable) {
      estadoSensorEstable = lecturaActual; // Actualizamos el estado estable

      // 4. CORRECCIÓN: Actuar en el FLANCO DE BAJADA (cuando el objeto llega)
      // Con INPUT_PULLUP: LOW = objeto presente, HIGH = objeto ausente
      if (estadoSensorEstable == LOW) {  // CAMBIO PRINCIPAL: LOW en lugar de HIGH
        pasosDetectados++;
        
        // Parpadeo rápido del LED para indicar detección
        digitalWrite(pinLEDIndicador, LOW);
        delay(50);
        digitalWrite(pinLEDIndicador, HIGH);

        // La lógica de conteo principal se mantiene igual
        if (pasosDetectados > 0 && pasosDetectados % 2 == 0) {
          conteoPrincipal++;

          if (conteoPrincipal >= PULSOS_OBJETIVO) {
            conteoPrincipal = 0;
            if (!secuenciaEnMarcha) {
              secuenciaEnMarcha = true;
              iniciarSecuenciaMotor();
            }
          }
        }
      }
    }
  }
}

void gestionarIndicadorLED() {
  // LED parpadea rápido durante secuencia de motor
  if (secuenciaEnMarcha) {
    if (millis() - tiempoAnteriorLED >= 100) {
      estadoLED = !estadoLED;
      digitalWrite(pinLEDIndicador, estadoLED);
      tiempoAnteriorLED = millis();
    }
  } else if (sistemaActivo) {
    // LED encendido fijo cuando sistema activo pero sin secuencia
    digitalWrite(pinLEDIndicador, HIGH);
  }
}

void iniciarSecuenciaMotor() {
  pulsoMotorActual = 1;
  digitalWrite(pinControlMotor, HIGH);
  tiempoAnteriorMotor = millis();
}

void gestionarSecuenciaMotor() {
  if (!secuenciaEnMarcha) {
    return;
  }

  if (digitalRead(pinControlMotor) == HIGH) {
    if (millis() - tiempoAnteriorMotor >= TIEMPO_ENCENDIDO_MS) {
      digitalWrite(pinControlMotor, LOW);
      tiempoAnteriorMotor = millis();
    }
  } else {
    if (millis() - tiempoAnteriorMotor >= TIEMPO_APAGADO_MS) {
      if (pulsoMotorActual < NUMERO_DE_PULSOS_MOTOR) {
        pulsoMotorActual++;
        digitalWrite(pinControlMotor, HIGH);
        tiempoAnteriorMotor = millis();
      } else {
        secuenciaEnMarcha = false;
        digitalWrite(pinLEDIndicador, HIGH); // Volver a LED fijo
      }
    }
  }
}