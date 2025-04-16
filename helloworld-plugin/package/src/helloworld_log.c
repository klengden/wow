#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helloworld.h"
#include "dm_helloworld.h"

void helloworld_message(UNUSED amxp_timer_t* timer, void* priv){
    
    message_data_t* msg_data = (message_data_t*) priv;
    
    amxd_object_t* message_instance = (amxd_object_t*) msg_data->object_in;
   
    char* author = amxd_object_get_cstring_t(message_instance,"Author",NULL);
    
    SAH_TRACEZ_INFO(ME,"hello world message invoked by %s",author);
}