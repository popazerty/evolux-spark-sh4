BEGIN {
code_ok="\x60"
code_mu="\x71"
code_pw="\x74"
code_00="\x0b"
code_01="\x02"
code_02="\x03"
code_03="\x04"
code_04="\x05"
code_05="\x06"
code_06="\x07"
code_07="\x08"
code_08="\x09"
code_09="\x30"

#ok + 4xPoweroff = not Reboot
#ok + 2xMute = cam restart
code_reset=code_ok code_pw code_pw code_pw code_pw
code_cam=code_ok code_mu code_mu
}


{
 l=0
 s=s $0 "0"
 l=length(s)
 if(l==32)
 {
  s=substr(s,11,1)
  #system(sprintf("echo '%s' | hexdump -n1",s))
  ms=ms s
  ms=substr(ms,0,5)
  if(s==code_ok)
   ms=code_ok
  if(ms==code_reset)
  {
   system("reboot -f")
  }
  if(ms==code_cam)
  {
   system("myCam='oscam';myOptions='-c /usr/keys';isCam=`pidof $myCam`;for i in $isCam; do kill $i;done;sleep 5;/usr/bin/${myCam} ${myOptions} &")
   ms=code_mu
  }
  s=""
 }
}
