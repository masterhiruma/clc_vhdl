# ANÁLISIS DE FALLAS - CÓDIGO TOLVA.INO

## 🚨 **POSIBLES CAUSAS DE FALLO**

### 1. **PROBLEMA MÁS PROBABLE: Lógica del Sensor Invertida**

**Síntoma**: El sensor no cuenta objetos o cuenta incorrectamente.

**Causa**: En la línea 88-89 del código:
```cpp
if (estadoSensorEstable == HIGH) {
    pasosDetectados++;
```

**Problema**: Con `INPUT_PULLUP`:
- Sin objeto: pin = HIGH (3.3V/5V)
- Con objeto: pin = LOW (0V)

Tu código cuenta cuando el pin va a HIGH (cuando el objeto SE VA), pero probablemente quieres contar cuando el objeto LLEGA.

**Solución**: Cambiar a:
```cpp
if (estadoSensorEstable == LOW) {  // Cuenta cuando llega el objeto
    pasosDetectados++;
```

### 2. **DESBORDAMIENTO DE VARIABLES**

**Síntoma**: El contador se resetea inesperadamente o deja de funcionar.

**Causa**: 
```cpp
int pasosDetectados = 0;  // Máximo 32,767
```

Después de 32,767 detecciones, la variable se desborda y vuelve a valores negativos.

**Solución**: Usar `unsigned long` en lugar de `int`.

### 3. **TIEMPO DE DEBOUNCE INSUFICIENTE**

**Síntoma**: Múltiples conteos por un solo objeto.

**Causa**: `TIEMPO_DEBOUNCE_MS = 20` puede ser muy corto para algunos sensores.

**Solución**: Aumentar a 50-100ms dependiendo del sensor.

### 4. **PROBLEMA DE INICIALIZACIÓN**

**Síntoma**: El sistema no arranca correctamente.

**Causa**: El estado inicial del sensor puede causar un falso trigger.

### 5. **CONEXIONES HARDWARE**

**Síntomas comunes**:
- El sensor siempre lee el mismo valor
- El motor no se activa
- Comportamiento errático

**Verificar**:
- Pin 2 conectado correctamente al sensor
- Pin 3 conectado al relé/control del motor
- Alimentación adecuada
- Resistencias pull-up si es necesario

### 6. **LÓGICA DE MOTOR**

**Síntoma**: El motor no se activa cuando debería.

**Posible causa**: La secuencia de motor se inicia pero la condición `!secuenciaEnMarcha` en línea 101 podría estar interfiriendo.

## 🔧 **DIAGNÓSTICO RÁPIDO**

### Para identificar el problema específico:

1. **Agregar LED indicador** para ver si el sensor detecta:
```cpp
pinMode(13, OUTPUT);  // LED incorporado
// En gestionarSensor(), después de detectar:
digitalWrite(13, !digitalRead(13));  // Cambiar LED
```

2. **Verificar conteo** con LEDs:
```cpp
// Parpadear LED según conteoPrincipal
for(int i = 0; i < conteoPrincipal; i++) {
    digitalWrite(13, HIGH);
    delay(100);
    digitalWrite(13, LOW);
    delay(100);
}
```

3. **Test del motor** independiente:
```cpp
// En setup(), temporal para test:
secuenciaEnMarcha = true;
iniciarSecuenciaMotor();
```

## 🎯 **SOLUCIÓN MÁS PROBABLE**

El 80% de las fallas en este tipo de código son por **lógica invertida del sensor**. 

**Prueba esto primero**:
Cambiar línea 88 de:
```cpp
if (estadoSensorEstable == HIGH) {
```
a:
```cpp
if (estadoSensorEstable == LOW) {
```

## 📋 **CHECKLIST DE VERIFICACIÓN**

- [ ] ¿El sensor está conectado al pin 2?
- [ ] ¿El control del motor está en pin 3?
- [ ] ¿El sensor responde físicamente? (probar con multímetro)
- [ ] ¿La alimentación es adecuada?
- [ ] ¿Has esperado los 10 segundos iniciales?
- [ ] ¿El LED del Arduino parpadea durante la operación?