savedcmd_/home/abdullah/Desktop/Image-Processor/kernelspace/image_driver.mod := printf '%s\n'   image_driver.o | awk '!x[$$0]++ { print("/home/abdullah/Desktop/Image-Processor/kernelspace/"$$0) }' > /home/abdullah/Desktop/Image-Processor/kernelspace/image_driver.mod