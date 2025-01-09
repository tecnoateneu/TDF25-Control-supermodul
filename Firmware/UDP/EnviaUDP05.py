#
# Temps de Flors
# Envia l'arxiu dels efectes dels ruscs
# Son tots caràcters corresponets a dígits hexadecimals
# actualitzacio 02/01/2025
# modificació: Envia trames individualment a rusc
# aplica el temps abans d'enviar el paquet

import sys
import time
import socket

temps   =  0
t0      =  0
t1      =  0
t2      =  0
t3      =  0
c0      = '0'
c1      = '0'
c2      = '0'
c3      = '0'
nomar   = ''
verbo   =  0
rusc    =  0
r0      =  0
r1      =  0
rc0     = '0'
rc1     = '0'

t11 = time.time()
t12 = 0.0

npaq = 0

UDP_IP0 = "192.168.10.10"
UDP_IP1 = "192.168.10.11"
UDP_IP2 = "192.168.10.12"
UDP_IP3 = "192.168.10.13"
UDP_IP4 = "192.168.10.14"
UDP_IP5 = "192.168.10.15"
UDP_IP6 = "192.168.10.16"
UDP_IP7 = "192.168.10.17"
UDP_IPM = "192.168.10.40"
UDP_IPR0= "192.168.10.20"
UDP_IPR1= "192.168.10.21"
UDP_IPR2= "192.168.10.22"
UDP_IPR3= "192.168.10.23"
UDP_IPR4= "192.168.10.24"
UDP_IPR5= "192.168.10.25"
UDP_IPR6= "192.168.10.26"
UDP_IPR7= "192.168.10.27"
UDP_MCST= "239.192.0.1"
UDP_BCST= "192.168.10.255"
UDP_PORT=  4210
MESSAGE = b"0000"
time.time()
#print("UDP target IP: %s"   % UDP_IP0 )
print("UDP target IP:   192.168.10.xx")
print("UDP target port: %s" % UDP_PORT)
print ("")
#print("message: %s"         % MESSAGE )

# print ('Number of arguments:', len(sys.argv), 'arguments.')
# print ('Argument List:', str(sys.argv))
# print (str(sys.argv[0]))
# print (str(sys.argv[1]))

# processat arguments
if (len(sys.argv) == 1):
  print ("Falta el nom de l'arxiu de comandes")
  quit()

if (len(sys.argv) == 2):
  if ((str(sys.argv[1])) == "-v"):
    verbo = 1
  else:
    nomar = str(sys.argv[1])
    
if (len(sys.argv) == 3):
  if ((str(sys.argv[1])) == "-v"):
    verbo = 1
    nomar = str(sys.argv[2])
4
# print (verbo)
# print (nomar)

# obertura d'arxiu 
try:
  arxiu = open(nomar, 'r')  # obre l'arxiu de comandes en mode lectura
except FileNotFoundError as e:
    print(f"No trobo l'arxiu de comandes\n") 
    quit()

linea= arxiu.readline()             # llegeix la primera línia
while linea != '':                  # mentre sigui diferent de null
  npaq = npaq + 1
#  print(linea)
  llinea=len(linea[0:len(linea)-1]) # eliminem el retorn de carro del final de línia
#  print(llinea)
  if (llinea == 4):                 # les línies de 4 dígits son de pauta temporal
    c3 = linea[0:1]                 # comencem la conversió dels 4 dígits en un enter
    c2 = linea[1:2]
    c1 = linea[2:3]
    c0 = linea[3:4]
    if   ((c3 >= '0') & (c3 <= '9')):
      t3 = ord(c3) - 48
    elif ((c3 >= 'A') & (c3 <= 'F')):
      t3 = ord(c3) - 55
    elif ((c3 >= 'a') & (c3 <= 'f')):
      t3 = ord(c3) - 87
    
    if   ((c2 >= '0') & (c2 <= '9')):
      t2 = ord(c2) - 48
    elif ((c2 >= 'A') & (c2 <= 'F')):
      t2 = ord(c2) - 55
    elif ((c2 >= 'a') & (c2 <= 'f')):
      t2 = ord(c2) - 87
    
    if   ((c1 >= '0') & (c1 <= '9')):
      t1 = ord(c1) - 48
    elif ((c1 >= 'A') & (c1 <= 'F')):
      t1 = ord(c1) - 55
    elif ((c1 >= 'a') & (c1 <= 'f')):
      t1 = ord(c1) - 87
    
    if   ((c0 >= '0') & (c0 <= '9')):
      t0 = ord(c0) - 48
    elif ((c0 >= 'A') & (c0 <= 'F')):
      t0 = ord(c0) - 55
    elif ((c0 >= 'a') & (c0 <= 'f')):
      t0 = ord(c0) - 87

  print(time.time() - t11)
  t12 = t12 + (time.time() - t11)
  t11 =  time.time()   
  temps = 1 + (((t3 * 16) + t2) * 256) + (t1 *16) + t0
  time.sleep(temps/1000)       # aplica el retard +1
    
  MESSAGEu = linea[0:len(linea)-1]
  MESSAGE = bytes(MESSAGEu, 'UTF-8')

  if (llinea != 4) & (llinea != 0):    # si no és pauta temporal o línia en blanc
     if ((llinea == 1) | (llinea == 7)):
        sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM)      # UDP
        sock.sendto(MESSAGE, (UDP_IPM, UDP_PORT))

#        print("Audio ",MESSAGE, " - " ,MESSAGEu, UDP_IPM, UDP_PORT)

     elif (llinea == 3):                     # Reles
        sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM)      # UDP
        sock.sendto(MESSAGE, (UDP_IPR0, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR1, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR2, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR3, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR4, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR5, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR6, UDP_PORT))
        sock.sendto(MESSAGE, (UDP_IPR7, UDP_PORT))
        print("rele ",MESSAGE, " - " ,MESSAGEu, UDP_IPM, UDP_PORT)
     elif (llinea == 392):
# MULTICAST
#        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#        sock.sendto(MESSAGE, UDP_MCST, UDP_PORT)        
#        print("Paquet multicast")
# BROADCAST
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.sendto(MESSAGE, (UDP_BCST, UDP_PORT))
#        sock.close()    # cal sempre?
#        print("Paquet broadcast")
     
     else:
        rc1 = linea[0:1]                 # 2 dígits hexa a un enter
        rc0 = linea[1:2]
        if   ((rc1 >= '0') & (rc1 <= '9')):
          r1 = ord(rc1) - 48
        elif ((rc1 >= 'A') & (rc1 <= 'F')):
          r1 = ord(rc1) - 55
        elif ((rc1 >= 'a') & (rc1 <= 'f')):
          r1 = ord(rc1) - 87
    
        if   ((rc0 >= '0') & (rc0 <= '9')):
          r0 = ord(rc0) - 48
        elif ((rc0 >= 'A') & (rc0 <= 'F')):
          r0 = ord(rc0) - 55
        elif ((rc0 >= 'a') & (rc0 <= 'f')):
          r0 = ord(rc0) - 87
    
        rusc = (r1 *16) + r0
#        print("rusc ",rusc)

        if  (rusc == 255):        
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP0, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP1, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP2, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP3, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP4, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP5, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP6, UDP_PORT))
          sock.sendto(MESSAGE, (UDP_IP7, UDP_PORT))
          if (verbo == 1):
            print(rusc, " ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 0):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP0, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 1):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP1, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 2):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP2, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 3):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP3, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 4):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP4, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 5):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP5, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 6):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP6, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

        elif (rusc == 7):
          sock = socket.socket(socket.AF_INET,     # Internet
                               socket.SOCK_DGRAM)  # UDP
          sock.sendto(MESSAGE, (UDP_IP7, UDP_PORT))
          if (verbo == 1):
            print(rusc, "   ", linea[0:len(linea)-1]) # per debug/ verbose
#            time.sleep(temps/1000)       # aplica el retard

#  time.sleep(temps/1000)       # aplica el retard

  linea = arxiu.readline()               # nova línia
  if (len(linea) == 0) :                 # si la nova línia és null s'ha acabat l'arxiu
     break

print(t12)
print("Arxiu acabat...", 7 * npaq / 9, " paquets")
#
# Fi de programa
#



