14/11/2021

To be uploaded to an Arduino using the Arduino IDE.

EEPROM and Servo libraries must be installed.

Full mechanical and electrical designs to be linked here later.


2 TB6600 Stepper motor drivers used in 1/4 microstepping and 1A configuration.
NEMA 23 stepper motors powered by 24v 5A supply.
driverPUL pin connected to the PUL pin of both drivers in parallel.
driver1DIR and driver2DIR connected to the DIR pins of the drivers seperately.

L293D H-Bridge IC used for driving DC motor. dcMotorEnable, IN1, and IN2 are connected to respective pins on L293D.
DC and servo motors powered by a serperate 5V supply.

homesensor1 and homesensor2 both wired to hall effect sensors placed at upper limit of elevaor for redundancy.
