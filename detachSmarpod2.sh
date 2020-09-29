## Using the thing for an arduino for now

DEVICE="/dev/SmarPod"
echo "Looking for" $DEVICE

## Find the full sysfs path of the device
BUSPATH=$(udevadm info -q path -n $DEVICE)
echo $BUSPATH
if [ -z "$BUSPATH" ]
then
    :
else
    ## Just get the bit we need to write into /driver/unbind
    BUSPORT=${BUSPATH##*/}

    ## Unbind the device from the its driver.
    #echo $BUSPORT | sudo tee /sys$BUSPATH/driver/unbind
    #if [ ${PIPESTATUS[1]} -eq "0" ]
    #then
    #    echo "Driver unbound"
    #else
    #    echo "unbind fail"
    #fi
    echo "${BUSPORT}:1.0" | sudo tee /sys/bus/usb/drivers/ftdi_sio/unbind
    echo "${BUSPORT}:1.0" | sudo tee /sys/bus/usb/drivers/usbserial_generic/bind
fi
