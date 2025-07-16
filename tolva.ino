/*
 *  DISPENSADOR AUTOMÁTICO - VERSIÓN MULTITAREA CONCURRENTE
 *  VERSIÓN AUTÓNOMA (SIN MONITOR SERIAL) - CON DEBOUNCE
 *
 *  Lógica de funcionamiento:
 *  - El sensor y el motor funcionan en paralelo sin interrumpirse.
 *
 *  1. Al encender, espera 10 segundos.
 *  2. Al terminar la espera, se dispara el CICLO DE MOTOR INICIAL.
 *  3. SIMULTÁNEAMENTE, el sensor comienza a monitorear y contar.
 *  4. El conteo del sensor NUNCA se detiene.
 *  5. Cada 2 pasadas de objeto, el contador principal suma 1.
 *  6. Cuando el contador llega a 30, se dispara la secuencia del motor.
 *  7. El sistema es cíclico e ininterrumpido.
 *
 *  ADVERTENCIA: Este circuito maneja ALTO VOLTAJE. Proceda con MÁXIMA PRECAUCIÓN.
 */

// --- Pines ---
const int pinSensor = 2;
const int pinControlMotor = 3;

// --- Parámetros de Configuración ---
const int PULSOS_OBJETIVO = 30;
const int NUMERO_DE_PULSOS_MOTOR = 6;
const int TIEMPO_ENCENDIDO_MS = 120;
const int TIEMPO_APAGADO_MS = 900; 
const long RETARDO_INICIAL_MS = 10000;
const unsigned long TIEMPO_DEBOUNCE_MS = 50; // Incrementado de 20 a 50ms

// --- Variables de Estado y Banderas de Control ---
bool sistemaActivo = false;
bool secuenciaEnMarcha = false;

// --- Variables para el arranque ---
unsigned long tiempoInicioSistema = 0;
bool inicioRegistrado = false;

// --- Variables para el Sensor ---
int pasosDetectados = 0;
int conteoPrincipal = 0;
// Variables para el Debounce del sensor
int estadoSensorEstable = HIGH;     // Estado estable y confirmado del sensor
int ultimoEstadoLeido = HIGH;       // Última lectura instantánea del sensor
unsigned long tiempoUltimoCambio = 0; // Momento en que se detectó un cambio por última vez

// --- Variables para el Motor ---
unsigned long tiempoAnteriorMotor = 0;
int pulsoMotorActual = 0;

// --- FUNCIÓN DE CONFIGURACIÓN ---
void setup() {
  pinMode(pinSensor, INPUT_PULLUP); // Usar INPUT_PULLUP es más robusto si el sensor es de contacto seco
  pinMode(pinControlMotor, OUTPUT);
  digitalWrite(pinControlMotor, LOW);
  
  // Leemos el estado inicial para evitar un falso trigger al arrancar
  ultimoEstadoLeido = digitalRead(pinSensor);
  estadoSensorEstable = ultimoEstadoLeido;
  
  // Registrar tiempo de inicio
  tiempoInicioSistema = millis();
  inicioRegistrado = true;
}

// --- BUCLE PRINCIPAL (ACTÚA COMO DESPACHADOR) ---
void loop() {
  // --- TAREA 0: GESTIÓN DEL ARRANQUE INICIAL ---
  if (!sistemaActivo && inicioRegistrado) {
    if ((unsigned long)(millis() - tiempoInicioSistema) >= RETARDO_INICIAL_MS) {
      sistemaActivo = true;
      secuenciaEnMarcha = true;
      iniciarSecuenciaMotor();
    }
    return; // No hacer nada más hasta que termine el retardo
  }

  // --- TAREAS CONCURRENTES ---
  gestionarSensor();
  gestionarSecuenciaMotor();
  
  // CRÍTICO: Pequeño delay para estabilizar el timing del sensor
  // Esto reemplaza el delay que introducían los Serial.print()
  delay(1);
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
  if ((unsigned long)(millis() - tiempoUltimoCambio) > TIEMPO_DEBOUNCE_MS) {
    // Si el tiempo ha pasado, la señal es estable.
    // Comprobamos si el estado estable es diferente del que teníamos guardado.
    if (lecturaActual != estadoSensorEstable) {
      estadoSensorEstable = lecturaActual; // Actualizamos el estado estable

      // 4. Actuar solo en el FLANCO DE SUBIDA (cuando el objeto se va)
      // Es decir, cuando el estado estable cambia de LOW a HIGH
      if (estadoSensorEstable == HIGH) {
        pasosDetectados++;

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
    if ((unsigned long)(millis() - tiempoAnteriorMotor) >= TIEMPO_ENCENDIDO_MS) {
      digitalWrite(pinControlMotor, LOW);
      tiempoAnteriorMotor = millis();
    }
  } else {
    if ((unsigned long)(millis() - tiempoAnteriorMotor) >= TIEMPO_APAGADO_MS) {
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
