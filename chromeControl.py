import time, serial, os, pyautogui, glob, alsaaudio, sys

# Find usb device
print ("Check for the Arduino")

#ports = glob.glob('/dev/tty[A-Za-z]*')
ports = glob.glob('/dev/ttyUSB*')

try: 
    print (ports[0])
except:
    print ("No Arduino found, check connection")
    sys.exit()
ser = serial.Serial(
	port=ports[0],
	baudrate=9600,
	parity=serial.PARITY_ODD,
	stopbits=serial.STOPBITS_TWO,
	bytesize=serial.EIGHTBITS,
	timeout=1)

#ser.open()
#ser.isOpen()

mixer = alsaaudio.Mixer(control='Master', id=0, cardindex=0)
value = mixer.getvolume()[0]
print ("Current volume: "+str(value))

time.sleep(1)

print ("Ok lets go")
ser.write(b'hello')

oldepochtime=0

# Loop and read serial port. Arduino sends values from time to time.
while True:
	try:
		status = ser.readline(1)
		#status = ser.read(5)
		#epochtime=time.time()
		#print epochtime
		#print (time.strftime("%Y-%m-%d %H:%M:%S", time.localtime(time.time())))
		#print(status)

		#decoded_bytes = float(status[0:len(status)-2].decode("utf-8"))
		#decoded_bytes = status.decode("utf-8")
		
		decoded_bytes = status.decode(errors='ignore')

		#print(decoded_bytes)
		decoded_bytes = decoded_bytes.strip()
		if (len(status) >0):
			#print (status[0])
			if (status[0]==80):	# = "P"
				print ("Movement")
			# B=66, G=71
			elif (status[0]==66):
				print ("Blue button")
				# Switch tab
				pyautogui.hotkey('ctrl', 'tab')

			elif (status[0]==71):
				print ("Green button")
				print ("Monitor on/off")
				
			elif (status[0]==117):
				print ("Up")
				value = value + 5
				if value > 100:
					value = 100
				mixer.setvolume(value)
				print ("Set volume to: " + str(value))

			elif (status[0]==100):
				print ("Down")
				value = value - 5
				if value < 0:
					value = 0
				mixer.setvolume(value)
				print ("Set volume to: " + str(value))
			else:
				pass
				#print (status)
		time.sleep(0.1)
		del status
		#oldepochtime=epochtime
	except KeyboardInterrupt:
		print("Keyboard Interrupt")
		break
exit()


