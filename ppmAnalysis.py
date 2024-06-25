def read_ppm_p3(filename):
    with open(filename, 'r') as f:
        header = f.readline().strip()
        if header != 'P3':
            raise ValueError("Unsupported PPM format: only ASCII P3 format is supported")
        
        # Read the width, height, and max color value, skipping comments
        dimensions = f.readline().strip()
        while dimensions.startswith('#'):
            dimensions = f.readline().strip()
        width, height = map(int, dimensions.split())

        max_color = int(f.readline().strip())
        if max_color != 255:
            raise ValueError("Unsupported max color value: only 255 is supported")
        
        # Read the pixel data
        pixel_data = []
        for line in f:
            pixel_data.extend(map(int, line.split()))
            
    return width, height, pixel_data

def count_black_and_white_pixels(width, height, pixel_data):
    black_pixel_count = 0
    white_pixel_count = 0
    total_pixels = width * height
    for i in range(0, len(pixel_data), 3):
        r = pixel_data[i]
        g = pixel_data[i + 1]
        b = pixel_data[i + 2]
        if r == 0 and g == 0 and b == 0:
            black_pixel_count += 1
        elif r == 255 and g == 255 and b == 255:
            white_pixel_count += 1
    
    return black_pixel_count, white_pixel_count, total_pixels

def calculate_pixel_ratios_p3(filename):
    width, height, pixel_data = read_ppm_p3(filename)
    black_pixel_count, white_pixel_count, total_pixels = count_black_and_white_pixels(width, height, pixel_data)
    # print(f"Number of black pixels: {black_pixel_count:.4f}")
    # print(f"Number of white pixels: {white_pixel_count:.4f}")
    black_pixel_ratio = black_pixel_count / total_pixels
    white_pixel_ratio = white_pixel_count / total_pixels
    return black_pixel_ratio, white_pixel_ratio

ppm_files = ['finalCampNoLights600x100.ppm', 'finalCamp600x100.ppm', 'finalCamp600x100v2.ppm',
    'beachNoLights600x100.ppm', 'beachHalfLight.ppm', 'beachMaxIntense600x100v6.ppm']

for ppm_file in ppm_files:
    print(ppm_file)
    black_ratio, white_ratio = calculate_pixel_ratios_p3(ppm_file)
    print(f"Ratio of black pixels: {black_ratio:.4f}")
    print(f"Ratio of white pixels: {white_ratio:.4f}")
