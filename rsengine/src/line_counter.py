import pathlib


def recursive_search(new_root: pathlib.Path, suffix=""):
    lines = 0
    texts = 0
    files = 0
    
    for path in new_root.iterdir():
        if path.is_dir():
            sub_lines, sub_texts, sub_files = recursive_search(path, suffix)
            
            lines += sub_lines
            texts += sub_texts
            files += sub_files
            
            continue
        
        if not suffix or path.suffix != suffix:
            continue
        
        files += 1
        
        try:
            data = path.read_text("utf8")
        
        except UnicodeDecodeError as err:
            try:
                data = path.read_text()
            
            except UnicodeDecodeError as err:
                print(f"Can't read file {path.as_posix()} - {err}")
                continue
        
        lines += len(data.splitlines())
        texts += len(data)
    
    return lines, texts, files


root = pathlib.Path("./")
total_lines = 0
total_texts = 0

for _extention in [".gd", ".py", ".md", ".cpp", ".c", ".h"]:
    _lines, _texts, _files = recursive_search(root, _extention)
    
    # if 0 files pass
    if _files == 0:
        continue
    
    total_lines += _lines
    total_texts += _texts
    print(f"{_extention:8} | lines: {_lines:7} | characters: {_texts:8} | files: {_files}")

print(f"Total {total_lines} lines, {total_texts} characters")
input("Press enter to continue")
