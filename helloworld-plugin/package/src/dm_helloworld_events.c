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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "helloworld.h"
#include "dm_helloworld.h"

void _print_event(const char* const sig_name,
    const amxc_var_t* const data,
    UNUSED void* const priv) {
        printf("Signal received - %s\n", sig_name);
        printf("Signal data = \n");
        fflush(stdout);
        if(!amxc_var_is_null(data)) {
        amxc_var_dump(data, STDOUT_FILENO);
        }

}

void _update_message_instance_timer(const char* const sig_name,
    const amxc_var_t* const data,
    void* const priv) {
    amxd_object_t* message_instance = amxd_dm_signal_get_object(helloworld_get_dm(), data);
    uint32_t secs = GETP_UINT32(data, "parameters.Interval.to");
    char* author = amxd_object_get_cstring_t(message_instance,"Author",NULL);
    SAH_TRACEZ_INFO(ME,"12354689797879987 message invoked by %s",author);
    SAH_TRACEZ_INFO(ME,"Interval changed to %d seconds" ,secs);
    message_data_t* msg_data = (message_data_t*) message_instance->priv;
    //amxp_timer_t* timer = (amxp_timer_t*) msg_data->timer;
    if(msg_data->timer != NULL) {
        SAH_TRACEZ_INFO(ME,"timer found");
        if(secs == 0) {
            amxp_timer_stop(msg_data->timer);
        } else {
            amxp_timer_stop(msg_data->timer);
            amxp_timer_set_interval(msg_data->timer, secs * 1000);
            amxp_timer_start(msg_data->timer, 0);
        }
    } else {
        SAH_TRACEZ_INFO(ME,"timer not found");
    }
    return ;
}
void _helloworld_server_enable(const char* const sig_name,
    const amxc_var_t* const data,
    void* const priv) {
    static bool in_helloworld_server_enable = false;
    if (in_helloworld_server_enable) {
        return;
    }
    in_helloworld_server_enable = true;
    SAH_TRACEZ_INFO(ME, "Event triggered: sig_name=%s", sig_name);
    amxd_object_t* helloworld = amxd_dm_signal_get_object(helloworld_get_dm(), data);
    if (helloworld == NULL) {
        in_helloworld_server_enable = false;
        return;
    }
    bool global_enable = amxd_object_get_bool(helloworld, "Enable", NULL);
    if (global_enable) {
        SAH_TRACEZ_INFO(ME, "HelloWorld.Enable is set to 1");
    } else {
        SAH_TRACEZ_INFO(ME, "HelloWorld.Enable is set to 0");
    }
    amxd_object_t* message = amxd_object_findf(helloworld, "Message.");
    if (message != NULL) {
        amxd_object_for_each(instance, it, message) {
            amxd_object_t* message_instance = amxc_container_of(it, amxd_object_t, it);
            char* author = amxd_object_get_cstring_t(message_instance, "Author", NULL);
            bool message_enable = amxd_object_get_bool(message_instance, "Enable", NULL);
            if (author == NULL) {
                continue;
            }
            if (global_enable) {
                if (message_enable) {
                    activate(message_instance);
                } else {
                    desactivate(message_instance);
                }
            } else {
                desactivate(message_instance);
            }
        }
    } else {
        SAH_TRACEZ_INFO(ME, "No Message object found under HelloWorld");
    }
    in_helloworld_server_enable = false;
}
void _message_instance_enable(const char* const sig_name,
    const amxc_var_t* const data,
    void* const priv) {
    amxd_object_t* helloworld = amxd_dm_signal_get_object(helloworld_get_dm(), data);
    if (helloworld == NULL) {
        return;
    }
    amxd_object_t* parent = amxd_dm_findf(helloworld_get_dm(),"HelloWorld.");
    if (parent == NULL) {
        return;}
    bool global_enable = amxd_object_get_bool(parent, "Enable", NULL);
    char* author = amxd_object_get_cstring_t(helloworld, "Author", NULL);
    if (author == NULL) {
        return;
    }
    bool enable = amxd_object_get_bool(helloworld, "Enable", NULL);

    if (!global_enable) {
        desactivate(helloworld);
        return;
    }
    if (enable) {
        activate(helloworld);
    } else {
        desactivate(helloworld);
    }
}
void activate(amxd_object_t* object) {
    if (object == NULL) {
        SAH_TRACEZ_ERROR(ME, "Object is NULL, cannot activate");
        return;
    }
    uint32_t secs = amxd_object_get_value(uint32_t, object, "Interval", NULL);
    char* author = amxd_object_get_cstring_t(object, "Author", NULL);
    if (author == NULL) {
        SAH_TRACEZ_ERROR(ME, "Author is NULL for the object");
        return;
    }
    SAH_TRACEZ_INFO(ME, "Going to activate %s", author);
    message_data_t* msg_data = (message_data_t*)object->priv;
    if (msg_data == NULL) {
        msg_data = (message_data_t*)calloc(1, sizeof(message_data_t));
        if (msg_data == NULL) {
            return;
        }
        msg_data->object_in = object;
        object->priv = msg_data;
    }
    if (msg_data->timer == NULL) {
        amxp_timer_new(&(msg_data->timer), helloworld_message, msg_data);
        if (msg_data->timer == NULL) {
            free(msg_data);
            object->priv = NULL;
            return;
        }
    }
    if (secs == 0) {
        amxp_timer_stop(msg_data->timer);
        return;
    }
    amxp_timer_stop(msg_data->timer);
    amxp_timer_set_interval(msg_data->timer, secs * 1000);
    amxp_timer_start(msg_data->timer, 0);
}
 void desactivate ( amxd_object_t* object){
    char* author = amxd_object_get_cstring_t(object, "Author", NULL);
    SAH_TRACEZ_INFO(ME, "Going to deactivate %s", author);
    message_data_t* msg_data = (message_data_t*)object->priv;
    if (msg_data == NULL) {
        return;
    }
    if (msg_data->timer != NULL) {
        amxp_timer_stop(msg_data->timer);
        amxp_timer_delete(&(msg_data->timer));
    }
    free(msg_data);
    object->priv = NULL;
 }

amxd_status_t _add_message(amxd_object_t* object, UNUSED amxd_function_t* func, amxc_var_t* args, amxc_var_t* ret) {
    amxd_dm_t* dm = helloworld_get_dm();
    amxd_object_t* message = amxd_object_get (object , "Message");
    const char* Author = GET_CHAR(args, "Author");
    bool Enable = GET_BOOL(args, "Enable");
    uint32_t Interval = GET_UINT32(args, "Interval");
    amxd_trans_t transaction;
    amxd_trans_init(&transaction);
    amxd_trans_select_object(&transaction, message);
    amxd_trans_add_inst(&transaction, 0, NULL);
    amxd_trans_set_value(cstring_t, &transaction, "Author", Author);
    amxd_trans_set_value(bool, &transaction, "Enable", Enable);
    amxd_trans_set_value(uint32_t, &transaction, "Interval", Interval);
    amxc_var_set(bool,ret,amxd_trans_apply(&transaction, dm) == amxd_status_ok);
    amxd_trans_clean(&transaction);

    return amxd_status_ok;
}

amxd_status_t _delete_message_instance(amxd_object_t* object,
    UNUSED amxd_function_t* func,
    amxc_var_t* args,
    amxc_var_t* ret) {

amxd_status_t retval = amxd_status_unknown_error;
const char* Alias = GET_CHAR(args, "Alias");
amxd_object_t* message = NULL;
message = amxd_object_findf(object, "Message.%s.", Alias);
char* author = amxd_object_get_cstring_t(message,"Author",NULL);   
SAH_TRACEZ_INFO(ME,"this message is  in ff f  %s",author);
const uint32_t index = amxd_object_get_uint32_t(message,"index",NULL);
SAH_TRACEZ_INFO(ME,"index is %d",index);
    if (Alias == NULL) {
        SAH_TRACEZ_ERROR(ME, "Instance alias is NULL");
        return amxd_status_unknown_error;
    }
    if (message == NULL) {
        SAH_TRACEZ_ERROR(ME, "Instance with alias '%s' not found", Alias);
        return amxd_status_unknown_error;
    }
    if (message->priv != NULL) {
        message_data_t* msg_data = (message_data_t*)message->priv;
        if (msg_data->timer != NULL) {
            amxp_timer_stop(msg_data->timer);
            amxp_timer_delete(&(msg_data->timer));
        }
        free(msg_data);
        message->priv = NULL;
    }
    amxd_object_delete(&message);
    return amxd_status_ok;
}