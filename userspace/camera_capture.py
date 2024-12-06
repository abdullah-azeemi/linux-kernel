import cv2
import numpy as np
import posix_ipc
import mmap
import struct
import sys

class ImageProcessor:
    def __init__(self, shared_memory_name="/image_transfer", memory_size=1024*1024):
        self.memory_size = memory_size
        self.shared_memory_name = shared_memory_name
        
    def capture_image(self):
        # Try multiple camera indices
        camera_indices = [0, 1, 2, 3]
        
        for index in camera_indices:
            cap = cv2.VideoCapture(index)
            
            # Check if camera is opened successfully
            if not cap.isOpened():
                print(f"Cannot open camera {index}")
                continue
            
            # Try to capture a frame
            ret, frame = cap.read()
            cap.release()
            
            if ret:
                return frame
        
        # If no camera works
        raise Exception("No camera found. Please check camera connections.")
    
    def send_image_to_kernel(self, image):
        try:
            # Convert image to grayscale to reduce size
            gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            flat_image = gray_image.flatten()
            
            memory = posix_ipc.SharedMemory(
                self.shared_memory_name, 
                posix_ipc.O_CREAT, 
                size=self.memory_size
            )
            
            mapped_memory = mmap.mmap(
                memory.fd, 
                self.memory_size
            )
            
            mapped_memory.write(struct.pack('II', gray_image.shape[0], gray_image.shape[1]))
            mapped_memory.write(flat_image.tobytes())
            
            mapped_memory.close()
            memory.close_fd()
            
            print("Image successfully transferred to kernel")
        
        except Exception as e:
            print(f"Transfer error: {e}")
            sys.exit(1)

if __name__ == "__main__":
    processor = ImageProcessor()
    try:
        image = processor.capture_image()
        processor.send_image_to_kernel(image)
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)