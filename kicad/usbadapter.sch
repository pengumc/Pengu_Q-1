EESchema Schematic File Version 2  date Tue 20 Jan 2015 04:43:29 PM CET
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
EELAYER 43  0
EELAYER END
$Descr A4 11700 8267
encoding utf-8
Sheet 1 1
Title ""
Date "20 jan 2015"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 2500 1750 0    60   ~ 0
D+ (green)
Text Notes 2500 1550 0    60   ~ 0
D- (white)
Text Notes 4500 3150 0    60   ~ 0
VCC (red)\n
Text Notes 3400 3400 0    60   ~ 0
GND (black)
Connection ~ 4250 2750
Wire Wire Line
	4000 2750 4250 2750
Wire Wire Line
	3800 1700 4250 1700
Wire Wire Line
	4250 1700 4250 3250
Wire Wire Line
	3550 3000 3700 3000
Wire Wire Line
	4150 3250 3550 3250
Connection ~ 3950 1500
Wire Wire Line
	3950 1350 3950 1500
Wire Wire Line
	4450 3250 4450 1350
Wire Wire Line
	3800 1500 4350 1500
Wire Wire Line
	4350 1500 4350 3250
Wire Wire Line
	3300 1500 3100 1500
Wire Wire Line
	4100 3000 4350 3000
Connection ~ 4350 3000
Wire Wire Line
	3600 2750 3550 2750
Wire Wire Line
	3550 2750 3550 3250
Connection ~ 3550 3000
Wire Wire Line
	3300 1700 3100 1700
$Comp
L R R2
U 1 1 54BE5B67
P 3550 1700
F 0 "R2" V 3650 1650 50  0000 C CNN
F 1 "68" V 3650 1800 50  0000 C CNN
	1    3550 1700
	0    1    1    0   
$EndComp
$Comp
L R R3
U 1 1 54BE5B5F
P 4200 1350
F 0 "R3" V 4100 1250 50  0000 C CNN
F 1 "2k2" V 4100 1400 50  0000 C CNN
	1    4200 1350
	0    1    1    0   
$EndComp
$Comp
L R R1
U 1 1 54BE5B57
P 3550 1500
F 0 "R1" V 3650 1600 50  0000 C CNN
F 1 "68" V 3650 1450 50  0000 C CNN
	1    3550 1500
	0    -1   -1   0   
$EndComp
$Comp
L ZENER D2
U 1 1 54BE5B0B
P 3800 2750
F 0 "D2" H 3950 2650 50  0000 C CNN
F 1 "3v3" H 3750 2650 40  0000 C CNN
	1    3800 2750
	1    0    0    1   
$EndComp
$Comp
L ZENER D1
U 1 1 54BE5AE7
P 3900 3000
F 0 "D1" H 4000 2900 50  0000 C CNN
F 1 "3v3" H 3800 2900 40  0000 C CNN
	1    3900 3000
	1    0    0    1   
$EndComp
$Comp
L USB_2 J1
U 1 1 54BE5A4C
P 4300 3450
F 0 "J1" H 4225 3700 60  0000 C CNN
F 1 "USB_2" H 4350 3150 60  0001 C CNN
F 4 "VCC" H 4625 3600 50  0001 C CNN "VCC"
F 5 "D+" H 4600 3500 50  0001 C CNN "Data+"
F 6 "D-" H 4600 3400 50  0001 C CNN "Data-"
F 7 "GND" H 4625 3300 50  0001 C CNN "Ground"
	1    4300 3450
	0    1    -1   0   
$EndComp
$EndSCHEMATC
