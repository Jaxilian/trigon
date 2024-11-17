import sys

def bin_to_header(input_file, output_file, array_name):
    with open(input_file, "rb") as f:
        data = f.read()

    with open(output_file, "w") as f:
        f.write(f"unsigned char {array_name}[] = {{\n")
        for i, byte in enumerate(data):
            if i % 12 == 0:
                f.write("\n   ")
            f.write(f"0x{byte:02x}, ")
        f.write("\n};\n")
        f.write(f"unsigned int {array_name}_len = {len(data)};\n")

if len(sys.argv) != 4:
    print("Usage: python bin_to_header.py <input_file> <output_file> <array_name>")
    sys.exit(1)

bin_to_header(sys.argv[1], sys.argv[2], sys.argv[3])