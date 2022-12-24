make clean
make server-float server-float-avx client


./server-float -j 1 -s 1024 -p 1707 & PID=$!
sleep 2s
./client -r 10 -t 20 -k 8 127.0.0.1:1707 -v > server_float_output.txt
kill -9 "${PID}"
./server-float-avx -j 1 -s 1024 -p 1707 & PID2=$!
sleep 2s
./client -r 10 -t 20 -k 8 127.0.0.1:1707 -v > server_avx_output.txt
kill -9 "${PID2}"
