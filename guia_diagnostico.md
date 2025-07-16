# 🔍 GUÍA DE DIAGNÓSTICO - TOLVA

## 🚨 **PROBLEMA: "Sigue fallando"**

Vamos a identificar **EXACTAMENTE** qué está pasando con tu sistema.

## 📋 **PASO 1: CARGAR VERSIÓN DIAGNÓSTICO**

1. **Carga** `tolva_diagnostico.ino` en tu Arduino
2. **Configuración reducida** para pruebas rápidas:
   - Solo necesitas **5 detecciones** para activar motor (en lugar de 60)
   - Espera inicial de solo **3 segundos**
   - Motor hace solo **3 pulsos** (fácil de ver)

## 🔌 **PASO 2: VERIFICAR CONEXIONES**

### **Conexiones mínimas:**
- **Pin 2** → Sensor
- **Pin 3** → Control motor/relé
- **Pin 13** → LED incorporado (automático)
- **Pin 12** → LED externo (opcional, para mostrar estado sensor)

### **Alimentación:**
- ¿Arduino recibe 5V?
- ¿El motor/relé tiene su propia alimentación?

## 📡 **PASO 3: INTERPRETAR CÓDIGOS LED**

Al cargar el código, observa el **LED del pin 13**:

| Comportamiento LED | Significado | ¿Qué hacer? |
|-------------------|-------------|-------------|
| **1 parpadeo largo (1s)** | Sistema iniciado ✅ | Continuar |
| **3 parpadeos de motor** | Test de motor ✅ | ¿Se activa el motor? |
| **Parpadeo rápido continuo** | Loop funcionando ✅ | Continuar |
| **5 parpadeos** | Sistema activo ✅ | Probar sensor |
| **LED apagado/fijo** | ❌ Sistema colgado | **PROBLEMA GRAVE** |

## 🔍 **PASO 4: DIAGNOSTICAR SENSOR**

### **Test del sensor:**
1. **Observa pin 12** (si tienes LED conectado):
   - **Encendido** = sensor detecta objeto
   - **Apagado** = sensor no detecta objeto

2. **Pasa objetos** por el sensor:
   - **¿Cambia el LED del pin 12?**
   - **¿Parpadea 3 veces el LED del pin 13?** = detección confirmada

### **Posibles resultados:**

#### ✅ **SENSOR FUNCIONA:**
- Pin 12 cambia cuando pasas objetos
- Pin 13 parpadea 3 veces por cada detección
- **→ Problema está en la lógica de conteo**

#### ❌ **SENSOR NO FUNCIONA:**
- Pin 12 nunca cambia
- Pin 13 nunca parpadea 3 veces
- **→ Problema de hardware o conexión**

#### ⚠️ **SENSOR PARCIAL:**
- Pin 12 cambia pero pin 13 no parpadea
- **→ Problema de timing/debounce**

## 🔧 **PASO 5: PROBAR LÓGICA ALTERNATIVA**

Si el sensor no detecta con la lógica actual:

1. **Edita** línea 113 en el código:
```cpp
// COMENTAR esta línea:
if (estadoSensorEstable == HIGH) {

// DESCOMENTAR estas líneas (línea 122-132):
/*
if (estadoSensorEstable == LOW) {
  // ... código ...
}
*/
```

2. **Recargar** y probar de nuevo
3. **Observar** si ahora detecta con 5 parpadeos en lugar de 3

## ⚙️ **PASO 6: DIAGNOSTICAR MOTOR**

### **Al activarse el motor deberías ver:**
1. **LED fijo 2 segundos** = motor iniciado
2. **Motor pulsando** 3 veces (500ms ON, 500ms OFF)
3. **10 parpadeos rápidos** = secuencia terminada

### **Si el motor no funciona:**
- ¿El LED se queda fijo 2 segundos? → Lógica funciona, problema de hardware
- ¿No hay LED fijo? → El conteo no llega a 5 detecciones

## 📊 **PASO 7: INTERPRETACIÓN DE RESULTADOS**

### **ESCENARIO A: Sensor nunca detecta**
```
Síntomas: Pin 12 nunca cambia, nunca parpadea 3 veces
Causas posibles:
- Sensor desconectado del pin 2
- Sensor defectuoso
- Sensor necesita alimentación externa
- Tipo de sensor incorrecto (NPN vs PNP)
```

### **ESCENARIO B: Sensor detecta pero no cuenta**
```
Síntomas: Pin 12 cambia, parpadea 3 veces, pero nunca llega a 5
Causas posibles:
- Detecciones múltiples del mismo objeto
- Lógica de conteo incorrecta
- Objetos demasiado rápidos/lentos
```

### **ESCENARIO C: Cuenta pero motor no activa**
```
Síntomas: Cuenta llega a 5, pero no hay LED fijo de 2 segundos
Causas posibles:
- Problema en la lógica del motor
- Motor/relé desconectado del pin 3
- Alimentación insuficiente para el motor
```

### **ESCENARIO D: Todo funciona una vez, luego para**
```
Síntomas: Primera secuencia OK, luego nada más
Causas posibles:
- Variables no se resetean correctamente
- Sistema se bloquea después de primera secuencia
```

## 🎯 **REPORTE EL RESULTADO**

Después de las pruebas, reporta:

1. **¿Qué LED ves al inicio?** (parpadeos y comportamiento)
2. **¿Cambia el pin 12 cuando pasas objetos?**
3. **¿Cuántas veces parpadea el pin 13 por detección?** (3 o 5)
4. **¿Llega a activar el motor?**
5. **¿Qué tipo de sensor estás usando?** (óptico, magnético, mecánico)

Con esta información podremos identificar **exactamente** dónde está el problema.