### Compile

`make`

### Register the module

`insmod chardriver.ko`

See if it was successfully registered

`ls /dev`

You should see the `/chardriver` directory in the output


### Run user side program

- Compile: `gcc -o user-program user-program.c`
- Run: `sudo ./user-program`

### Unregister the module if wanted

`sudo rmmod chardriver.ko`



### Obs. 

 - Only run this driver in a virtual machine environment.
 - This was made on kernel 5.4.0-48-generic.

