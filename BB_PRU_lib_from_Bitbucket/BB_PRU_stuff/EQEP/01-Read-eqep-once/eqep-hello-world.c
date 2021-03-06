/** Program to load a PRU program that flashes an LED until a button is
*   pressed. By Derek Molloy, for the book Exploring BeagleBone
*   based on the example code at:
*   http://processors.wiki.ti.com/index.php/PRU_Linux_Application_Loader_API_Guide
*/

#include <stdio.h>
#include <stdlib.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include <unistd.h> // for unsigned int sleep(unsigned int seconds);

#define PRU_NUM	   0   // using PRU0 for these examples
#define MMAP_LOC   "/sys/class/uio/uio0/maps/map0/"

enum FREQUENCY {    // measured and calibrated, but can be calculated
	FREQ_12_5MHz =  1,
	FREQ_6_25MHz =  5,
	FREQ_5MHz    =  7,
	FREQ_3_85MHz = 10,
	FREQ_1MHz   =  47,
	FREQ_500kHz =  97,
	FREQ_250kHz = 247,
	FREQ_100kHz = 497,
	FREQ_25kHz = 1997,
	FREQ_10kHz = 4997,
	FREQ_5kHz =  9997,
	FREQ_2kHz = 24997,
	FREQ_1kHz = 49997
};

enum CONTROL_PKT {
	PAUSED = 0,
	RUNNING = 1,
	UPDATE = 3
};

unsigned int readFileValue(char filename[]){
   FILE* fp;
   unsigned int value = 0;
   fp = fopen(filename, "rt");
   fscanf(fp, "%x", &value);
   fclose(fp);
   return value;
}

int main (void)
{

  printf("Hey, if you get a seg fault, make sure the EBB-PRU-Example device tree overlay has been loaded!!\n");

  printf("Here's the cmd: root@beaglebone:/home/debian/beaglebone_pru_adc/src# echo EBB-PRU-Example > /sys/devices/bone_capemgr.9/slots\n");

   if(getuid()!=0){
      printf("You must run this program as root. Exiting.\n");
      exit(EXIT_FAILURE);
   }
   // Initialize structure used by prussdrv_pruintc_intc
   // PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h
   tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

   // Read in the location and address of the shared memory. This value changes
   // each time a new block of memory is allocated.

	// bit 0: old=0/new=1 pkt
	// bit 1: run=1/stop=0



   // Allocate and initialize memory
   prussdrv_init ();
   prussdrv_open (PRU_EVTOUT_0);

   // Map PRU's interrupts
   prussdrv_pruintc_init(&pruss_intc_initdata);

   void *pru0DataMemory;
   unsigned int *pru0DataMemory_int;
   // Copy data to PRU memory - different way
   prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pru0DataMemory);
   pru0DataMemory_int = (unsigned int *) pru0DataMemory;

   // Use the second 4 bytes for the num of low iters
   //   *(pru0DataMemory_int+1) = 1000;
   // Use the second 4 bytes for the num of high iters
   //   *(pru0DataMemory_int+2) = 2000;
   // Use the first 4 bytes for the cmd pkt
   // Comes last because don't want PRU to read before all vals are written
   //   *pru0DataMemory_int = 3; // 3: "new pkt (bit 0)"=1 and "run (bit 1)"=1

   //   printf("Running PRU 66percent duty.\n");

   // Load and execute the PRU program on the PRU
   const char* binname =  "./eqep-hello-world.bin";
   printf("executing bin file: %s\n",binname);
   prussdrv_exec_program (PRU_NUM, binname);

 printf("Waiting...\n"); 

   /* int n = prussdrv_pru_wait_event (PRU_EVTOUT_0); */
   /* printf("PRU program completed, event number %d.\n", n); */
   /* printf("The data that is in memory is:\n"); */
   /* printf("- the number of samples used is %d.\n", *pru0DataMemory_int); */
   /* printf("- the time delay used is %d.\n", *(pru0DataMemory_int+1)); */
   /* unsigned int raw_distance = *(pru0DataMemory_int+2); */
   /* printf("- the last distance sample is %d.\n", raw_distance); */

   //////////////////
   // Works:
   /* printf("Sleeping."); */

   /* sleep(5); */

   /* printf("Shutting down."); */
   /* *pru0DataMemory_int = 0; */
   ///////////

   sleep(1);

   //   printf("Setting to 50percent duty \n");

   // Use the second 4 bytes for the num of low iters
   //   *(pru0DataMemory_int+1) = 2000;
   // Use the second 4 bytes for the num of high iters
   //   *(pru0DataMemory_int+2) = 2000;
   // Use the first 4 bytes for the cmd pkt
   // comes last because don't want PRU to read before all vals are written
   // *pru0DataMemory_int = 3; // 3: "new pkt bit"=1 and "run"=1

   unsigned int val, val2;
   int i;
   int max=150;
   int usec_delay = 200000;
   for( i=0; i<max; i++) {
     val =  *pru0DataMemory_int;
     val2 =  *(pru0DataMemory_int+1);
     printf("Iter %d/%d, val0: %u\t\t val1: %u\n",i,max,val,val2);
     usleep(usec_delay);
   }





   /* printf("Sending HALT pkt\n."); */
   /* *pru0DataMemory_int = 1; // "new pkt"(1) and "stop"(0) */
   /* printf("It should've stopped by now!\n"); */

   /* sleep(2); */

   printf("Shutting down.\n");
   prussdrv_pru_disable(PRU_NUM);
   prussdrv_exit ();
   return EXIT_SUCCESS;
}
