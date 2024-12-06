import posix_ipc
import mmap
import struct
import cv2
import numpy as np

def receive_image_from_kernel(shared_memory_name="/image_transfer", memory_size=1024*1024):
    try:
        # Open existing shared memory
        memory = posix_ipc.SharedMemory(shared_memory_name)
        
        # Memory map the shared memory
        mapped_memory = mmap.mmap(memory.fd, memory_size)
        
        # Read image dimensions
        height, width = struct.unpack('II', mapped_memory.read(8))
        
        # Read image data
        image_data = np.frombuffer(mapped_memory.read(), dtype=np.uint8)
        
        # Reshape image
        image = image_data.reshape((height, width))
        
        # Optional: Save or process image
        cv2.imwrite('processed_image.jpg', image)
        
        mapped_memory.close()
        memory.close_fd()
        
        return image
    
    except Exception as e:
        print(f"Image receive error: {e}")
        return None

if __name__ == "__main__":
    received_image = receive_image_from_kernel()
    if received_image is not None:
        print("Image received and saved successfully")