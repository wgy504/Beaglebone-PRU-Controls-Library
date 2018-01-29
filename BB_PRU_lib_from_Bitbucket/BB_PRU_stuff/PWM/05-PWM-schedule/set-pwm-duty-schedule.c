#include "jpp-pru-lib.h"
#include <stdio.h> // printf
#include <stdlib.h> // exit
#include <string.h> //  memcpy
#include <unistd.h> // usleep
#include <math.h> // fmod
int main() {

  start_pru();

  CommandBuffer cb;

  for( int i=0; i<PKTS_PER_CMD_BUFFER; i++ ) {
    cb[i].cycle_count = 0x11111111;
    cb[i].sample_num  = 0x22222222;
    cb[i].gpio        = GPIO_CW;
    unsigned int duty = 0;
    /* if( 0 <= i && i < PKTS_PER_CMD_BUFFER/4 ) { */
    /*   duty = round(00.0 / 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD); */
    /* } else if( PKTS_PER_CMD_BUFFER/4 <= i && i < PKTS_PER_CMD_BUFFER/2 ) { */
    /*   duty = round(25.0 / 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD); */
    /* } else if( PKTS_PER_CMD_BUFFER/2 <= i && i < 3*PKTS_PER_CMD_BUFFER/4 ) { */
    /*   duty = round(50.0 / 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD); */
    /* } else if( 3*PKTS_PER_CMD_BUFFER/4 <= i && i < PKTS_PER_CMD_BUFFER ) { */
    /*   duty = round(75.0 / 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD); */
    /* } else { */
    /*   duty = round(100.0/ 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD); */
    /* } */
    if( i%2 == 0 ) {
      duty = round(10.0 / 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD);
    }
    else {
      duty = round(50.0 / 100.0 * _100MHZ_TICKS_PER_PWM_PERIOD);
    }

    cb[i].duty = duty;
    //    cb[i].duty = 333;
  }

  DataBuffer data_buf;
    
  printf("Data data_buf:\n");
  get_data_buf( &data_buf );
  print_data_buf( data_buf );

  cb[0].sample_num = data_buf[PKTS_PER_DATA_BUFFER-1].sample_num - 5;

  printf("Sending cmd...\n");
  set_cmd_buf( cb );

  printf("Sleeping for 20ms...\n");
  usleep(20000);

  printf("Data after:\n");
  get_data_buf( &data_buf );
  print_data_buf( data_buf );

  printf("Sleeping for 2 sec...\n");
  sleep(2);

#if 1
  CommandBuffer b;
  make_const_cmd_buf(&b, 0, 0, GPIO_STBY, 0);
  printf("Sending STBY cmd...\n");
  set_cmd_buf( b );
#endif

  return(0);

}
