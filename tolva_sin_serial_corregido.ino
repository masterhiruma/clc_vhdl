/*
 *  DISPENSADOR AUTOMÁTICO - VERSIÓN SIN SERIAL.PRINT()
 *  PROBLEMA SOLUCIONADO: Timing corregido tras quitar Serial.print()
 *
 *  EXPLICACIÓN DEL PROBLEMA:
 *  Los Serial.print() introducían micro-delays que el sensor necesitaba.
 *  Al quitarlos, el código corre demasiado rápido y el timing falla.
 *
 *  SOLUCIÓN APLICADA:
 *  - Debounce aumentado significativamente
 *  - Delay no bloqueante después de detección
 *  - Timing ajustado para compensar velocidad extra
 */

// --- Pines ---
const int pinSensor = 2;
const int pinControlMotor = 3;

// --- Parámetros de Configuración AJUSTADOS ---
const int PULSOS_OBJETIVO = 30;
const int NUMERO_DE_PULSOS_MOTOR = 6;
const int TIEMPO_ENCENDIDO_MS = 120;
const int TIEMPO_APAGADO_MS = 900; 
const long RETARDO_INICIAL_MS = 10000;
const unsigned long TIEMPO_DEBOUNCE_MS = 150; // AUMENTADO: antes era 20ms
const unsigned long DELAY_POST_DETECCION_MS = 50; // NUEVO: delay tras detectar objeto

// --- Variables de Estado y Banderas de Control ---
bool sistemaActivo = false;
bool secuenciaEnMarcha = false;

// --- Variables para el Sensor ---
int pasosDetectados = 0;
int conteoPrincipal = 0;
// Variables para el Debounce del sensor
int estadoSensorEstable = HIGH;
int ultimoEstadoLeido = HIGH;
unsigned long tiempoUltimoCambio = 0;
unsigned long tiempoUltimaDeteccion = 0; // NUEVO: para delay post-detección

// --- Variables para el Motor ---
unsigned long tiempoAnteriorMotor = 0;
int pulsoMotorActual = 0;

// --- FUNCIÓN DE CONFIGURACIÓN ---
void setup() {
  pinMode(pinSensor, INPUT_PULLUP);
  pinMode(pinControlMotor, OUTPUT);
  digitalWrite(pinControlMotor, LOW);
  
  // Leemos el estado inicial para evitar un falso trigger al arrancar
  ultimoEstadoLeido = digitalRead(pinSensor);
  estadoSensorEstable = ultimoEstadoLeido;
}

// --- BUCLE PRINCIPAL ---
void loop() {
  // --- TAREA 0: GESTIÓN DEL ARRANQUE INICIAL ---
  if (!sistemaActivo) {
    if (millis() >= RETARDO_INICIAL_MS) {
      sistemaActivo = true;
      secuenciaEnMarcha = true;
      iniciarSecuenciaMotor();
    }
    return;
  }

  // --- TAREAS CONCURRENTES ---
  gestionarSensor();
  gestionarSecuenciaMotor();
}

// --- FUNCIONES AUXILIARES ---

void gestionarSensor() {
  // 1. Verificar si aún estamos en período de "silencio" post-detección
  if (millis() - tiempoUltimaDeteccion < DELAY_POST_DETECCION_MS) {
    return; // No procesar sensor durante este tiempo
  }

  // 2. Leer el estado actual del pin del sensor
  int lecturaActual = digitalRead(pinSensor);

  // 3. Comprobar si la lectura ha cambiado respecto a la última vez
  if (lecturaActual != ultimoEstadoLeido) {
    tiempoUltimoCambio = millis();
  }
  
  ultimoEstadoLeido = lecturaActual;

  // 4. Comprobar si ha pasado suficiente tiempo desde el último cambio
  if ((millis() - tiempoUltimoCambio) > TIEMPO_DEBOUNCE_MS) {
    if (lecturaActual != estadoSensorEstable) {
      estadoSensorEstable = lecturaActual;

      // 5. Actuar en el FLANCO DE SUBIDA (cuando el objeto se va)
      if (estadoSensorEstable == HIGH) {
        pasosDetectados++;
        tiempoUltimaDeteccion = millis(); // Iniciar período de silencio

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
      }
    }
  }
}