#!/bin/sh
wget -q -O /dev/null "http://127.0.0.1/control/message?popup=E2PLI activated!%20Rebooting now.."
(sleep 6;rm /etc/.nhd2;touch /etc/.start_enigma2;reboot -f &) &
exit
