/*
 * TOLVA - VERSIÓN DIAGNÓSTICO COMPLETA
 * 
 * Esta versión te ayudará a identificar EXACTAMENTE qué está fallando:
 * - LED indicadores para cada estado
 * - Múltiples niveles de debug visual
 * - Tests independientes de sensor y motor
 * 
 * CÓDIGOS DE LED (pin 13):
 * - 1 parpadeo largo al inicio = Sistema iniciado
 * - Parpadeo rápido continuo = Sensor funcionando
 * - 3 parpadeos + pausa = Objeto detectado
 * - LED fijo 5 segundos = Motor activado
 * 
 * PINES:
 * - Pin 2: Sensor
 * - Pin 3: Motor
 * - Pin 13: LED diagnóstico
 * - Pin 12: LED contador (opcional)
 */

// --- Pines ---
const int pinSensor = 2;
const int pinControlMotor = 3;
const int pinLEDDiagnostico = 13;
const int pinLEDContador = 12; // LED adicional para mostrar conteo

// --- Configuración para DIAGNÓSTICO ---
const int PULSOS_OBJETIVO = 5; // REDUCIDO para pruebas rápidas
const int NUMERO_DE_PULSOS_MOTOR = 3; // REDUCIDO para pruebas
const int TIEMPO_ENCENDIDO_MS = 500; // AUMENTADO para ver mejor
const int TIEMPO_APAGADO_MS = 500; // REDUCIDO para pruebas más rápidas
const long RETARDO_INICIAL_MS = 3000; // REDUCIDO para pruebas rápidas
const unsigned long TIEMPO_DEBOUNCE_MS = 100;

// --- Variables de diagnóstico ---
bool modoTest = true; // Cambia a false para modo normal
unsigned long contadorTotalDetecciones = 0;
unsigned long ultimaDeteccionTiempo = 0;

// --- Variables del sistema original ---
bool sistemaActivo = false;
bool secuenciaEnMarcha = false;
int pasosDetectados = 0;
int conteoPrincipal = 0;

// Variables del sensor
int estadoSensorEstable = HIGH;
int ultimoEstadoLeido = HIGH;
unsigned long tiempoUltimoCambio = 0;

// Variables del motor
unsigned long tiempoAnteriorMotor = 0;
int pulsoMotorActual = 0;

void setup() {
  // Configurar pines
  pinMode(pinSensor, INPUT_PULLUP);
  pinMode(pinControlMotor, OUTPUT);
  pinMode(pinLEDDiagnostico, OUTPUT);
  pinMode(pinLEDContador, OUTPUT);
  
  // Estado inicial
  digitalWrite(pinControlMotor, LOW);
  digitalWrite(pinLEDDiagnostico, LOW);
  digitalWrite(pinLEDContador, LOW);
  
  // Leer estado inicial del sensor
  ultimoEstadoLeido = digitalRead(pinSensor);
  estadoSensorEstable = ultimoEstadoLeido;
  
  // Señal de inicio: 1 parpadeo largo
  digitalWrite(pinLEDDiagnostico, HIGH);
  delay(1000);
  digitalWrite(pinLEDDiagnostico, LOW);
  delay(500);
  
  // Test inicial del motor (si está en modo test)
  if (modoTest) {
    testMotor();
  }
}

void loop() {
  // Diagnóstico: parpadeo rápido continuo = loop funcionando
  static unsigned long ultimoParpadeoLoop = 0;
  if (millis() - ultimoParpadeoLoop >= 100) {
    static bool estadoParpadeo = false;
    digitalWrite(pinLEDDiagnostico, estadoParpadeo);
    estadoParpadeo = !estadoParpadeo;
    ultimoParpadeoLoop = millis();
  }
  
  // Gestión del arranque inicial
  if (!sistemaActivo) {
    if (millis() >= RETARDO_INICIAL_MS) {
      sistemaActivo = true;
      secuenciaEnMarcha = true;
      iniciarSecuenciaMotor();
      
      // Señal: sistema activo
      for(int i = 0; i < 5; i++) {
        digitalWrite(pinLEDDiagnostico, HIGH);
        delay(100);
        digitalWrite(pinLEDDiagnostico, LOW);
        delay(100);
      }
    }
    return;
  }

  // Tareas principales
  gestionarSensorConDiagnostico();
  gestionarSecuenciaMotor();
  mostrarContadorEnLED();
}

void gestionarSensorConDiagnostico() {
  int lecturaActual = digitalRead(pinSensor);
  
  // Diagnóstico: mostrar estado actual del sensor en pin 12
  digitalWrite(pinLEDContador, lecturaActual == LOW ? HIGH : LOW);

  if (lecturaActual != ultimoEstadoLeido) {
    tiempoUltimoCambio = millis();
  }
  
  ultimoEstadoLeido = lecturaActual;

  if ((millis() - tiempoUltimoCambio) > TIEMPO_DEBOUNCE_MS) {
    if (lecturaActual != estadoSensorEstable) {
      estadoSensorEstable = lecturaActual;

      // PRUEBA AMBAS LÓGICAS: Primero HIGH (objeto se va)
      if (estadoSensorEstable == HIGH) {
        contadorTotalDetecciones++;
        ultimaDeteccionTiempo = millis();
        
        // Señal visual: 3 parpadeos rápidos = detección HIGH
        signalDeteccion(3, 100);
        
        pasosDetectados++;
        procesarConteo();
      }
      
      // También probar LOW (objeto llega) - descomentar para probar
      /*
      if (estadoSensorEstable == LOW) {
        contadorTotalDetecciones++;
        ultimaDeteccionTiempo = millis();
        
        // Señal visual: 5 parpadeos rápidos = detección LOW
        signalDeteccion(5, 100);
        
        pasosDetectados++;
        procesarConteo();
      }
      */
    }
  }
}

void procesarConteo() {
  if (pasosDetectados > 0 && pasosDetectados % 2 == 0) {
    conteoPrincipal++;
    
    // Señal: avance en conteo principal
    signalDeteccion(conteoPrincipal, 200); // Parpadeos = contador actual

    if (conteoPrincipal >= PULSOS_OBJETIVO) {
      conteoPrincipal = 0;
      if (!secuenciaEnMarcha) {
        secuenciaEnMarcha = true;
        iniciarSecuenciaMotor();
        
        // Señal: motor activado - LED fijo 2 segundos
        digitalWrite(pinLEDDiagnostico, HIGH);
        delay(2000);
        digitalWrite(pinLEDDiagnostico, LOW);
      }
    }
  }
}

void signalDeteccion(int numParpadeos, int duracion) {
  for(int i = 0; i < numParpadeos; i++) {
    digitalWrite(pinLEDDiagnostico, HIGH);
    delay(duracion);
    digitalWrite(pinLEDDiagnostico, LOW);
    delay(duracion);
  }
}

void mostrarContadorEnLED() {
  // Cada 3 segundos, mostrar el contador principal en parpadeos
  static unsigned long ultimoMostrarContador = 0;
  if (millis() - ultimoMostrarContador >= 3000) {
    if (conteoPrincipal > 0) {
      delay(500); // Pausa antes de mostrar
      signalDeteccion(conteoPrincipal, 300);
      delay(1000); // Pausa después de mostrar
    }
    ultimoMostrarContador = millis();
  }
}

void testMotor() {
  // Test independiente del motor
  for(int i = 0; i < 3; i++) {
    digitalWrite(pinControlMotor, HIGH);
    digitalWrite(pinLEDDiagnostico, HIGH);
    delay(200);
    digitalWrite(pinControlMotor, LOW);
    digitalWrite(pinLEDDiagnostico, LOW);
    delay(200);
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
        
        // Señal: secuencia terminada
        for(int i = 0; i < 10; i++) {
          digitalWrite(pinLEDDiagnostico, HIGH);
          delay(50);
          digitalWrite(pinLEDDiagnostico, LOW);
          delay(50);
        }
      }
    }
  }
}