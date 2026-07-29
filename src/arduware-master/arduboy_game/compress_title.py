import math
from PIL import Image

def compress_arduboy(image_path):
    # Load image and convert to 1-bit
    img = Image.open(image_path).convert('1')
    width, height = img.size
    
    if height % 8 != 0:
        print(f"Warning: Height {height} is not a multiple of 8. Clipping.")
        height = (height // 8) * 8
        img = img.crop((0, 0, width, height))

    # Convert to Arduboy column-major bitmap
    pixels = img.load()
    bitmap = []
    for row_v in range(0, height, 8):
        for col in range(width):
            byte = 0
            for bit in range(8):
                if pixels[col, row_v + bit]:
                    byte |= (1 << bit)
            bitmap.append(byte)
            
    # Flatten bits into a single list
    bits = []
    for byte in bitmap:
        for i in range(8):
            bits.append((byte >> i) & 1)
            
    # RLE Spans
    spans = []
    if not bits:
        return []
        
    current_color = bits[0]
    count = 0
    for bit in bits:
        if bit == current_color:
            count += 1
        else:
            spans.append(count)
            current_color = bit
            count = 1
    spans.append(count)
    
    # Encode wait bits and lengths
    output_bits = []
    
    # Header: width-1 (8 bits), height-1 (8 bits), start color (1 bit)
    for i in range(8):
        output_bits.append(((width - 1) >> i) & 1)
    for i in range(8):
        output_bits.append(((height - 1) >> i) & 1)
    output_bits.append(bits[0])
    
    for span in spans:
        span_val = span - 1
        # Determine bitLength
        # bitLength = 1, 3, 5, 7, 9...
        # max_val = 2**bitLength - 1
        # We need bitLength such that 2**bitLength > span_val
        bl = 1
        zeros = 0
        while (1 << bl) <= span_val:
            bl += 2
            zeros += 1
            
        # Write zeros
        for _ in range(zeros):
            output_bits.append(0)
        # Write termination 1
        output_bits.append(1)
        # Write span_val in bl bits
        for i in range(bl):
            output_bits.append((span_val >> i) & 1)
            
    # Convert bits back to bytes
    output_bytes = []
    for i in range(0, len(output_bits), 8):
        byte = 0
        for j in range(8):
            if i + j < len(output_bits):
                if output_bits[i + j]:
                    byte |= (1 << j)
        output_bytes.append(byte)
        
    return output_bytes

def main():
    image_path = "arduware_title.png"
    compressed = compress_arduboy(image_path)
    print(f"Original size: 1024 bytes")
    print(f"Compressed size: {len(compressed)} bytes")
    
    with open("arduware_title_compressed.h", "w") as f:
        f.write("const uint8_t PROGMEM arduware_title_compressed[] = {\n    ")
        for i, b in enumerate(compressed):
            f.write(f"0x{b:02x}, ")
            if (i + 1) % 12 == 0:
                f.write("\n    ")
        f.write("\n};\n")
    print("Saved to arduware_title_compressed.h")

if __name__ == "__main__":
    main()
