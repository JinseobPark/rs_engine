import sys
import os

def file_to_c_array(filename, array_name):
    with open(filename, 'rb') as f:
        data = f.read()
    length = len(data)

    lines = []
    lines.append(f"unsigned char {array_name}[] = {{")
    hex_line = []
    for i, b in enumerate(data):
        hex_line.append(f"0x{b:02X},")
        if (i + 1) % 12 == 0:
            lines.append("  " + " ".join(hex_line))
            hex_line = []
    if hex_line:
        lines.append("  " + " ".join(hex_line))
    lines.append("};")
    lines.append(f"unsigned int {array_name}_len = {length};")
    return "\n".join(lines)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print(f"Usage: python {sys.argv[0]} <input_directory> <output_header>")
        sys.exit(1)

    input_dir = sys.argv[1]
    output_file = sys.argv[2]

    # Gather all .stl files in the specified directory
    stl_files = [f for f in os.listdir(input_dir) if f.lower().endswith(".stl")]

    # Open the output file in write mode and append each STL as a C array
    with open(output_file, "w") as out:
        for stl_file in stl_files:
            full_path = os.path.join(input_dir, stl_file)
            base_name = os.path.splitext(stl_file)[0]
            array_name = base_name.replace('.', '_').replace('-', '_')
            header_content = file_to_c_array(full_path, array_name)
            out.write(header_content)
            out.write("\n\n")
