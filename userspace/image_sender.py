import posix_ipc
import mmap
import struct
import cv2
import numpy as np
import os

def receive_image_from_kernel(shared_memory_name="/image_transfer", memory_size=1024*1024):
    try:
        # Open existing shared memory
        memory = posix_ipc.SharedMemory(shared_memory_name)
        
        # Memory map the shared memory
        mapped_memory = mmap.mmap(memory.fd, memory_size)
        
        # Read image dimensions
        height, width = struct.unpack('II', mapped_memory.read(8))
        
        # Read image data
        image_data = np.frombuffer(mapped_memory.read(height * width), dtype=np.uint8)
        
        # Reshape image
        image = image_data.reshape((height, width))
        
        # Save processed image
        cv2.imwrite('processed_image.jpg', image)
        
        mapped_memory.close()
        memory.close_fd()
        
        return image
    
    except Exception as e:
        print(f"Image receive error: {e}")
        return None

def read_kernel_raw_image(file_path='/home/abdullah/Desktop/Image-Processor/kernelspace/processed_image.raw'):
    try:
        with open(file_path, 'rb') as f:
            # Read dimensions
            height = int.from_bytes(f.read(4), byteorder='little')
            width = int.from_bytes(f.read(4), byteorder='little')
            
            # Read image data
            image_data = np.frombuffer(f.read(), dtype=np.uint8)
            
            # Reshape image
            image = image_data.reshape((height, width))
            
            # Save as image
            cv2.imwrite('kernel_processed_image.jpg', image)
            
            return image
    
    except Exception as e:
        print(f"Error reading kernel image: {e}")
        return None

if __name__ == "__main__":
    # Try receiving from shared memory first
    received_image = receive_image_from_kernel()
    
    # If shared memory fails, try reading from kernel's raw file
    if received_image is None:
        kernel_image = read_kernel_raw_image()
    
    if received_image is not None or kernel_image is not None:
        print("Image processed and saved successfully")
    else:
        print("Failed to process image")