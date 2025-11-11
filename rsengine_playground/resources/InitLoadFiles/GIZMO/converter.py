# stl_to_header.py
import sys
import os

def file_to_c_array(filename, array_name):
    with open(filename, 'rb') as f:
        data = f.read()
    length = len(data)

    # Prepare header text
    lines = []
    lines.append(f"unsigned char {array_name}[] = {{")
    hex_line = []
    for i, b in enumerate(data):
        hex_line.append(f"0x{b:02X},")
        # Group hex values in lines of 12 for readability
        if (i + 1) % 12 == 0:
            lines.append("  " + " ".join(hex_line))
            hex_line = []
    # If leftover bytes remain
    if hex_line:
        lines.append("  " + " ".join(hex_line))
    lines.append("};")
    lines.append(f"unsigned int {array_name}_len = {length};")

    return "\n".join(lines)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python {sys.argv[0]} <input_file> <output_header>")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]

    # Derive array name from input filename by removing non-alphanumerics
    # or you can hardcode "my_model_stl" as needed
    base_name = os.path.splitext(os.path.basename(input_file))[0]
    array_name = base_name.replace('.', '_').replace('-', '_')

    header_content = file_to_c_array(input_file, array_name)

    with open(output_file, 'w') as out:
        out.write(header_content + "\n")
