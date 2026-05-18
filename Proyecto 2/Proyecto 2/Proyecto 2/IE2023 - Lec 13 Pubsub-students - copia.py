# Proyecto 2 - Puente Adafruit IO <-> Microchip/ATmega328P
# Control de 4 servos desde Adafruit con retroalimentacion a gauges

import sys
import time
import serial
from Adafruit_IO import MQTTClient

# =====================================
# CONFIGURACION ADAFRUIT
# =====================================
ADAFRUIT_AIO_USERNAME = 
ADAFRUIT_AIO_KEY      = 

# =====================================
# CONFIGURACION SERIAL
# =====================================
SERIAL_PORT = "COM3"   
BAUD_RATE = 9600

# =====================================
# FEEDS ADAFRUIT
# Sliders = SEND
# Gauges = READ
# =====================================
FEED_SERVO1_SEND = "counter-tx"
FEED_SERVO1_READ = "counter-rx"

FEED_SERVO2_SEND = "servo-2"
FEED_SERVO2_READ = "servo-2-read"

FEED_SERVO3_SEND = "servo-3-send"
FEED_SERVO3_READ = "servo-3-read"

FEED_SERVO4_SEND = "servo-4-send"
FEED_SERVO4_READ = "servo-4-read"

# =====================================
# CONEXION SERIAL
# =====================================
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    time.sleep(2)
    print("Serial conectado correctamente.")
except Exception as e:
    print("Error abriendo serial:", e)
    sys.exit(1)

# =====================================
# CALLBACKS ADAFRUIT IO
# =====================================
def connected(client):
    print("Conectado a Adafruit IO.")

    client.subscribe(FEED_SERVO1_SEND)
    client.subscribe(FEED_SERVO2_SEND)
    client.subscribe(FEED_SERVO3_SEND)
    client.subscribe(FEED_SERVO4_SEND)

    print("Suscrito a feeds de control.")
    print("Esperando datos desde Adafruit...")


def disconnected(client):
    print("Desconectado de Adafruit IO.")
    sys.exit(1)


def message(client, feed_id, payload):
    print(f"Adafruit -> {feed_id}: {payload}")

    try:
        angle = int(float(payload))
    except ValueError:
        print("Dato invalido recibido.")
        return

    if angle < 0:
        angle = 0
    elif angle > 180:
        angle = 180

    if feed_id == FEED_SERVO1_SEND:
        comando = f"S1:{angle}\n"

    elif feed_id == FEED_SERVO2_SEND:
        comando = f"S2:{angle}\n"

    elif feed_id == FEED_SERVO3_SEND:
        comando = f"S3:{angle}\n"

    elif feed_id == FEED_SERVO4_SEND:
        comando = f"S4:{angle}\n"

    else:
        return

    ser.write(comando.encode())
    print(f"Python -> Microchip: {comando.strip()}")


# =====================================
# CLIENTE MQTT
# =====================================
client = MQTTClient(ADAFRUIT_AIO_USERNAME, ADAFRUIT_AIO_KEY)

client.on_connect = connected
client.on_disconnect = disconnected
client.on_message = message

client.connect()
client.loop_background()

# =====================================
# LOOP PRINCIPAL
# Microchip debe responder:
# R1:90
# R2:45
# R3:120
# R4:180
# =====================================
while True:
    try:
        if ser.in_waiting > 0:
            linea = ser.readline().decode(errors="ignore").strip()

            if linea:
                print(f"Microchip -> Python: {linea}")

                if ":" in linea:
                    tag, valor = linea.split(":", 1)

                    try:
                        valor = int(float(valor))
                    except ValueError:
                        continue

                    if valor < 0:
                        valor = 0
                    elif valor > 180:
                        valor = 180

                    if tag == "R1":
                        client.publish(FEED_SERVO1_READ, valor)
                        print(f"Python -> Adafruit {FEED_SERVO1_READ}: {valor}")

                    elif tag == "R2":
                        client.publish(FEED_SERVO2_READ, valor)
                        print(f"Python -> Adafruit {FEED_SERVO2_READ}: {valor}")

                    elif tag == "R3":
                        client.publish(FEED_SERVO3_READ, valor)
                        print(f"Python -> Adafruit {FEED_SERVO3_READ}: {valor}")

                    elif tag == "R4":
                        client.publish(FEED_SERVO4_READ, valor)
                        print(f"Python -> Adafruit {FEED_SERVO4_READ}: {valor}")

        time.sleep(0.05)

    except KeyboardInterrupt:
        print("Programa terminado por usuario.")
        ser.close()
        sys.exit(0)

    except Exception as e:
        print("Error:", e)
        time.sleep(1)