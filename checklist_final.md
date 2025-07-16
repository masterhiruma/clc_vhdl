# ✅ CHECKLIST DEFINITIVO - SOLUCIÓN PASO A PASO

## 🚨 **SITUACIÓN: Tu código sigue fallando**

Ya intentamos:
- ❌ Corregir lógica del sensor (HIGH vs LOW)
- ❌ Ajustar timing del Serial.print()
- ❌ Mejorar debounce

**Necesitamos diagnóstico profundo.**

---

## 📋 **FASE 1: VERIFICACIÓN BÁSICA DE HARDWARE**

### **🔌 Paso 1.1: Cargar test_basico.ino**
```
Objetivo: Verificar que sensor y motor responden físicamente
Tiempo: 2 minutos
```

**Qué observar:**
- [ ] **3 parpadeos al inicio** = Arduino funciona
- [ ] **LED pin 12 cambia** cuando pasas objetos = sensor funciona
- [ ] **LED pin 13 parpadea** cuando detecta cambios = lógica básica OK
- [ ] **Motor se activa cada 5 segundos** = conexión motor OK

**Si alguno falla → PROBLEMA DE HARDWARE**

### **🔧 Paso 1.2: Verificar conexiones físicas**
- [ ] Pin 2 conectado al sensor
- [ ] Pin 3 conectado al motor/relé
- [ ] Sensor tiene alimentación (si la necesita)
- [ ] Motor tiene alimentación independiente
- [ ] Tierra común entre Arduino y otros componentes

---

## 📡 **FASE 2: DIAGNÓSTICO AVANZADO**

### **🔍 Paso 2.1: Cargar tolva_diagnostico.ino**
```
Objetivo: Identificar dónde exactamente falla la lógica
Tiempo: 5 minutos
```

**Secuencia esperada:**
1. [ ] **1 parpadeo largo** = Sistema iniciado
2. [ ] **3 parpadeos motor** = Test motor inicial
3. [ ] **Parpadeo rápido continuo** = Loop funcionando
4. [ ] **5 parpadeos** = Sistema activo después de 3 segundos

### **🧪 Paso 2.2: Probar detección de objetos**
```
Pasar 5 objetos por el sensor, uno por uno
```

**Por cada objeto, verificar:**
- [ ] **LED pin 12 cambia** = sensor responde
- [ ] **3 parpadeos pin 13** = detección registrada
- [ ] **Conteo visual cada 3 segundos** = lógica de conteo funciona

**Al llegar a 5 objetos:**
- [ ] **LED fijo 2 segundos** = motor se activa
- [ ] **Motor pulsa 3 veces** = secuencia motor funciona
- [ ] **10 parpadeos rápidos** = secuencia completada

---

## 🎯 **FASE 3: DIAGNÓSTICO ESPECÍFICO POR SÍNTOMAS**

### **❌ SÍNTOMA A: LED pin 12 nunca cambia**
```
PROBLEMA: Sensor no responde físicamente
```
**Verificar:**
- [ ] ¿Sensor está conectado al pin 2?
- [ ] ¿Sensor tiene alimentación correcta?
- [ ] ¿Es un sensor NPN o PNP? (puede necesitar lógica invertida)
- [ ] ¿Multímetro muestra cambio de voltaje en pin 2?

**SOLUCIÓN:**
- Revisar datasheet del sensor
- Verificar alimentación (3.3V vs 5V)
- Probar con INPUT en lugar de INPUT_PULLUP

### **❌ SÍNTOMA B: Pin 12 cambia, pero pin 13 no parpadea**
```
PROBLEMA: Sensor responde pero debounce muy estricto
```
**Soluciones a probar:**
- [ ] Reducir `TIEMPO_DEBOUNCE_MS` de 100 a 20
- [ ] Cambiar lógica de HIGH a LOW (o viceversa)
- [ ] Objetos pasan muy rápido para el debounce

### **❌ SÍNTOMA C: Detecta objetos pero no cuenta bien**
```
PROBLEMA: Lógica de conteo fallando
```
**Verificar:**
- [ ] ¿Llega a 5 detecciones pero no activa motor?
- [ ] ¿Cuenta más de 1 por objeto? → Debounce insuficiente
- [ ] ¿Se resetea el contador? → Variable overflow

### **❌ SÍNTOMA D: Todo funciona una vez, luego para**
```
PROBLEMA: Sistema se bloquea después de primera secuencia
```
**Verificar:**
- [ ] ¿Variables se resetean correctamente?
- [ ] ¿Sistema entra en loop infinito?
- [ ] ¿Alimentación suficiente durante operación del motor?

---

## 📊 **FASE 4: REPORTE DE DIAGNÓSTICO**

### **📝 Información necesaria:**

**Hardware:**
- [ ] Tipo de sensor: _____________
- [ ] Alimentación sensor: _____________
- [ ] Tipo de motor/relé: _____________
- [ ] Alimentación motor: _____________

**Comportamiento observado:**
- [ ] Test básico: Sensor _____ Motor _____
- [ ] Diagnóstico avanzado: Detecta _____ Cuenta _____ Motor _____
- [ ] Síntoma específico: _____________

**Mediciones (si es posible):**
- [ ] Voltaje pin 2 sin objeto: _____V
- [ ] Voltaje pin 2 con objeto: _____V
- [ ] ¿Motor se activa con 5V directo?: _____

---

## 🔧 **FASE 5: SOLUCIONES DIRIGIDAS**

Una vez identificado el problema específico, aplicar:

### **Para sensor que no responde:**
1. Cambiar a configuración sin pull-up
2. Agregar resistencia externa si es necesario
3. Verificar compatible 5V vs 3.3V

### **Para detección errática:**
1. Ajustar debounce específico para tu sensor
2. Agregar filtro capacitivo en hardware
3. Cambiar lógica de flanco de subida a bajada

### **Para conteo incorrecto:**
1. Revisar velocidad de objetos
2. Ajustar lógica de conteo (cada pulso vs cada 2 pulsos)
3. Agregar delay post-detección

### **Para motor que no activa:**
1. Verificar alimentación independiente
2. Comprobar límites de corriente del pin 3
3. Usar transistor/MOSFET para cargas pesadas

---

## 🚀 **RESULTADO ESPERADO**

Al final de este proceso deberías tener:
1. ✅ **Diagnóstico claro** del problema específico
2. ✅ **Solución dirigida** para tu caso particular
3. ✅ **Código funcionando** adaptado a tu hardware

**¿Qué observas en cada fase?** Reporta los resultados para obtener ayuda específica.