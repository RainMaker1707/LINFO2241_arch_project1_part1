make clean
make server-queue client-queue


./server-queue -j 1 -s 1024 -p 1707 -v > output/server-queue.txt & PID=$!
sleep 2s
./client-queue -r 10 -t 20 -k 8 127.0.0.1:1707 -v > output/client-queue.txt
kill -2 "${PID}"

make clean
