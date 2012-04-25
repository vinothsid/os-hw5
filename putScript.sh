for ((i=0;i<5;i++))
do
	./client 6 6 5 PUT k"$i" v"$i"
done
