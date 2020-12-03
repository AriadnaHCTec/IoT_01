import mysql.connector
import matplotlib.pyplot as plt
import numpy as np
import datetime

def medicionGeneral(mydb,tabla,medicion,ide):
    mycursor = mydb.cursor()
    query =  "SELECT * FROM "+tabla+" WHERE claveUsuario = "+str(ide)
    mycursor.execute(query)
    myresult = mycursor.fetchall()
    x = []
    y = []
    for i in myresult:
      t = (datetime.datetime.min + i[2]).time()
      x.append(datetime.datetime.combine(i[1], 
                              t))
      y.append(i[3])
    
    x = np.array(x)
    y = np.array(y)
    
    query = "SELECT valorMinimo,valorMaximo FROM medidanormal WHERE medicion ='"+medicion+"'"
    mycursor.execute(query)
    myresult = mycursor.fetchall()
    y1 = np.array([myresult[0][0]]*len(x))
    y2 = np.array([myresult[0][1]]*len(x))            
    
    fig, ax = plt.subplots()
    fig.autofmt_xdate()
    ax.plot(x, y1)
    plt.plot(x, y, 'o-', label = "tu medida")
    plt.plot(x,y1)
    plt.plot(x,y2)
    #plt.scatter(x,y)
    
    """plt.legend("Mínimo")
    plt.legend("Máximo")"""
    plt.xlabel("Fecha")
    plt.title(tabla)
    plt.legend(["Tu medida","Mínimo","Máximo"])
    
    if(medicion == "temperatura"):
        plt.ylabel("°C")
    elif(medicion == "Oximetria"):
        plt.ylabel("%")
    elif(medicion == "frecuenciaCardiaca"):
        plt.ylabel("Pulso por minuto")
    
    plt.savefig(tabla+".jpg")

mydb = mysql.connector.connect(
  host="localhost",
  user="root",
  password="",
  database="estacion_01" #estacion_01
)

medicionGeneral(mydb,"medicionTemperaturaCorporal","temperatura",1)
medicionGeneral(mydb,"medicionOximetria","Oximetria",1)
medicionGeneral(mydb,"medicionFrecuenciaCardiaca","frecuenciaCardiaca",1)