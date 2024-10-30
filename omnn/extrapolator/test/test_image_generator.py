import numpy as np
from PIL import Image

# Create a 2x2 test image with RGBA channels
img_data = np.zeros((2, 2, 4), dtype=np.uint8)

# Set some test patterns
img_data[0,0] = [255, 0, 0, 255]    # Red
img_data[0,1] = [0, 255, 0, 255]    # Green
img_data[1,0] = [0, 0, 255, 255]    # Blue
img_data[1,1] = [255, 255, 255, 255] # White

# Create PIL Image
img = Image.fromarray(img_data, 'RGBA')

# Save as TGA
img.save('g.tga')

# Create a slightly larger resized version (4x4)
img_resized = img.resize((4, 4), Image.Resampling.NEAREST)
img_resized.save('gresized.tga')
