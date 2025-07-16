# Análisis de Problemas en tolva.ino

## Posibles Causas de Fallo

### 1. **Problema de Overflow de millis() (Más Crítico)**
```cpp
// Problema en estas líneas:
if (millis() >= RETARDO_INICIAL_MS)
if ((millis() - tiempoUltimoCambio) > TIEMPO_DEBOUNCE_MS)
if (millis() - tiempoAnteriorMotor >= TIEMPO_ENCENDIDO_MS)
```

**Problema**: La función `millis()` hace overflow cada ~49 días, causando que estas comparaciones fallen.

**Solución**:
```cpp
// En lugar de:
if (millis() - tiempoAnteriorMotor >= TIEMPO_ENCENDIDO_MS)

// Usar:
if ((unsigned long)(millis() - tiempoAnteriorMotor) >= TIEMPO_ENCENDIDO_MS)
```

### 2. **Configuración del Sensor**
```cpp
pinMode(pinSensor, INPUT_PULLUP);
```

**Posible Problema**: Si tu sensor necesita pull-down en lugar de pull-up, la lógica se invertiría.

**Verificar**: 
- ¿Tu sensor da HIGH cuando detecta objeto o LOW?
- Con INPUT_PULLUP: sin objeto = HIGH, con objeto = LOW
- Si es al revés, cambiar a `pinMode(pinSensor, INPUT)` y usar resistencia externa

### 3. **Timing de Debounce Muy Corto**
```cpp
const unsigned long TIEMPO_DEBOUNCE_MS = 20;
```

**Problema**: 20ms puede ser insuficiente para sensores mecánicos o con ruido.

**Solución**: Incrementar a 50-100ms:
```cpp
const unsigned long TIEMPO_DEBOUNCE_MS = 50;
```

### 4. **Inicialización del Retardo**
```cpp
if (millis() >= RETARDO_INICIAL_MS)
```

**Problema**: Si el sistema se enciende y `millis()` ya es mayor que 10000, podría comportarse inesperadamente.

**Solución**: Usar una variable de tiempo de inicio:
```cpp
unsigned long tiempoInicio = 0;
bool tiempoInicioRegistrado = false;

// En setup():
tiempoInicio = millis();
tiempoInicioRegistrado = true;

// En loop():
if (!sistemaActivo && tiempoInicioRegistrado) {
    if ((unsigned long)(millis() - tiempoInicio) >= RETARDO_INICIAL_MS) {
        sistemaActivo = true;
        // ...
    }
}
```

## Diagnóstico Recomendado

### 1. **Habilitar Monitor Serial para Debug**
Agregar al código para diagnosticar:

```cpp
void setup() {
    Serial.begin(9600);
    Serial.println("Sistema iniciando...");
    // resto del setup...
}

// En gestionarSensor(), agregar:
if (estadoSensorEstable == HIGH) {
    pasosDetectados++;
    Serial.print("Pasos: ");
    Serial.print(pasosDetectados);
    Serial.print(" - Conteo: ");
    Serial.println(conteoPrincipal);
}
```

### 2. **Verificar Conexiones de Hardware**
- Pin 2 (sensor): ¿Está correctamente conectado?
- Pin 3 (motor): ¿Necesita un driver/relé?
- Alimentación: ¿Es suficiente para el motor?

### 3. **Probar Valores de Configuración**
```cpp
// Para pruebas, reducir valores:
const int PULSOS_OBJETIVO = 2;        // En lugar de 30
const long RETARDO_INICIAL_MS = 2000; // En lugar de 10000
```

## Código Corregido (Parte crítica)

```cpp
// Variables globales adicionales
unsigned long tiempoInicioSistema = 0;
bool inicioRegistrado = false;

void setup() {
    Serial.begin(9600); // Para debug
    pinMode(pinSensor, INPUT_PULLUP);
    pinMode(pinControlMotor, OUTPUT);
    digitalWrite(pinControlMotor, LOW);
    
    ultimoEstadoLeido = digitalRead(pinSensor);
    estadoSensorEstable = ultimoEstadoLeido;
    
    tiempoInicioSistema = millis();
    inicioRegistrado = true;
    
    Serial.println("Sistema iniciado");
}

void loop() {
    if (!sistemaActivo && inicioRegistrado) {
        if ((unsigned long)(millis() - tiempoInicioSistema) >= RETARDO_INICIAL_MS) {
            sistemaActivo = true;
            secuenciaEnMarcha = true;
            iniciarSecuenciaMotor();
            Serial.println("Sistema activado");
        }
        return;
    }
    
    gestionarSensor();
    gestionarSecuenciaMotor();
}

void gestionarSecuenciaMotor() {
    if (!secuenciaEnMarcha) return;

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
                Serial.println("Secuencia motor completada");
            }
        }
    }
}
```

## Síntomas Comunes y Causas

| Síntoma | Causa Probable |
|---------|----------------|
| No detecta objetos | Cableado sensor, configuración pull-up/down |
| Motor no funciona | Pin 3 necesita driver, problema alimentación |
| Conteo errático | Tiempo debounce insuficiente, ruido eléctrico |
| Se cuelga después de tiempo | Overflow de millis() |
| No inicia secuencia | Lógica de conteo o umbral PULSOS_OBJETIVO |

**Recomendación**: Empezar habilitando el monitor serial para ver exactamente dónde falla el sistema.