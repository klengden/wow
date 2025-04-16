#!/bin/sh
name="helloworld"

case $1 in
    start|boot)
        ${name} -D
        ;;
    stop|shutdown)
        if [ -f "/var/run/${name}.pid" ]; then
            kill $(cat "/var/run/${name}.pid")
        fi
        ;;
    restart)
        $0 stop
        $0 start
        ;;
    *)
        echo "Usage : $0 [start|boot|stop|shutdown|debuginfo|restart]"
        ;;
esac
