
I am
/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence/readme.txt


sep 21, 2016


changes

X DANGEROUS but useful: new get_data_cmd_bufs(): like get_data_buf,
  PRU copies the PRU data buf into the CPU data buf, BUT ALSO copies
  the PRU cmd buf into the CPU cmd buf. Because the CPU is frozen
  while waiting for pru to say ok, cpu won't be reading / writing to
  the cmd buf, so safe. This allows us to see what buffer the PRU is working
  on.
    - NOTE: To do this quickly, I changed .p to ALWAYS copy PRU cmd buf into CPU cmd buf each timestep. (see DELIVER_TO_CPU)

    - OR: get_cmd_buf(): retrieve the PRU's current cmd buffer. WARNING: TODO. How to do this easily and safely?


X need a way for C code to learn what samp number we're on, so it can use that to specify the next actuation sequence 
  - using the last samp in the data buff is clunky and requires copyting a shitload of data
  - add to ctrl regs in pru data ram: u32 for sample number. At each samp time, pru writes this. 
    - Also need a 'lock' int that pru sets before updating it, and the cpu sets when reading it. 
      - if pru goes to write sample num but it's locked, gives up.

X sample time is now dynamically set by C program, not hard-coded into .hp file. (How?)
  - need C code to be able to set smaple period (in real time? prob not, but would be cool.)
    - easiest: each fast-iter, pru checks the 'new samp period' reg. if set, it reads the samp period reg into its samp period and resets the 'new samp period' reg.
      - cpu writes the new samp period, then sets the 'new period' reg. 

X when cmd buf comes, copy whole thing in. no throw away old pkts.

X when time to actuate, scan cmd buf for the current sample number and execute the first cmd pkt with that sample num.

X when it's a sample time, if cucle count > some big #, reset it. avoids cycle counter hitting ceiling.
    - Q: can configure cc to wrap around automatically? Is this even desirable?




X need a way to actuate immediately, not at specific sample time. secret way to bypass sample number thing.
    - if sample num is 0xffffffff, execute that sample immediately. 
      - note: because we just search thru the cmd buf from the beginning each sample time, this sample will get executed over and over, until you replace it with a new one.
    - Set that sample num to 0 so that you don't execute it again? Or, just let it execute over and over, until another cmd overwrites it?
      - if sample num is 0xfffffffe, execute that sample once.

X set_cmd_buf waits for ack from PRU before continuing. 
   - I checked this, it seems that the CPU was already doing this.

- need better way for CPU to wait (no busy-loop). how??







testname=test01-set-sample-period
testname=test02-get-sample-time
testname=test03-get-cmd-buf
testname=test04-cycle-count-reset
testname=test05-magic-cmd-pkts
testname=test06-reconstruct-cmd-seq



////////////////////////////////////////////////////////////////////////////////

test01-set-sample-period

- by default pru runs at 5ms/timestpe <=> sample period = 1e6. 
- this prog changes sample period. you set it from the cmd line when you run the program.

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# ./run b
I'm in: 
/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence
Resetting eqep...
building...
==========================
building PRU C library...
------------------------
Using pasm to assemble .p file jpp-pru-lib.p into .bin file ...


PRU Assembler Version 0.84
Copyright (C) 2005-2013 by Texas Instruments Inc.


Pass 2 : 0 Error(s), 0 Warning(s)

Writing Code Image of 396 word(s)

~~~~~~~~~~~~~~~~~
Compiling .c file jpp-pru-lib.c into .o file jpp-pru-lib.o
~~~~~~~~~~~~~~~~~~~~~~~~~~
Building test .c file test01-set-sample-period.c to make test01-set-sample-period
Fri Sep 23 10:12:18 UTC 2016
Fri Sep 23 10:12:19 UTC 2016
you gave me 'b', so I'll just build. Run it yourself: ./test01-set-sample-period or chrt 20 ./test01-set-sample-period
root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# ./test01-set-sample-period 500000
You gave me 2 args
Arg #0 = ./test01-set-sample-period
Arg #1 = 500000
Sample period cmd: 500000
Starting PRU with default period (1000000).
Welcome to the Beaglebone PRU library for real-time controls!
NOTE: if you get a seg fault, make sure the jppprugpio device tree overlay has been loaded!!
Here's the cmd: root@beaglebone# echo jppprugpio > /sys/devices/bone_capemgr.9/slots
Assumes you have jppprugpio-00A0.dto in /lib/firmware
Also do this:
echo bone_eqep1 > $SLOTS
prussdrv_init() (returned 0)
prussdrv_open (returned 0)
prussdrv_pruintc_init (returned 0)
pruss_map_prumem (returned 0)
pru0 data memory addr: 0xb6c0f000
 ctrladdr_CPU_wants_data : 0xb6c0f000
 ctrladdr_PRU_data_ready : 0xb6c0f004
 ctrladdr_CPU_new_sched  : 0xb6c0f008
 ctrladdr_PRU_ack_sched  : 0xb6c0f00c
 ctrladdr_CPU_new_period    : 0xb6c0f010
 ctrladdr_SAMPLE_PERIOD_CMD : 0xb6c0f014
 ctrladdr_SAMPLE_NUM_LOCK   : 0xb6c0f018
 ctrladdr_SAMPLE_NUM        : 0xb6c0f01c
 addr_cpu_data_buf : 0xb6c0f020
 addr_pru_data_buf : 0xb6c0f420
 addr_cpu_cmd_buf  : 0xb6c0f820
 addr_pru_cmd_buf  : 0xb6c0fa20
executing bin file: ./jpp-pru-lib.bin
NOTE: a lot of stuff assumes PRU0 in this lib, sry.
prussdrv_exec_program returned 0
Waiting 1000ms to let PRU fill its internal buffer...done.
cycle:  169000021 sample:    170 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  170000028 sample:    171 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  171000034 sample:    172 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  172000021 sample:    173 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  173000028 sample:    174 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  174000034 sample:    175 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  175000021 sample:    176 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  176000028 sample:    177 adc:  1800 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  177000034 sample:    178 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  178000021 sample:    179 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  179000028 sample:    180 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  180000034 sample:    181 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  181000021 sample:    182 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000028 sample:    183 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000034 sample:    184 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000021 sample:    185 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000028 sample:    186 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000034 sample:    187 adc:  1796 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000021 sample:    188 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000028 sample:    189 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000034 sample:    190 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000021 sample:    191 adc:  1806 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000028 sample:    192 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000034 sample:    193 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000024 sample:    194 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000030 sample:    195 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000017 sample:    196 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000024 sample:    197 adc:  1800 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000030 sample:    198 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000017 sample:    199 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000024 sample:    200 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000030 sample:    201 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
Setting period to 500000...
cycle:  182000028 sample:    183 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000034 sample:    184 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000021 sample:    185 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000028 sample:    186 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000034 sample:    187 adc:  1796 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000021 sample:    188 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000028 sample:    189 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000034 sample:    190 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000021 sample:    191 adc:  1806 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000028 sample:    192 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000034 sample:    193 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000024 sample:    194 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000030 sample:    195 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000017 sample:    196 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000024 sample:    197 adc:  1800 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000030 sample:    198 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000017 sample:    199 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000024 sample:    200 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000030 sample:    201 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000034 sample:    202 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:     500029 sample:    203 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    1000031 sample:    204 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    1500033 sample:    205 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    2000016 sample:    206 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    2500019 sample:    207 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    3000021 sample:    208 adc:  1806 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    3500023 sample:    209 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    4000025 sample:    210 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    4500027 sample:    211 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    5000029 sample:    212 adc:  1829 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    5500031 sample:    213 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
cycle:    6000033 sample:    214 adc:  1828 eqep:     0 gpio:    0 duty:   333 last-data-req:  200541370 last-cmd:          0
root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# 






//////////////////////////////////////////////////



testname=test02-get-sample-num

- exercises the new function unsigned int get_sample_num().

The meat of the code:

  for( int i=0; i<4; i++ ) {
  sn_preget = get_sample_num();
  get_data_buf( &db );
  sn_preprint = get_sample_num();
  print_data_buf( db );
  sn_postprint = get_sample_num();
  printf("preget: %u preprint: %u postprint: %u\n",sn_preget, sn_preprint, sn_postprint);
  usleep(30000); // wait a few ticks so that the next db show the change 

Example output w/ interpretation:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# ./run
I'm in: 
/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence
Resetting eqep...
building...
==========================
building PRU C library...
------------------------
Using pasm to assemble .p file jpp-pru-lib.p into .bin file ...


PRU Assembler Version 0.84
Copyright (C) 2005-2013 by Texas Instruments Inc.


Pass 2 : 0 Error(s), 0 Warning(s)

Writing Code Image of 396 word(s)

~~~~~~~~~~~~~~~~~
Compiling .c file jpp-pru-lib.c into .o file jpp-pru-lib.o
~~~~~~~~~~~~~~~~~~~~~~~~~~
Building test .c file test02-get-sample-num.c to make test02-get-sample-num
Fri Sep 23 10:49:57 UTC 2016
Fri Sep 23 10:49:58 UTC 2016
Just running test02-get-sample-num ...
Running test02-get-sample-num at realtime priority:
Welcome to the Beaglebone PRU library for real-time controls!
NOTE: if you get a seg fault, make sure the jppprugpio device tree overlay has been loaded!!
Here's the cmd: root@beaglebone# echo jppprugpio > /sys/devices/bone_capemgr.9/slots
Assumes you have jppprugpio-00A0.dto in /lib/firmware
Also do this:
echo bone_eqep1 > $SLOTS
prussdrv_init() (returned 0)
prussdrv_open (returned 0)
prussdrv_pruintc_init (returned 0)
pruss_map_prumem (returned 0)
pru0 data memory addr: 0xb6c1d000
 ctrladdr_CPU_wants_data : 0xb6c1d000
 ctrladdr_PRU_data_ready : 0xb6c1d004
 ctrladdr_CPU_new_sched  : 0xb6c1d008
 ctrladdr_PRU_ack_sched  : 0xb6c1d00c
 ctrladdr_CPU_new_period    : 0xb6c1d010
 ctrladdr_SAMPLE_PERIOD_CMD : 0xb6c1d014
 ctrladdr_SAMPLE_NUM_LOCK   : 0xb6c1d018
 ctrladdr_SAMPLE_NUM        : 0xb6c1d01c
 addr_cpu_data_buf : 0xb6c1d020
 addr_pru_data_buf : 0xb6c1d420
 addr_cpu_cmd_buf  : 0xb6c1d820
 addr_pru_cmd_buf  : 0xb6c1da20
executing bin file: ./jpp-pru-lib.bin
NOTE: a lot of stuff assumes PRU0 in this lib, sry.
prussdrv_exec_program returned 0
Waiting 1000ms to let PRU fill its internal buffer...done.
cycle:  169000030 sample:    170 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  170000036 sample:    171 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  171000023 sample:    172 adc:  1806 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  172000030 sample:    173 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  173000036 sample:    174 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  174000023 sample:    175 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  175000030 sample:    176 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  176000036 sample:    177 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  177000023 sample:    178 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  178000030 sample:    179 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  179000036 sample:    180 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  180000023 sample:    181 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  181000030 sample:    182 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000036 sample:    183 adc:  1829 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000023 sample:    184 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000030 sample:    185 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000036 sample:    186 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000023 sample:    187 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000030 sample:    188 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1833 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
preget: 201 preprint: 201 postprint: 201

- The sample num updates first thing on seeing cycle count increment beyond cycles_at_next_sample_time.
- So sn = 201 in register, then data pkt is appended to pru buf. This all happens w/in a few us. 
- After that, there's a long period of time where the last pkt in teh buf is 201, and the sn in the reg is 201.
- That's why it's 201 and that matches last pkt in buf.

cycle:  175000030 sample:    176 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  176000036 sample:    177 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  177000023 sample:    178 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  178000030 sample:    179 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  179000036 sample:    180 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  180000023 sample:    181 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  181000030 sample:    182 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000036 sample:    183 adc:  1829 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000023 sample:    184 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000030 sample:    185 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000036 sample:    186 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000023 sample:    187 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000030 sample:    188 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1833 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000025 sample:    202 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  202000032 sample:    203 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  203000038 sample:    204 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  204000025 sample:    205 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  205000032 sample:    206 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  206000038 sample:    207 adc:  1826 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
preget: 207 preprint: 207 postprint: 207

- same as 201.

cycle:  181000030 sample:    182 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000036 sample:    183 adc:  1829 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000023 sample:    184 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000030 sample:    185 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000036 sample:    186 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000023 sample:    187 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000030 sample:    188 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1833 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000025 sample:    202 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  202000032 sample:    203 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  203000038 sample:    204 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  204000025 sample:    205 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  205000032 sample:    206 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  206000038 sample:    207 adc:  1826 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  207000023 sample:    208 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  208000030 sample:    209 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  209000036 sample:    210 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  210000023 sample:    211 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  211000030 sample:    212 adc:  1805 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  212000036 sample:    213 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
preget: 213 preprint: 213 postprint: 214

here, printing took long enough that the sn advanced while printing, but of course the data buf was gathered at a time when 213 was last sn.


cycle:  188000036 sample:    189 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1833 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000025 sample:    202 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  202000032 sample:    203 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  203000038 sample:    204 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  204000025 sample:    205 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  205000032 sample:    206 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  206000038 sample:    207 adc:  1826 eqep:     0 gpio:    0 duty:   333 last-data-req:  200063770 last-cmd:          0
cycle:  207000023 sample:    208 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  208000030 sample:    209 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  209000036 sample:    210 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  210000023 sample:    211 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  211000030 sample:    212 adc:  1805 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  212000036 sample:    213 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  206523549 last-cmd:          0
cycle:  213000040 sample:    214 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
cycle:  214000027 sample:    215 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
cycle:  215000034 sample:    216 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
cycle:  216000040 sample:    217 adc:  1828 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
cycle:  217000027 sample:    218 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
cycle:  218000034 sample:    219 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
cycle:  219000040 sample:    220 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  212963606 last-cmd:          0
preget: 220 preprint: 220 postprint: 261

- Here the printing took so long that the sn is way later by the time we're done printing.





//////////////////////////////////////////////////


#testname=test03-get-data-cmd-bufs-at-once


- here we're testing the ability to retrieve the data and cmd bufs at the same time.
- seems to work.
- hm, iter 219 is repeated... why?
- Q: still works as expecetd if several cpu cmd bufs come in a single sample tiem?

Example output:


root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# ./run
I'm in: 
/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence
Resetting eqep...
building...
==========================
building PRU C library...
------------------------
Using pasm to assemble .p file jpp-pru-lib.p into .bin file ...


PRU Assembler Version 0.84
Copyright (C) 2005-2013 by Texas Instruments Inc.


Pass 2 : 0 Error(s), 0 Warning(s)

Writing Code Image of 396 word(s)

~~~~~~~~~~~~~~~~~
Compiling .c file jpp-pru-lib.c into .o file jpp-pru-lib.o
~~~~~~~~~~~~~~~~~~~~~~~~~~
Building test .c file test03-get-data-cmd-bufs-at-once.c to make test03-get-data-cmd-bufs-at-once
Fri Sep 23 12:28:46 UTC 2016
Fri Sep 23 12:28:47 UTC 2016
Just running test03-get-data-cmd-bufs-at-once ...
Running test03-get-data-cmd-bufs-at-once at realtime priority:
Welcome to the Beaglebone PRU library for real-time controls!
NOTE: if you get a seg fault, make sure the jppprugpio device tree overlay has been loaded!!
Here's the cmd: root@beaglebone# echo jppprugpio > /sys/devices/bone_capemgr.9/slots
Assumes you have jppprugpio-00A0.dto in /lib/firmware
Also do this:
echo bone_eqep1 > $SLOTS
prussdrv_init() (returned 0)
prussdrv_open (returned 0)
prussdrv_pruintc_init (returned 0)
pruss_map_prumem (returned 0)
pru0 data memory addr: 0xb6cea000
 ctrladdr_CPU_wants_data : 0xb6cea000
 ctrladdr_PRU_data_ready : 0xb6cea004
 ctrladdr_CPU_new_sched  : 0xb6cea008
 ctrladdr_PRU_ack_sched  : 0xb6cea00c
 ctrladdr_CPU_new_period    : 0xb6cea010
 ctrladdr_SAMPLE_PERIOD_CMD : 0xb6cea014
 ctrladdr_SAMPLE_NUM_LOCK   : 0xb6cea018
 ctrladdr_SAMPLE_NUM        : 0xb6cea01c
 addr_cpu_data_buf : 0xb6cea020
 addr_pru_data_buf : 0xb6cea420
 addr_cpu_cmd_buf  : 0xb6cea820
 addr_pru_cmd_buf  : 0xb6ceaa20
executing bin file: ./jpp-pru-lib.bin
NOTE: a lot of stuff assumes PRU0 in this lib, sry.
prussdrv_exec_program returned 0
Waiting 1000ms to let PRU fill its internal buffer...done.

i=0:
Data:
cycle:  169000030 sample:    170 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  170000036 sample:    171 adc:  1840 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  171000023 sample:    172 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  172000030 sample:    173 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  173000036 sample:    174 adc:  1806 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  174000023 sample:    175 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  175000030 sample:    176 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  176000036 sample:    177 adc:  1832 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  177000023 sample:    178 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  178000030 sample:    179 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  179000036 sample:    180 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  180000023 sample:    181 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  181000030 sample:    182 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000036 sample:    183 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000023 sample:    184 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000030 sample:    185 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000036 sample:    186 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000023 sample:    187 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000030 sample:    188 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
Cmd:
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0
cycle:          0 sample:      0 gpio:    0 duty:     0

i=1:
Data:
cycle:  175000030 sample:    176 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  176000036 sample:    177 adc:  1832 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  177000023 sample:    178 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  178000030 sample:    179 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  179000036 sample:    180 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  180000023 sample:    181 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  181000030 sample:    182 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000036 sample:    183 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000023 sample:    184 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000030 sample:    185 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000036 sample:    186 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000023 sample:    187 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000030 sample:    188 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000039 sample:    202 adc:  1820 eqep:     0 gpio:    0 duty:   202 last-data-req:  200060844 last-cmd:  200113500
cycle:  202000032 sample:    203 adc:  1811 eqep:     0 gpio:    0 duty:   203 last-data-req:  200060844 last-cmd:  200113500
cycle:  203000038 sample:    204 adc:  1804 eqep:     0 gpio:    0 duty:   204 last-data-req:  200060844 last-cmd:  200113500
cycle:  204000037 sample:    205 adc:  1815 eqep:     0 gpio:    0 duty:   205 last-data-req:  200060844 last-cmd:  200113500
cycle:  205000024 sample:    206 adc:  1827 eqep:     0 gpio:    0 duty:   206 last-data-req:  200060844 last-cmd:  200113500
cycle:  206000041 sample:    207 adc:  1807 eqep:     0 gpio:    0 duty:   207 last-data-req:  200060844 last-cmd:  200113500
Cmd:
cycle:  200113500 sample:    201 gpio:    0 duty:   201
cycle:  200113500 sample:    202 gpio:    0 duty:   202
cycle:  200113500 sample:    203 gpio:    0 duty:   203
cycle:  200113500 sample:    204 gpio:    0 duty:   204
cycle:  200113500 sample:    205 gpio:    0 duty:   205
cycle:  200113500 sample:    206 gpio:    0 duty:   206
cycle:  200113500 sample:    207 gpio:    0 duty:   207
cycle:  200113500 sample:    208 gpio:    0 duty:   208
cycle:  200113500 sample:    209 gpio:    0 duty:   209
cycle:  200113500 sample:    210 gpio:    0 duty:   210
cycle:  200113500 sample:    211 gpio:    0 duty:   211
cycle:  200113500 sample:    212 gpio:    0 duty:   212
cycle:  200113500 sample:    213 gpio:    0 duty:   213
cycle:  200113500 sample:    214 gpio:    0 duty:   214
cycle:  200113500 sample:    215 gpio:    0 duty:   215
cycle:  200113500 sample:    216 gpio:    0 duty:   216
cycle:  200113500 sample:    217 gpio:    0 duty:   217
cycle:  200113500 sample:    218 gpio:    0 duty:   218
cycle:  200113500 sample:    219 gpio:    0 duty:   219
cycle:  200113500 sample:    220 gpio:    0 duty:   220
cycle:  200113500 sample:    221 gpio:    0 duty:   221
cycle:  200113500 sample:    222 gpio:    0 duty:   222
cycle:  200113500 sample:    223 gpio:    0 duty:   223
cycle:  200113500 sample:    224 gpio:    0 duty:   224
cycle:  200113500 sample:    225 gpio:    0 duty:   225
cycle:  200113500 sample:    226 gpio:    0 duty:   226
cycle:  200113500 sample:    227 gpio:    0 duty:   227
cycle:  200113500 sample:    228 gpio:    0 duty:   228
cycle:  200113500 sample:    229 gpio:    0 duty:   229
cycle:  200113500 sample:    230 gpio:    0 duty:   230
cycle:  200113500 sample:    231 gpio:    0 duty:   231
cycle:  200113500 sample:    232 gpio:    0 duty:   232

i=2:
Data:
cycle:  181000030 sample:    182 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  182000036 sample:    183 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  183000023 sample:    184 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  184000030 sample:    185 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  185000036 sample:    186 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  186000023 sample:    187 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  187000030 sample:    188 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000039 sample:    202 adc:  1820 eqep:     0 gpio:    0 duty:   202 last-data-req:  200060844 last-cmd:  200113500
cycle:  202000032 sample:    203 adc:  1811 eqep:     0 gpio:    0 duty:   203 last-data-req:  200060844 last-cmd:  200113500
cycle:  203000038 sample:    204 adc:  1804 eqep:     0 gpio:    0 duty:   204 last-data-req:  200060844 last-cmd:  200113500
cycle:  204000037 sample:    205 adc:  1815 eqep:     0 gpio:    0 duty:   205 last-data-req:  200060844 last-cmd:  200113500
cycle:  205000024 sample:    206 adc:  1827 eqep:     0 gpio:    0 duty:   206 last-data-req:  200060844 last-cmd:  200113500
cycle:  206000041 sample:    207 adc:  1807 eqep:     0 gpio:    0 duty:   207 last-data-req:  200060844 last-cmd:  200113500
cycle:  207000028 sample:    208 adc:  1815 eqep:     0 gpio:    0 duty:   208 last-data-req:  206148214 last-cmd:  206189869
cycle:  208000039 sample:    209 adc:  1827 eqep:     0 gpio:    0 duty:   209 last-data-req:  206148214 last-cmd:  206189869
cycle:  209000027 sample:    210 adc:  1799 eqep:     0 gpio:    0 duty:   210 last-data-req:  206148214 last-cmd:  206189869
cycle:  210000027 sample:    211 adc:  1820 eqep:     0 gpio:    0 duty:   211 last-data-req:  206148214 last-cmd:  206189869
cycle:  211000033 sample:    212 adc:  1817 eqep:     0 gpio:    0 duty:   212 last-data-req:  206148214 last-cmd:  206189869
cycle:  212000032 sample:    213 adc:  1806 eqep:     0 gpio:    0 duty:   213 last-data-req:  206148214 last-cmd:  206189869
Cmd:
cycle:  206189869 sample:    207 gpio:    0 duty:   207
cycle:  206189869 sample:    208 gpio:    0 duty:   208
cycle:  206189869 sample:    209 gpio:    0 duty:   209
cycle:  206189869 sample:    210 gpio:    0 duty:   210
cycle:  206189869 sample:    211 gpio:    0 duty:   211
cycle:  206189869 sample:    212 gpio:    0 duty:   212
cycle:  206189869 sample:    213 gpio:    0 duty:   213
cycle:  206189869 sample:    214 gpio:    0 duty:   214
cycle:  206189869 sample:    215 gpio:    0 duty:   215
cycle:  206189869 sample:    216 gpio:    0 duty:   216
cycle:  206189869 sample:    217 gpio:    0 duty:   217
cycle:  206189869 sample:    218 gpio:    0 duty:   218
cycle:  206189869 sample:    219 gpio:    0 duty:   219
cycle:  206189869 sample:    220 gpio:    0 duty:   220
cycle:  206189869 sample:    221 gpio:    0 duty:   221
cycle:  206189869 sample:    222 gpio:    0 duty:   222
cycle:  206189869 sample:    223 gpio:    0 duty:   223
cycle:  206189869 sample:    224 gpio:    0 duty:   224
cycle:  206189869 sample:    225 gpio:    0 duty:   225
cycle:  206189869 sample:    226 gpio:    0 duty:   226
cycle:  206189869 sample:    227 gpio:    0 duty:   227
cycle:  206189869 sample:    228 gpio:    0 duty:   228
cycle:  206189869 sample:    229 gpio:    0 duty:   229
cycle:  206189869 sample:    230 gpio:    0 duty:   230
cycle:  206189869 sample:    231 gpio:    0 duty:   231
cycle:  206189869 sample:    232 gpio:    0 duty:   232
cycle:  206189869 sample:    233 gpio:    0 duty:   233
cycle:  206189869 sample:    234 gpio:    0 duty:   234
cycle:  206189869 sample:    235 gpio:    0 duty:   235
cycle:  206189869 sample:    236 gpio:    0 duty:   236
cycle:  206189869 sample:    237 gpio:    0 duty:   237
cycle:  206189869 sample:    238 gpio:    0 duty:   238

i=3:
Data:
cycle:  187000030 sample:    188 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000039 sample:    202 adc:  1820 eqep:     0 gpio:    0 duty:   202 last-data-req:  200060844 last-cmd:  200113500
cycle:  202000032 sample:    203 adc:  1811 eqep:     0 gpio:    0 duty:   203 last-data-req:  200060844 last-cmd:  200113500
cycle:  203000038 sample:    204 adc:  1804 eqep:     0 gpio:    0 duty:   204 last-data-req:  200060844 last-cmd:  200113500
cycle:  204000037 sample:    205 adc:  1815 eqep:     0 gpio:    0 duty:   205 last-data-req:  200060844 last-cmd:  200113500
cycle:  205000024 sample:    206 adc:  1827 eqep:     0 gpio:    0 duty:   206 last-data-req:  200060844 last-cmd:  200113500
cycle:  206000041 sample:    207 adc:  1807 eqep:     0 gpio:    0 duty:   207 last-data-req:  200060844 last-cmd:  200113500
cycle:  207000028 sample:    208 adc:  1815 eqep:     0 gpio:    0 duty:   208 last-data-req:  206148214 last-cmd:  206189869
cycle:  208000039 sample:    209 adc:  1827 eqep:     0 gpio:    0 duty:   209 last-data-req:  206148214 last-cmd:  206189869
cycle:  209000027 sample:    210 adc:  1799 eqep:     0 gpio:    0 duty:   210 last-data-req:  206148214 last-cmd:  206189869
cycle:  210000027 sample:    211 adc:  1820 eqep:     0 gpio:    0 duty:   211 last-data-req:  206148214 last-cmd:  206189869
cycle:  211000033 sample:    212 adc:  1817 eqep:     0 gpio:    0 duty:   212 last-data-req:  206148214 last-cmd:  206189869
cycle:  212000032 sample:    213 adc:  1806 eqep:     0 gpio:    0 duty:   213 last-data-req:  206148214 last-cmd:  206189869
cycle:  213000037 sample:    214 adc:  1819 eqep:     0 gpio:    0 duty:   214 last-data-req:  212218219 last-cmd:  212268538
cycle:  214000030 sample:    215 adc:  1820 eqep:     0 gpio:    0 duty:   215 last-data-req:  212218219 last-cmd:  212268538
cycle:  215000036 sample:    216 adc:  1807 eqep:     0 gpio:    0 duty:   216 last-data-req:  212218219 last-cmd:  212268538
cycle:  216000035 sample:    217 adc:  1801 eqep:     0 gpio:    0 duty:   217 last-data-req:  212218219 last-cmd:  212268538
cycle:  217000041 sample:    218 adc:  1819 eqep:     0 gpio:    0 duty:   218 last-data-req:  212218219 last-cmd:  212268538
cycle:  218000040 sample:    219 adc:  1806 eqep:     0 gpio:    0 duty:   219 last-data-req:  212218219 last-cmd:  212268538
Cmd:
cycle:  212268538 sample:    213 gpio:    0 duty:   213
cycle:  212268538 sample:    214 gpio:    0 duty:   214
cycle:  212268538 sample:    215 gpio:    0 duty:   215
cycle:  212268538 sample:    216 gpio:    0 duty:   216
cycle:  212268538 sample:    217 gpio:    0 duty:   217
cycle:  212268538 sample:    218 gpio:    0 duty:   218
cycle:  212268538 sample:    219 gpio:    0 duty:   219
cycle:  212268538 sample:    220 gpio:    0 duty:   220
cycle:  212268538 sample:    221 gpio:    0 duty:   221
cycle:  212268538 sample:    222 gpio:    0 duty:   222
cycle:  212268538 sample:    223 gpio:    0 duty:   223
cycle:  212268538 sample:    224 gpio:    0 duty:   224
cycle:  212268538 sample:    225 gpio:    0 duty:   225
cycle:  212268538 sample:    226 gpio:    0 duty:   226
cycle:  212268538 sample:    227 gpio:    0 duty:   227
cycle:  212268538 sample:    228 gpio:    0 duty:   228
cycle:  212268538 sample:    229 gpio:    0 duty:   229
cycle:  212268538 sample:    230 gpio:    0 duty:   230
cycle:  212268538 sample:    231 gpio:    0 duty:   231
cycle:  212268538 sample:    232 gpio:    0 duty:   232
cycle:  212268538 sample:    233 gpio:    0 duty:   233
cycle:  212268538 sample:    234 gpio:    0 duty:   234
cycle:  212268538 sample:    235 gpio:    0 duty:   235
cycle:  212268538 sample:    236 gpio:    0 duty:   236
cycle:  212268538 sample:    237 gpio:    0 duty:   237
cycle:  212268538 sample:    238 gpio:    0 duty:   238
cycle:  212268538 sample:    239 gpio:    0 duty:   239
cycle:  212268538 sample:    240 gpio:    0 duty:   240
cycle:  212268538 sample:    241 gpio:    0 duty:   241
cycle:  212268538 sample:    242 gpio:    0 duty:   242
cycle:  212268538 sample:    243 gpio:    0 duty:   243
cycle:  212268538 sample:    244 gpio:    0 duty:   244

i=4:
Data:
cycle:  187000030 sample:    188 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  188000036 sample:    189 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  189000023 sample:    190 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  190000030 sample:    191 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  191000036 sample:    192 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  192000023 sample:    193 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  193000033 sample:    194 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  194000040 sample:    195 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  195000027 sample:    196 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  196000034 sample:    197 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  197000040 sample:    198 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  198000027 sample:    199 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  199000034 sample:    200 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000039 sample:    202 adc:  1820 eqep:     0 gpio:    0 duty:   202 last-data-req:  200060844 last-cmd:  200113500
cycle:  202000032 sample:    203 adc:  1811 eqep:     0 gpio:    0 duty:   203 last-data-req:  200060844 last-cmd:  200113500
cycle:  203000038 sample:    204 adc:  1804 eqep:     0 gpio:    0 duty:   204 last-data-req:  200060844 last-cmd:  200113500
cycle:  204000037 sample:    205 adc:  1815 eqep:     0 gpio:    0 duty:   205 last-data-req:  200060844 last-cmd:  200113500
cycle:  205000024 sample:    206 adc:  1827 eqep:     0 gpio:    0 duty:   206 last-data-req:  200060844 last-cmd:  200113500
cycle:  206000041 sample:    207 adc:  1807 eqep:     0 gpio:    0 duty:   207 last-data-req:  200060844 last-cmd:  200113500
cycle:  207000028 sample:    208 adc:  1815 eqep:     0 gpio:    0 duty:   208 last-data-req:  206148214 last-cmd:  206189869
cycle:  208000039 sample:    209 adc:  1827 eqep:     0 gpio:    0 duty:   209 last-data-req:  206148214 last-cmd:  206189869
cycle:  209000027 sample:    210 adc:  1799 eqep:     0 gpio:    0 duty:   210 last-data-req:  206148214 last-cmd:  206189869
cycle:  210000027 sample:    211 adc:  1820 eqep:     0 gpio:    0 duty:   211 last-data-req:  206148214 last-cmd:  206189869
cycle:  211000033 sample:    212 adc:  1817 eqep:     0 gpio:    0 duty:   212 last-data-req:  206148214 last-cmd:  206189869
cycle:  212000032 sample:    213 adc:  1806 eqep:     0 gpio:    0 duty:   213 last-data-req:  206148214 last-cmd:  206189869
cycle:  213000037 sample:    214 adc:  1819 eqep:     0 gpio:    0 duty:   214 last-data-req:  212218219 last-cmd:  212268538
cycle:  214000030 sample:    215 adc:  1820 eqep:     0 gpio:    0 duty:   215 last-data-req:  212218219 last-cmd:  212268538
cycle:  215000036 sample:    216 adc:  1807 eqep:     0 gpio:    0 duty:   216 last-data-req:  212218219 last-cmd:  212268538
cycle:  216000035 sample:    217 adc:  1801 eqep:     0 gpio:    0 duty:   217 last-data-req:  212218219 last-cmd:  212268538
cycle:  217000041 sample:    218 adc:  1819 eqep:     0 gpio:    0 duty:   218 last-data-req:  212218219 last-cmd:  212268538
cycle:  218000040 sample:    219 adc:  1806 eqep:     0 gpio:    0 duty:   219 last-data-req:  212218219 last-cmd:  212268538
Cmd:
cycle:  218342589 sample:    219 gpio:    0 duty:   219
cycle:  218342589 sample:    220 gpio:    0 duty:   220
cycle:  218342589 sample:    221 gpio:    0 duty:   221
cycle:  218342589 sample:    222 gpio:    0 duty:   222
cycle:  218342589 sample:    223 gpio:    0 duty:   223
cycle:  218342589 sample:    224 gpio:    0 duty:   224
cycle:  218342589 sample:    225 gpio:    0 duty:   225
cycle:  218342589 sample:    226 gpio:    0 duty:   226
cycle:  218342589 sample:    227 gpio:    0 duty:   227
cycle:  218342589 sample:    228 gpio:    0 duty:   228
cycle:  218342589 sample:    229 gpio:    0 duty:   229
cycle:  218342589 sample:    230 gpio:    0 duty:   230
cycle:  218342589 sample:    231 gpio:    0 duty:   231
cycle:  218342589 sample:    232 gpio:    0 duty:   232
cycle:  218342589 sample:    233 gpio:    0 duty:   233
cycle:  218342589 sample:    234 gpio:    0 duty:   234
cycle:  218342589 sample:    235 gpio:    0 duty:   235
cycle:  218342589 sample:    236 gpio:    0 duty:   236
cycle:  218342589 sample:    237 gpio:    0 duty:   237
cycle:  218342589 sample:    238 gpio:    0 duty:   238
cycle:  218342589 sample:    239 gpio:    0 duty:   239
cycle:  218342589 sample:    240 gpio:    0 duty:   240
cycle:  218342589 sample:    241 gpio:    0 duty:   241
cycle:  218342589 sample:    242 gpio:    0 duty:   242
cycle:  218342589 sample:    243 gpio:    0 duty:   243
cycle:  218342589 sample:    244 gpio:    0 duty:   244
cycle:  218342589 sample:    245 gpio:    0 duty:   245
cycle:  218342589 sample:    246 gpio:    0 duty:   246
cycle:  218342589 sample:    247 gpio:    0 duty:   247
cycle:  218342589 sample:    248 gpio:    0 duty:   248
cycle:  218342589 sample:    249 gpio:    0 duty:   249
cycle:  218342589 sample:    250 gpio:    0 duty:   250

i=5:
Data:
cycle:  199000034 sample:    200 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  200000040 sample:    201 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:  201000039 sample:    202 adc:  1820 eqep:     0 gpio:    0 duty:   202 last-data-req:  200060844 last-cmd:  200113500
cycle:  202000032 sample:    203 adc:  1811 eqep:     0 gpio:    0 duty:   203 last-data-req:  200060844 last-cmd:  200113500
cycle:  203000038 sample:    204 adc:  1804 eqep:     0 gpio:    0 duty:   204 last-data-req:  200060844 last-cmd:  200113500
cycle:  204000037 sample:    205 adc:  1815 eqep:     0 gpio:    0 duty:   205 last-data-req:  200060844 last-cmd:  200113500
cycle:  205000024 sample:    206 adc:  1827 eqep:     0 gpio:    0 duty:   206 last-data-req:  200060844 last-cmd:  200113500
cycle:  206000041 sample:    207 adc:  1807 eqep:     0 gpio:    0 duty:   207 last-data-req:  200060844 last-cmd:  200113500
cycle:  207000028 sample:    208 adc:  1815 eqep:     0 gpio:    0 duty:   208 last-data-req:  206148214 last-cmd:  206189869
cycle:  208000039 sample:    209 adc:  1827 eqep:     0 gpio:    0 duty:   209 last-data-req:  206148214 last-cmd:  206189869
cycle:  209000027 sample:    210 adc:  1799 eqep:     0 gpio:    0 duty:   210 last-data-req:  206148214 last-cmd:  206189869
cycle:  210000027 sample:    211 adc:  1820 eqep:     0 gpio:    0 duty:   211 last-data-req:  206148214 last-cmd:  206189869
cycle:  211000033 sample:    212 adc:  1817 eqep:     0 gpio:    0 duty:   212 last-data-req:  206148214 last-cmd:  206189869
cycle:  212000032 sample:    213 adc:  1806 eqep:     0 gpio:    0 duty:   213 last-data-req:  206148214 last-cmd:  206189869
cycle:  213000037 sample:    214 adc:  1819 eqep:     0 gpio:    0 duty:   214 last-data-req:  212218219 last-cmd:  212268538
cycle:  214000030 sample:    215 adc:  1820 eqep:     0 gpio:    0 duty:   215 last-data-req:  212218219 last-cmd:  212268538
cycle:  215000036 sample:    216 adc:  1807 eqep:     0 gpio:    0 duty:   216 last-data-req:  212218219 last-cmd:  212268538
cycle:  216000035 sample:    217 adc:  1801 eqep:     0 gpio:    0 duty:   217 last-data-req:  212218219 last-cmd:  212268538
cycle:  217000041 sample:    218 adc:  1819 eqep:     0 gpio:    0 duty:   218 last-data-req:  212218219 last-cmd:  212268538
cycle:  218000040 sample:    219 adc:  1806 eqep:     0 gpio:    0 duty:   219 last-data-req:  212218219 last-cmd:  212268538
cycle:  219000026 sample:    220 adc:  1804 eqep:     0 gpio:    0 duty:   220 last-data-req:  218298084 last-cmd:  218342589
cycle:  220000037 sample:    221 adc:  1825 eqep:     0 gpio:    0 duty:   221 last-data-req:  218298084 last-cmd:  218342589
cycle:  221000025 sample:    222 adc:  1807 eqep:     0 gpio:    0 duty:   222 last-data-req:  218298084 last-cmd:  218342589
cycle:  222000025 sample:    223 adc:  1821 eqep:     0 gpio:    0 duty:   223 last-data-req:  218298084 last-cmd:  218342589
cycle:  223000039 sample:    224 adc:  1801 eqep:     0 gpio:    0 duty:   224 last-data-req:  218298084 last-cmd:  218342589
cycle:  224000038 sample:    225 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  218298084 last-cmd:  218342589
cycle:  225000025 sample:    226 adc:  1815 eqep:     0 gpio:    0 duty:   226 last-data-req:  224371862 last-cmd:  224442452
cycle:  226000037 sample:    227 adc:  1820 eqep:     0 gpio:    0 duty:   227 last-data-req:  224371862 last-cmd:  224442452
cycle:  227000025 sample:    228 adc:  1809 eqep:     0 gpio:    0 duty:   228 last-data-req:  224371862 last-cmd:  224442452
cycle:  228000025 sample:    229 adc:  1820 eqep:     0 gpio:    0 duty:   229 last-data-req:  224371862 last-cmd:  224442452
cycle:  229000039 sample:    230 adc:  1835 eqep:     0 gpio:    0 duty:   230 last-data-req:  224371862 last-cmd:  224442452
cycle:  230000038 sample:    231 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  224371862 last-cmd:  224442452
Cmd:
cycle:  224442452 sample:    225 gpio:    0 duty:   225
cycle:  224442452 sample:    226 gpio:    0 duty:   226
cycle:  224442452 sample:    227 gpio:    0 duty:   227
cycle:  224442452 sample:    228 gpio:    0 duty:   228
cycle:  224442452 sample:    229 gpio:    0 duty:   229
cycle:  224442452 sample:    230 gpio:    0 duty:   230
cycle:  224442452 sample:    231 gpio:    0 duty:   231
cycle:  224442452 sample:    232 gpio:    0 duty:   232
cycle:  224442452 sample:    233 gpio:    0 duty:   233
cycle:  224442452 sample:    234 gpio:    0 duty:   234
cycle:  224442452 sample:    235 gpio:    0 duty:   235
cycle:  224442452 sample:    236 gpio:    0 duty:   236
cycle:  224442452 sample:    237 gpio:    0 duty:   237
cycle:  224442452 sample:    238 gpio:    0 duty:   238
cycle:  224442452 sample:    239 gpio:    0 duty:   239
cycle:  224442452 sample:    240 gpio:    0 duty:   240
cycle:  224442452 sample:    241 gpio:    0 duty:   241
cycle:  224442452 sample:    242 gpio:    0 duty:   242
cycle:  224442452 sample:    243 gpio:    0 duty:   243
cycle:  224442452 sample:    244 gpio:    0 duty:   244
cycle:  224442452 sample:    245 gpio:    0 duty:   245
cycle:  224442452 sample:    246 gpio:    0 duty:   246
cycle:  224442452 sample:    247 gpio:    0 duty:   247
cycle:  224442452 sample:    248 gpio:    0 duty:   248
cycle:  224442452 sample:    249 gpio:    0 duty:   249
cycle:  224442452 sample:    250 gpio:    0 duty:   250
cycle:  224442452 sample:    251 gpio:    0 duty:   251
cycle:  224442452 sample:    252 gpio:    0 duty:   252
cycle:  224442452 sample:    253 gpio:    0 duty:   253
cycle:  224442452 sample:    254 gpio:    0 duty:   254
cycle:  224442452 sample:    255 gpio:    0 duty:   255
cycle:  224442452 sample:    256 gpio:    0 duty:   256

i=6:
Data:
cycle:  205000024 sample:    206 adc:  1827 eqep:     0 gpio:    0 duty:   206 last-data-req:  200060844 last-cmd:  200113500
cycle:  206000041 sample:    207 adc:  1807 eqep:     0 gpio:    0 duty:   207 last-data-req:  200060844 last-cmd:  200113500
cycle:  207000028 sample:    208 adc:  1815 eqep:     0 gpio:    0 duty:   208 last-data-req:  206148214 last-cmd:  206189869
cycle:  208000039 sample:    209 adc:  1827 eqep:     0 gpio:    0 duty:   209 last-data-req:  206148214 last-cmd:  206189869
cycle:  209000027 sample:    210 adc:  1799 eqep:     0 gpio:    0 duty:   210 last-data-req:  206148214 last-cmd:  206189869
cycle:  210000027 sample:    211 adc:  1820 eqep:     0 gpio:    0 duty:   211 last-data-req:  206148214 last-cmd:  206189869
cycle:  211000033 sample:    212 adc:  1817 eqep:     0 gpio:    0 duty:   212 last-data-req:  206148214 last-cmd:  206189869
cycle:  212000032 sample:    213 adc:  1806 eqep:     0 gpio:    0 duty:   213 last-data-req:  206148214 last-cmd:  206189869
cycle:  213000037 sample:    214 adc:  1819 eqep:     0 gpio:    0 duty:   214 last-data-req:  212218219 last-cmd:  212268538
cycle:  214000030 sample:    215 adc:  1820 eqep:     0 gpio:    0 duty:   215 last-data-req:  212218219 last-cmd:  212268538
cycle:  215000036 sample:    216 adc:  1807 eqep:     0 gpio:    0 duty:   216 last-data-req:  212218219 last-cmd:  212268538
cycle:  216000035 sample:    217 adc:  1801 eqep:     0 gpio:    0 duty:   217 last-data-req:  212218219 last-cmd:  212268538
cycle:  217000041 sample:    218 adc:  1819 eqep:     0 gpio:    0 duty:   218 last-data-req:  212218219 last-cmd:  212268538
cycle:  218000040 sample:    219 adc:  1806 eqep:     0 gpio:    0 duty:   219 last-data-req:  212218219 last-cmd:  212268538
cycle:  219000026 sample:    220 adc:  1804 eqep:     0 gpio:    0 duty:   220 last-data-req:  218298084 last-cmd:  218342589
cycle:  220000037 sample:    221 adc:  1825 eqep:     0 gpio:    0 duty:   221 last-data-req:  218298084 last-cmd:  218342589
cycle:  221000025 sample:    222 adc:  1807 eqep:     0 gpio:    0 duty:   222 last-data-req:  218298084 last-cmd:  218342589
cycle:  222000025 sample:    223 adc:  1821 eqep:     0 gpio:    0 duty:   223 last-data-req:  218298084 last-cmd:  218342589
cycle:  223000039 sample:    224 adc:  1801 eqep:     0 gpio:    0 duty:   224 last-data-req:  218298084 last-cmd:  218342589
cycle:  224000038 sample:    225 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  218298084 last-cmd:  218342589
cycle:  225000025 sample:    226 adc:  1815 eqep:     0 gpio:    0 duty:   226 last-data-req:  224371862 last-cmd:  224442452
cycle:  226000037 sample:    227 adc:  1820 eqep:     0 gpio:    0 duty:   227 last-data-req:  224371862 last-cmd:  224442452
cycle:  227000025 sample:    228 adc:  1809 eqep:     0 gpio:    0 duty:   228 last-data-req:  224371862 last-cmd:  224442452
cycle:  228000025 sample:    229 adc:  1820 eqep:     0 gpio:    0 duty:   229 last-data-req:  224371862 last-cmd:  224442452
cycle:  229000039 sample:    230 adc:  1835 eqep:     0 gpio:    0 duty:   230 last-data-req:  224371862 last-cmd:  224442452
cycle:  230000038 sample:    231 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  224371862 last-cmd:  224442452
cycle:  231000024 sample:    232 adc:  1807 eqep:     0 gpio:    0 duty:   232 last-data-req:  230474459 last-cmd:  230531086
cycle:  232000035 sample:    233 adc:  1821 eqep:     0 gpio:    0 duty:   233 last-data-req:  230474459 last-cmd:  230531086
cycle:  233000023 sample:    234 adc:  1816 eqep:     0 gpio:    0 duty:   234 last-data-req:  230474459 last-cmd:  230531086
cycle:  234000023 sample:    235 adc:  1816 eqep:     0 gpio:    0 duty:   235 last-data-req:  230474459 last-cmd:  230531086
cycle:  235000029 sample:    236 adc:  1823 eqep:     0 gpio:    0 duty:   236 last-data-req:  230474459 last-cmd:  230531086
cycle:  236000028 sample:    237 adc:  1803 eqep:     0 gpio:    0 duty:   237 last-data-req:  230474459 last-cmd:  230531086
Cmd:
cycle:  230531086 sample:    231 gpio:    0 duty:   231
cycle:  230531086 sample:    232 gpio:    0 duty:   232
cycle:  230531086 sample:    233 gpio:    0 duty:   233
cycle:  230531086 sample:    234 gpio:    0 duty:   234
cycle:  230531086 sample:    235 gpio:    0 duty:   235
cycle:  230531086 sample:    236 gpio:    0 duty:   236
cycle:  230531086 sample:    237 gpio:    0 duty:   237
cycle:  230531086 sample:    238 gpio:    0 duty:   238
cycle:  230531086 sample:    239 gpio:    0 duty:   239
cycle:  230531086 sample:    240 gpio:    0 duty:   240
cycle:  230531086 sample:    241 gpio:    0 duty:   241
cycle:  230531086 sample:    242 gpio:    0 duty:   242
cycle:  230531086 sample:    243 gpio:    0 duty:   243
cycle:  230531086 sample:    244 gpio:    0 duty:   244
cycle:  230531086 sample:    245 gpio:    0 duty:   245
cycle:  230531086 sample:    246 gpio:    0 duty:   246
cycle:  230531086 sample:    247 gpio:    0 duty:   247
cycle:  230531086 sample:    248 gpio:    0 duty:   248
cycle:  230531086 sample:    249 gpio:    0 duty:   249
cycle:  230531086 sample:    250 gpio:    0 duty:   250
cycle:  230531086 sample:    251 gpio:    0 duty:   251
cycle:  230531086 sample:    252 gpio:    0 duty:   252
cycle:  230531086 sample:    253 gpio:    0 duty:   253
cycle:  230531086 sample:    254 gpio:    0 duty:   254
cycle:  230531086 sample:    255 gpio:    0 duty:   255
cycle:  230531086 sample:    256 gpio:    0 duty:   256
cycle:  230531086 sample:    257 gpio:    0 duty:   257
cycle:  230531086 sample:    258 gpio:    0 duty:   258
cycle:  230531086 sample:    259 gpio:    0 duty:   259
cycle:  230531086 sample:    260 gpio:    0 duty:   260
cycle:  230531086 sample:    261 gpio:    0 duty:   261
cycle:  230531086 sample:    262 gpio:    0 duty:   262

i=7:
Data:
cycle:  211000033 sample:    212 adc:  1817 eqep:     0 gpio:    0 duty:   212 last-data-req:  206148214 last-cmd:  206189869
cycle:  212000032 sample:    213 adc:  1806 eqep:     0 gpio:    0 duty:   213 last-data-req:  206148214 last-cmd:  206189869
cycle:  213000037 sample:    214 adc:  1819 eqep:     0 gpio:    0 duty:   214 last-data-req:  212218219 last-cmd:  212268538
cycle:  214000030 sample:    215 adc:  1820 eqep:     0 gpio:    0 duty:   215 last-data-req:  212218219 last-cmd:  212268538
cycle:  215000036 sample:    216 adc:  1807 eqep:     0 gpio:    0 duty:   216 last-data-req:  212218219 last-cmd:  212268538
cycle:  216000035 sample:    217 adc:  1801 eqep:     0 gpio:    0 duty:   217 last-data-req:  212218219 last-cmd:  212268538
cycle:  217000041 sample:    218 adc:  1819 eqep:     0 gpio:    0 duty:   218 last-data-req:  212218219 last-cmd:  212268538
cycle:  218000040 sample:    219 adc:  1806 eqep:     0 gpio:    0 duty:   219 last-data-req:  212218219 last-cmd:  212268538
cycle:  219000026 sample:    220 adc:  1804 eqep:     0 gpio:    0 duty:   220 last-data-req:  218298084 last-cmd:  218342589
cycle:  220000037 sample:    221 adc:  1825 eqep:     0 gpio:    0 duty:   221 last-data-req:  218298084 last-cmd:  218342589
cycle:  221000025 sample:    222 adc:  1807 eqep:     0 gpio:    0 duty:   222 last-data-req:  218298084 last-cmd:  218342589
cycle:  222000025 sample:    223 adc:  1821 eqep:     0 gpio:    0 duty:   223 last-data-req:  218298084 last-cmd:  218342589
cycle:  223000039 sample:    224 adc:  1801 eqep:     0 gpio:    0 duty:   224 last-data-req:  218298084 last-cmd:  218342589
cycle:  224000038 sample:    225 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  218298084 last-cmd:  218342589
cycle:  225000025 sample:    226 adc:  1815 eqep:     0 gpio:    0 duty:   226 last-data-req:  224371862 last-cmd:  224442452
cycle:  226000037 sample:    227 adc:  1820 eqep:     0 gpio:    0 duty:   227 last-data-req:  224371862 last-cmd:  224442452
cycle:  227000025 sample:    228 adc:  1809 eqep:     0 gpio:    0 duty:   228 last-data-req:  224371862 last-cmd:  224442452
cycle:  228000025 sample:    229 adc:  1820 eqep:     0 gpio:    0 duty:   229 last-data-req:  224371862 last-cmd:  224442452
cycle:  229000039 sample:    230 adc:  1835 eqep:     0 gpio:    0 duty:   230 last-data-req:  224371862 last-cmd:  224442452
cycle:  230000038 sample:    231 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  224371862 last-cmd:  224442452
cycle:  231000024 sample:    232 adc:  1807 eqep:     0 gpio:    0 duty:   232 last-data-req:  230474459 last-cmd:  230531086
cycle:  232000035 sample:    233 adc:  1821 eqep:     0 gpio:    0 duty:   233 last-data-req:  230474459 last-cmd:  230531086
cycle:  233000023 sample:    234 adc:  1816 eqep:     0 gpio:    0 duty:   234 last-data-req:  230474459 last-cmd:  230531086
cycle:  234000023 sample:    235 adc:  1816 eqep:     0 gpio:    0 duty:   235 last-data-req:  230474459 last-cmd:  230531086
cycle:  235000029 sample:    236 adc:  1823 eqep:     0 gpio:    0 duty:   236 last-data-req:  230474459 last-cmd:  230531086
cycle:  236000028 sample:    237 adc:  1803 eqep:     0 gpio:    0 duty:   237 last-data-req:  230474459 last-cmd:  230531086
cycle:  237000033 sample:    238 adc:  1807 eqep:     0 gpio:    0 duty:   238 last-data-req:  236585409 last-cmd:  236634721
cycle:  238000026 sample:    239 adc:  1824 eqep:     0 gpio:    0 duty:   239 last-data-req:  236585409 last-cmd:  236634721
cycle:  239000032 sample:    240 adc:  1820 eqep:     0 gpio:    0 duty:   240 last-data-req:  236585409 last-cmd:  236634721
cycle:  240000031 sample:    241 adc:  1819 eqep:     0 gpio:    0 duty:   241 last-data-req:  236585409 last-cmd:  236634721
cycle:  241000037 sample:    242 adc:  1815 eqep:     0 gpio:    0 duty:   242 last-data-req:  236585409 last-cmd:  236634721
cycle:  242000036 sample:    243 adc:  1816 eqep:     0 gpio:    0 duty:   243 last-data-req:  236585409 last-cmd:  236634721
Cmd:
cycle:  236634721 sample:    237 gpio:    0 duty:   237
cycle:  236634721 sample:    238 gpio:    0 duty:   238
cycle:  236634721 sample:    239 gpio:    0 duty:   239
cycle:  236634721 sample:    240 gpio:    0 duty:   240
cycle:  236634721 sample:    241 gpio:    0 duty:   241
cycle:  236634721 sample:    242 gpio:    0 duty:   242
cycle:  236634721 sample:    243 gpio:    0 duty:   243
cycle:  236634721 sample:    244 gpio:    0 duty:   244
cycle:  236634721 sample:    245 gpio:    0 duty:   245
cycle:  236634721 sample:    246 gpio:    0 duty:   246
cycle:  236634721 sample:    247 gpio:    0 duty:   247
cycle:  236634721 sample:    248 gpio:    0 duty:   248
cycle:  236634721 sample:    249 gpio:    0 duty:   249
cycle:  236634721 sample:    250 gpio:    0 duty:   250
cycle:  236634721 sample:    251 gpio:    0 duty:   251
cycle:  236634721 sample:    252 gpio:    0 duty:   252
cycle:  236634721 sample:    253 gpio:    0 duty:   253
cycle:  236634721 sample:    254 gpio:    0 duty:   254
cycle:  236634721 sample:    255 gpio:    0 duty:   255
cycle:  236634721 sample:    256 gpio:    0 duty:   256
cycle:  236634721 sample:    257 gpio:    0 duty:   257
cycle:  236634721 sample:    258 gpio:    0 duty:   258
cycle:  236634721 sample:    259 gpio:    0 duty:   259
cycle:  236634721 sample:    260 gpio:    0 duty:   260
cycle:  236634721 sample:    261 gpio:    0 duty:   261
cycle:  236634721 sample:    262 gpio:    0 duty:   262
cycle:  236634721 sample:    263 gpio:    0 duty:   263
cycle:  236634721 sample:    264 gpio:    0 duty:   264
cycle:  236634721 sample:    265 gpio:    0 duty:   265
cycle:  236634721 sample:    266 gpio:    0 duty:   266
cycle:  236634721 sample:    267 gpio:    0 duty:   267
cycle:  236634721 sample:    268 gpio:    0 duty:   268

i=8:
Data:
cycle:  217000041 sample:    218 adc:  1819 eqep:     0 gpio:    0 duty:   218 last-data-req:  212218219 last-cmd:  212268538
cycle:  218000040 sample:    219 adc:  1806 eqep:     0 gpio:    0 duty:   219 last-data-req:  212218219 last-cmd:  212268538
cycle:  219000026 sample:    220 adc:  1804 eqep:     0 gpio:    0 duty:   220 last-data-req:  218298084 last-cmd:  218342589
cycle:  220000037 sample:    221 adc:  1825 eqep:     0 gpio:    0 duty:   221 last-data-req:  218298084 last-cmd:  218342589
cycle:  221000025 sample:    222 adc:  1807 eqep:     0 gpio:    0 duty:   222 last-data-req:  218298084 last-cmd:  218342589
cycle:  222000025 sample:    223 adc:  1821 eqep:     0 gpio:    0 duty:   223 last-data-req:  218298084 last-cmd:  218342589
cycle:  223000039 sample:    224 adc:  1801 eqep:     0 gpio:    0 duty:   224 last-data-req:  218298084 last-cmd:  218342589
cycle:  224000038 sample:    225 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  218298084 last-cmd:  218342589
cycle:  225000025 sample:    226 adc:  1815 eqep:     0 gpio:    0 duty:   226 last-data-req:  224371862 last-cmd:  224442452
cycle:  226000037 sample:    227 adc:  1820 eqep:     0 gpio:    0 duty:   227 last-data-req:  224371862 last-cmd:  224442452
cycle:  227000025 sample:    228 adc:  1809 eqep:     0 gpio:    0 duty:   228 last-data-req:  224371862 last-cmd:  224442452
cycle:  228000025 sample:    229 adc:  1820 eqep:     0 gpio:    0 duty:   229 last-data-req:  224371862 last-cmd:  224442452
cycle:  229000039 sample:    230 adc:  1835 eqep:     0 gpio:    0 duty:   230 last-data-req:  224371862 last-cmd:  224442452
cycle:  230000038 sample:    231 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  224371862 last-cmd:  224442452
cycle:  231000024 sample:    232 adc:  1807 eqep:     0 gpio:    0 duty:   232 last-data-req:  230474459 last-cmd:  230531086
cycle:  232000035 sample:    233 adc:  1821 eqep:     0 gpio:    0 duty:   233 last-data-req:  230474459 last-cmd:  230531086
cycle:  233000023 sample:    234 adc:  1816 eqep:     0 gpio:    0 duty:   234 last-data-req:  230474459 last-cmd:  230531086
cycle:  234000023 sample:    235 adc:  1816 eqep:     0 gpio:    0 duty:   235 last-data-req:  230474459 last-cmd:  230531086
cycle:  235000029 sample:    236 adc:  1823 eqep:     0 gpio:    0 duty:   236 last-data-req:  230474459 last-cmd:  230531086
cycle:  236000028 sample:    237 adc:  1803 eqep:     0 gpio:    0 duty:   237 last-data-req:  230474459 last-cmd:  230531086
cycle:  237000033 sample:    238 adc:  1807 eqep:     0 gpio:    0 duty:   238 last-data-req:  236585409 last-cmd:  236634721
cycle:  238000026 sample:    239 adc:  1824 eqep:     0 gpio:    0 duty:   239 last-data-req:  236585409 last-cmd:  236634721
cycle:  239000032 sample:    240 adc:  1820 eqep:     0 gpio:    0 duty:   240 last-data-req:  236585409 last-cmd:  236634721
cycle:  240000031 sample:    241 adc:  1819 eqep:     0 gpio:    0 duty:   241 last-data-req:  236585409 last-cmd:  236634721
cycle:  241000037 sample:    242 adc:  1815 eqep:     0 gpio:    0 duty:   242 last-data-req:  236585409 last-cmd:  236634721
cycle:  242000036 sample:    243 adc:  1816 eqep:     0 gpio:    0 duty:   243 last-data-req:  236585409 last-cmd:  236634721
cycle:  243000041 sample:    244 adc:  1816 eqep:     0 gpio:    0 duty:   244 last-data-req:  242666471 last-cmd:  242727145
cycle:  244000034 sample:    245 adc:  1816 eqep:     0 gpio:    0 duty:   245 last-data-req:  242666471 last-cmd:  242727145
cycle:  245000040 sample:    246 adc:  1819 eqep:     0 gpio:    0 duty:   246 last-data-req:  242666471 last-cmd:  242727145
cycle:  246000039 sample:    247 adc:  1821 eqep:     0 gpio:    0 duty:   247 last-data-req:  242666471 last-cmd:  242727145
cycle:  247000026 sample:    248 adc:  1819 eqep:     0 gpio:    0 duty:   248 last-data-req:  242666471 last-cmd:  242727145
cycle:  248000024 sample:    249 adc:  1826 eqep:     0 gpio:    0 duty:   249 last-data-req:  242666471 last-cmd:  242727145
Cmd:
cycle:  242727145 sample:    243 gpio:    0 duty:   243
cycle:  242727145 sample:    244 gpio:    0 duty:   244
cycle:  242727145 sample:    245 gpio:    0 duty:   245
cycle:  242727145 sample:    246 gpio:    0 duty:   246
cycle:  242727145 sample:    247 gpio:    0 duty:   247
cycle:  242727145 sample:    248 gpio:    0 duty:   248
cycle:  242727145 sample:    249 gpio:    0 duty:   249
cycle:  242727145 sample:    250 gpio:    0 duty:   250
cycle:  242727145 sample:    251 gpio:    0 duty:   251
cycle:  242727145 sample:    252 gpio:    0 duty:   252
cycle:  242727145 sample:    253 gpio:    0 duty:   253
cycle:  242727145 sample:    254 gpio:    0 duty:   254
cycle:  242727145 sample:    255 gpio:    0 duty:   255
cycle:  242727145 sample:    256 gpio:    0 duty:   256
cycle:  242727145 sample:    257 gpio:    0 duty:   257
cycle:  242727145 sample:    258 gpio:    0 duty:   258
cycle:  242727145 sample:    259 gpio:    0 duty:   259
cycle:  242727145 sample:    260 gpio:    0 duty:   260
cycle:  242727145 sample:    261 gpio:    0 duty:   261
cycle:  242727145 sample:    262 gpio:    0 duty:   262
cycle:  242727145 sample:    263 gpio:    0 duty:   263
cycle:  242727145 sample:    264 gpio:    0 duty:   264
cycle:  242727145 sample:    265 gpio:    0 duty:   265
cycle:  242727145 sample:    266 gpio:    0 duty:   266
cycle:  242727145 sample:    267 gpio:    0 duty:   267
cycle:  242727145 sample:    268 gpio:    0 duty:   268
cycle:  242727145 sample:    269 gpio:    0 duty:   269
cycle:  242727145 sample:    270 gpio:    0 duty:   270
cycle:  242727145 sample:    271 gpio:    0 duty:   271
cycle:  242727145 sample:    272 gpio:    0 duty:   272
cycle:  242727145 sample:    273 gpio:    0 duty:   273
cycle:  242727145 sample:    274 gpio:    0 duty:   274

i=9:
Data:
cycle:  223000039 sample:    224 adc:  1801 eqep:     0 gpio:    0 duty:   224 last-data-req:  218298084 last-cmd:  218342589
cycle:  224000038 sample:    225 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  218298084 last-cmd:  218342589
cycle:  225000025 sample:    226 adc:  1815 eqep:     0 gpio:    0 duty:   226 last-data-req:  224371862 last-cmd:  224442452
cycle:  226000037 sample:    227 adc:  1820 eqep:     0 gpio:    0 duty:   227 last-data-req:  224371862 last-cmd:  224442452
cycle:  227000025 sample:    228 adc:  1809 eqep:     0 gpio:    0 duty:   228 last-data-req:  224371862 last-cmd:  224442452
cycle:  228000025 sample:    229 adc:  1820 eqep:     0 gpio:    0 duty:   229 last-data-req:  224371862 last-cmd:  224442452
cycle:  229000039 sample:    230 adc:  1835 eqep:     0 gpio:    0 duty:   230 last-data-req:  224371862 last-cmd:  224442452
cycle:  230000038 sample:    231 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  224371862 last-cmd:  224442452
cycle:  231000024 sample:    232 adc:  1807 eqep:     0 gpio:    0 duty:   232 last-data-req:  230474459 last-cmd:  230531086
cycle:  232000035 sample:    233 adc:  1821 eqep:     0 gpio:    0 duty:   233 last-data-req:  230474459 last-cmd:  230531086
cycle:  233000023 sample:    234 adc:  1816 eqep:     0 gpio:    0 duty:   234 last-data-req:  230474459 last-cmd:  230531086
cycle:  234000023 sample:    235 adc:  1816 eqep:     0 gpio:    0 duty:   235 last-data-req:  230474459 last-cmd:  230531086
cycle:  235000029 sample:    236 adc:  1823 eqep:     0 gpio:    0 duty:   236 last-data-req:  230474459 last-cmd:  230531086
cycle:  236000028 sample:    237 adc:  1803 eqep:     0 gpio:    0 duty:   237 last-data-req:  230474459 last-cmd:  230531086
cycle:  237000033 sample:    238 adc:  1807 eqep:     0 gpio:    0 duty:   238 last-data-req:  236585409 last-cmd:  236634721
cycle:  238000026 sample:    239 adc:  1824 eqep:     0 gpio:    0 duty:   239 last-data-req:  236585409 last-cmd:  236634721
cycle:  239000032 sample:    240 adc:  1820 eqep:     0 gpio:    0 duty:   240 last-data-req:  236585409 last-cmd:  236634721
cycle:  240000031 sample:    241 adc:  1819 eqep:     0 gpio:    0 duty:   241 last-data-req:  236585409 last-cmd:  236634721
cycle:  241000037 sample:    242 adc:  1815 eqep:     0 gpio:    0 duty:   242 last-data-req:  236585409 last-cmd:  236634721
cycle:  242000036 sample:    243 adc:  1816 eqep:     0 gpio:    0 duty:   243 last-data-req:  236585409 last-cmd:  236634721
cycle:  243000041 sample:    244 adc:  1816 eqep:     0 gpio:    0 duty:   244 last-data-req:  242666471 last-cmd:  242727145
cycle:  244000034 sample:    245 adc:  1816 eqep:     0 gpio:    0 duty:   245 last-data-req:  242666471 last-cmd:  242727145
cycle:  245000040 sample:    246 adc:  1819 eqep:     0 gpio:    0 duty:   246 last-data-req:  242666471 last-cmd:  242727145
cycle:  246000039 sample:    247 adc:  1821 eqep:     0 gpio:    0 duty:   247 last-data-req:  242666471 last-cmd:  242727145
cycle:  247000026 sample:    248 adc:  1819 eqep:     0 gpio:    0 duty:   248 last-data-req:  242666471 last-cmd:  242727145
cycle:  248000024 sample:    249 adc:  1826 eqep:     0 gpio:    0 duty:   249 last-data-req:  242666471 last-cmd:  242727145
cycle:  249000029 sample:    250 adc:  1811 eqep:     0 gpio:    0 duty:   250 last-data-req:  248759255 last-cmd:  248806287
cycle:  250000041 sample:    251 adc:  1815 eqep:     0 gpio:    0 duty:   251 last-data-req:  248759255 last-cmd:  248806287
cycle:  251000029 sample:    252 adc:  1798 eqep:     0 gpio:    0 duty:   252 last-data-req:  248759255 last-cmd:  248806287
cycle:  252000029 sample:    253 adc:  1815 eqep:     0 gpio:    0 duty:   253 last-data-req:  248759255 last-cmd:  248806287
cycle:  253000035 sample:    254 adc:  1831 eqep:     0 gpio:    0 duty:   254 last-data-req:  248759255 last-cmd:  248806287
cycle:  254000034 sample:    255 adc:  1816 eqep:     0 gpio:    0 duty:   255 last-data-req:  248759255 last-cmd:  248806287
Cmd:
cycle:  248806287 sample:    249 gpio:    0 duty:   249
cycle:  248806287 sample:    250 gpio:    0 duty:   250
cycle:  248806287 sample:    251 gpio:    0 duty:   251
cycle:  248806287 sample:    252 gpio:    0 duty:   252
cycle:  248806287 sample:    253 gpio:    0 duty:   253
cycle:  248806287 sample:    254 gpio:    0 duty:   254
cycle:  248806287 sample:    255 gpio:    0 duty:   255
cycle:  248806287 sample:    256 gpio:    0 duty:   256
cycle:  248806287 sample:    257 gpio:    0 duty:   257
cycle:  248806287 sample:    258 gpio:    0 duty:   258
cycle:  248806287 sample:    259 gpio:    0 duty:   259
cycle:  248806287 sample:    260 gpio:    0 duty:   260
cycle:  248806287 sample:    261 gpio:    0 duty:   261
cycle:  248806287 sample:    262 gpio:    0 duty:   262
cycle:  248806287 sample:    263 gpio:    0 duty:   263
cycle:  248806287 sample:    264 gpio:    0 duty:   264
cycle:  248806287 sample:    265 gpio:    0 duty:   265
cycle:  248806287 sample:    266 gpio:    0 duty:   266
cycle:  248806287 sample:    267 gpio:    0 duty:   267
cycle:  248806287 sample:    268 gpio:    0 duty:   268
cycle:  248806287 sample:    269 gpio:    0 duty:   269
cycle:  248806287 sample:    270 gpio:    0 duty:   270
cycle:  248806287 sample:    271 gpio:    0 duty:   271
cycle:  248806287 sample:    272 gpio:    0 duty:   272
cycle:  248806287 sample:    273 gpio:    0 duty:   273
cycle:  248806287 sample:    274 gpio:    0 duty:   274
cycle:  248806287 sample:    275 gpio:    0 duty:   275
cycle:  248806287 sample:    276 gpio:    0 duty:   276
cycle:  248806287 sample:    277 gpio:    0 duty:   277
cycle:  248806287 sample:    278 gpio:    0 duty:   278
cycle:  248806287 sample:    279 gpio:    0 duty:   279
cycle:  248806287 sample:    280 gpio:    0 duty:   280
root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# 













///////////////////////////////////////////////

testname=test04-cycle-count-reset

- if the cycle counter is too big, it might max out soon. 
- So, each sample time, see if it's too big. If so, reset it, and also reset the "cycles at next sample time" reg to the sample period,
  so the next sample triggers at the right time.
- the samples will be still be spaced temporally correctly.


Example output snippet: you can see the cyc counter resetting:

iter i=19
cycle:   4077000030 sample:   4075 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4078000032 sample:   4076 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4079000016 sample:   4077 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4080000018 sample:   4078 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4081000020 sample:   4079 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4082000022 sample:   4080 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4083000024 sample:   4081 adc:  1798 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4084000026 sample:   4082 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4085000028 sample:   4083 adc:  1803 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4086000030 sample:   4084 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4087000032 sample:   4085 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4088000016 sample:   4086 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4089000018 sample:   4087 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4090000020 sample:   4088 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4091000022 sample:   4089 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4092000024 sample:   4090 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4093000026 sample:   4091 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:   4094000028 sample:   4092 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:            8 sample:   4093 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      1000028 sample:   4094 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      2000030 sample:   4095 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      3000032 sample:   4096 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      4000016 sample:   4097 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      5000021 sample:   4098 adc:  1805 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      6000024 sample:   4099 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      7000026 sample:   4100 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      8000028 sample:   4101 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:      9000030 sample:   4102 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:     10000032 sample:   4103 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:     11000016 sample:   4104 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:     12000018 sample:   4105 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0
cycle:     13000020 sample:   4106 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req: 3908351784 last-cmd:          0


Example output:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# ./run
I'm in: 
/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence
Resetting eqep...
building...
==========================
building PRU C library...
------------------------
Using pasm to assemble .p file jpp-pru-lib.p into .bin file ...


PRU Assembler Version 0.84
Copyright (C) 2005-2013 by Texas Instruments Inc.


Pass 2 : 0 Error(s), 0 Warning(s)

Writing Code Image of 394 word(s)

~~~~~~~~~~~~~~~~~
Compiling .c file jpp-pru-lib.c into .o file jpp-pru-lib.o
~~~~~~~~~~~~~~~~~~~~~~~~~~
Building test .c file test04-cycle-count-reset.c to make test04-cycle-count-reset
Fri Sep 23 09:33:47 UTC 2016
Fri Sep 23 09:33:48 UTC 2016
Just running test04-cycle-count-reset ...
Running test04-cycle-count-reset at realtime priority:
Welcome to the Beaglebone PRU library for real-time controls!
NOTE: if you get a seg fault, make sure the jppprugpio device tree overlay has been loaded!!
Here's the cmd: root@beaglebone# echo jppprugpio > /sys/devices/bone_capemgr.9/slots
Assumes you have jppprugpio-00A0.dto in /lib/firmware
Also do this:
echo bone_eqep1 > $SLOTS
prussdrv_init() (returned 0)
prussdrv_open (returned 0)
prussdrv_pruintc_init (returned 0)
pruss_map_prumem (returned 0)
pru0 data memory addr: 0xb6c8c000
 ctrladdr_CPU_wants_data : 0xb6c8c000
 ctrladdr_PRU_data_ready : 0xb6c8c004
 ctrladdr_CPU_new_sched  : 0xb6c8c008
 ctrladdr_PRU_ack_sched  : 0xb6c8c00c
 ctrladdr_CPU_new_period    : 0xb6c8c010
 ctrladdr_SAMPLE_PERIOD_CMD : 0xb6c8c014
 ctrladdr_SAMPLE_NUM_LOCK   : 0xb6c8c018
 ctrladdr_SAMPLE_NUM        : 0xb6c8c01c
 addr_cpu_data_buf : 0xb6c8c020
 addr_pru_data_buf : 0xb6c8c420
 addr_cpu_cmd_buf  : 0xb6c8c820
 addr_pru_cmd_buf  : 0xb6c8ca20
executing bin file: ./jpp-pru-lib.bin
NOTE: a lot of stuff assumes PRU0 in this lib, sry.
prussdrv_exec_program returned 0
Waiting 1000ms to let PRU fill its internal buffer...done.

iter i=0
cycle:    269000032 sample:    267 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    270000016 sample:    268 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    271000018 sample:    269 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    272000020 sample:    270 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    273000022 sample:    271 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    274000024 sample:    272 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    275000026 sample:    273 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    276000028 sample:    274 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    277000030 sample:    275 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    278000032 sample:    276 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    279000016 sample:    277 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    280000018 sample:    278 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    281000020 sample:    279 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    282000022 sample:    280 adc:  1796 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    283000024 sample:    281 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    284000026 sample:    282 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    285000028 sample:    283 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    286000030 sample:    284 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    287000032 sample:    285 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    288000016 sample:    286 adc:  1787 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    289000018 sample:    287 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    290000020 sample:    288 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    291000022 sample:    289 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    292000027 sample:    290 adc:  1799 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    293000030 sample:    291 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    294000032 sample:    292 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    295000016 sample:    293 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    296000018 sample:    294 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    297000020 sample:    295 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    298000022 sample:    296 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    299000024 sample:    297 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0
cycle:    300000026 sample:    298 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:          0 last-cmd:          0

iter i=1
cycle:    469000016 sample:    467 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    470000018 sample:    468 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    471000020 sample:    469 adc:  1791 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    472000022 sample:    470 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    473000024 sample:    471 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    474000026 sample:    472 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    475000028 sample:    473 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    476000030 sample:    474 adc:  1828 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    477000032 sample:    475 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    478000016 sample:    476 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    479000018 sample:    477 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    480000020 sample:    478 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    481000022 sample:    479 adc:  1799 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    482000024 sample:    480 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    483000026 sample:    481 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    484000031 sample:    482 adc:  1796 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    485000016 sample:    483 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    486000018 sample:    484 adc:  1798 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    487000020 sample:    485 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    488000022 sample:    486 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    489000024 sample:    487 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    490000026 sample:    488 adc:  1805 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    491000028 sample:    489 adc:  1796 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    492000030 sample:    490 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    493000032 sample:    491 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    494000016 sample:    492 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    495000018 sample:    493 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    496000020 sample:    494 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    497000022 sample:    495 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    498000024 sample:    496 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    499000026 sample:    497 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0
cycle:    500000028 sample:    498 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req:  300089438 last-cmd:          0

iter i=2
cycle:    670000020 sample:    668 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    671000022 sample:    669 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    672000024 sample:    670 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    673000026 sample:    671 adc:  1831 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    674000028 sample:    672 adc:  1836 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    675000030 sample:    673 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    676000017 sample:    674 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    677000020 sample:    675 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    678000022 sample:    676 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    679000024 sample:    677 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    680000026 sample:    678 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    681000028 sample:    679 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    682000030 sample:    680 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    683000032 sample:    681 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    684000016 sample:    682 adc:  1793 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    685000018 sample:    683 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    686000020 sample:    684 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    687000022 sample:    685 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    688000024 sample:    686 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    689000026 sample:    687 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    690000028 sample:    688 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    691000030 sample:    689 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    692000032 sample:    690 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    693000016 sample:    691 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    694000018 sample:    692 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    695000020 sample:    693 adc:  1832 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    696000022 sample:    694 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    697000024 sample:    695 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    698000026 sample:    696 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    699000028 sample:    697 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    700000030 sample:    698 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0
cycle:    701000032 sample:    699 adc:  1805 eqep:     0 gpio:    0 duty:   333 last-data-req:  500595366 last-cmd:          0

iter i=3
cycle:    870000032 sample:    868 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    871000016 sample:    869 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    872000018 sample:    870 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    873000020 sample:    871 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    874000022 sample:    872 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    875000024 sample:    873 adc:  1802 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    876000026 sample:    874 adc:  1806 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    877000028 sample:    875 adc:  1798 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    878000030 sample:    876 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    879000032 sample:    877 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    880000016 sample:    878 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    881000018 sample:    879 adc:  1832 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    882000020 sample:    880 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    883000022 sample:    881 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    884000024 sample:    882 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    885000026 sample:    883 adc:  1799 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    886000028 sample:    884 adc:  1804 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    887000030 sample:    885 adc:  1799 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    888000032 sample:    886 adc:  1828 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    889000016 sample:    887 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    890000018 sample:    888 adc:  1799 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    891000020 sample:    889 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    892000022 sample:    890 adc:  1793 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    893000024 sample:    891 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    894000026 sample:    892 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    895000028 sample:    893 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    896000030 sample:    894 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    897000032 sample:    895 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    898000016 sample:    896 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    899000018 sample:    897 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    900000023 sample:    898 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0
cycle:    901000026 sample:    899 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req:  701087842 last-cmd:          0

...



iter i=28
cycle:   1786000026 sample:   5879 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1787000028 sample:   5880 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1788000030 sample:   5881 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1789000032 sample:   5882 adc:  1824 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1790000016 sample:   5883 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1791000018 sample:   5884 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1792000020 sample:   5885 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1793000022 sample:   5886 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1794000024 sample:   5887 adc:  1791 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1795000026 sample:   5888 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1796000028 sample:   5889 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1797000033 sample:   5890 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1798000018 sample:   5891 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1799000020 sample:   5892 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1800000022 sample:   5893 adc:  1795 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1801000024 sample:   5894 adc:  1832 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1802000026 sample:   5895 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1803000028 sample:   5896 adc:  1801 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1804000030 sample:   5897 adc:  1820 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1805000032 sample:   5898 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1806000016 sample:   5899 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1807000018 sample:   5900 adc:  1813 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1808000020 sample:   5901 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1809000022 sample:   5902 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1810000024 sample:   5903 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1811000026 sample:   5904 adc:  1821 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1812000028 sample:   5905 adc:  1799 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1813000030 sample:   5906 adc:  1811 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1814000032 sample:   5907 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1815000016 sample:   5908 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1816000018 sample:   5909 adc:  1819 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0
cycle:   1817000020 sample:   5910 adc:  1818 eqep:     0 gpio:    0 duty:   333 last-data-req: 1617002796 last-cmd:          0

iter i=29
cycle:   1986000028 sample:   6079 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1987000030 sample:   6080 adc:  1808 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1988000032 sample:   6081 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1989000019 sample:   6082 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1990000022 sample:   6083 adc:  1800 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1991000024 sample:   6084 adc:  1793 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1992000026 sample:   6085 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1993000028 sample:   6086 adc:  1816 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1994000030 sample:   6087 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1995000032 sample:   6088 adc:  1810 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1996000016 sample:   6089 adc:  1800 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1997000018 sample:   6090 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1998000020 sample:   6091 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   1999000022 sample:   6092 adc:  1814 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2000000024 sample:   6093 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2001000026 sample:   6094 adc:  1817 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2002000028 sample:   6095 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2003000030 sample:   6096 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2004000032 sample:   6097 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2005000016 sample:   6098 adc:  1807 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2006000018 sample:   6099 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2007000020 sample:   6100 adc:  1802 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2008000022 sample:   6101 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2009000024 sample:   6102 adc:  1825 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2010000026 sample:   6103 adc:  1815 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2011000028 sample:   6104 adc:  1796 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2012000030 sample:   6105 adc:  1809 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2013000032 sample:   6106 adc:  1823 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2014000016 sample:   6107 adc:  1827 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2015000018 sample:   6108 adc:  1832 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2016000020 sample:   6109 adc:  1822 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
cycle:   2017000022 sample:   6110 adc:  1812 eqep:     0 gpio:    0 duty:   333 last-data-req: 1817463220 last-cmd:          0
root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# 




///////////////////////////////////////////////

test05-magic-cmd-pkts


- if sample num = 0xffffffff, that sample is executed no matter what.
- since pru just scans the PCB each actuation, that cmd pkt will get actuated over and over again, forever.

example output:



i=6:
Data:
cycle:  205000035 sample:    206 adc:  1808 eqep:     0 gpio:    0 duty:   201 last-data-req:  200070211 last-cmd:  200124615
cycle:  206000039 sample:    207 adc:  1805 eqep:     0 gpio:    0 duty:   201 last-data-req:  200070211 last-cmd:  200124615
cycle:  207000036 sample:    208 adc:  1808 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  208000039 sample:    209 adc:  1811 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  209000024 sample:    210 adc:  1815 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  210000027 sample:    211 adc:  1816 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  211000031 sample:    212 adc:  1812 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  212000035 sample:    213 adc:  1825 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  213000032 sample:    214 adc:  1803 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  214000035 sample:    215 adc:  1795 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  215000039 sample:    216 adc:  1808 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  216000024 sample:    217 adc:  1812 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  217000027 sample:    218 adc:  1804 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  218000031 sample:    219 adc:  1799 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  219000028 sample:    220 adc:  1807 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  220000031 sample:    221 adc:  1811 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  221000035 sample:    222 adc:  1823 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  222000039 sample:    223 adc:  1801 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  223000024 sample:    224 adc:  1804 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  224000027 sample:    225 adc:  1823 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  225000025 sample:    226 adc:  1820 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  226000029 sample:    227 adc:  1815 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  227000033 sample:    228 adc:  1812 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  228000037 sample:    229 adc:  1814 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  229000041 sample:    230 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  230000026 sample:    231 adc:  1803 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  231000041 sample:    232 adc:  1801 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  232000026 sample:    233 adc:  1819 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  233000029 sample:    234 adc:  1825 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  234000033 sample:    235 adc:  1807 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  235000037 sample:    236 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  236000041 sample:    237 adc:  1789 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019

you can see duty=231 was actuated the last several packets. 

That's bc sample=-1 in the 1st cmd pkt, so it executes. it has duty 231. 

Cmd:
cycle:  230500019 sample:     -1 gpio:    0 duty:   231
cycle:  230500019 sample:     -1 gpio:    0 duty:   232
cycle:  230500019 sample:     -1 gpio:    0 duty:   233
cycle:  230500019 sample:     -1 gpio:    0 duty:   234
cycle:  230500019 sample:     -1 gpio:    0 duty:   235
cycle:  230500019 sample:     -1 gpio:    0 duty:   236
cycle:  230500019 sample:     -1 gpio:    0 duty:   237
cycle:  230500019 sample:     -1 gpio:    0 duty:   238
cycle:  230500019 sample:     -1 gpio:    0 duty:   239
cycle:  230500019 sample:     -1 gpio:    0 duty:   240
cycle:  230500019 sample:     -1 gpio:    0 duty:   241
cycle:  230500019 sample:     -1 gpio:    0 duty:   242
cycle:  230500019 sample:     -1 gpio:    0 duty:   243
cycle:  230500019 sample:     -1 gpio:    0 duty:   244
cycle:  230500019 sample:     -1 gpio:    0 duty:   245
cycle:  230500019 sample:     -1 gpio:    0 duty:   246
cycle:  230500019 sample:     -1 gpio:    0 duty:   247
cycle:  230500019 sample:     -1 gpio:    0 duty:   248
cycle:  230500019 sample:     -1 gpio:    0 duty:   249
cycle:  230500019 sample:     -1 gpio:    0 duty:   250
cycle:  230500019 sample:     -1 gpio:    0 duty:   251
cycle:  230500019 sample:     -1 gpio:    0 duty:   252
cycle:  230500019 sample:     -1 gpio:    0 duty:   253
cycle:  230500019 sample:     -1 gpio:    0 duty:   254
cycle:  230500019 sample:     -1 gpio:    0 duty:   255
cycle:  230500019 sample:     -1 gpio:    0 duty:   256
cycle:  230500019 sample:     -1 gpio:    0 duty:   257
cycle:  230500019 sample:     -1 gpio:    0 duty:   258
cycle:  230500019 sample:     -1 gpio:    0 duty:   259
cycle:  230500019 sample:     -1 gpio:    0 duty:   260
cycle:  230500019 sample:     -1 gpio:    0 duty:   261
cycle:  230500019 sample:     -1 gpio:    0 duty:   262


Armed with the knowledge that sample num = 237, cpu sends a new cmd buf w/ first pkt duty=237 at samp num 0xffffffff.

30ms later, you can see duty = 237 the last several iters.

i=7:
Data:
cycle:  211000031 sample:    212 adc:  1812 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  212000035 sample:    213 adc:  1825 eqep:     0 gpio:    0 duty:   207 last-data-req:  206156867 last-cmd:  206200859
cycle:  213000032 sample:    214 adc:  1803 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  214000035 sample:    215 adc:  1795 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  215000039 sample:    216 adc:  1808 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  216000024 sample:    217 adc:  1812 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  217000027 sample:    218 adc:  1804 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  218000031 sample:    219 adc:  1799 eqep:     0 gpio:    0 duty:   213 last-data-req:  212233433 last-cmd:  212278413
cycle:  219000028 sample:    220 adc:  1807 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  220000031 sample:    221 adc:  1811 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  221000035 sample:    222 adc:  1823 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  222000039 sample:    223 adc:  1801 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  223000024 sample:    224 adc:  1804 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  224000027 sample:    225 adc:  1823 eqep:     0 gpio:    0 duty:   219 last-data-req:  218307206 last-cmd:  218350932
cycle:  225000025 sample:    226 adc:  1820 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  226000029 sample:    227 adc:  1815 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  227000033 sample:    228 adc:  1812 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  228000037 sample:    229 adc:  1814 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  229000041 sample:    230 adc:  1808 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  230000026 sample:    231 adc:  1803 eqep:     0 gpio:    0 duty:   225 last-data-req:  224381457 last-cmd:  224426264
cycle:  231000041 sample:    232 adc:  1801 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  232000026 sample:    233 adc:  1819 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  233000029 sample:    234 adc:  1825 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  234000033 sample:    235 adc:  1807 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  235000037 sample:    236 adc:  1815 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  236000041 sample:    237 adc:  1789 eqep:     0 gpio:    0 duty:   231 last-data-req:  230455875 last-cmd:  230500019
cycle:  237000038 sample:    238 adc:  1807 eqep:     0 gpio:    0 duty:   237 last-data-req:  236537724 last-cmd:  236584110
cycle:  238000041 sample:    239 adc:  1820 eqep:     0 gpio:    0 duty:   237 last-data-req:  236537724 last-cmd:  236584110
cycle:  239000026 sample:    240 adc:  1814 eqep:     0 gpio:    0 duty:   237 last-data-req:  236537724 last-cmd:  236584110
cycle:  240000029 sample:    241 adc:  1809 eqep:     0 gpio:    0 duty:   237 last-data-req:  236537724 last-cmd:  236584110
cycle:  241000033 sample:    242 adc:  1810 eqep:     0 gpio:    0 duty:   237 last-data-req:  236537724 last-cmd:  236584110
cycle:  242000037 sample:    243 adc:  1828 eqep:     0 gpio:    0 duty:   237 last-data-req:  236537724 last-cmd:  236584110
Cmd:
cycle:  236584110 sample:     -1 gpio:    0 duty:   237
cycle:  236584110 sample:     -1 gpio:    0 duty:   238
cycle:  236584110 sample:     -1 gpio:    0 duty:   239
cycle:  236584110 sample:     -1 gpio:    0 duty:   240
cycle:  236584110 sample:     -1 gpio:    0 duty:   241
cycle:  236584110 sample:     -1 gpio:    0 duty:   242
cycle:  236584110 sample:     -1 gpio:    0 duty:   243
cycle:  236584110 sample:     -1 gpio:    0 duty:   244
cycle:  236584110 sample:     -1 gpio:    0 duty:   245
cycle:  236584110 sample:     -1 gpio:    0 duty:   246
cycle:  236584110 sample:     -1 gpio:    0 duty:   247
cycle:  236584110 sample:     -1 gpio:    0 duty:   248
cycle:  236584110 sample:     -1 gpio:    0 duty:   249
cycle:  236584110 sample:     -1 gpio:    0 duty:   250
cycle:  236584110 sample:     -1 gpio:    0 duty:   251
cycle:  236584110 sample:     -1 gpio:    0 duty:   252
cycle:  236584110 sample:     -1 gpio:    0 duty:   253
cycle:  236584110 sample:     -1 gpio:    0 duty:   254
cycle:  236584110 sample:     -1 gpio:    0 duty:   255
cycle:  236584110 sample:     -1 gpio:    0 duty:   256
cycle:  236584110 sample:     -1 gpio:    0 duty:   257
cycle:  236584110 sample:     -1 gpio:    0 duty:   258
cycle:  236584110 sample:     -1 gpio:    0 duty:   259
cycle:  236584110 sample:     -1 gpio:    0 duty:   260
cycle:  236584110 sample:     -1 gpio:    0 duty:   261
cycle:  236584110 sample:     -1 gpio:    0 duty:   262
cycle:  236584110 sample:     -1 gpio:    0 duty:   263
cycle:  236584110 sample:     -1 gpio:    0 duty:   264
cycle:  236584110 sample:     -1 gpio:    0 duty:   265
cycle:  236584110 sample:     -1 gpio:    0 duty:   266
cycle:  236584110 sample:     -1 gpio:    0 duty:   267
cycle:  236584110 sample:     -1 gpio:    0 duty:   268


Note taht the other cmd pkts don't get a chnace to actuate.



Now we change the samp # to 0xfffffffe.


i=8:
Data:
cycle:  217000029 sample:    218 adc:  1823 eqep:     0 gpio:    0 duty:   217 last-data-req:  212446424 last-cmd:  212489675
cycle:  218000026 sample:    219 adc:  1822 eqep:     0 gpio:    0 duty:   218 last-data-req:  212446424 last-cmd:  212489675
cycle:  219000038 sample:    220 adc:  1814 eqep:     0 gpio:    0 duty:   219 last-data-req:  218520795 last-cmd:  218567941
cycle:  220000039 sample:    221 adc:  1807 eqep:     0 gpio:    0 duty:   220 last-data-req:  218520795 last-cmd:  218567941
cycle:  221000034 sample:    222 adc:  1815 eqep:     0 gpio:    0 duty:   221 last-data-req:  218520795 last-cmd:  218567941
cycle:  222000023 sample:    223 adc:  1836 eqep:     0 gpio:    0 duty:   222 last-data-req:  218520795 last-cmd:  218567941
cycle:  223000025 sample:    224 adc:  1826 eqep:     0 gpio:    0 duty:   223 last-data-req:  218520795 last-cmd:  218567941
cycle:  224000033 sample:    225 adc:  1817 eqep:     0 gpio:    0 duty:   224 last-data-req:  218520795 last-cmd:  218567941
cycle:  225000028 sample:    226 adc:  1811 eqep:     0 gpio:    0 duty:   225 last-data-req:  224594811 last-cmd:  224642525
cycle:  226000037 sample:    227 adc:  1795 eqep:     0 gpio:    0 duty:   226 last-data-req:  224594811 last-cmd:  224642525
cycle:  227000032 sample:    228 adc:  1815 eqep:     0 gpio:    0 duty:   227 last-data-req:  224594811 last-cmd:  224642525
cycle:  228000040 sample:    229 adc:  1804 eqep:     0 gpio:    0 duty:   228 last-data-req:  224594811 last-cmd:  224642525
cycle:  229000041 sample:    230 adc:  1823 eqep:     0 gpio:    0 duty:   229 last-data-req:  224594811 last-cmd:  224642525
cycle:  230000030 sample:    231 adc:  1820 eqep:     0 gpio:    0 duty:   230 last-data-req:  224594811 last-cmd:  224642525
cycle:  231000023 sample:    232 adc:  1817 eqep:     0 gpio:    0 duty:   231 last-data-req:  230673540 last-cmd:  230719413
cycle:  232000033 sample:    233 adc:  1815 eqep:     0 gpio:    0 duty:   232 last-data-req:  230673540 last-cmd:  230719413
cycle:  233000028 sample:    234 adc:  1824 eqep:     0 gpio:    0 duty:   233 last-data-req:  230673540 last-cmd:  230719413
cycle:  234000036 sample:    235 adc:  1807 eqep:     0 gpio:    0 duty:   234 last-data-req:  230673540 last-cmd:  230719413
cycle:  235000037 sample:    236 adc:  1819 eqep:     0 gpio:    0 duty:   235 last-data-req:  230673540 last-cmd:  230719413
cycle:  236000026 sample:    237 adc:  1821 eqep:     0 gpio:    0 duty:   236 last-data-req:  230673540 last-cmd:  230719413
cycle:  237000038 sample:    238 adc:  1823 eqep:     0 gpio:    0 duty:   237 last-data-req:  236748966 last-cmd:  236795656
cycle:  238000039 sample:    239 adc:  1816 eqep:     0 gpio:    0 duty:   238 last-data-req:  236748966 last-cmd:  236795656
cycle:  239000034 sample:    240 adc:  1823 eqep:     0 gpio:    0 duty:   239 last-data-req:  236748966 last-cmd:  236795656
cycle:  240000023 sample:    241 adc:  1817 eqep:     0 gpio:    0 duty:   240 last-data-req:  236748966 last-cmd:  236795656
cycle:  241000025 sample:    242 adc:  1818 eqep:     0 gpio:    0 duty:   241 last-data-req:  236748966 last-cmd:  236795656
cycle:  242000033 sample:    243 adc:  1822 eqep:     0 gpio:    0 duty:   242 last-data-req:  236748966 last-cmd:  236795656
cycle:  243000027 sample:    244 adc:  1804 eqep:     0 gpio:    0 duty:   243 last-data-req:  242826684 last-cmd:  242874248
cycle:  244000037 sample:    245 adc:  1799 eqep:     0 gpio:    0 duty:   244 last-data-req:  242826684 last-cmd:  242874248
cycle:  245000032 sample:    246 adc:  1836 eqep:     0 gpio:    0 duty:   245 last-data-req:  242826684 last-cmd:  242874248
cycle:  246000040 sample:    247 adc:  1809 eqep:     0 gpio:    0 duty:   246 last-data-req:  242826684 last-cmd:  242874248
cycle:  247000041 sample:    248 adc:  1808 eqep:     0 gpio:    0 duty:   247 last-data-req:  242826684 last-cmd:  242874248
cycle:  248000030 sample:    249 adc:  1804 eqep:     0 gpio:    0 duty:   248 last-data-req:  242826684 last-cmd:  242874248
Cmd:
cycle:  242874248 sample:      0 gpio:    0 duty:   243
cycle:  242874248 sample:      0 gpio:    0 duty:   244
cycle:  242874248 sample:      0 gpio:    0 duty:   245
cycle:  242874248 sample:      0 gpio:    0 duty:   246
cycle:  242874248 sample:      0 gpio:    0 duty:   247
cycle:  242874248 sample:      0 gpio:    0 duty:   248
cycle:  242874248 sample:     -2 gpio:    0 duty:   249
cycle:  242874248 sample:     -2 gpio:    0 duty:   250
cycle:  242874248 sample:     -2 gpio:    0 duty:   251
cycle:  242874248 sample:     -2 gpio:    0 duty:   252
cycle:  242874248 sample:     -2 gpio:    0 duty:   253
cycle:  242874248 sample:     -2 gpio:    0 duty:   254
cycle:  242874248 sample:     -2 gpio:    0 duty:   255
cycle:  242874248 sample:     -2 gpio:    0 duty:   256
cycle:  242874248 sample:     -2 gpio:    0 duty:   257
cycle:  242874248 sample:     -2 gpio:    0 duty:   258
cycle:  242874248 sample:     -2 gpio:    0 duty:   259
cycle:  242874248 sample:     -2 gpio:    0 duty:   260
cycle:  242874248 sample:     -2 gpio:    0 duty:   261
cycle:  242874248 sample:     -2 gpio:    0 duty:   262
cycle:  242874248 sample:     -2 gpio:    0 duty:   263
cycle:  242874248 sample:     -2 gpio:    0 duty:   264
cycle:  242874248 sample:     -2 gpio:    0 duty:   265
cycle:  242874248 sample:     -2 gpio:    0 duty:   266
cycle:  242874248 sample:     -2 gpio:    0 duty:   267
cycle:  242874248 sample:     -2 gpio:    0 duty:   268
cycle:  242874248 sample:     -2 gpio:    0 duty:   269
cycle:  242874248 sample:     -2 gpio:    0 duty:   270
cycle:  242874248 sample:     -2 gpio:    0 duty:   271
cycle:  242874248 sample:     -2 gpio:    0 duty:   272
cycle:  242874248 sample:     -2 gpio:    0 duty:   273
cycle:  242874248 sample:     -2 gpio:    0 duty:   274

i=9:
Data:
cycle:  223000025 sample:    224 adc:  1826 eqep:     0 gpio:    0 duty:   223 last-data-req:  218520795 last-cmd:  218567941
cycle:  224000033 sample:    225 adc:  1817 eqep:     0 gpio:    0 duty:   224 last-data-req:  218520795 last-cmd:  218567941
cycle:  225000028 sample:    226 adc:  1811 eqep:     0 gpio:    0 duty:   225 last-data-req:  224594811 last-cmd:  224642525
cycle:  226000037 sample:    227 adc:  1795 eqep:     0 gpio:    0 duty:   226 last-data-req:  224594811 last-cmd:  224642525
cycle:  227000032 sample:    228 adc:  1815 eqep:     0 gpio:    0 duty:   227 last-data-req:  224594811 last-cmd:  224642525
cycle:  228000040 sample:    229 adc:  1804 eqep:     0 gpio:    0 duty:   228 last-data-req:  224594811 last-cmd:  224642525
cycle:  229000041 sample:    230 adc:  1823 eqep:     0 gpio:    0 duty:   229 last-data-req:  224594811 last-cmd:  224642525
cycle:  230000030 sample:    231 adc:  1820 eqep:     0 gpio:    0 duty:   230 last-data-req:  224594811 last-cmd:  224642525
cycle:  231000023 sample:    232 adc:  1817 eqep:     0 gpio:    0 duty:   231 last-data-req:  230673540 last-cmd:  230719413
cycle:  232000033 sample:    233 adc:  1815 eqep:     0 gpio:    0 duty:   232 last-data-req:  230673540 last-cmd:  230719413
cycle:  233000028 sample:    234 adc:  1824 eqep:     0 gpio:    0 duty:   233 last-data-req:  230673540 last-cmd:  230719413
cycle:  234000036 sample:    235 adc:  1807 eqep:     0 gpio:    0 duty:   234 last-data-req:  230673540 last-cmd:  230719413
cycle:  235000037 sample:    236 adc:  1819 eqep:     0 gpio:    0 duty:   235 last-data-req:  230673540 last-cmd:  230719413
cycle:  236000026 sample:    237 adc:  1821 eqep:     0 gpio:    0 duty:   236 last-data-req:  230673540 last-cmd:  230719413
cycle:  237000038 sample:    238 adc:  1823 eqep:     0 gpio:    0 duty:   237 last-data-req:  236748966 last-cmd:  236795656
cycle:  238000039 sample:    239 adc:  1816 eqep:     0 gpio:    0 duty:   238 last-data-req:  236748966 last-cmd:  236795656
cycle:  239000034 sample:    240 adc:  1823 eqep:     0 gpio:    0 duty:   239 last-data-req:  236748966 last-cmd:  236795656
cycle:  240000023 sample:    241 adc:  1817 eqep:     0 gpio:    0 duty:   240 last-data-req:  236748966 last-cmd:  236795656
cycle:  241000025 sample:    242 adc:  1818 eqep:     0 gpio:    0 duty:   241 last-data-req:  236748966 last-cmd:  236795656
cycle:  242000033 sample:    243 adc:  1822 eqep:     0 gpio:    0 duty:   242 last-data-req:  236748966 last-cmd:  236795656
cycle:  243000027 sample:    244 adc:  1804 eqep:     0 gpio:    0 duty:   243 last-data-req:  242826684 last-cmd:  242874248
cycle:  244000037 sample:    245 adc:  1799 eqep:     0 gpio:    0 duty:   244 last-data-req:  242826684 last-cmd:  242874248
cycle:  245000032 sample:    246 adc:  1836 eqep:     0 gpio:    0 duty:   245 last-data-req:  242826684 last-cmd:  242874248
cycle:  246000040 sample:    247 adc:  1809 eqep:     0 gpio:    0 duty:   246 last-data-req:  242826684 last-cmd:  242874248
cycle:  247000041 sample:    248 adc:  1808 eqep:     0 gpio:    0 duty:   247 last-data-req:  242826684 last-cmd:  242874248
cycle:  248000030 sample:    249 adc:  1804 eqep:     0 gpio:    0 duty:   248 last-data-req:  242826684 last-cmd:  242874248
cycle:  249000023 sample:    250 adc:  1826 eqep:     0 gpio:    0 duty:   249 last-data-req:  248904086 last-cmd:  248948401
cycle:  250000033 sample:    251 adc:  1812 eqep:     0 gpio:    0 duty:   250 last-data-req:  248904086 last-cmd:  248948401
cycle:  251000028 sample:    252 adc:  1811 eqep:     0 gpio:    0 duty:   251 last-data-req:  248904086 last-cmd:  248948401
cycle:  252000036 sample:    253 adc:  1822 eqep:     0 gpio:    0 duty:   252 last-data-req:  248904086 last-cmd:  248948401
cycle:  253000037 sample:    254 adc:  1815 eqep:     0 gpio:    0 duty:   253 last-data-req:  248904086 last-cmd:  248948401
cycle:  254000026 sample:    255 adc:  1804 eqep:     0 gpio:    0 duty:   254 last-data-req:  248904086 last-cmd:  248948401
Cmd:
cycle:  248948401 sample:      0 gpio:    0 duty:   249
cycle:  248948401 sample:      0 gpio:    0 duty:   250
cycle:  248948401 sample:      0 gpio:    0 duty:   251
cycle:  248948401 sample:      0 gpio:    0 duty:   252
cycle:  248948401 sample:      0 gpio:    0 duty:   253
cycle:  248948401 sample:      0 gpio:    0 duty:   254
cycle:  248948401 sample:     -2 gpio:    0 duty:   255
cycle:  248948401 sample:     -2 gpio:    0 duty:   256
cycle:  248948401 sample:     -2 gpio:    0 duty:   257
cycle:  248948401 sample:     -2 gpio:    0 duty:   258
cycle:  248948401 sample:     -2 gpio:    0 duty:   259
cycle:  248948401 sample:     -2 gpio:    0 duty:   260
cycle:  248948401 sample:     -2 gpio:    0 duty:   261
cycle:  248948401 sample:     -2 gpio:    0 duty:   262
cycle:  248948401 sample:     -2 gpio:    0 duty:   263
cycle:  248948401 sample:     -2 gpio:    0 duty:   264
cycle:  248948401 sample:     -2 gpio:    0 duty:   265
cycle:  248948401 sample:     -2 gpio:    0 duty:   266
cycle:  248948401 sample:     -2 gpio:    0 duty:   267
cycle:  248948401 sample:     -2 gpio:    0 duty:   268
cycle:  248948401 sample:     -2 gpio:    0 duty:   269
cycle:  248948401 sample:     -2 gpio:    0 duty:   270
cycle:  248948401 sample:     -2 gpio:    0 duty:   271
cycle:  248948401 sample:     -2 gpio:    0 duty:   272
cycle:  248948401 sample:     -2 gpio:    0 duty:   273
cycle:  248948401 sample:     -2 gpio:    0 duty:   274
cycle:  248948401 sample:     -2 gpio:    0 duty:   275
cycle:  248948401 sample:     -2 gpio:    0 duty:   276
cycle:  248948401 sample:     -2 gpio:    0 duty:   277
cycle:  248948401 sample:     -2 gpio:    0 duty:   278
cycle:  248948401 sample:     -2 gpio:    0 duty:   279
cycle:  248948401 sample:     -2 gpio:    0 duty:   280
root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/07-actuate-specific-ctrl-sequence# 

You can see that the pru is setting the sn's to 0 after it executes them.

The idea here is that you can have a cmd buf executed once, immediately, without having to care about the sample num.




///////////////////////////////////////////////////////////////////

test06-reconstruct-cmd-seq


- This test requires some thought.
- Suppose want to apply duty = f(k) for sample number k=1,2,3,...
- want to verify on the oscope this is actually workiung
- also should work if cpu actuates several times in a timestep
- should work "as well as possible" if cpu gets preempted or otherwise falls behind.


tests
	- 5ms/timestep, 

























///////////////////////
old: 
/home/debian/BB_PRU_stuff/Integration-test/06-play-recorded-sensors/readme.txt

Play pre-recorded sensor data to the BB and see if the solver computes the same answers (motor voltages) ("optU") as the matlab code that generated the BB's C code.

file of interest:

root@beaglebone:/home/debian/BB_PRU_stuff/Integration-test/06-play-recorded-sensors# 

20160917_Ts20_T5_L3_angle_only-header-row-FAKE_DATA_COMPARE.c

- gotta be careful you're using the same weights uWeight, dWeight, etc.

Conclusion: 



- ensure you're computing the ref the same way. At first we piped in Matlab's ref to the BB, then tested whether BB was computing ref correctly. Hard bc was offsetting by pru_start_time. But we got it working.

- dangerous to use sign(sin(w*t)) with w=3*PI because at integer multiples, sin() will be close to 0. A little pos or neg will cause sign() to slam up or down. Numerical differences btwn matlab and BB can cause differencse, see tstep 46 adn 47. see below.

- unexplained discrepancy in tstep 14. see below.




Unexplained discrepancy in optU btwn matlab and BB in tstep 14:


======================
top of tstep=13

Fake upasts:
-0.000000 0.000000 -0.000000 
Fake ypasts:
3.141593 3.141593 3.141593 3.141593 
Fake optUs:
-1.907011 -4.000000 4.000000 -4.000000 4.000000 
Fake refs:
3.141593 3.141593 3.141593 -3.141593 
pru time: 1.300000 (delta: 0.000000) status: 0 iter: 9 time: 0.007777
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
 10:status=0, cost=  4.83318e+03, -4.83318e+03, |eq|=  5.00e-09, ineq=  1.14e-08,
              dual=  5.09e-07, gap=  1.28e-04, last alpha=  9.90e-01, |grad|=  1.03e-03 (8506.6us,850.66us/iter)
tstep: 13, optU matlab: -1.90701, BB: -1.90701
tstep: 13, optU matlab: -4, BB: -4
tstep: 13, optU matlab: 4, BB: 4
tstep: 13, optU matlab: -4, BB: -4
tstep: 13, optU matlab: 4, BB: 4

optJ:
4833.18 
optU:
-1.90701 -4 4 -4 4 
optD:
1.07528e-09 2.66652e-06 6.78076e-05 -2.48405e-12 0.000128356 -3.12458e-07 1.5444e-07 0 
optEnergy:
-50.7679 
optNoise:
6.66134e-15 1.62252e-11 3.95366e-10 -4.73587e-10 
ref:
3.14159 3.14159 3.14159 -3.14159 
ref Matlab:
3.14159 3.14159 3.14159 -3.14159 
======================
top of tstep=14


oh my!!!


Fake upasts:
0.000000 -0.000000 -1.907011 
Fake ypasts:
3.141593 3.141593 3.141593 3.141593 
Fake optUs:
-3.999997 -1.194927 3.999982 -3.999988 3.999986 
Fake refs:
3.141593 3.141593 -3.141593 -3.141593 
pru time: 1.440000 (delta: 0.140000) status: 0 iter: 10 time: 0.008507
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
 13:status=0, cost=  4.89142e+03, -4.89142e+03, |eq|=  1.42e-14, ineq=  3.99e-08,
              dual=  2.06e-06, gap=  1.23e-03, last alpha=  9.90e-01, |grad|=  8.25e-04 (5184.6us,398.81us/iter)
tstep: 14, optU matlab: -4, BB: -4
tstep: 14, optU matlab: -1.19493, BB: -1.18954
tstep: 14, optU matlab: 3.99998, BB: -0.530329
tstep: 14, optU matlab: -3.99999, BB: -1.46873
tstep: 14, optU matlab: 3.99999, BB: 0.423499

optJ:
4891.42 
optU:
-4 -1.18954 -0.530329 -1.46873 0.423499 
optD:
-2.14485e-10 6.76903e-05 4.36854e-07 0.00012793 7.91113e-08 -7.34379e-08 1.63513e-08 0 
optEnergy:
7.47379 
optNoise:
-1.33227e-15 4.12066e-10 -4.39043e-10 -3.54112e-11 
ref:
3.14159 3.14159 -3.14159 -3.14159 
ref Matlab:
3.14159 3.14159 -3.14159 -3.14159 oh my (tstep=14, the firsrt time optU differs from Matlab)!!!
======================
top of tstep=15

Fake upasts:
-0.000000 -1.907011 -3.999997 
Fake ypasts:
3.141593 3.141593 3.141593 3.141593 
Fake optUs:
-1.199591 1.867169 -3.999994 3.999995 -3.999995 
Fake refs:
3.141593 -3.141593 -3.141593 -3.141593 
pru time: 1.440000 (delta: 0.000000) status: 0 iter: 13 time: 0.005185
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
  5:status=0, cost=  4.83897e+03, -4.83897e+03, |eq|=  5.68e-14, ineq=  5.21e-06,
              dual=  1.39e-06, gap=  6.07e-04, last alpha=  9.90e-01, |grad|=  5.30e-04 (1941.9us,388.38us/iter)
tstep: 15, optU matlab: -1.19959, BB: -1.19959
tstep: 15, optU matlab: 1.86717, BB: 1.86717
tstep: 15, optU matlab: -3.99999, BB: -3.99999
tstep: 15, optU matlab: 4, BB: 3.99999
tstep: 15, optU matlab: -4, BB: -3.99999

optJ:
4838.97 
optU:
-1.19959 1.86717 -3.99999 3.99999 -3.99999 
optD:
1.96997e-09 2.62547e-07 0.000127499 7.90999e-08 -7.3052e-08 3.12458e-07 -1.5444e-07 0 
optEnergy:
-45.0155 
optNoise:
1.19904e-14 1.5854e-12 7.74411e-10 -8.34056e-10 
ref:
3.14159 -3.14159 -3.14159 -3.14159 
ref Matlab:
3.14159 -3.14159 -3.14159 -3.14159 
======================
top of tstep=16

Fake upasts:
-1.907011 -3.999997 -1.199591 
Fake ypasts:
3.141593 3.141593 3.141593 1.610094 
Fake optUs:
1.861621 0.974346 -0.000214 0.004608 -0.004809 
Fake refs:
-3.141593 -3.141593 -3.141593 -3.141593 
pru time: 1.440000 (delta: 0.000000) status: 0 iter: 5 time: 0.001942
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
  6:status=0, cost=  2.54269e+03, -2.54269e+03, |eq|=  2.84e-14, ineq=  5.00e-01,
              dual=  1.05e-07, gap=  2.12e-04, last alpha=  9.90e-01, |grad|=  1.23e-03 (2335.4us,389.23us/iter)
tstep: 16, optU matlab: 1.86162, BB: 1.86162
tstep: 16, optU matlab: 0.974346, BB: 0.974346
tstep: 16, optU matlab: -0.000214, BB: -0.000213759
tstep: 16, optU matlab: 0.004608, BB: 0.00460742
tstep: 16, optU matlab: -0.004809, BB: -0.00480884

optJ:
2542.69 
optU:
1.86162 0.974346 -0.000213759 0.00460742 -0.00480884 
optD:
2.27354e-07 0.00012714 -7.35011e-07 -7.30511e-08 -7.63985e-08 3.67864e-10 -1.8567e-10 0 
optEnergy:
4.20866 
optNoise:
1.38423e-12 7.72479e-10 -8.36841e-10 -1.52116e-09 
ref:
-3.14159 -3.14159 -3.14159 -3.14159 
ref Matlab:
-3.14159 -3.14159 -3.14159 -3.14159 
======================



... optUs matche betwn matlab and BB ...




Discrepancy, I think bc sign(sin(w*t)) numerical errors slamming sign differences. Results in different refs. 

======================
top of tstep=45

Fake upasts:
-0.000000 0.000000 -0.000000 
Fake ypasts:
3.141593 3.141593 3.141593 3.141593 
Fake optUs:
0.000000 -0.000000 0.000000 -0.000000 0.000000 
Fake refs:
3.141593 3.141593 3.141593 3.141593 
pru time: 2.080000 (delta: 0.060000) status: 0 iter: 9 time: 0.003434
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
  9:status=0, cost=  2.40078e-10, -2.40078e-10, |eq|=  3.33e-16, ineq=  5.00e-01,
              dual=  2.50e-16, gap=  2.60e-14, last alpha=  9.90e-01, |grad|=  2.47e-03 (3997.9us,444.21us/iter)
tstep: 45, optU matlab: 0, BB: -4.3137e-07
tstep: 45, optU matlab: -0, BB: -1.52043e-08
tstep: 45, optU matlab: 0, BB: 2.60979e-09
tstep: 45, optU matlab: -0, BB: -2.13079e-07
tstep: 45, optU matlab: 0, BB: 2.08242e-07

optJ:
2.40078e-10 
optU:
-4.3137e-07 -1.52043e-08 2.60979e-09 -2.13079e-07 2.08242e-07 
optD:
1.03603e-15 5.46956e-11 2.78264e-11 1.08634e-18 1.66552e-14 -1.64535e-14 8.04025e-15 0 
optEnergy:
4.01969e-14 
optNoise:
4.44089e-16 4.44089e-16 0 -4.44089e-16 
ref:
3.14159 3.14159 3.14159 3.14159 
ref Matlab:
3.14159 3.14159 3.14159 3.14159 
======================
top of tstep=46

Fake upasts:
0.000000 -0.000000 0.000000 
Fake ypasts:
3.141593 3.141593 3.141593 3.141593 
Fake optUs:
-1.907011 -4.000000 4.000000 -4.000000 4.000000 
Fake refs:
3.141593 3.141593 3.141593 -3.141593 
pru time: 2.100000 (delta: 0.020000) status: 0 iter: 9 time: 0.003998
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
  9:status=0, cost=  2.40078e-10, -2.40078e-10, |eq|=  3.33e-16, ineq=  5.00e-01,
              dual=  2.50e-16, gap=  2.60e-14, last alpha=  9.90e-01, |grad|=  2.47e-03 (3471.5us,385.72us/iter)
tstep: 46, optU matlab: -1.90701, BB: -4.3137e-07
tstep: 46, optU matlab: -4, BB: -1.52043e-08
tstep: 46, optU matlab: 4, BB: 2.60979e-09
tstep: 46, optU matlab: -4, BB: -2.13079e-07
tstep: 46, optU matlab: 4, BB: 2.08242e-07

optJ:
2.40078e-10 
optU:
-4.3137e-07 -1.52043e-08 2.60979e-09 -2.13079e-07 2.08242e-07 
optD:
1.03603e-15 5.46956e-11 2.78264e-11 1.08634e-18 1.66552e-14 -1.64535e-14 8.04025e-15 0 
optEnergy:
4.01969e-14 
optNoise:
4.44089e-16 4.44089e-16 0 -4.44089e-16 
ref:
3.14159 3.14159 3.14159 3.14159 
ref Matlab:
3.14159 3.14159 3.14159 -3.14159 
======================
top of tstep=47

Fake upasts:
-0.000000 0.000000 -1.907011 
Fake ypasts:
3.141593 3.141593 3.141593 3.141593 
Fake optUs:
-4.000000 -1.182726 -3.999997 3.999996 -3.999997 
Fake refs:
3.141593 3.141593 -3.141593 -3.141593 
pru time: 2.100000 (delta: 0.000000) status: 0 iter: 9 time: 0.003471
ipmPDeq_CSsolver (skipAffine=0,delta=3,allowSave=1): 37 primal variables (5+11), 21 eq. constr., 26 ineq. constr.
 10:status=0, cost=  4.92297e+03, -4.92297e+03, |eq|=  1.29e-09, ineq=  2.83e-06,
              dual=  7.79e-06, gap=  6.84e-03, last alpha=  9.31e-01, |grad|=  7.83e-04 (3707.4us,370.74us/iter)
tstep: 47, optU matlab: -4, BB: 0.433252
tstep: 47, optU matlab: -1.18273, BB: -4
tstep: 47, optU matlab: -4, BB: 4
tstep: 47, optU matlab: 4, BB: -4
tstep: 47, optU matlab: -4, BB: 3.99999

optJ:
4922.97 
optU:
0.433252 -4 4 -4 3.99999 
optD:
-2.41281e-09 -0.000116568 -7.25159e-05 7.36279e-08 9.50086e-05 -3.12458e-07 1.5444e-07 0 
optEnergy:
-87.3667 
optNoise:
-1.46549e-14 -7.09594e-10 3.19242e-10 1.87386e-09 
ref:
3.14159 3.14159 3.14159 -3.14159 
ref Matlab:
3.14159 3.14159 -3.14159 -3.14159 









