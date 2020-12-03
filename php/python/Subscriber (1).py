# importación de librerías
import time
import paho.mqtt.client as mqttClient

#definición de funciones

def on_connect(client,userdata,flags,rc):
    if rc == 0:
        print("Conectado al broker")
        global Connected
        Connected = True
    else:
        print("Fallo en la conexión")

def on_message(client,userdata,message):
    #print(+": ".format(message.payload))
    print("Mensaje recibido: {}".format(message.payload))

Connected = False             #variable globar para verificar estado de la conexión
broker = "broker.hivemq.com"  #ip o URL del broker
port = 1883                   # puerto por default
#tag = "/medicion/1"   #Etiqueta
tag = "/medicion/temperatura"   #Etiqueta
tag1 = "/medicion/oximetria"   #Etiqueta
tag2 = "/medicion/frecuencia"   #Etiqueta
#user = ""
#password = ""
clientS = mqttClient.Client("Cliente") #crea una instancia
clientS.on_connect = on_connect        #agrega la función al objeto
clientS.on_message = on_message
clientS.connect(broker,port)
clientS.loop_start()

while Connected != True:
    time.sleep(1)
    clientS.subscribe(tag)
    clientS.subscribe(tag1)
    clientS.subscribe(tag2)
    try:
        while True:
            time.sleep(1)
    except KeyboardInterrupt:
        print("Recepción de mensajes detenida por el usuario")
        clientS.disconnect()
        clientS.loop_stop()