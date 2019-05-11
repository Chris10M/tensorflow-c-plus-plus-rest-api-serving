 #!/bin/sh -x 
 
SOCKET_ADDRESS="0.0.0.0:52118" 
WORKERS=6
TIMEOUT=180

fuser -k 52118/tcp 

if [ -f ./bin/app ]; then
    printf "Starting app server at $SOCKET_ADDRESS ..!\n"
    ./bin/app --ip $SOCKET_ADDRESS --workers $WORKERS --timeout $TIMEOUT;
else
    printf "No Binary Found..!\n";
    read -n1 -r -p "Press y to continue... " key;
    printf "\n";
    if [ "$key" = 'y' ]; then
	printf "Buiding Binary..!\n";
    	cmake .;
    	make;
	printf "Binary Successfully Built..!\n";
        printf "Starting app server at $SOCKET_ADDRESS ..!\n";
	./bin/app --ip $SOCKET_ADDRESS --workers $WORKERS --timeout $TIMEOUT;
    else
        printf "Exiting Program\n";
    fi
fi

