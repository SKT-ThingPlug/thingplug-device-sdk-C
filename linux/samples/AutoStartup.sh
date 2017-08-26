ip=$(hostname -I)
echo $ip
for i in 1 2 3 4 5
do
	echo ${i}
	check=$( echo "$ip" |cut -d ' ' -f${i} )
	if [ -z "$check" ]; then
		echo "empty ip"
		break
	else
		echo $check
		/home/nvidia/thingplug-device-sdk-C/linux/samples/output/ThingPlug_oneM2M_SDK #<<<<<<<<<< Setting Here
		break;
	fi
	sleep 2
done
