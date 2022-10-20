#! /bin/bash


IP="127.0.0.1"
PORT=2241

TIME=120
MAX_RATE=300

make clean
make server
make client

for((I=1; I<=1000; I++))
do
  for((J=2; J<=32768; J=J*2))
  do
    ./server -j ${I} -s ${J} -p ${PORT} &
    SERVER_PID=$(ps -A | grep "./server -j ${I} -s ${J} -p ${PORT}" | cut -d " " -f1 | xargs | cut -d " " -f1)
    for ((K=2; K<=J; K=K*2))
    do
      for ((R=1; R<MAX_RATE; R++)) # need change the 2 in max rate
      do
        ./client -k ${K} -r ${R} -t ${TIME} ${IP}:${PORT} > output/${I}${J}${K}${R}.txt
      done
    done
    kill "${SERVER_PID}"
  done
done
