/*
 * TEST BÁSICO - VERIFICACIÓN DE HARDWARE
 * 
 * Este código súper simple te ayuda a verificar que el hardware funciona.
 * NO hace conteo ni lógica compleja, solo muestra el estado del sensor.
 * 
 * OBSERVAR:
 * - LED pin 13: parpadea si el sensor cambia de estado
 * - LED pin 12: refleja el estado actual del sensor
 * - Motor pin 3: se activa 1 segundo cada 5 segundos (para verificar conexión)
 */

const int pinSensor = 2;
const int pinMotor = 3;
const int pinLED1 = 13; // LED principal
const int pinLED2 = 12; // LED sensor

int ultimoEstadoSensor = HIGH;
unsigned long ultimoTiempoMotor = 0;

void setup() {
  pinMode(pinSensor, INPUT_PULLUP);
  pinMode(pinMotor, OUTPUT);
  pinMode(pinLED1, OUTPUT);
  pinMode(pinLED2, OUTPUT);
  
  digitalWrite(pinMotor, LOW);
  digitalWrite(pinLED1, LOW);
  digitalWrite(pinLED2, LOW);
  
  // Señal de inicio: 3 parpadeos
  for(int i = 0; i < 3; i++) {
    digitalWrite(pinLED1, HIGH);
    delay(300);
    digitalWrite(pinLED1, LOW);
    delay(300);
  }
  
  ultimoEstadoSensor = digitalRead(pinSensor);
}

void loop() {
  // 1. Leer sensor
  int estadoActual = digitalRead(pinSensor);
  
  // 2. Mostrar estado del sensor en LED2
  digitalWrite(pinLED2, estadoActual == LOW ? HIGH : LOW);
  
  // 3. Si el sensor cambia, parpadear LED1
  if (estadoActual != ultimoEstadoSensor) {
    ultimoEstadoSensor = estadoActual;
    
    // Parpadeo para indicar cambio
    digitalWrite(pinLED1, HIGH);
    delay(100);
    digitalWrite(pinLED1, LOW);
    delay(100);
    digitalWrite(pinLED1, HIGH);
    delay(100);
    digitalWrite(pinLED1, LOW);
  }
  
  // 4. Test de motor cada 5 segundos
  if (millis() - ultimoTiempoMotor >= 5000) {
    digitalWrite(pinMotor, HIGH);
    digitalWrite(pinLED1, HIGH);
    delay(1000);
    digitalWrite(pinMotor, LOW);
    digitalWrite(pinLED1, LOW);
    ultimoTiempoMotor = millis();
  }
  
  delay(50); // Pequeña pausa para estabilidad
}

/*
 * INTERPRETACIÓN DE RESULTADOS:
 * 
 * ✅ SENSOR FUNCIONA SI:
 * - LED pin 12 cambia cuando pasas objetos
 * - LED pin 13 parpadea cuando detecta cambios
 * 
 * ✅ MOTOR FUNCIONA SI:
 * - Cada 5 segundos se activa 1 segundo
 * - LED pin 13 se queda encendido durante activación
 * 
 * ❌ PROBLEMAS:
 * - LED pin 12 nunca cambia = sensor no conectado/roto
 * - LED pin 13 no parpadea con objetos = problema de sensor
 * - Motor no se activa = problema de conexión motor
 */