oscope these pins to see these songs "play" on the oscope:


r30.t0 P9_31 switch btwn 0 & 1
r30.t1 P9_29 ghostbusters
r30.t2 P9_30 terminator
t30.t5 P9_27 mario

That is, the CPU sends a cmd buffer to the pru, which the pru executes. 

That cmd buffer contains 32 cmd pkts, which are exeuted sequentially at 5ms intervals as soon as the pru gets it.

Currently the pru only uses the GPIO field of a cmd pkt, switching 4 pins as specified in bits 0,1,2,5 of the gpio unsigned integer field of the cmd pkt.

For each of the 4 gpios, the C program has a hard-coded pattern of 1's and 0's that the pin executes. For example, pin P9_31 is configured to reg 30, bit 0, and the PRU sets this bit according to bit 0 of the gpio unsigned int in each cmd pkt. The hard-coded cmd buffer in this C program was configured so that bit 0 toggles high and low alternately. Bit 1 swiches in time to the beat of the Ghostbuster movie theme song. ETc.

We're sending a cmd pkt to the PRU and it's playing it every 5ms. 


//////////
Also, if you set an int to a wrapped unsigned int, it seems to convert to negative correctly:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/02-gpio-plays-songs# ./test-unsigned-to-signed-int
ui_minusone: 4294967295 -1 ffffffff
i_minusone_equals: 4294967295 -1 ffffffff
i_minusone_pointer: 4294967295 -1 ffffffff
for loop
ui_minusone: 3 3 3
i_minusone_equals: 3 3 3
i_minusone_pointer: 3 3 3
ui_minusone: 2 2 2
i_minusone_equals: 2 2 2
i_minusone_pointer: 2 2 2
ui_minusone: 1 1 1
i_minusone_equals: 1 1 1
i_minusone_pointer: 1 1 1
ui_minusone: 0 0 0
i_minusone_equals: 0 0 0
i_minusone_pointer: 0 0 0
ui_minusone: 4294967295 -1 ffffffff
i_minusone_equals: 4294967295 -1 ffffffff
i_minusone_pointer: 4294967295 -1 ffffffff
ui_minusone: 4294967294 -2 fffffffe
i_minusone_equals: 4294967294 -2 fffffffe
i_minusone_pointer: 4294967294 -2 fffffffe
ui_minusone: 4294967293 -3 fffffffd
i_minusone_equals: 4294967293 -3 fffffffd
i_minusone_pointer: 4294967293 -3 fffffffd
ui_minusone: 4294967292 -4 fffffffc
i_minusone_equals: 4294967292 -4 fffffffc
i_minusone_pointer: 4294967292 -4 fffffffc
ui_minusone: 4294967291 -5 fffffffb
i_minusone_equals: 4294967291 -5 fffffffb
i_minusone_pointer: 4294967291 -5 fffffffb
ui_minusone: 4294967290 -6 fffffffa
i_minusone_equals: 4294967290 -6 fffffffa
i_minusone_pointer: 4294967290 -6 fffffffa

yay twos-complement.


//////////
old:


This test showcases the fancy PRU code that:
1. listens to see if CPU requests data
2. listens to see if CPU uploaded new actuation schedule
3. at sample times (5ms), read the adc, eqep, and write the gpios in accordance with the actuation schedule.

Note: don't forget to load the DTOs:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio# cat $SLOTS
 0: 54:PF--- 
 1: 55:PF--- 
 2: 56:PF--- 
 3: 57:PF--- 
 4: ff:P-O-L Bone-LT-eMMC-2G,00A0,Texas Instrument,BB-BONE-EMMC-2G
 5: ff:P-O-- Bone-Black-HDMI,00A0,Texas Instrument,BB-BONELT-HDMI
 6: ff:P-O-- Bone-Black-HDMIN,00A0,Texas Instrument,BB-BONELT-HDMIN
 8: ff:P-O-L Override Board Name,00A0,Override Manuf,BB-ADC
 9: ff:P-O-L Override Board Name,00A0,Override Manuf,bone_pwm_P8_34
10: ff:P-O-L Override Board Name,00A0,Override Manuf,am33xx_pwm
11: ff:P-O-L Override Board Name,00A0,Override Manuf,bone_eqep1
12: ff:P-O-L Override Board Name,00A0,Override Manuf,jppprugpio


Note: For some reason this test doesn't work if you call eqep_init()
or eqep_set_period(). Not sure why. The default vals seem to work:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio# cat /sys/devices/ocp.3/48302000.epwmss/48302180.eqep/period
1000000000

Seems likea big #. but what is this number exactly? It's clearly not refresh rate, you can see here it changes every sample:

cycle: 298000024 sample: 297 adc: 2045 eqep: 8704 gpio: 48 duty: 100 last-data-req: 262686243 last-cmd: 200288986
cycle: 299000024 sample: 298 adc: 2045 eqep: 8714 gpio: 48 duty: 100 last-data-req: 262686243 last-cmd: 200288986
cycle: 300000024 sample: 299 adc: 2044 eqep: 8727 gpio: 48 duty: 100 last-data-req: 262686243 last-cmd: 200288986
cycle: 301000024 sample: 300 adc: 2045 eqep: 8740 gpio: 48 duty: 100 last-data-req: 262686243 last-cmd: 200288986
cycle: 302000024 sample: 301 adc: 2046 eqep: 8759 gpio: 48 duty: 100 last-data-req: 262686243 last-cmd: 200288986
cycle: 303000024 sample: 302 adc: 2045 eqep: 8780 gpio: 48 duty: 100 last-data-req: 262686243 last-cmd: 200288986

See the QUPRD register in the TRM, I think this is not acually the number of ns between when the encoder looks at for doing velocity estimation at low speed. 

Should read this part in the TRM whne I've gotten more sleep.


Note: this test doesn't do fancy actuator scheduling at certain times. CPU sends x (1 - 32) samples, writes x into a ctrl register, and PRU writes one gpio values once every 5ms.

Note: See the top of adc-eqep-gpio.h file for how I laid out the buffers in the pru0 data ram. It's complicated.

Main concepts: 

- data packet
  - 8 ints long: cycle time, sample num, adc, eqep, gpio, etc
- cmd packet
  - 4 ints long: cycle time, 
  - only gpio used currently
- data buffer
  - 32 data packets
- cmd buffer
  - 32 cmd packets
- PRU Data Buffer
  - pru writes new value to this (cyclically) every 5ms. 
- CPU Data Buffer
  - when CPU asks, PRU copies PRU Data Buffer to CPU Data Buffer, then writes 1 to the (32-bit) PRU Data Ready control register.
- PRU Cmd Buffer
  - every 5ms, the PRU writes one of these gpio values to the gpios. 
  - when it runs out of cmd pkts, oh well. 
- CPU Cmd Buffer
  - 32 cmd pkts
  - CPU decides what it wants the gpios & pwm to do, writes it into the CPU Cmd Buffer, then sets teh CPU New Schedule control register
- CPU PRU Ctrl Registers
  - 4 ints at PRU0 Data RAM addr 0
  - 1-word ctrl regs
  -  'cpu wants data' 0x0 - 0x3
  -  'pru data ready' 0x4 - 0x7
  -  'cpu new sched'  0x8 - 0xB
  -  'pru ack sched'  0xC - 0xF


The basic idea is to have the cpu set a bit high when it wants
data. The PRU quickly copies the data buffer from its private PRU Data
Buffer to the CPU Data Buffer, then sets the "data ready" bit.

When the CPU wants to write values to the GPIOs or PWM, it populates
the CPU Cmd Buffer then writes 1 to teh 'cpu new sched' control
register.



What the code actually does
-----------------------------

Test 1. How long does it take to copy over an cmd schedule?

In principle, if we get data and the last sample is time 10ms, if we
already have a cmd schedule, we should be able to copy it over to the
PRU and have the PRU start executing it by the very next timestep at 15ms.

To test that, we do this:

1. cpu asks for data

cycle: 169000022 sample: 168 adc: 2046 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 170000022 sample: 169 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
...
cycle: 198000024 sample: 197 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 199000024 sample: 198 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0

 (last pkt is sample 199, so hopefully we can get our schedule executed by step 200)

2. cpu copies over cmd sched to PRU
3. cpu tells pru about new sched. Hopefully we'll see that sample 200 has somethign in 'gpio'.
4. wait a few ms just to let a couple sample go by.
5. cpu asks for data again

New data buffer:
cycle: 171000022 sample: 170 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 172000022 sample: 171 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 173000022 sample: 172 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
...
cycle: 199000024 sample: 198 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 201000029 sample: 200 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 202000026 sample: 201 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 200288949 last-cmd: 200376618

Success! We see 'gpio: 48' at sample 200, meaning that if our ctrl alg
computation time is fast enough, it will suffer no more than 1-sample
delay.

Also, note that sample time 200000024, then data req came in at
(non-sample-time) last-data-req: 200288949, then new cmd sched came in
at last-cmd: 200376618

1.000000120 sec   sample 199
1.001444745 sec   data req arrives
1.001883090 sec   cmd sch arrives
1.005000145 sec   next sample time


That is, it takes a


Test 2: OS Preemption
-----------------------

The following code in adc-... .c shows that OS preemption can cause
the CPU to lose samples. For example, successive buffers soemtimes
lose a lot ofsamples in between:


cycle: 173000022 sample: 172 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
...
cycle: 204000022 sample: 203 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 202491093 last-cmd: 200376618

next time data is retrieved, only 2 samples advanced (so CPU did it very fast):

cycle: 176000022 sample: 175 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
...
cycle: 207000016 sample: 206 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 204717777 last-cmd: 200376618

next time, we're still ok:

cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
...
cycle: 231000020 sample: 230 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618

next time, we missed samples 231 thru 236:


cycle: 238000020 sample: 237 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
...
cycle: 269000022 sample: 268 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618

next time, we missed samples 269 thru 280:

cycle: 282000022 sample: 281 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
...
cycle: 313000024 sample: 312 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618


The printfs are probably causing the problem.




Device tree
-----------------

This should be your DTO:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio# cat $SLOTS
 0: 54:PF--- 
 1: 55:PF--- 
 2: 56:PF--- 
 3: 57:PF--- 
 4: ff:P-O-L Bone-LT-eMMC-2G,00A0,Texas Instrument,BB-BONE-EMMC-2G
 5: ff:P-O-- Bone-Black-HDMI,00A0,Texas Instrument,BB-BONELT-HDMI
 6: ff:P-O-- Bone-Black-HDMIN,00A0,Texas Instrument,BB-BONELT-HDMIN
 8: ff:P-O-L Override Board Name,00A0,Override Manuf,BB-ADC
 9: ff:P-O-L Override Board Name,00A0,Override Manuf,bone_pwm_P8_34
10: ff:P-O-L Override Board Name,00A0,Override Manuf,am33xx_pwm
17: ff:P-O-L Override Board Name,00A0,Override Manuf,jppprugpio


Note: I've got somethign wrong with my DTO, if you load teh bone_eqep1 overlay and run, it hangs the whole BBB.



Build script
---------------------

Note: Some weird shit involving different versions of the C standard: POSIX, ISO-C99, GNU99, etc. 
I wanted to declare a var inside a for loop: for(int i=0; i<4; i++). 
Error: ‘for’ loop initial declarations are only allowed in C99 mode
Fix: gcc -std=c99 or -std=gnu99.
Did gnu99, but now:
warning: implicit declaration of function ‘usleep’ [-Wimplicit-function-declaration]
Fix: add
#define _POSIX_C_SOURCE199309L  // dammit, weird posix stuff. 
Ugh.



root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio# cat run
#!/bin/bash

set -e

echo "I'm in: "
echo `pwd`

echo "NOT building jadcpwmeqep.o"
# gcc -g -Wall -Werror -pthread -lstdc++ -std=gnu99  -c jadcpwmeqep.c


name=adc-eqep-gpio
binfile=$name.p
cfile=$name.c
exename=$(basename $cfile .c)



echo "Building .bin file $binfile"

pasm -b $binfile

echo "Building .c file $cfile to make $exename"

echo "(Careful - does your .c load the correct .p / .bin?)"

# use -std=c99 to permit for( int i=0
# without this you get: ‘for’ loop initial declarations are only allowed in C99 mode
# Or: -std=gnu99 and usleep. gnu99 and nanosleep no work.
gcc -std=gnu99 -g $cfile jadcpwmeqep.o -o $exename -lprussdrv -lpthread -lrt

echo "Running $exename"

./$exename







Example output:
----------------------------------

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio# ./run
I'm in: 
/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio
NOT building jadcpwmeqep.o
Building .bin file adc-eqep-gpio.p


PRU Assembler Version 0.84
Copyright (C) 2005-2013 by Texas Instruments Inc.


Pass 2 : 0 Error(s), 0 Warning(s)

Writing Code Image of 306 word(s)

NOT Building .c file adc-eqep-gpio.c to make adc-eqep-gpio
(Careful - does your .c load the correct .p / .bin?)
NOT Running adc-eqep-gpio
Hey, if you get a seg fault, make sure the jppprugpio device tree overlay has been loaded!!
Here's the cmd: root@beaglebone:/home/debian/beaglebone_pru_adc/src# echo jppprugpio > /sys/devices/bone_capemgr.9/slots
Also do this:
echo bone_eqep1 > $SLOTSNote: I'm using the qQEP sysfs entry:/sys/devices/ocp.3/48302000.epwmss/48302180.eqep, so make sure that exists!!
Calling constructor...
[eQEP /sys/devices/ocp.3/48302000.epwmss/48302180.eqep] Unable to open mode for write[eQEP /sys/devices/ocp.3/48302000.epwmss/48302180.eqep] Unable to open position for writeSetting period...
[eQEP /sys/devices/ocp.3/48302000.epwmss/48302180.eqep] Unable to open period for writePROBLEM: prussdrv_init()
PROBLEM: prussdrv_open
PROBLEM: prussdrv_pruintc_init
pru0 data memory: b6d42000
About to start PRU, time=
Main CPU time:     0.000003375 (since last call:     0.000000791) 
executing bin file: ./adc-eqep-gpio.bin
Waiting...
About to set "CPU Want data" bit, time=
Main CPU time:     1.001874167 (since last call:     1.001870792) 
Waiting for "Data ready" bit...
Got it! Time:
Main CPU time:     1.002110250 (since last call:     0.000236083) 
Copying cmd to PRU data ram, time=
Main CPU time:     1.002236459 (since last call:     0.000126209) 
Done copying, time=
Main CPU time:     1.002383667 (since last call:     0.000147208) 
Waiting until pru says he copied over the new schedule...time=
Main CPU time:     1.002502792 (since last call:     0.000119125) 
He got it! time=
Main CPU time:     1.002615584 (since last call:     0.000112792) 
Waiting for "Data ready" bit...
Got it!
Old data buffer:
cycle: 169000022 sample: 168 adc: 2046 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 170000022 sample: 169 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 171000022 sample: 170 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 172000022 sample: 171 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 173000022 sample: 172 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 174000022 sample: 173 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 175000022 sample: 174 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 176000022 sample: 175 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 177000022 sample: 176 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 178000022 sample: 177 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 179000022 sample: 178 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 180000022 sample: 179 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 181000022 sample: 180 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 182000022 sample: 181 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 183000022 sample: 182 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 184000022 sample: 183 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 185000022 sample: 184 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 186000022 sample: 185 adc: 2046 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 187000022 sample: 186 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 188000022 sample: 187 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 189000022 sample: 188 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 190000022 sample: 189 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 191000022 sample: 190 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 192000022 sample: 191 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 193000025 sample: 192 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 194000024 sample: 193 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 195000024 sample: 194 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 196000024 sample: 195 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 197000024 sample: 196 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 198000024 sample: 197 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 199000024 sample: 198 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
New data buffer:
cycle: 171000022 sample: 170 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 172000022 sample: 171 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 173000022 sample: 172 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 174000022 sample: 173 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 175000022 sample: 174 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 176000022 sample: 175 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 177000022 sample: 176 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 178000022 sample: 177 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 179000022 sample: 178 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 180000022 sample: 179 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 181000022 sample: 180 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 182000022 sample: 181 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 183000022 sample: 182 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 184000022 sample: 183 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 185000022 sample: 184 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 186000022 sample: 185 adc: 2046 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 187000022 sample: 186 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 188000022 sample: 187 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 189000022 sample: 188 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 190000022 sample: 189 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 191000022 sample: 190 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 192000022 sample: 191 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 193000025 sample: 192 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 194000024 sample: 193 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 195000024 sample: 194 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 196000024 sample: 195 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 197000024 sample: 196 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 198000024 sample: 197 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 199000024 sample: 198 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 201000029 sample: 200 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 202000026 sample: 201 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 200288949 last-cmd: 200376618
Here's the data buffer a few more times, just to show it's flushing:
Setting "CPU Wants" bit, time=
Main CPU time:     1.023989792 (since last call:     0.021374208) 
Waiting for "Data ready" bit, time=
Main CPU time:     1.024301375 (since last call:     0.000311583) 
Got it! time=
Main CPU time:     1.024568625 (since last call:     0.000267250) 
Copy over the new data buf, time=
Main CPU time:     1.024899834 (since last call:     0.000331209) 
Done! time=
Main CPU time:     1.025288459 (since last call:     0.000388625) 
Printing Data buffer (iter 0) time=:
Main CPU time:     1.025503709 (since last call:     0.000215250) 
cycle: 173000022 sample: 172 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 174000022 sample: 173 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 175000022 sample: 174 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 176000022 sample: 175 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 177000022 sample: 176 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 178000022 sample: 177 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 179000022 sample: 178 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 180000022 sample: 179 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 181000022 sample: 180 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 182000022 sample: 181 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 183000022 sample: 182 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 184000022 sample: 183 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 185000022 sample: 184 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 186000022 sample: 185 adc: 2046 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 187000022 sample: 186 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 188000022 sample: 187 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 189000022 sample: 188 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 190000022 sample: 189 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 191000022 sample: 190 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 192000022 sample: 191 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 193000025 sample: 192 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 194000024 sample: 193 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 195000024 sample: 194 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 196000024 sample: 195 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 197000024 sample: 196 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 198000024 sample: 197 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 199000024 sample: 198 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 201000029 sample: 200 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 202000026 sample: 201 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 203000024 sample: 202 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 202491093 last-cmd: 200376618
cycle: 204000022 sample: 203 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 202491093 last-cmd: 200376618
Sleeping for 10ms, time=
Main CPU time:     1.029299625 (since last call:     0.003795916) 
Awake! time=
Main CPU time:     1.039686375 (since last call:     0.010386750) 
Setting "CPU Wants" bit, time=
Main CPU time:     1.039842375 (since last call:     0.000156000) 
Waiting for "Data ready" bit, time=
Main CPU time:     1.145903375 (since last call:     0.106061000) 
Got it! time=
Main CPU time:     1.146188959 (since last call:     0.000285584) 
Copy over the new data buf, time=
Main CPU time:     1.146311042 (since last call:     0.000122083) 
Done! time=
Main CPU time:     1.146538792 (since last call:     0.000227750) 
Printing Data buffer (iter 1) time=:
Main CPU time:     1.146649125 (since last call:     0.000110333) 
cycle: 176000022 sample: 175 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 177000022 sample: 176 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 178000022 sample: 177 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 179000022 sample: 178 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 180000022 sample: 179 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 181000022 sample: 180 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 182000022 sample: 181 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 183000022 sample: 182 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 184000022 sample: 183 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 185000022 sample: 184 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 186000022 sample: 185 adc: 2046 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 187000022 sample: 186 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 188000022 sample: 187 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 189000022 sample: 188 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 190000022 sample: 189 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 191000022 sample: 190 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 192000022 sample: 191 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 193000025 sample: 192 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 194000024 sample: 193 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 195000024 sample: 194 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 196000024 sample: 195 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 197000024 sample: 196 adc: 2044 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 198000024 sample: 197 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 199000024 sample: 198 adc: 2043 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 201000029 sample: 200 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 202000026 sample: 201 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 203000024 sample: 202 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 202491093 last-cmd: 200376618
cycle: 204000022 sample: 203 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 202491093 last-cmd: 200376618
cycle: 205000020 sample: 204 adc: 2048 eqep: 0 gpio: 48 duty: 100 last-data-req: 204717777 last-cmd: 200376618
cycle: 206000018 sample: 205 adc: 2045 eqep: 0 gpio: 6f duty: 100 last-data-req: 204717777 last-cmd: 200376618
cycle: 207000016 sample: 206 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 204717777 last-cmd: 200376618
Sleeping for 10ms, time=
Main CPU time:     1.149013167 (since last call:     0.002364042) 
Awake! time=
Main CPU time:     1.159430709 (since last call:     0.010417542) 
Setting "CPU Wants" bit, time=
Main CPU time:     1.159697334 (since last call:     0.000266625) 
Waiting for "Data ready" bit, time=
Main CPU time:     1.159833500 (since last call:     0.000136166) 
Got it! time=
Main CPU time:     1.159948042 (since last call:     0.000114542) 
Copy over the new data buf, time=
Main CPU time:     1.160062792 (since last call:     0.000114750) 
Done! time=
Main CPU time:     1.160278834 (since last call:     0.000216042) 
Printing Data buffer (iter 2) time=:
Main CPU time:     1.160384125 (since last call:     0.000105291) 
cycle: 200000024 sample: 199 adc: 2045 eqep: 0 gpio: 0 duty: 100 last-data-req: 0 last-cmd: 0
cycle: 201000029 sample: 200 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 202000026 sample: 201 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 200288949 last-cmd: 200376618
cycle: 203000024 sample: 202 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 202491093 last-cmd: 200376618
cycle: 204000022 sample: 203 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 202491093 last-cmd: 200376618
cycle: 205000020 sample: 204 adc: 2048 eqep: 0 gpio: 48 duty: 100 last-data-req: 204717777 last-cmd: 200376618
cycle: 206000018 sample: 205 adc: 2045 eqep: 0 gpio: 6f duty: 100 last-data-req: 204717777 last-cmd: 200376618
cycle: 207000016 sample: 206 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 204717777 last-cmd: 200376618
cycle: 208000014 sample: 207 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 209000028 sample: 208 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 210000026 sample: 209 adc: 2048 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 211000024 sample: 210 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 212000022 sample: 211 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 213000020 sample: 212 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 214000018 sample: 213 adc: 2048 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 215000016 sample: 214 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 216000014 sample: 215 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 217000028 sample: 216 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 218000026 sample: 217 adc: 2048 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 219000024 sample: 218 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 220000022 sample: 219 adc: 2044 eqep: 0 gpio: 6f duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 221000020 sample: 220 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 222000018 sample: 221 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 223000018 sample: 222 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 224000018 sample: 223 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 225000021 sample: 224 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 226000020 sample: 225 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 227000020 sample: 226 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 228000020 sample: 227 adc: 2048 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 229000020 sample: 228 adc: 2046 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 230000020 sample: 229 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
cycle: 231000020 sample: 230 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 207868347 last-cmd: 200376618
Sleeping for 10ms, time=
Main CPU time:     1.338194959 (since last call:     0.177810834) 
Awake! time=
Main CPU time:     1.348578959 (since last call:     0.010384000) 
Setting "CPU Wants" bit, time=
Main CPU time:     1.348752917 (since last call:     0.000173958) 
Waiting for "Data ready" bit, time=
Main CPU time:     1.348872834 (since last call:     0.000119917) 
Got it! time=
Main CPU time:     1.348986709 (since last call:     0.000113875) 
Copy over the new data buf, time=
Main CPU time:     1.349183709 (since last call:     0.000197000) 
Done! time=
Main CPU time:     1.349411334 (since last call:     0.000227625) 
Printing Data buffer (iter 3) time=:
Main CPU time:     1.349518917 (since last call:     0.000107583) 
cycle: 238000020 sample: 237 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 239000020 sample: 238 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 240000020 sample: 239 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 241000020 sample: 240 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 242000020 sample: 241 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 243000020 sample: 242 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 244000020 sample: 243 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 245000020 sample: 244 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 246000020 sample: 245 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 247000020 sample: 246 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 248000020 sample: 247 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 249000020 sample: 248 adc: 2048 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 250000020 sample: 249 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 251000020 sample: 250 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 252000020 sample: 251 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 253000020 sample: 252 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 254000020 sample: 253 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 255000020 sample: 254 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 256000020 sample: 255 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 257000023 sample: 256 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 258000022 sample: 257 adc: 2048 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 259000022 sample: 258 adc: 2046 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 260000022 sample: 259 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 261000022 sample: 260 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 262000022 sample: 261 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 263000022 sample: 262 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 264000022 sample: 263 adc: 2047 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 265000022 sample: 264 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 266000022 sample: 265 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 267000022 sample: 266 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 268000022 sample: 267 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
cycle: 269000022 sample: 268 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 231842465 last-cmd: 200376618
Sleeping for 10ms, time=
Main CPU time:     1.556971875 (since last call:     0.207452958) 
Awake! time=
Main CPU time:     1.567352334 (since last call:     0.010380459) 
Setting "CPU Wants" bit, time=
Main CPU time:     1.567535292 (since last call:     0.000182958) 
Waiting for "Data ready" bit, time=
Main CPU time:     1.567654834 (since last call:     0.000119542) 
Got it! time=
Main CPU time:     1.567767500 (since last call:     0.000112666) 
Copy over the new data buf, time=
Main CPU time:     1.567961667 (since last call:     0.000194167) 
Done! time=
Main CPU time:     1.568186375 (since last call:     0.000224708) 
Printing Data buffer (iter 4) time=:
Main CPU time:     1.568295542 (since last call:     0.000109167) 
cycle: 282000022 sample: 281 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 283000022 sample: 282 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 284000022 sample: 283 adc: 2048 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 285000022 sample: 284 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 286000022 sample: 285 adc: 2046 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 287000022 sample: 286 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 288000022 sample: 287 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 289000025 sample: 288 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 290000024 sample: 289 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 291000024 sample: 290 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 292000024 sample: 291 adc: 2043 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 293000024 sample: 292 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 294000024 sample: 293 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 295000024 sample: 294 adc: 2046 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 296000024 sample: 295 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 297000024 sample: 296 adc: 2046 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 298000024 sample: 297 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 299000024 sample: 298 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 300000024 sample: 299 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 301000024 sample: 300 adc: 2046 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 302000024 sample: 301 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 303000024 sample: 302 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 304000024 sample: 303 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 305000024 sample: 304 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 306000024 sample: 305 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 307000024 sample: 306 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 308000024 sample: 307 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 309000024 sample: 308 adc: 2044 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 310000024 sample: 309 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 311000024 sample: 310 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 312000024 sample: 311 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
cycle: 313000024 sample: 312 adc: 2045 eqep: 0 gpio: 48 duty: 100 last-data-req: 269651027 last-cmd: 200376618
Sleeping for 10ms, time=
Main CPU time:     1.767765834 (since last call:     0.199470292) 
Awake! time=
Main CPU time:     1.778142334 (since last call:     0.010376500) 
Shutting down.
root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/01-adc-eqep-gpio# 

