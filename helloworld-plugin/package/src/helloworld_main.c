/****************************************************************************
**
** Copyright (c) 2020 SoftAtHome
**
** Redistribution and use in source and binary forms, with or
** without modification, are permitted provided that the following
** conditions are met:
**
** 1. Redistributions of source code must retain the above copyright
** notice, this list of conditions and the following disclaimer.
**
** 2. Redistributions in binary form must reproduce the above
** copyright notice, this list of conditions and the following
** disclaimer in the documentation and/or other materials provided
** with the distribution.
**
** Subject to the terms and conditions of this license, each
** copyright holder and contributor hereby grants to those receiving
** rights under this license a perpetual, worldwide, non-exclusive,
** no-charge, royalty-free, irrevocable (except for failure to
** satisfy the conditions of this license) patent license to make,
** have made, use, offer to sell, sell, import, and otherwise
** transfer this software, where such license applies only to those
** patent claims, already acquired or hereafter acquired, licensable
** by such copyright holder or contributor that are necessarily
** infringed by:
**
** (a) their Contribution(s) (the licensed copyrights of copyright
** holders and non-copyrightable additions of contributors, in
** source or binary form) alone; or
**
** (b) combination of their Contribution(s) with the work of
** authorship to which such Contribution(s) was added by such
** copyright holder or contributor, if, at the time the Contribution
** is added, such addition causes such combination to be necessarily
** infringed. The patent license shall not apply to any other
** combinations which include the Contribution.
**
** Except as expressly stated above, no rights or licenses from any
** copyright holder or contributor is granted under this license,
** whether expressly, by implication, estoppel or otherwise.
**
** DISCLAIMER
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
** CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
** MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR
** CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
** USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
** AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
** ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
**
****************************************************************************/

#include <stdio.h>

#include "helloworld.h"
#include "dm_helloworld.h"
static helloworld_app_t app;

amxd_dm_t* PRIVATE helloworld_get_dm(void) {
    return app.dm;
}


amxo_parser_t* PRIVATE helloworld_get_parser(void) {
    return app.parser;
}
void check_enable(void){
    bool enable;
    amxd_object_t* helloworld = amxd_dm_findf(helloworld_get_dm(),"HelloWorld.");
    enable = amxd_object_get_bool(helloworld, "Enable", NULL);
    if(enable){
        SAH_TRACEZ_INFO(ME, "helloworld invoked");
        amxd_object_t* message = amxd_object_findf(helloworld,"Message.");
        amxd_object_for_each(instance,it,message) {
            amxd_object_t* message_instance = amxc_container_of(it, amxd_object_t, it);
                SAH_TRACEZ_INFO(ME,"entering for each %s",  amxd_object_get_cstring_t(message_instance,"Alias",NULL));
                char* author = amxd_object_get_cstring_t(message_instance,"Author",NULL);
                uint32_t secs = amxd_object_get_value(uint32_t, message_instance, "Interval", NULL);
                message_data_t* msg_data = (message_data_t*) calloc(1, sizeof(message_data_t));
                msg_data->object_in = message_instance;
                message_instance->priv = msg_data;
                amxp_timer_new(&(msg_data->timer), helloworld_message, message_instance->priv);
                amxp_timer_set_interval(msg_data->timer, secs * 1000);
                amxp_timer_start(msg_data->timer, 0);

        }
        SAH_TRACEZ_INFO(ME, "For each invoked");
    }
    else{
        SAH_TRACEZ_INFO(ME,"hello world message not invoked");
    }
}

void helloworldserver_init(void) {
    amxd_object_t* helloworld = amxd_dm_findf(helloworld_get_dm(),"HelloWorld.");
    if(helloworld != NULL){
        SAH_TRACEZ_INFO(ME, "Starting %s plugin", ME);
        bool enable = amxd_object_get_bool(helloworld,"Enable",NULL);
        if (enable) {
            SAH_TRACEZ_INFO(ME, "Enabled %s server", ME);
            uint32_t secs = amxd_object_get_value(uint32_t, helloworld, "Interval", NULL);
            amxp_timer_t* timer = (amxp_timer_t*) helloworld->priv;
            SAH_TRACEZ_INFO(ME,"Handling HelloWorld.Enable=%d...", enable ? 1 : 0);
            if(timer == NULL) {
                amxp_timer_new(&timer, helloworld_message, NULL);
                helloworld->priv = timer;
            }
            if(secs == 0) {
                amxp_timer_stop(timer);
                goto exit;
            }        
            amxp_timer_set_interval(timer, secs * 1000);
            amxp_timer_start(timer, 0);
        }

    } else {
        SAH_TRACEZ_ERROR(ME, "Cannot start %s , missing DataModel", ME);
        goto exit;
    }

exit:
 return;
}

int _helloworld_main(int reason,
                  amxd_dm_t* dm,
                  amxo_parser_t* parser) {

    switch(reason) {
    case 0:     // START
        app.dm = dm;
        app.parser = parser;
        check_enable();
        break;
    case 1:     // STOP
        app.dm = NULL;
        app.parser = NULL;
        SAH_TRACEZ_INFO(ME, "Stopping %s", ME);
        break;
    }
    return 0;
}