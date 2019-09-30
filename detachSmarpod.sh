## Using the thing for an arduino for now

DEVICE="/dev/noxbox"
echo "Looking for" $DEVICE

## Find the full sysfs path of the device
BUSPATH=$(udevadm info -q path -n $DEVICE)
if [ -z "$BUSPATH" ]
then
    :
else
    ## Just get the bit we need to write into /driver/unbind
    BUSPORT=${BUSPATH##*/}

    ## Unbind the device from the its driver.
    echo $BUSPORT | sudo tee /sys$BUSPATH/driver/unbind
    if [ ${PIPESTATUS[1]} -eq "0" ]
    then
        echo "Driver unbound"
    else
        echo "unbind fail"
    fi
fi
